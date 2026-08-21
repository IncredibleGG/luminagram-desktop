/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_transcriber_whisper.h"

#ifdef LUMINA_HAVE_WHISPER

#include "lumina/lumina_audio_decode.h"
#include "lumina/lumina_whisper_model.h"
#include "base/weak_ptr.h"

#include <crl/crl_async.h>
#include <crl/crl_on_main.h>

#include <whisper.h>

#include <algorithm>
#include <atomic>
#include <mutex>
#include <thread>
#include <vector>

#include <QtCore/QString>

namespace Lumina {
namespace {

// One whisper_context for the whole process, loaded on first use and kept.
// Loading parses ~59 MB and builds the model once; doing it per request would
// dwarf the transcription itself. whisper_full() is NOT safe to call
// concurrently on one context, so a single mutex serialises both the one-time
// load and every decode - voice notes are transcribed one at a time anyway.
std::mutex &ContextMutex() {
	static std::mutex mutex;
	return mutex;
}

// Guarded by ContextMutex(). Null until the first successful load; a failed
// load leaves it null and every call then answers Unavailable.
whisper_context *SharedContext() {
	static whisper_context *context = [] {
		// Silence whisper/ggml's stderr chatter: an offline transcription of a
		// private voice note has no business printing model details to the
		// console.
		whisper_log_set([](
			ggml_log_level, const char*, void*) {}, nullptr);
		auto params = whisper_context_default_params();
		params.use_gpu = false;
		const auto path = WhisperModelPath().toStdString();
		return whisper_init_from_file_with_params(path.c_str(), params);
	}();
	return context;
}

// Offline, on-device speech-to-text via whisper.cpp (id "whispercpp"). Like
// the Apple engine it needs no API key and uploads nothing: the voice note is
// decoded to 16 kHz mono PCM by the shared LuminaAudioDecoder, converted to
// float, and fed straight into whisper_full() on a background thread.
//
// Lifetime mirrors AppleSpeechEngine: a shared State holds a `cancelled` flag
// the background thread polls (through whisper's abort_callback) and a
// `delivered` flag that keeps `done` to exactly one call, so a result that
// finishes after the engine is destroyed sees the cancellation and never
// touches freed memory. `done` is delivered once, on the main thread.
class WhisperCppEngine final
	: public TranscribeEngine
	, public base::has_weak_ptr {
public:
	WhisperCppEngine() = default;
	~WhisperCppEngine();

	QString id() const override {
		return u"whispercpp"_q;
	}
	void transcribe(
		TranscribeInput input,
		Fn<void(TranscribeResult)> done) override;

private:
	struct State {
		std::atomic<bool> cancelled = false;
		std::atomic<bool> delivered = false;
	};

	// Never touches the engine, so it is safe from the background thread: it
	// only reads the shared state and hops to the main thread to run `done` at
	// most once.
	static void Deliver(
		std::shared_ptr<State> state,
		Fn<void(TranscribeResult)> done,
		TranscribeResult result);

	const std::shared_ptr<State> _state = std::make_shared<State>();

};

WhisperCppEngine::~WhisperCppEngine() {
	_state->cancelled.store(true);
}

void WhisperCppEngine::Deliver(
		std::shared_ptr<State> state,
		Fn<void(TranscribeResult)> done,
		TranscribeResult result) {
	if (!state || state->cancelled.load()) {
		return;
	}
	crl::on_main([
			state = std::move(state),
			done = std::move(done),
			result = std::move(result)]() mutable {
		if (state->cancelled.load() || state->delivered.exchange(true)) {
			return;
		}
		done(std::move(result));
	});
}

void WhisperCppEngine::transcribe(
		TranscribeInput input,
		Fn<void(TranscribeResult)> done) {
	const auto state = _state;
	if (!WhisperModelReady()) {
		// The model has not been downloaded. TranscriberConfigured() gates the
		// UI on the same check, so this is a belt-and-braces guard.
		Deliver(state, std::move(done), { .error = TranscribeError::NoKey });
		return;
	}
	auto ok = false;
	auto pcm = Lumina::DecodeToPcm16Mono16k(
		input.content,
		input.fileName,
		&ok);
	if (!ok || pcm.isEmpty() || (pcm.size() % int(sizeof(int16_t)) != 0)) {
		Deliver(
			state,
			std::move(done),
			{ .error = TranscribeError::UnsupportedMedia });
		return;
	}

	// Convert signed 16-bit LE mono PCM to the float [-1, 1] buffer
	// whisper_full() wants. Done here so the background thread owns only the
	// float vector, not the QByteArray.
	const auto samples = int(pcm.size() / int(sizeof(int16_t)));
	auto floats = std::vector<float>(samples);
	const auto s16 = reinterpret_cast<const int16_t*>(pcm.constData());
	for (auto i = 0; i != samples; ++i) {
		floats[i] = float(s16[i]) / 32768.0f;
	}

	crl::async([
			state,
			done = std::move(done),
			samples = std::move(floats)]() mutable {
		if (state->cancelled.load()) {
			return;
		}
		auto locked = std::unique_lock<std::mutex>(ContextMutex());
		const auto context = SharedContext();
		if (!context) {
			Deliver(
				state,
				std::move(done),
				{ .error = TranscribeError::Unavailable });
			return;
		}

		auto params = whisper_full_default_params(WHISPER_SAMPLING_GREEDY);
		// "auto" tells whisper to detect the spoken language itself, which is
		// the whole point of this engine in a cross-language chat.
		params.language = "auto";
		params.detect_language = false;
		params.translate = false;
		params.no_timestamps = true;
		params.no_context = true;
		params.print_realtime = false;
		params.print_progress = false;
		params.print_timestamps = false;
		params.print_special = false;
		const auto hw = int(std::thread::hardware_concurrency());
		params.n_threads = std::clamp(hw ? hw : 1, 1, 8);
		// Polled by ggml before each compute step; returning true aborts the
		// run promptly when the engine is being destroyed.
		params.abort_callback = [](void *data) -> bool {
			return static_cast<State*>(data)->cancelled.load();
		};
		params.abort_callback_user_data = state.get();

		const auto rc = whisper_full(
			context,
			params,
			samples.data(),
			int(samples.size()));
		if (rc != 0) {
			Deliver(
				state,
				std::move(done),
				{ .error = TranscribeError::Unavailable });
			return;
		}

		auto text = QString();
		const auto count = whisper_full_n_segments(context);
		for (auto i = 0; i != count; ++i) {
			const auto segment = whisper_full_get_segment_text(context, i);
			if (segment) {
				text += QString::fromUtf8(segment);
			}
		}
		locked.unlock();

		text = text.trimmed();
		Deliver(state, std::move(done), text.isEmpty()
			? TranscribeResult{ .error = TranscribeError::NoSpeech }
			: TranscribeResult{ .text = text });
	});
}

} // namespace

std::unique_ptr<TranscribeEngine> MakeWhisperCppEngine() {
	return std::make_unique<WhisperCppEngine>();
}

} // namespace Lumina

#else // LUMINA_HAVE_WHISPER

namespace Lumina {

// Built without whisper.cpp (e.g. mac, which keeps its Apple Speech engine):
// the factory answers null and MakeTranscribeEngine() simply never offers
// "whispercpp".
std::unique_ptr<TranscribeEngine> MakeWhisperCppEngine() {
	return nullptr;
}

} // namespace Lumina

#endif // LUMINA_HAVE_WHISPER
