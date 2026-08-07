/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_translate_gating.h"
#include "lumina/lumina_translate_providers.h"

#include "base/random.h"
#include "lang/lang_keys.h"
#include "lang/lang_text_entity.h"
#include "lang/translate_mtproto_provider.h"
#include "lang/translate_provider.h"
#include "lumina/lumina_locale.h"
#include "lumina/lumina_settings.h"

#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonParseError>
#include <QtCore/QUrl>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <crl/crl_on_main.h>

namespace Lumina {
namespace {

// tdesktop's own HTTP translate template (lang/translate_url_provider.cpp)
// sets no timeout at all, so a dead endpoint leaves the reply pending forever
// and its callback never runs. Everything here goes through SendHttp(), which
// always sets one.
constexpr auto kRequestTimeoutMs = 15000;

// The free Google web endpoint is a GET, so the whole text travels in the
// query string. Android cuts at 5000 characters of percent-encoded text; the
// same budget is used here, measured the same way.
constexpr auto kGoogleMaxEncoded = 5000;

constexpr auto kUserAgents = std::array<const char*, 6>{ {
	"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36"
		" (KHTML, like Gecko) Chrome/96.0.4664.45 Safari/537.36",
	"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36"
		" (KHTML, like Gecko) Chrome/96.0.4664.110 Safari/537.36",
	"Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:94.0)"
		" Gecko/20100101 Firefox/94.0",
	"Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:95.0)"
		" Gecko/20100101 Firefox/95.0",
	"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36"
		" (KHTML, like Gecko) Chrome/96.0.4664.93 Safari/537.36",
	"Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36"
		" (KHTML, like Gecko) Chrome/96.0.4664.55 Safari/537.36",
} };

[[nodiscard]] QString ProviderKey() {
	return u"translateProvider"_q;
}

[[nodiscard]] QString ApiKeyPrefix() {
	return u"translateKey_"_q;
}

[[nodiscard]] QString BaseUrlKey() {
	return u"translateBaseUrl"_q;
}

[[nodiscard]] QString ModelKey() {
	return u"translateModel"_q;
}

[[nodiscard]] QString PromptKey() {
	return u"translatePrompt"_q;
}

[[nodiscard]] QString FallbackKey() {
	return u"translateFallbackTelegram"_q;
}

// The read-language override belongs to lumina_translate_readlang.h, which is
// where it is written, documented and turned into a LanguageId. Only the
// stored string is wanted here - the whole point is the dialect that a
// LanguageId cannot carry - and reading the key directly keeps this file from
// depending on a sibling that ships separately. Rename it in both places.
[[nodiscard]] QString ReadLanguageKey() {
	return u"trReadLang"_q;
}

[[nodiscard]] QString ApiKeyName(const QString &providerId) {
	return providerId.isEmpty()
		? QString()
		: (ApiKeyPrefix() + providerId);
}

[[nodiscard]] bool IsLanguageSubtag(const QString &part) {
	if (part.size() != 2 && part.size() != 3) {
		return false;
	}
	for (const auto ch : part) {
		if (ch.unicode() < 'a' || ch.unicode() > 'z') {
			return false;
		}
	}
	return true;
}

// Only ever consulted for a code whose language subtag is already known to be
// Chinese, which is what keeps these two-letter markers from matching by
// accident somewhere in an unrelated language tag.
[[nodiscard]] bool IsTraditionalChinese(const QString &lowered) {
	return lowered.contains(u"hant"_q)
		|| lowered.contains(u"tw"_q)
		|| lowered.contains(u"hk"_q)
		|| lowered.contains(u"mo"_q);
}

[[nodiscard]] QByteArray RandomUserAgent() {
	const auto count = int(kUserAgents.size());
	return QByteArray(kUserAgents[base::RandomIndex(count)]);
}

[[nodiscard]] TranslateError ErrorForStatus(int status) {
	// 429 is the ordinary rate limit, 456 is DeepL's quota-exceeded code and
	// 403 is what several of these endpoints answer once a key is exhausted.
	return (status == 429 || status == 456 || status == 403)
		? TranslateError::RateLimited
		: TranslateError::Network;
}

struct HttpHeader {
	QByteArray name;
	QByteArray value;
};

// An empty `body` sends a GET, anything else a POST.
//
// `done` is handed the response body, the mapped error and the raw HTTP status
// - 0 when the request never got one - and nothing else. Nothing here is ever
// logged: the url carries the text being translated, the headers carry the
// user's API key, and an error body can echo either one back. The status is
// the one detail that is safe to carry out and that ErrorForStatus() throws
// away, and the settings test row needs it to name what actually went wrong.
void SendHttp(
		not_null<QNetworkAccessManager*> network,
		const QUrl &url,
		const std::vector<HttpHeader> &headers,
		const QByteArray &body,
		Fn<void(QByteArray, TranslateError, int)> done) {
	if (!url.isValid()) {
		done(QByteArray(), TranslateError::Network, 0);
		return;
	}
	auto request = QNetworkRequest(url);
	request.setTransferTimeout(kRequestTimeoutMs);
	for (const auto &header : headers) {
		request.setRawHeader(header.name, header.value);
	}
	const auto reply = body.isEmpty()
		? network->get(request)
		: network->post(request, body);
	QObject::connect(reply, &QNetworkReply::finished, [=] {
		const auto status = reply->attribute(
			QNetworkRequest::HttpStatusCodeAttribute).toInt();
		const auto failure = reply->error();
		auto received = reply->readAll();
		reply->deleteLater();
		if (status >= 400) {
			done(QByteArray(), ErrorForStatus(status), status);
		} else if (failure != QNetworkReply::NoError) {
			done(QByteArray(), TranslateError::Network, status);
		} else {
			done(std::move(received), TranslateError::None, status);
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

[[nodiscard]] int EncodedLength(const QString &text) {
	return int(QUrl::toPercentEncoding(text).size());
}

[[nodiscard]] std::vector<QString> SplitTokens(const QString &text) {
	auto result = std::vector<QString>();
	const auto size = int(text.size());
	auto start = 0;
	for (auto i = 0; i != size; ++i) {
		const auto ch = text.at(i);
		if (ch == QChar('\n') || ch == QChar(' ')) {
			result.push_back(text.mid(start, i + 1 - start));
			start = i + 1;
		}
	}
	if (start != size) {
		result.push_back(text.mid(start));
	}
	return result;
}

// Last resort for a single "word" that does not fit on its own. Splits on code
// point boundaries so that a surrogate pair is never cut in half.
[[nodiscard]] std::vector<QString> SplitHard(const QString &token, int limit) {
	auto result = std::vector<QString>();
	auto chunk = QString();
	auto length = 0;
	const auto size = int(token.size());
	for (auto i = 0; i != size;) {
		const auto step = (token.at(i).isHighSurrogate() && (i + 1 != size))
			? 2
			: 1;
		const auto piece = token.mid(i, step);
		const auto pieceLength = EncodedLength(piece);
		if (length > 0 && length + pieceLength > limit) {
			result.push_back(base::take(chunk));
			length = 0;
		}
		chunk += piece;
		length += pieceLength;
		i += step;
	}
	if (!chunk.isEmpty()) {
		result.push_back(std::move(chunk));
	}
	return result;
}

// Percent-encoding is per character and context free, so the encoded length of
// a concatenation is the sum of the encoded lengths of its parts. That is what
// lets this budget by accumulating token lengths instead of re-encoding the
// whole chunk on every step - and it is also why the split has to happen on
// the decoded text: cutting an already-encoded string can land in the middle
// of a "%E4" escape, or between the three escapes of one CJK code point.
[[nodiscard]] std::vector<QString> SplitForRequest(
		const QString &text,
		int limit) {
	if (text.isEmpty()) {
		return {};
	} else if (EncodedLength(text) <= limit) {
		return { text };
	}
	auto result = std::vector<QString>();
	auto chunk = QString();
	auto length = 0;
	for (const auto &token : SplitTokens(text)) {
		const auto tokenLength = EncodedLength(token);
		if (tokenLength > limit) {
			if (!chunk.isEmpty()) {
				result.push_back(base::take(chunk));
				length = 0;
			}
			for (auto &piece : SplitHard(token, limit)) {
				result.push_back(std::move(piece));
			}
			continue;
		}
		if (length > 0 && length + tokenLength > limit) {
			result.push_back(base::take(chunk));
			length = 0;
		}
		chunk += token;
		length += tokenLength;
	}
	if (!chunk.isEmpty()) {
		result.push_back(std::move(chunk));
	}
	return result;
}

[[nodiscard]] QString GoogleRequestUrl(
		const QString &text,
		const QString &wireCode) {
	// Deliberately built by concatenation rather than with QString::arg():
	// percent-encoded text contains sequences like "%20", which a second
	// chained arg() call would happily mistake for its own placeholder.
	auto result = u"https://translate.googleapis.com/translate_a/single"_q;
	result += u"?client=gtx&sl=auto&ie=UTF-8&oe=UTF-8&otf=1"_q;
	result += u"&ssel=0&tsel=0&kc=7"_q;
	result += u"&dt=t&dt=at&dt=bd&dt=ex&dt=ld&dt=md&dt=qca&dt=rw&dt=rm&dt=ss"_q;
	result += u"&tl="_q
		+ QString::fromLatin1(QUrl::toPercentEncoding(wireCode));
	result += u"&q="_q + QString::fromLatin1(QUrl::toPercentEncoding(text));
	return result;
}

struct GoogleWebResponse {
	QString text;
	QString detectedFrom;
};

// The endpoint answers a nested array whose first element is the list of
// translated segments, each of them an array whose first element is the
// translated text, and whose third top-level element is the detected source
// language. Everything else in it is alternatives and transliterations that
// this fork does not use.
[[nodiscard]] std::optional<GoogleWebResponse> ParseGoogleWebResponse(
		const QByteArray &body) {
	auto error = QJsonParseError{ 0, QJsonParseError::NoError };
	const auto document = QJsonDocument::fromJson(body, &error);
	if (error.error != QJsonParseError::NoError || !document.isArray()) {
		return std::nullopt;
	}
	const auto root = document.array();
	if (root.isEmpty() || !root.at(0).isArray()) {
		return std::nullopt;
	}
	auto result = GoogleWebResponse();
	const auto segments = root.at(0).toArray();
	for (const auto &value : segments) {
		if (!value.isArray()) {
			continue;
		}
		const auto segment = value.toArray();
		if (segment.isEmpty() || !segment.at(0).isString()) {
			continue;
		}
		const auto part = segment.at(0).toString();
		if (part != u"null"_q) {
			result.text += part;
		}
	}
	if (result.text.isEmpty()) {
		return std::nullopt;
	}
	if (root.size() > 2 && root.at(2).isString()) {
		result.detectedFrom = root.at(2).toString();
	}
	return result;
}

class GoogleWebEngine final : public TranslateEngine {
public:
	[[nodiscard]] QString id() const override {
		return GoogleWebProviderId();
	}

	void translate(
		const QString &text,
		const QString &toCode,
		Fn<void(TranslateResult)> done) override;

private:
	struct Batch {
		std::vector<QString> results;
		QString detectedFrom;
		Fn<void(TranslateResult)> done;
		int remaining = 0;
		bool finished = false;
	};

	void requestPart(
		const std::shared_ptr<Batch> &batch,
		int index,
		const QString &part,
		const QString &wireCode);

	QNetworkAccessManager _network;

};

void GoogleWebEngine::translate(
		const QString &text,
		const QString &toCode,
		Fn<void(TranslateResult)> done) {
	const auto parts = SplitForRequest(text, kGoogleMaxEncoded);
	if (parts.empty()) {
		done({ .error = TranslateError::BadResponse });
		return;
	}
	const auto count = int(parts.size());
	const auto batch = std::make_shared<Batch>();
	batch->results.resize(count);
	batch->remaining = count;
	batch->done = std::move(done);

	const auto wireCode = GoogleLanguageCode(toCode);
	for (auto i = 0; i != count && !batch->finished; ++i) {
		requestPart(batch, i, parts[i], wireCode);
	}
}

void GoogleWebEngine::requestPart(
		const std::shared_ptr<Batch> &batch,
		int index,
		const QString &part,
		const QString &wireCode) {
	const auto headers = std::vector<HttpHeader>{
		{ "User-Agent", RandomUserAgent() },
	};
	SendHttp(
		&_network,
		QUrl(GoogleRequestUrl(part, wireCode)),
		headers,
		QByteArray(),
		[=](QByteArray body, TranslateError error, int status) {
			if (batch->finished) {
				return;
			} else if (error != TranslateError::None) {
				batch->finished = true;
				batch->done({ .error = error, .httpStatus = status });
				return;
			}
			const auto parsed = ParseGoogleWebResponse(body);
			if (!parsed) {
				batch->finished = true;
				batch->done({
					.error = TranslateError::BadResponse,
					.httpStatus = status,
				});
				return;
			}
			batch->results[index] = parsed->text;
			if (batch->detectedFrom.isEmpty()) {
				batch->detectedFrom = parsed->detectedFrom;
			}
			if (--batch->remaining) {
				return;
			}
			batch->finished = true;
			auto text = QString();
			for (const auto &piece : batch->results) {
				text += piece;
			}
			batch->done({
				.text = text,
				.detectedFrom = batch->detectedFrom,
			});
		});
}

class DeepLEngine final : public TranslateEngine {
public:
	[[nodiscard]] QString id() const override {
		return DeepLProviderId();
	}

	void translate(
		const QString &text,
		const QString &toCode,
		Fn<void(TranslateResult)> done) override;

private:
	QNetworkAccessManager _network;

};

void DeepLEngine::translate(
		const QString &text,
		const QString &toCode,
		Fn<void(TranslateResult)> done) {
	const auto key = ProviderApiKey(DeepLProviderId());
	if (key.isEmpty()) {
		done({ .error = TranslateError::NoKey });
		return;
	}
	// DeepL free keys carry a ":fx" suffix and are only accepted by the free
	// host, so the endpoint follows from the key and never needs its own row.
	const auto host = key.endsWith(u":fx"_q)
		? u"https://api-free.deepl.com"_q
		: u"https://api.deepl.com"_q;
	const auto body = QJsonDocument(QJsonObject{
		{ u"text"_q, QJsonArray{ text } },
		{ u"target_lang"_q, DeepLLanguageCode(toCode) },
	}).toJson(QJsonDocument::Compact);
	const auto headers = std::vector<HttpHeader>{
		{ "Authorization", "DeepL-Auth-Key " + key.toUtf8() },
		{ "Content-Type", "application/json" },
	};
	SendHttp(
		&_network,
		QUrl(host + u"/v2/translate"_q),
		headers,
		body,
		[done](QByteArray received, TranslateError error, int status) {
			if (error != TranslateError::None) {
				done({ .error = error, .httpStatus = status });
				return;
			}
			const auto object = ParseJsonObject(received);
			if (!object) {
				done({
					.error = TranslateError::BadResponse,
					.httpStatus = status,
				});
				return;
			}
			const auto list = object->value(u"translations"_q).toArray();
			if (list.isEmpty()) {
				done({
					.error = TranslateError::BadResponse,
					.httpStatus = status,
				});
				return;
			}
			const auto first = list.at(0).toObject();
			done({
				.text = first.value(u"text"_q).toString(),
				.detectedFrom = first.value(
					u"detected_source_language"_q).toString(),
			});
		});
}

class LlmEngine final : public TranslateEngine {
public:
	[[nodiscard]] QString id() const override {
		return LlmProviderId();
	}

	void translate(
		const QString &text,
		const QString &toCode,
		Fn<void(TranslateResult)> done) override;

private:
	QNetworkAccessManager _network;

};

void LlmEngine::translate(
		const QString &text,
		const QString &toCode,
		Fn<void(TranslateResult)> done) {
	const auto key = ProviderApiKey(LlmProviderId());
	if (key.isEmpty()) {
		done({ .error = TranslateError::NoKey });
		return;
	}
	auto baseUrl = LlmBaseUrl();
	while (baseUrl.endsWith(QChar('/'))) {
		baseUrl.chop(1);
	}
	if (baseUrl.isEmpty()) {
		baseUrl = DefaultLlmBaseUrl();
	}
	const auto prompt = LlmPrompt().replace(
		u"{lang}"_q,
		LanguageEnglishName(toCode));
	const auto body = QJsonDocument(QJsonObject{
		{ u"model"_q, LlmModel() },
		{ u"temperature"_q, 0.2 },
		{ u"messages"_q, QJsonArray{
			QJsonObject{
				{ u"role"_q, u"system"_q },
				{ u"content"_q, prompt },
			},
			QJsonObject{
				{ u"role"_q, u"user"_q },
				{ u"content"_q, text },
			},
		} },
	}).toJson(QJsonDocument::Compact);
	const auto headers = std::vector<HttpHeader>{
		{ "Authorization", "Bearer " + key.toUtf8() },
		{ "Content-Type", "application/json" },
	};
	SendHttp(
		&_network,
		QUrl(baseUrl + u"/chat/completions"_q),
		headers,
		body,
		[done](QByteArray received, TranslateError error, int status) {
			if (error != TranslateError::None) {
				done({ .error = error, .httpStatus = status });
				return;
			}
			const auto object = ParseJsonObject(received);
			if (!object) {
				done({
					.error = TranslateError::BadResponse,
					.httpStatus = status,
				});
				return;
			}
			const auto choices = object->value(u"choices"_q).toArray();
			if (choices.isEmpty()) {
				done({
					.error = TranslateError::BadResponse,
					.httpStatus = status,
				});
				return;
			}
			const auto message = choices.at(0).toObject().value(
				u"message"_q).toObject();
			done({
				.text = message.value(u"content"_q).toString().trimmed(),
			});
		});
}

// Telegram's own engine, wrapped so that it can act both as a tier of the
// fallback chain and as an ordinary choice in the picker.
//
// The request carries text and no message id on purpose: the MTProto provider
// then takes its by-text branch, which is the only one that works for a
// message this client is not looking at, and the only one that exists at all
// on the send side.
class TelegramEngine final : public TranslateEngine {
public:
	explicit TelegramEngine(not_null<Main::Session*> session)
	: _provider(Ui::CreateMTProtoTranslateProvider(session)) {
	}

	[[nodiscard]] QString id() const override {
		return TelegramProviderId();
	}

	void translate(
			const QString &text,
			const QString &toCode,
			Fn<void(TranslateResult)> done) override {
		if (text.isEmpty()) {
			done({ .error = TranslateError::BadResponse });
			return;
		}
		_provider->request(
			Ui::TranslateProviderRequest{ .text = tr::marked(text) },
			LanguageId::FromName(toCode),
			[done](Ui::TranslateProviderResult result) {
				if (!result.text || result.text->text.isEmpty()) {
					done({ .error = TranslateError::Network });
				} else {
					done({ .text = result.text->text });
				}
			});
	}

private:
	const std::unique_ptr<Ui::TranslateProvider> _provider;

};

// Retries the whole request through a second engine when the first one fails,
// which is how a bad key, an exhausted quota or a dead self-hosted endpoint
// stops being a dead end for the user.
class FallbackEngine final : public TranslateEngine {
public:
	FallbackEngine(
		std::unique_ptr<TranslateEngine> primary,
		std::unique_ptr<TranslateEngine> secondary)
	: _primary(std::move(primary))
	, _secondary(std::move(secondary)) {
	}

	[[nodiscard]] QString id() const override {
		return _primary->id();
	}

	void translate(
			const QString &text,
			const QString &toCode,
			Fn<void(TranslateResult)> done) override {
		const auto secondary = _secondary.get();
		_primary->translate(text, toCode, [=](TranslateResult result) {
			if (!result.failed()) {
				done(std::move(result));
			} else {
				secondary->translate(text, toCode, done);
			}
		});
	}

private:
	const std::unique_ptr<TranslateEngine> _primary;
	const std::unique_ptr<TranslateEngine> _secondary;

};

// The adapter onto tdesktop's own interface. supportsMessageId() is false and
// must stay false - see the note on CreateTranslateProvider() in the header.
class EngineProvider final : public Ui::TranslateProvider {
public:
	explicit EngineProvider(std::unique_ptr<TranslateEngine> engine)
	: _engine(std::move(engine)) {
	}

	[[nodiscard]] bool supportsMessageId() const override {
		return false;
	}

	void request(
			Ui::TranslateProviderRequest request,
			LanguageId to,
			Fn<void(Ui::TranslateProviderResult)> done) override {
		const auto text = request.text.text;
		if (text.isEmpty()) {
			done({ .error = Ui::TranslateProviderError::Unknown });
			return;
		}
		_engine->translate(
			text,
			TargetLanguageCode(to),
			[done](TranslateResult result) {
				if (result.failed()) {
					done({ .error = Ui::TranslateProviderError::Unknown });
				} else {
					done({ .text = tr::marked(result.text) });
				}
			});
	}

private:
	const std::unique_ptr<TranslateEngine> _engine;

};

} // namespace

QString TelegramProviderId() {
	return u"telegram"_q;
}

QString GoogleWebProviderId() {
	return u"google_web"_q;
}

QString DeepLProviderId() {
	return u"deepl"_q;
}

QString LlmProviderId() {
	return u"llm"_q;
}

QString DefaultProviderId() {
	return GoogleWebProviderId();
}

const std::vector<TranslateProviderInfo> &TranslateProviders() {
	// The first three names are the services' own, and a brand is not ours to
	// translate. The LLM entry names a kind of endpoint rather than a product,
	// so it comes from our table - and it is refreshed in place, because
	// FindTranslateProvider() hands out pointers into this vector and no entry
	// may ever move.
	static auto result = std::vector<TranslateProviderInfo>{
		{
			.id = TelegramProviderId(),
			.name = u"Telegram"_q,
		},
		{
			.id = GoogleWebProviderId(),
			.name = u"Google"_q,
		},
		{
			.id = DeepLProviderId(),
			.name = u"DeepL"_q,
			.needsKey = true,
		},
		{
			.id = LlmProviderId(),
			.needsKey = true,
			.needsBaseUrl = true,
			.needsModel = true,
			.needsPrompt = true,
		},
	};
	static auto builtFor = QString();
	const auto locale = LocaleCode();
	if (builtFor != locale) {
		builtFor = locale;
		const auto i = ranges::find(
			result,
			LlmProviderId(),
			&TranslateProviderInfo::id);
		if (i != end(result)) {
			i->name = Tr(u"LuminaTranslateProviderLlm"_q);
		}
	}
	return result;
}

const TranslateProviderInfo *FindTranslateProvider(const QString &id) {
	const auto &list = TranslateProviders();
	const auto i = ranges::find(list, id, &TranslateProviderInfo::id);
	return (i != end(list)) ? &*i : nullptr;
}

QString CurrentProviderId() {
	const auto stored = Settings::Instance().getString(
		ProviderKey(),
		DefaultProviderId()).trimmed();
	return FindTranslateProvider(stored) ? stored : TelegramProviderId();
}

bool ProviderExplicitlyChosen() {
	// True only once the user has picked something on the Service row. The
	// stored key is absent on a fresh profile, where CurrentProviderId()
	// answers with a default instead - and a default is not a choice.
	const auto stored = Settings::Instance().getString(
		ProviderKey(),
		QString()).trimmed();
	return !stored.isEmpty() && (FindTranslateProvider(stored) != nullptr);
}

void SetCurrentProviderId(const QString &id) {
	if (!FindTranslateProvider(id)) {
		return;
	}
	Settings::Instance().set(ProviderKey(), id);
}

bool UsingOwnProvider() {
	// TranslateProviderConfigured() is load bearing, not belt and braces: an
	// unconfigured DeepL / LLM answers NoKey on every request, and the
	// Telegram fallback (on by default) then serves the whole conversation
	// from Telegram's paid service - with the Premium gate relaxed, because
	// this predicate is what relaxes it. See the header.
	const auto id = CurrentProviderId();
	return (id != TelegramProviderId()) && TranslateProviderConfigured(id);
}

bool TranslateProviderConfigured(const QString &id) {
	const auto info = FindTranslateProvider(id);
	if (!info) {
		return false;
	}
	return !info->needsKey || !ProviderApiKey(id).isEmpty();
}

rpl::producer<> TranslateProviderChanges() {
	return Settings::Instance().changes(
	) | rpl::filter([](const QString &key) {
		return (key == ProviderKey())
			|| (key == BaseUrlKey())
			|| (key == ModelKey())
			|| (key == PromptKey())
			|| (key == FallbackKey())
			|| key.startsWith(ApiKeyPrefix());
	}) | rpl::to_empty;
}

QString DefaultLlmBaseUrl() {
	return u"https://api.openai.com/v1"_q;
}

QString DefaultLlmModel() {
	return u"gpt-4o-mini"_q;
}

QString DefaultLlmPrompt() {
	return u"You are a professional translator. Translate the user's message "
		"into {lang}. Output ONLY the translation, with no quotes, no notes, "
		"and no explanations. Preserve tone, emojis and formatting."_q;
}

QString ProviderApiKey(const QString &providerId) {
	const auto name = ApiKeyName(providerId);
	return name.isEmpty()
		? QString()
		: Settings::Instance().getString(name).trimmed();
}

void SetProviderApiKey(const QString &providerId, const QString &key) {
	const auto name = ApiKeyName(providerId);
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

QString LlmBaseUrl() {
	const auto stored = Settings::Instance().getString(
		BaseUrlKey()).trimmed();
	return stored.isEmpty() ? DefaultLlmBaseUrl() : stored;
}

void SetLlmBaseUrl(const QString &value) {
	const auto trimmed = value.trimmed();
	if (trimmed.isEmpty() || trimmed == DefaultLlmBaseUrl()) {
		Settings::Instance().remove(BaseUrlKey());
	} else {
		Settings::Instance().set(BaseUrlKey(), trimmed);
	}
}

QString LlmModel() {
	const auto stored = Settings::Instance().getString(ModelKey()).trimmed();
	return stored.isEmpty() ? DefaultLlmModel() : stored;
}

void SetLlmModel(const QString &value) {
	const auto trimmed = value.trimmed();
	if (trimmed.isEmpty() || trimmed == DefaultLlmModel()) {
		Settings::Instance().remove(ModelKey());
	} else {
		Settings::Instance().set(ModelKey(), trimmed);
	}
}

QString LlmPrompt() {
	const auto stored = Settings::Instance().getString(PromptKey()).trimmed();
	return stored.isEmpty() ? DefaultLlmPrompt() : stored;
}

void SetLlmPrompt(const QString &value) {
	const auto trimmed = value.trimmed();
	if (trimmed.isEmpty() || trimmed == DefaultLlmPrompt()) {
		Settings::Instance().remove(PromptKey());
	} else {
		Settings::Instance().set(PromptKey(), trimmed);
	}
}

bool TranslateFallbackToTelegram() {
	return Settings::Instance().getBool(FallbackKey(), true);
}

void SetTranslateFallbackToTelegram(bool value) {
	Settings::Instance().set(FallbackKey(), value);
}

QString NormalizeLanguageCode(const QString &code) {
	const auto lowered = code.trimmed().toLower().replace(
		QChar('_'),
		QChar('-'));
	if (lowered.isEmpty()) {
		return QString();
	}
	// Telegram language pack ids are not always plain BCP-47: they can carry
	// a vendor prefix ("classic-zh-tw") or a pack suffix ("zh-hant-raw").
	// Dropping the leading subtags that cannot be a language code makes both
	// of those resolve to the language they are actually written in.
	auto parts = lowered.split(QChar('-'), Qt::SkipEmptyParts);
	while (!parts.isEmpty() && !IsLanguageSubtag(parts.front())) {
		parts.removeFirst();
	}
	if (parts.isEmpty()) {
		return QString();
	}
	const auto language = parts.front();
	if (language == u"zh"_q) {
		return IsTraditionalChinese(lowered) ? u"zh-TW"_q : u"zh-CN"_q;
	}
	for (auto i = 1; i < int(parts.size()); ++i) {
		const auto &part = parts[i];
		if (part.size() == 2) {
			return language + '-' + part.toUpper();
		}
	}
	return language;
}

QString BaseLanguageCode(const QString &code) {
	const auto normalized = NormalizeLanguageCode(code);
	const auto index = normalized.indexOf(QChar('-'));
	return (index < 0) ? normalized : normalized.mid(0, index);
}

QString InterfaceLanguageCode() {
	return NormalizeLanguageCode(Lang::LanguageIdOrDefault(Lang::Id()));
}

QString TargetLanguageCode(LanguageId id) {
	const auto language = id.known() ? id.twoLetterCode() : QString();
	const auto readLang = NormalizeLanguageCode(
		Settings::Instance().getString(ReadLanguageKey()));
	if (!readLang.isEmpty()
		&& (language.isEmpty() || BaseLanguageCode(readLang) == language)) {
		return readLang;
	}
	const auto interfaceCode = InterfaceLanguageCode();
	if (language.isEmpty()) {
		return interfaceCode;
	} else if (!interfaceCode.isEmpty()
		&& BaseLanguageCode(interfaceCode) == language) {
		return interfaceCode;
	}
	return language;
}

QString GoogleLanguageCode(const QString &code) {
	const auto normalized = NormalizeLanguageCode(code);
	const auto language = BaseLanguageCode(normalized);
	if (language == u"zh"_q) {
		return normalized;
	} else if (language == u"nb"_q) {
		// Google knows Norwegian as "no". DeepL, on the other hand, spells it
		// "NB", so this rewrite belongs here and not in the normaliser.
		return u"no"_q;
	}
	return language;
}

QString DeepLLanguageCode(const QString &code) {
	const auto normalized = NormalizeLanguageCode(code);
	const auto language = BaseLanguageCode(normalized);
	if (language == u"en"_q) {
		return (normalized == u"en-GB"_q) ? u"EN-GB"_q : u"EN-US"_q;
	} else if (language == u"pt"_q) {
		return (normalized == u"pt-PT"_q) ? u"PT-PT"_q : u"PT-BR"_q;
	} else if (language == u"zh"_q) {
		return (normalized == u"zh-TW"_q) ? u"ZH-HANT"_q : u"ZH"_q;
	}
	return language.toUpper();
}

QString LanguageEnglishName(const QString &code) {
	const auto normalized = NormalizeLanguageCode(code);
	if (normalized.isEmpty()) {
		return code.trimmed();
	}
	// QLocale cannot name a dialect, and an LLM asked for "Chinese" answers in
	// whichever one it feels like, so the ones that actually differ are named
	// here instead.
	static const auto named = base::flat_map<QString, QString>{
		{ u"zh-TW"_q, u"Traditional Chinese"_q },
		{ u"zh-CN"_q, u"Simplified Chinese"_q },
		{ u"pt-BR"_q, u"Brazilian Portuguese"_q },
		{ u"pt-PT"_q, u"European Portuguese"_q },
		{ u"en-GB"_q, u"British English"_q },
		{ u"en-US"_q, u"American English"_q },
	};
	const auto i = named.find(normalized);
	if (i != named.end()) {
		return i->second;
	}
	const auto id = LanguageId::FromName(BaseLanguageCode(normalized));
	if (id.value == QLocale::AnyLanguage || id.value == QLocale::C) {
		return normalized;
	}
	const auto name = QLocale::languageToString(id.language());
	return name.isEmpty() ? normalized : name;
}

std::unique_ptr<TranslateEngine> MakeTranslateEngine(
		const QString &providerId,
		Main::Session *session) {
	if (providerId == GoogleWebProviderId()) {
		return std::make_unique<GoogleWebEngine>();
	} else if (providerId == DeepLProviderId()) {
		return std::make_unique<DeepLEngine>();
	} else if (providerId == LlmProviderId()) {
		return std::make_unique<LlmEngine>();
	} else if (providerId == TelegramProviderId() && session) {
		return std::make_unique<TelegramEngine>(session);
	}
	return nullptr;
}

std::unique_ptr<TranslateEngine> MakeCurrentTranslateEngine(
		Main::Session *session) {
	const auto id = CurrentProviderId();
	auto primary = MakeTranslateEngine(id, session);
	if (!primary
		|| !session
		|| (id == TelegramProviderId())
		|| !TranslateFallbackToTelegram()) {
		return primary;
	}
	auto secondary = MakeTranslateEngine(TelegramProviderId(), session);
	if (!secondary) {
		return primary;
	}
	return std::make_unique<FallbackEngine>(
		std::move(primary),
		std::move(secondary));
}

void TranslateText(
		Main::Session *session,
		const QString &text,
		const QString &toCode,
		Fn<void(TranslateResult)> done) {
	Expects(done != nullptr);

	auto created = MakeCurrentTranslateEngine(session);
	if (!created) {
		done({ .error = TranslateError::Unavailable });
		return;
	}
	const auto holder = std::make_shared<std::unique_ptr<TranslateEngine>>(
		std::move(created));
	holder->get()->translate(text, toCode, [=](TranslateResult result) {
		done(std::move(result));

		// This callback runs inside the engine's own network reply, so the
		// engine cannot be destroyed from here: that would delete the reply,
		// and with it the lambda currently executing. Release it from the
		// next main thread turn instead.
		crl::on_main([holder] { holder->reset(); });
	});
}

std::unique_ptr<Ui::TranslateProvider> CreateTranslateProvider(
		not_null<Main::Session*> session) {
	// The master opt-in has to gate this too: without it a Premium account
	// with the feature OFF still had its chat translations sent to Google
	// instead of to Telegram, which is exactly what OFF must not do.
	if (!ContinuousTranslationAvailable() || !UsingOwnProvider()) {
		return nullptr;
	}
	auto engine = MakeCurrentTranslateEngine(session);
	if (!engine) {
		return nullptr;
	}
	return std::make_unique<EngineProvider>(std::move(engine));
}

} // namespace Lumina
