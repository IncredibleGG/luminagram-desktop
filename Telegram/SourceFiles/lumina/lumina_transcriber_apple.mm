/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_transcriber_apple.h"

#ifdef __APPLE__

#include "lumina/lumina_audio_decode.h"
#include "base/weak_ptr.h"
#include "base/platform/mac/base_utilities_mac.h"

#include <crl/crl_on_main.h>

#include <atomic>
#include <cstring>
#include <memory>

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import <Speech/Speech.h>

namespace Lumina {
namespace {

// Apple's own free, on-device speech-to-text (id "apple"). Unlike the cloud
// engines it needs no API key and no upload: the voice note is decoded to raw
// 16 kHz mono PCM by the shared LuminaAudioDecoder and fed straight into
// SFSpeechRecognizer, exactly as Android's Vosk path does with its decoder.
//
// It is a member-owned request the same way WhisperEngine's _network is: the
// engine holds the running SFSpeechRecognitionTask and a shared cancelled
// flag, so destroying the engine cancels the task and the shared flag stops
// the completion from ever firing afterwards. The flag is a shared_ptr rather
// than a member because the recognition block can outlive the engine, and it
// must be able to see the cancellation without touching freed memory. `done`
// is delivered exactly once, on the main thread, through crl::on_main.
class AppleSpeechEngine final
	: public TranscribeEngine
	, public base::has_weak_ptr {
public:
	AppleSpeechEngine() = default;
	~AppleSpeechEngine();

	QString id() const override {
		return u"apple"_q;
	}
	void transcribe(
		TranscribeInput input,
		Fn<void(TranscribeResult)> done) override;

private:
	// Shared with the recognition block so a result that arrives after the
	// engine is gone sees the cancellation and never calls `done`. `delivered`
	// keeps `done` to exactly one call even if the handler fires more than
	// once.
	struct State {
		std::atomic<bool> cancelled = false;
		std::atomic<bool> delivered = false;
	};

	// Runs on the main thread with the engine guaranteed alive (guarded
	// crl::on_main), so it is safe to store the task in a member the
	// destructor can cancel.
	void start(
		QByteArray pcm,
		QString langHint,
		Fn<void(TranscribeResult)> done);

	// Never touches the engine, so it is safe from the recognition block on an
	// arbitrary queue: it only reads the shared state and hops to the main
	// thread to run `done` at most once.
	static void Deliver(
		std::shared_ptr<State> state,
		Fn<void(TranscribeResult)> done,
		TranscribeResult result);

	const std::shared_ptr<State> _state = std::make_shared<State>();
	// Held as a plain id rather than SFSpeechRecognitionTask* so this member does not
	// name a macOS 10.15-only type in a class reachable from 10.13 code; every real
	// Speech API call below sits inside an @available(macOS 10.15, *) guard.
	::id _task = nil;  // ::id -> the Objective-C type; bare 'id' would bind to the id() method above

};

AppleSpeechEngine::~AppleSpeechEngine() {
	_state->cancelled.store(true);
	if (_task != nil) {
		[_task cancel];
		[_task release];
		_task = nil;
	}
}

void AppleSpeechEngine::Deliver(
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

void AppleSpeechEngine::transcribe(
		TranscribeInput input,
		Fn<void(TranscribeResult)> done) {
	auto ok = false;
	auto pcm = Lumina::DecodeToPcm16Mono16k(input.content, input.fileName, &ok);
	const auto state = _state;
	if (!ok || pcm.isEmpty()) {
		// The container is something the decoder could not read; no request is
		// started, and nothing is uploaded because nothing is ever uploaded.
		Deliver(state, std::move(done), { .error = TranscribeError::UnsupportedMedia });
		return;
	}
	const auto weak = base::make_weak(this);
	const auto langHint = input.langHint;
	// Authorization is asked exactly once here; the system remembers the
	// answer, so a second voice note never re-prompts. The block runs on an
	// arbitrary queue, so it hops back to the main thread - guarded by the
	// engine's weak_ptr - before it touches the engine.
	if (@available(macOS 10.15, *)) {
		[SFSpeechRecognizer requestAuthorization:^(
				SFSpeechRecognizerAuthorizationStatus status) {
			if (status != SFSpeechRecognizerAuthorizationStatusAuthorized) {
				Deliver(state, done, { .error = TranscribeError::Unavailable });
				return;
			}
			crl::on_main(weak, [=] {
				weak->start(pcm, langHint, done);
			});
		}];
	} else {
		Deliver(state, std::move(done), { .error = TranscribeError::Unavailable });
	}
}

void AppleSpeechEngine::start(
		QByteArray pcm,
		QString langHint,
		Fn<void(TranscribeResult)> done) {
	const auto state = _state;

	if (@available(macOS 10.15, *)) {
		// A non-empty hint pins the locale; anything unusable falls back to the
		// system default recognizer rather than failing outright.
		SFSpeechRecognizer *recognizer = nil;
		if (!langHint.isEmpty()) {
			NSLocale *locale = [NSLocale
				localeWithLocaleIdentifier:Platform::Q2NSString(langHint)];
			recognizer = [[[SFSpeechRecognizer alloc]
				initWithLocale:locale] autorelease];
		}
		if (!recognizer) {
			recognizer = [[[SFSpeechRecognizer alloc] init] autorelease];
		}
		if (!recognizer || !recognizer.isAvailable) {
			Deliver(state, std::move(done), { .error = TranscribeError::Unavailable });
			return;
		}

		// The decoder already handed us signed 16-bit little-endian mono at
		// 16 kHz, which is exactly the buffer SFSpeechRecognizer wants; the format
		// here just describes those bytes so no resampling happens.
		AVAudioFormat *format = [[[AVAudioFormat alloc]
			initWithCommonFormat:AVAudioPCMFormatInt16
			sampleRate:16000.0
			channels:1
			interleaved:YES] autorelease];
		const auto samples = int(pcm.size() / int(sizeof(int16_t)));
		if (!format || samples <= 0) {
			Deliver(state, std::move(done), { .error = TranscribeError::UnsupportedMedia });
			return;
		}
		AVAudioPCMBuffer *buffer = [[[AVAudioPCMBuffer alloc]
			initWithPCMFormat:format
			frameCapacity:(AVAudioFrameCount)samples] autorelease];
		if (!buffer || buffer.int16ChannelData == nullptr) {
			Deliver(state, std::move(done), { .error = TranscribeError::UnsupportedMedia });
			return;
		}
		buffer.frameLength = (AVAudioFrameCount)samples;
		std::memcpy(
			buffer.int16ChannelData[0],
			pcm.constData(),
			samples * sizeof(int16_t));

		SFSpeechAudioBufferRecognitionRequest *request =
			[[[SFSpeechAudioBufferRecognitionRequest alloc] init] autorelease];
		// Stay on-device when the recognizer can - that is the whole reason this
		// engine exists, a private voice note that never leaves the machine - but
		// fall back to the server path rather than refuse when it cannot.
		if (recognizer.supportsOnDeviceRecognition) {
			request.requiresOnDeviceRecognition = YES;
		}
		request.shouldReportPartialResults = NO;
		[request appendAudioPCMBuffer:buffer];
		[request endAudio];

		// start() only runs with the engine alive (guarded crl::on_main), so the
		// destructor cannot have cancelled yet; the check is a cheap guard against
		// a future caller that reaches here off the main thread.
		if (state->cancelled.load()) {
			return;
		}
		SFSpeechRecognitionTask *task = [recognizer
			recognitionTaskWithRequest:request
			resultHandler:^(SFSpeechRecognitionResult *result, NSError *error) {
				if (result != nil && result.isFinal) {
					const auto text = Platform::NS2QString(
						result.bestTranscription.formattedString).trimmed();
					Deliver(state, done, text.isEmpty()
						? TranscribeResult{ .error = TranscribeError::NoSpeech }
						: TranscribeResult{ .text = text });
				} else if (error != nil) {
					// It ran and produced nothing usable; the enum cannot tell a
					// silent note from a recognizer that gave up, so both read as
					// "nothing to hear".
					Deliver(state, done, { .error = TranscribeError::NoSpeech });
				}
			}];
		[_task release];
		_task = [task retain];
	} else {
		Deliver(state, std::move(done), { .error = TranscribeError::Unavailable });
	}
}

} // namespace

std::unique_ptr<TranscribeEngine> MakeAppleSpeechEngine() {
	if (@available(macOS 10.15, *)) {
		return std::make_unique<AppleSpeechEngine>();
	}
	return nullptr;
}

} // namespace Lumina

#else // __APPLE__

namespace Lumina {

// Not an Apple platform: there is no on-device Apple Speech, so the factory
// answers null and MakeTranscribeEngine() simply never offers "apple".
std::unique_ptr<TranscribeEngine> MakeAppleSpeechEngine() {
	return nullptr;
}

} // namespace Lumina

#endif // __APPLE__
