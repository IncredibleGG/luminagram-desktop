/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_audio_decode.h"

#include "ffmpeg/ffmpeg_bytes_io_wrap.h"
#include "ffmpeg/ffmpeg_utility.h"

namespace Lumina {
namespace {

// Both the Apple Speech recogniser and Vosk take signed 16-bit mono PCM.
// 16 kHz is the rate the Vosk models are trained at and is plenty for speech,
// so we downmix and resample everything to it once, here.
constexpr auto kDstRate = 16000;

} // namespace

QByteArray DecodeToPcm16Mono16k(
		const QByteArray &content,
		const QString &fileName,
		bool *ok) {
	using namespace FFmpeg;

	const auto fail = [&]() -> QByteArray {
		if (ok) {
			*ok = false;
		}
		return {};
	};

	if (content.isEmpty()) {
		return fail();
	}

	// Feed the in-memory container to libavformat through a seekable custom
	// AVIOContext, exactly like media_audio_local_cache.cpp does.
	auto wrap = ReadBytesWrap{
		.size = content.size(),
		.data = reinterpret_cast<const uchar*>(content.constData()),
	};
	auto input = MakeFormatPointer(
		&wrap,
		&ReadBytesWrap::Read,
		nullptr,
		&ReadBytesWrap::Seek);
	if (!input) {
		return fail();
	}

	if (AvErrorWrap error = avformat_find_stream_info(input.get(), nullptr)) {
		LogError(u"avformat_find_stream_info"_q, error, fileName);
		return fail();
	}

	auto inCodec = (const AVCodec*)nullptr;
	const auto streamId = av_find_best_stream(
		input.get(),
		AVMEDIA_TYPE_AUDIO,
		-1,
		-1,
		&inCodec,
		0);
	if (streamId < 0) {
		LogError(u"av_find_best_stream"_q, AvErrorWrap(streamId), fileName);
		return fail();
	}

	const auto inStream = input->streams[streamId];
	auto codecContext = CodecPointer(avcodec_alloc_context3(nullptr));
	if (!codecContext) {
		return fail();
	}
	if (AvErrorWrap error = avcodec_parameters_to_context(
			codecContext.get(),
			inStream->codecpar)) {
		LogError(u"avcodec_parameters_to_context"_q, error, fileName);
		return fail();
	}
	codecContext->pkt_timebase = inStream->time_base;
	if (AvErrorWrap error = avcodec_open2(
			codecContext.get(),
			inCodec,
			nullptr)) {
		LogError(u"avcodec_open2"_q, error, fileName);
		return fail();
	}
	if (codecContext->sample_rate <= 0) {
		LogError(u"DecodeToPcm16Mono16k: bad sample rate"_q, fileName);
		return fail();
	}

	// Resample every decoded frame down to S16 / mono / 16 kHz.
	auto dstLayout = AVChannelLayout(AV_CHANNEL_LAYOUT_MONO);
	auto swrContext = MakeSwresamplePointer(
		&codecContext->ch_layout,
		codecContext->sample_fmt,
		codecContext->sample_rate,
		&dstLayout,
		AV_SAMPLE_FMT_S16,
		kDstRate);
	if (!swrContext) {
		return fail();
	}

	auto frame = MakeFramePointer();
	auto outFrame = MakeFramePointer();
	if (!frame || !outFrame) {
		return fail();
	}

	// A single reused output buffer, grown on demand.
	auto outCapacity = 0;
	const auto ensureOut = [&](int samples) {
		if (samples <= outCapacity) {
			return true;
		}
		av_frame_unref(outFrame.get());
		outFrame->nb_samples = samples;
		outFrame->format = AV_SAMPLE_FMT_S16;
		av_channel_layout_copy(&outFrame->ch_layout, &dstLayout);
		outFrame->sample_rate = kDstRate;
		if (AvErrorWrap error = av_frame_get_buffer(outFrame.get(), 0)) {
			LogError(u"av_frame_get_buffer"_q, error, fileName);
			outCapacity = 0;
			return false;
		}
		outCapacity = samples;
		return true;
	};

	auto result = QByteArray();

	// in is null and inSamples is 0 to flush the resampler at the very end.
	const auto convertAndAppend = [&](
			const uint8_t **in,
			int inSamples) {
		const auto maxSamples = int(av_rescale_rnd(
			swr_get_delay(swrContext.get(), codecContext->sample_rate)
				+ inSamples,
			kDstRate,
			codecContext->sample_rate,
			AV_ROUND_UP));
		if (maxSamples <= 0) {
			return true;
		} else if (!ensureOut(maxSamples)) {
			return false;
		}
		const auto samples = swr_convert(
			swrContext.get(),
			outFrame->data,
			maxSamples,
			in,
			inSamples);
		if (samples < 0) {
			LogError(u"swr_convert"_q, AvErrorWrap(samples), fileName);
			return false;
		} else if (samples > 0) {
			result.append(
				reinterpret_cast<const char*>(outFrame->data[0]),
				samples * int(sizeof(int16_t)));
		}
		return true;
	};

	auto packet = av_packet_alloc();
	if (!packet) {
		return fail();
	}
	const auto packetGuard = gsl::finally([&] {
		av_packet_free(&packet);
	});

	for (;;) {
		auto error = AvErrorWrap(av_read_frame(input.get(), packet));
		const auto finished = (error.code() == AVERROR_EOF);
		if (!finished) {
			if (error) {
				LogError(u"av_read_frame"_q, error, fileName);
				return fail();
			}
			const auto unref = gsl::finally([&] {
				av_packet_unref(packet);
			});
			if (packet->stream_index != streamId) {
				continue;
			}
			if (AvErrorWrap sendError = avcodec_send_packet(
					codecContext.get(),
					packet)) {
				LogError(u"avcodec_send_packet"_q, sendError, fileName);
				return fail();
			}
		} else if (AvErrorWrap drainError = avcodec_send_packet(
				codecContext.get(),
				nullptr)) {
			if (drainError.code() != AVERROR_EOF) {
				LogError(u"avcodec_send_packet"_q, drainError, fileName);
				return fail();
			}
		}

		for (;;) {
			auto error = AvErrorWrap(avcodec_receive_frame(
				codecContext.get(),
				frame.get()));
			if (error.code() == AVERROR(EAGAIN)
				|| error.code() == AVERROR_EOF) {
				break;
			} else if (error) {
				LogError(u"avcodec_receive_frame"_q, error, fileName);
				return fail();
			}
			if (!convertAndAppend(
					(const uint8_t**)frame->extended_data,
					frame->nb_samples)) {
				return fail();
			}
		}

		if (finished) {
			break;
		}
	}

	// Drain whatever the resampler still holds buffered.
	if (!convertAndAppend(nullptr, 0)) {
		return fail();
	}

	if (result.isEmpty()) {
		return fail();
	}
	if (ok) {
		*ok = true;
	}
	return result;
}

} // namespace Lumina
