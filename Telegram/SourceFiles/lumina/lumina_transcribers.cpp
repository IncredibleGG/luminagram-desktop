/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_transcribers.h"

#include "base/random.h"
#include "lumina/lumina_settings.h"
#ifdef Q_OS_MAC
#include "lumina/lumina_transcriber_apple.h" // LuminaGram: on-device Apple engine.
#endif // Q_OS_MAC

#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonParseError>
#include <QtCore/QJsonValue>
#include <QtCore/QUrl>
#include <QtCore/QUrlQuery>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

namespace Lumina {
namespace {

// Uploading a voice note is not a 15-second job on a slow line the way a
// translation request is, so this is four times the translate timeout. It is
// still always set: lumina_translate_providers.cpp documents why a reply with
// no timeout is a callback that may never run, and that applies here too.
constexpr auto kRequestTimeoutMs = 60000;

// The engines' own documented ceilings. Checked before the upload starts, so
// a voice note that cannot possibly work costs the user no bandwidth.
constexpr auto kWhisperMaxBytes = 25 * 1024 * 1024;
constexpr auto kGoogleMaxBytes = 10 * 1024 * 1024;

[[nodiscard]] QString EngineKey() {
	return u"sttEngine"_q;
}

[[nodiscard]] QString ApiKeyPrefix() {
	return u"sttKey_"_q;
}

[[nodiscard]] QString BaseUrlKey() {
	return u"sttCloudBaseUrl"_q;
}

[[nodiscard]] QString ModelKey() {
	return u"sttModel"_q;
}

[[nodiscard]] QString ApiKeyName(const QString &id) {
	return id.isEmpty() ? QString() : (ApiKeyPrefix() + id);
}

[[nodiscard]] QString TrimTrailingSlash(const QString &url) {
	auto result = url.trimmed();
	while (result.endsWith('/')) {
		result.chop(1);
	}
	return result;
}

[[nodiscard]] TranscribeError ErrorForStatus(int status) {
	// Same mapping the translation layer uses: 429 is the ordinary rate
	// limit, 456 is a quota-exceeded code and 403 is what several of these
	// endpoints answer once a key is exhausted.
	return (status == 429 || status == 456 || status == 403)
		? TranscribeError::RateLimited
		: TranscribeError::Network;
}

struct HttpHeader {
	QByteArray name;
	QByteArray value;
};

// `done` is handed the response body, the mapped error and the raw HTTP
// status - 0 when the request never got one - and nothing else. Nothing here
// is ever logged: the headers carry the user's API key, the body carries the
// contents of a private voice message, and an error body can echo either one
// back.
void SendPost(
		not_null<QNetworkAccessManager*> network,
		const QUrl &url,
		const std::vector<HttpHeader> &headers,
		const QByteArray &body,
		Fn<void(QByteArray, TranscribeError, int)> done) {
	if (!url.isValid() || body.isEmpty()) {
		done(QByteArray(), TranscribeError::Network, 0);
		return;
	}
	auto request = QNetworkRequest(url);
	request.setTransferTimeout(kRequestTimeoutMs);
	for (const auto &header : headers) {
		request.setRawHeader(header.name, header.value);
	}
	const auto reply = network->post(request, body);
	QObject::connect(reply, &QNetworkReply::finished, [=] {
		const auto status = reply->attribute(
			QNetworkRequest::HttpStatusCodeAttribute).toInt();
		const auto failure = reply->error();
		auto received = reply->readAll();
		reply->deleteLater();
		if (status >= 400) {
			done(QByteArray(), ErrorForStatus(status), status);
		} else if (failure != QNetworkReply::NoError) {
			done(QByteArray(), TranscribeError::Network, status);
		} else {
			done(std::move(received), TranscribeError::None, status);
		}
	});
}

[[nodiscard]] std::optional<QJsonObject> ParseJsonObject(
		const QByteArray &body) {
	auto error = QJsonParseError{ 0, QJsonParseError::NoError };
	const auto document = QJsonDocument::fromJson(body, &error);
	if (error.error != QJsonParseError::NoError || !document.isObject()) {
		return std::nullopt;
	}
	return document.object();
}

// Whisper's endpoint is the one place in the fork that does not take JSON: it
// wants multipart/form-data, which QNetworkAccessManager will not build for
// us in a form we can hand a plain QByteArray body to. Fifteen lines of
// RFC 7578 are cheaper than a second request-sending path, and this is the
// same envelope Android's WhisperTranscriber writes by hand.
//
// A voice note is small (Telegram caps voice at a few MB, and the engines cap
// the upload well below that - see kWhisperMaxBytes), so the body is
// assembled in memory rather than streamed. `boundary` is random per request
// so that it can never collide with the audio bytes.
class MultipartBody final {
public:
	MultipartBody()
	: _boundary("----LuminaGramBoundary"
		+ QByteArray::number(qint64(base::RandomValue<uint64>()), 16)) {
	}

	void addText(const QString &name, const QString &value) {
		if (name.isEmpty() || value.isEmpty()) {
			return;
		}
		_body += "--" + _boundary + "\r\n";
		_body += "Content-Disposition: form-data; name=\""
			+ Escaped(name) + "\"\r\n\r\n";
		_body += value.toUtf8();
		_body += "\r\n";
	}

	void addFile(
			const QString &name,
			const QString &fileName,
			const QString &mimeType,
			const QByteArray &content) {
		_body += "--" + _boundary + "\r\n";
		_body += "Content-Disposition: form-data; name=\""
			+ Escaped(name) + "\"; filename=\""
			+ Escaped(fileName) + "\"\r\n";
		_body += "Content-Type: " + mimeType.toUtf8() + "\r\n\r\n";
		_body += content;
		_body += "\r\n";
	}

	[[nodiscard]] QByteArray finish() {
		auto result = _body;
		result += "--" + _boundary + "--\r\n";
		return result;
	}

	[[nodiscard]] QByteArray contentType() const {
		return "multipart/form-data; boundary=" + _boundary;
	}

private:
	// A quote or a newline in a form-data name would let the value break out
	// of its own header line. Nothing we send today can contain either - the
	// names are literals and the file names are ours - but a header builder
	// that trusts its input is a bug waiting for the first caller that does
	// not know that.
	[[nodiscard]] static QByteArray Escaped(const QString &value) {
		auto cleaned = value;
		cleaned.replace('"', QString());
		cleaned.replace('\r', QString());
		cleaned.replace('\n', QString());
		return cleaned.toUtf8();
	}

	const QByteArray _boundary;
	QByteArray _body;

};

// Best-effort ISO language hint -> the BCP-47 code Google Speech expects.
// Region-qualified hints pass through with an upper-cased region; bare codes
// map to a common regional default; unknown or empty falls back to "en-US".
// Ported verbatim from Android's GoogleSpeechTranscriber.bcp47().
[[nodiscard]] QString GoogleLanguageCode(const QString &hint) {
	const auto s = hint.trimmed().toLower().replace('_', '-');
	if (s.isEmpty()) {
		return u"en-US"_q;
	}
	const auto dash = s.indexOf('-');
	if (dash > 0) {
		return s.mid(0, dash) + '-' + s.mid(dash + 1).toUpper();
	}
	static const auto defaults = std::vector<std::pair<QString, QString>>{
		{ u"en"_q, u"en-US"_q }, { u"zh"_q, u"zh-CN"_q },
		{ u"es"_q, u"es-ES"_q }, { u"pt"_q, u"pt-BR"_q },
		{ u"fr"_q, u"fr-FR"_q }, { u"de"_q, u"de-DE"_q },
		{ u"it"_q, u"it-IT"_q }, { u"ru"_q, u"ru-RU"_q },
		{ u"ja"_q, u"ja-JP"_q }, { u"ko"_q, u"ko-KR"_q },
		{ u"ar"_q, u"ar-SA"_q }, { u"hi"_q, u"hi-IN"_q },
		{ u"tr"_q, u"tr-TR"_q }, { u"nl"_q, u"nl-NL"_q },
		{ u"pl"_q, u"pl-PL"_q }, { u"uk"_q, u"uk-UA"_q },
		{ u"vi"_q, u"vi-VN"_q }, { u"id"_q, u"id-ID"_q },
		{ u"th"_q, u"th-TH"_q }, { u"fa"_q, u"fa-IR"_q },
	};
	for (const auto &[code, full] : defaults) {
		if (code == s) {
			return full;
		}
	}
	return s;
}

// OpenAI-compatible /audio/transcriptions. Bring your own key, base url and
// model, so a self-hosted whisper.cpp server works exactly as api.openai.com
// does.
//
// The language is deliberately NOT sent. Whisper detects it on its own, and
// this feature exists for chats where the other side speaks a language the
// reader does not - pinning the interface language as a hint there would tell
// the engine to transcribe Japanese speech as English.
class WhisperEngine final : public TranscribeEngine {
public:
	QString id() const override {
		return WhisperTranscriberId();
	}

	void transcribe(
			TranscribeInput input,
			Fn<void(TranscribeResult)> done) override {
		const auto key = TranscriberApiKey(id());
		if (key.isEmpty()) {
			done({ .error = TranscribeError::NoKey });
			return;
		} else if (input.content.isEmpty()) {
			done({ .error = TranscribeError::Unavailable });
			return;
		} else if (input.content.size() > kWhisperMaxBytes) {
			done({ .error = TranscribeError::TooLarge });
			return;
		}
		auto multipart = MultipartBody();
		multipart.addText(u"model"_q, SttModel());
		multipart.addText(u"response_format"_q, u"json"_q);
		multipart.addFile(
			u"file"_q,
			input.fileName,
			input.mimeType,
			input.content);
		const auto contentType = multipart.contentType();
		const auto body = multipart.finish();
		SendPost(
			&_network,
			QUrl(TrimTrailingSlash(SttBaseUrl()) + u"/audio/transcriptions"_q),
			{ { "Authorization", "Bearer " + key.toUtf8() },
				{ "Content-Type", contentType } },
			body,
			[done](QByteArray body, TranscribeError error, int status) {
				if (error != TranscribeError::None) {
					done({ .error = error, .httpStatus = status });
					return;
				}
				const auto parsed = ParseJsonObject(body);
				if (!parsed) {
					done({
						.error = TranscribeError::BadResponse,
						.httpStatus = status,
					});
					return;
				}
				const auto text = parsed->value(u"text"_q).toString().trimmed();
				done(text.isEmpty()
					? TranscribeResult{
						.error = TranscribeError::NoSpeech,
						.httpStatus = status,
					}
					: TranscribeResult{ .text = text, .httpStatus = status });
			});
	}

private:
	QNetworkAccessManager _network;

};

// Google Cloud Speech-to-Text v1, synchronous recognize. The audio is
// base64-encoded into the JSON body, which is why the ceiling is 10 MB rather
// than Whisper's 25.
//
// Telegram voice notes are 48 kHz Opus in an OGG container, so the request is
// pinned to OGG_OPUS / 48000 Hz exactly as Android pins it. That is also why
// a round video is refused here rather than uploaded: it is an MP4, this
// endpoint cannot read one, and sending it anyway would spend the user's
// quota to be told so.
//
// The API key travels in the query string because that is Google's documented
// API-key auth for this endpoint. It is never logged, and errors carry the
// HTTP status only.
class GoogleEngine final : public TranscribeEngine {
public:
	QString id() const override {
		return GoogleTranscriberId();
	}

	void transcribe(
			TranscribeInput input,
			Fn<void(TranscribeResult)> done) override {
		const auto key = TranscriberApiKey(id());
		if (key.isEmpty()) {
			done({ .error = TranscribeError::NoKey });
			return;
		} else if (input.roundVideo) {
			done({ .error = TranscribeError::UnsupportedMedia });
			return;
		} else if (input.content.isEmpty()) {
			done({ .error = TranscribeError::Unavailable });
			return;
		} else if (input.content.size() > kGoogleMaxBytes) {
			done({ .error = TranscribeError::TooLarge });
			return;
		}
		auto config = QJsonObject();
		config.insert(u"encoding"_q, u"OGG_OPUS"_q);
		config.insert(u"sampleRateHertz"_q, 48000);
		config.insert(
			u"languageCode"_q,
			GoogleLanguageCode(input.langHint));
		auto audio = QJsonObject();
		audio.insert(
			u"content"_q,
			QString::fromLatin1(input.content.toBase64()));
		auto root = QJsonObject();
		root.insert(u"config"_q, config);
		root.insert(u"audio"_q, audio);

		auto url = QUrl(u"https://speech.googleapis.com/v1/speech:recognize"_q);
		auto query = QUrlQuery();
		query.addQueryItem(u"key"_q, key);
		url.setQuery(query);

		SendPost(
			&_network,
			url,
			{ { "Content-Type", "application/json; charset=utf-8" } },
			QJsonDocument(root).toJson(QJsonDocument::Compact),
			[done](QByteArray body, TranscribeError error, int status) {
				if (error != TranscribeError::None) {
					done({ .error = error, .httpStatus = status });
					return;
				}
				const auto parsed = ParseJsonObject(body);
				if (!parsed) {
					done({
						.error = TranscribeError::BadResponse,
						.httpStatus = status,
					});
					return;
				}
				auto text = QString();
				const auto results = parsed->value(u"results"_q).toArray();
				for (const auto &entry : results) {
					const auto alternatives = entry.toObject().value(
						u"alternatives"_q).toArray();
					if (alternatives.isEmpty()) {
						continue;
					}
					const auto part = alternatives.at(0).toObject().value(
						u"transcript"_q).toString().trimmed();
					if (part.isEmpty()) {
						continue;
					}
					if (!text.isEmpty()) {
						text += ' ';
					}
					text += part;
				}
				done(text.isEmpty()
					? TranscribeResult{
						.error = TranscribeError::NoSpeech,
						.httpStatus = status,
					}
					: TranscribeResult{ .text = text, .httpStatus = status });
			});
	}

private:
	QNetworkAccessManager _network;

};

} // namespace

QString WhisperTranscriberId() {
	return u"whisper"_q;
}

QString GoogleTranscriberId() {
	return u"google"_q;
}

QString VoskTranscriberId() {
	return u"vosk"_q;
}

// LuminaGram: Apple's on-device Speech framework - macOS-only, free, no key.
QString AppleTranscriberId() {
	return u"apple"_q;
}

QString DefaultTranscriberId() {
#ifdef Q_OS_MAC
	// LuminaGram: mac ships the free on-device engine, so it is the
	// default there; every other platform keeps Whisper.
	return AppleTranscriberId();
#else // Q_OS_MAC
	return WhisperTranscriberId();
#endif // Q_OS_MAC
}

const std::vector<TranscriberInfo> &Transcribers() {
	static const auto result = std::vector<TranscriberInfo>{
#ifdef Q_OS_MAC
		// LuminaGram: Apple's on-device engine - macOS-only, free, no API
		// key and no model to download. Listed first so the free default
		// leads the picker, the way Vosk does on Android.
		{
			.id = AppleTranscriberId(),
			.name = u"Apple Speech"_q,
			.needsKey = false,
			.needsModel = false,
		},
#endif // Q_OS_MAC
		{
			.id = WhisperTranscriberId(),
			.name = u"OpenAI Whisper"_q,
			.needsKey = true,
			.needsBaseUrl = true,
			.needsModel = true,
		},
		{
			.id = GoogleTranscriberId(),
			.name = u"Google Speech"_q,
			.needsKey = true,
		},
	};
	return result;
}

const TranscriberInfo *FindTranscriber(const QString &id) {
	if (!id.isEmpty()) {
		for (const auto &info : Transcribers()) {
			if (info.id == id) {
				return &info;
			}
		}
	}
	return nullptr;
}

QString CurrentTranscriberId() {
	const auto stored = Settings::Instance().getString(EngineKey()).trimmed();
	// "vosk" lands here too, and on purpose: a profile restored from the
	// phone carries it, and answering an id with no engine behind it would
	// make every row on the settings page read blank.
	// LuminaGram: a profile carried from an older version can hold an engine that
	// cannot run here (e.g. "whisper" with no API key). Rather than answer a dead
	// id -- which leaves the transcribe row hidden and the button inert -- fall back
	// to the platform default (mac = free on-device Apple Speech) whenever the stored
	// engine is unknown, or is known but unconfigured while the default IS configured.
	// An explicitly key-configured cloud engine is left untouched.
	const auto def = DefaultTranscriberId();
	if (!FindTranscriber(stored)) {
		return def;
	}
	if (!TranscriberConfigured(stored) && TranscriberConfigured(def)) {
		return def;
	}
	return stored;
}

const TranscriberInfo &CurrentTranscriber() {
	// CurrentTranscriberId() only ever answers an id that exists, so the
	// fallback is unreachable while Transcribers() is non-empty. It is here
	// so that a future build which empties the list cannot dereference null
	// on the settings page.
	static const auto fallback = TranscriberInfo();
	const auto found = FindTranscriber(CurrentTranscriberId());
	return found ? *found : fallback;
}

void SetCurrentTranscriberId(const QString &id) {
	if (FindTranscriber(id)) {
		Settings::Instance().set(EngineKey(), id);
	}
}

QString TranscriberApiKey(const QString &id) {
	const auto name = ApiKeyName(id);
	return name.isEmpty()
		? QString()
		: Settings::Instance().getString(name).trimmed();
}

void SetTranscriberApiKey(const QString &id, const QString &key) {
	const auto name = ApiKeyName(id);
	if (name.isEmpty()) {
		return;
	}
	const auto trimmed = key.trimmed();
	if (trimmed.isEmpty()) {
		Settings::Instance().remove(name);
	} else {
		Settings::Instance().set(name, trimmed, Store::Private);
	}
}

QString DefaultSttBaseUrl() {
	return u"https://api.openai.com/v1"_q;
}

QString DefaultSttModel() {
	return u"whisper-1"_q;
}

QString SttBaseUrl() {
	const auto stored = Settings::Instance().getString(
		BaseUrlKey()).trimmed();
	return stored.isEmpty() ? DefaultSttBaseUrl() : stored;
}

void SetSttBaseUrl(const QString &value) {
	const auto trimmed = value.trimmed();
	if (trimmed.isEmpty() || trimmed == DefaultSttBaseUrl()) {
		Settings::Instance().remove(BaseUrlKey());
	} else {
		Settings::Instance().set(BaseUrlKey(), trimmed);
	}
}

QString SttModel() {
	const auto stored = Settings::Instance().getString(ModelKey()).trimmed();
	return stored.isEmpty() ? DefaultSttModel() : stored;
}

void SetSttModel(const QString &value) {
	const auto trimmed = value.trimmed();
	if (trimmed.isEmpty() || trimmed == DefaultSttModel()) {
		Settings::Instance().remove(ModelKey());
	} else {
		Settings::Instance().set(ModelKey(), trimmed);
	}
}

bool TranscriberConfigured(const QString &id) {
	const auto info = FindTranscriber(id);
	return info && (!info->needsKey || !TranscriberApiKey(id).isEmpty());
}

rpl::producer<> TranscriberChanges() {
	return Settings::Instance().changes(
	) | rpl::filter([](const QString &key) {
		return (key == EngineKey())
			|| (key == BaseUrlKey())
			|| (key == ModelKey())
			|| key.startsWith(ApiKeyPrefix());
	}) | rpl::to_empty;
}

std::unique_ptr<TranscribeEngine> MakeTranscribeEngine(const QString &id) {
	if (id == WhisperTranscriberId()) {
		return std::make_unique<WhisperEngine>();
	} else if (id == GoogleTranscriberId()) {
		return std::make_unique<GoogleEngine>();
	}
#ifdef Q_OS_MAC
	// LuminaGram: the Apple engine is Objective-C++ (implemented in
	// lumina_transcriber_apple.mm) so it is only wired up on mac.
	if (id == AppleTranscriberId()) {
		return MakeAppleSpeechEngine();
	}
#endif // Q_OS_MAC
	return nullptr;
}

std::unique_ptr<TranscribeEngine> MakeCurrentTranscribeEngine() {
	return MakeTranscribeEngine(CurrentTranscriberId());
}

} // namespace Lumina
