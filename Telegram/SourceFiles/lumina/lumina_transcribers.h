/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "base/basic_types.h"

#include <rpl/producer.h>

#include <memory>
#include <vector>

#include <QtCore/QByteArray>
#include <QtCore/QString>

namespace Lumina {

// LuminaGram's speech-to-text backend: the user's own API key instead of
// Telegram Premium's server-side transcription.
//
// This is the desktop half of Android's LuminaTranscriber /
// LuminaTranscribers pair, and it deliberately carries only the CLOUD
// engines.
//
// WHY NO VOSK. Android's default engine is Vosk, which is offline and free.
// It is not here, and shipping it would be a different project: Vosk is a
// native library plus a ~50 MB model that would have to be built, signed,
// notarised and shipped for Windows, macOS and Linux separately, and it needs
// raw PCM - so a voice note would first have to be decoded from Opus, which
// is why Android carries LuminaAudioDecoder at all. A cloud engine needs none
// of that: the .ogg / .mp4 goes up exactly as Telegram stored it. The engine
// id "vosk" is still RECOGNISED here (a settings file synced or restored from
// a phone can carry it) and resolves to the default cloud engine rather than
// to nothing, but it is never offered in the picker, and the settings page
// says so out loud instead of pretending offline transcription exists.
//
// The shape mirrors lumina/lumina_translate_providers.h on purpose: same
// registry / current-id / api-key layout, same "engine object owns its
// requests, destroying it cancels them" lifetime contract, same "errors never
// carry the request or response body" rule.

// Stable engine ids. They are persisted under the same `sttEngine` key
// Android uses, so they must never change.
[[nodiscard]] QString WhisperTranscriberId(); // "whisper"
[[nodiscard]] QString GoogleTranscriberId();  // "google"
[[nodiscard]] QString VoskTranscriberId();    // "vosk", Android-only.
// LuminaGram: Apple's on-device Speech framework - macOS-only, free, no key.
[[nodiscard]] QString AppleTranscriberId();   // "apple", macOS-only.

// Whisper: it is the only one of the two that detects the spoken language on
// its own, which is exactly what a cross-language chat needs.
[[nodiscard]] QString DefaultTranscriberId();

struct TranscriberInfo {
	QString id;
	QString name;
	bool needsKey = false;
	bool needsBaseUrl = false;
	bool needsModel = false;
};

// Registration order is the order of the settings picker.
[[nodiscard]] const std::vector<TranscriberInfo> &Transcribers();

// Null for an unknown id.
[[nodiscard]] const TranscriberInfo *FindTranscriber(const QString &id);

// Always an id that exists: an unknown or unset stored value - including the
// Android-only "vosk" - resolves to DefaultTranscriberId().
[[nodiscard]] QString CurrentTranscriberId();
[[nodiscard]] const TranscriberInfo &CurrentTranscriber();
void SetCurrentTranscriberId(const QString &id);

// API keys live in Store::Private, one per engine id, under Android's
// `sttKey_<id>` spelling.
//
// As with the translation keys: Lumina::Settings::set() defaults to
// Store::Prefs, and passing that default for a key that lives in
// Store::Private MOVES the value into the plaintext pref file.
// SetTranscriberApiKey() is the only supported way to write one.
[[nodiscard]] QString TranscriberApiKey(const QString &id);
void SetTranscriberApiKey(const QString &id, const QString &key);

// Getters never answer an empty string: an unset or blank value reads back as
// the default, and setting a value back to the default drops the key instead
// of pinning today's default into the pref file forever.
[[nodiscard]] QString DefaultSttBaseUrl(); // https://api.openai.com/v1
[[nodiscard]] QString DefaultSttModel();   // whisper-1
[[nodiscard]] QString SttBaseUrl();
void SetSttBaseUrl(const QString &value);
[[nodiscard]] QString SttModel();
void SetSttModel(const QString &value);

// False when the selected engine needs an API key that is not set. There is
// no free fallback on desktop, so this is the whole difference between "the
// feature can run" and "the feature can only apologise".
[[nodiscard]] bool TranscriberConfigured(const QString &id);

// Fires when the engine, a key, the base url or the model changes.
[[nodiscard]] rpl::producer<> TranscriberChanges();

enum class TranscribeError : uchar {
	None,
	Unavailable,   // No engine could run the request at all.
	NoKey,         // The engine needs an API key that is not set.
	TooLarge,      // Past the engine's own upload limit.
	UnsupportedMedia, // The engine cannot read this container.
	RateLimited,   // HTTP 429 / 456 / 403.
	Network,       // Transport failure, timeout, or any other HTTP error.
	BadResponse,   // The engine answered with something unparseable.
	NoSpeech,      // It ran, and there was nothing to hear.
};

struct TranscribeResult {
	QString text;
	TranscribeError error = TranscribeError::None;

	// The HTTP status the service answered with, 0 when there never was one.
	// Only meaningful next to a non-None `error`, and carried for the same
	// reason TranslateResult carries it: the enum cannot tell a rejected key
	// (401) from a host that never answered.
	int httpStatus = 0;

	[[nodiscard]] bool failed() const {
		return (error != TranscribeError::None) || text.isEmpty();
	}
};

struct TranscribeInput {
	QByteArray content;
	QString fileName;  // "voice.ogg" / "round.mp4"
	QString mimeType;  // "audio/ogg" / "video/mp4"
	QString langHint;  // ISO-639 or BCP-47; may be empty.
	bool roundVideo = false;
};

// Lifetime contract, the same one TranslateEngine has: destroying an engine
// cancels every request it still has in flight, and their callbacks then
// never run - which is what makes it safe to hold one as a member of
// something shorter-lived than the session.
//
// The other side of that: `done` runs inside the engine's own network reply,
// so `done` must not destroy the engine. Drop it from a later main-thread
// turn.
//
// `done` is invoked exactly once, on the main thread.
class TranscribeEngine {
public:
	virtual ~TranscribeEngine() = default;

	[[nodiscard]] virtual QString id() const = 0;
	virtual void transcribe(
		TranscribeInput input,
		Fn<void(TranscribeResult)> done) = 0;

};

// Null for an unknown id.
[[nodiscard]] std::unique_ptr<TranscribeEngine> MakeTranscribeEngine(
	const QString &id);

// The selected engine. Never null while Transcribers() is non-empty, because
// CurrentTranscriberId() never answers an id that does not exist.
[[nodiscard]] std::unique_ptr<TranscribeEngine> MakeCurrentTranscribeEngine();

} // namespace Lumina
