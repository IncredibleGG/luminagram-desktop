/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "spellcheck/spellcheck_types.h" // LanguageId.

#include <rpl/producer.h>

#include <memory>
#include <vector>

namespace Main {
class Session;
} // namespace Main

namespace Ui {
class TranslateProvider;
} // namespace Ui

namespace Lumina {

// LuminaGram's translation backend: the user's own engine or API key instead
// of Telegram's paid service.
//
// Two layers, because they cannot be one:
//
//  * TranslateEngine is the LuminaGram-native one. It takes and returns plain
//    QString, and its target language is a full language *code* ("zh-TW"), not
//    a LanguageId. This is the layer every LuminaGram feature should use.
//  * CreateTranslateProvider() adapts an engine to Ui::TranslateProvider so
//    that tdesktop's own translate bar, translate box and per-chat translation
//    keep working unchanged.
//
// The split exists because of the dialect blocker: Ui::TranslateProvider takes
// a LanguageId, which is a bare QLocale::Language with no script and no region
// (spellcheck/spellcheck_types.h), so it cannot tell zh-TW from zh-CN, pt-BR
// from pt-PT or en-GB from en-US. This fork's own interface is zh-TW, so
// carrying LanguageId around would ship Simplified Chinese to every user.
// Anything inside namespace Lumina therefore carries QString codes end to end,
// and LanguageId appears only at the tdesktop boundary, where
// TargetLanguageCode() recovers as much of the dialect as it can.

// Stable provider ids. They are persisted, so they must never change.
[[nodiscard]] QString TelegramProviderId();   // "telegram"
[[nodiscard]] QString GoogleWebProviderId();  // "google_web"
[[nodiscard]] QString DeepLProviderId();      // "deepl"
[[nodiscard]] QString LlmProviderId();        // "llm"

// The free, keyless Google web engine, so that translation works out of the
// box for users without Telegram Premium. An unknown stored id falls back to
// TelegramProviderId(), never to this one.
[[nodiscard]] QString DefaultProviderId();

struct TranslateProviderInfo {
	QString id;
	QString name;
	bool needsKey = false;
	bool needsBaseUrl = false;
	bool needsModel = false;
	bool needsPrompt = false;
};

// Registration order, Telegram first, is the order of the settings picker.
[[nodiscard]] const std::vector<TranslateProviderInfo> &TranslateProviders();

// Null for an unknown id.
[[nodiscard]] const TranslateProviderInfo *FindTranslateProvider(
	const QString &id);

// Always the id of a provider that exists: an *unknown* stored value resolves
// to TelegramProviderId(), while an unset one resolves to DefaultProviderId().
[[nodiscard]] QString CurrentProviderId();
void SetCurrentProviderId(const QString &id);

// The predicate every Premium-gate relaxation hangs off. True exactly when
// translation runs through the user's own engine or key instead of Telegram's
// paid server-side service - which is the only case where relaxing the gate is
// not a way of getting Telegram's paid feature for free.
//
// It therefore also requires TranslateProviderConfigured(): a provider that
// needs an API key but has none answers TranslateError::NoKey on every
// request, and TranslateFallbackToTelegram() - which is on by default - then
// retries the whole thing through Telegram's own engine. Selecting DeepL and
// entering no key would otherwise unlock the Premium-gated whole-chat
// translation and then serve every message from Telegram's paid service,
// which is exactly the thing this predicate exists to prevent.
[[nodiscard]] bool UsingOwnProvider();

// False when the provider needs an API key that is not set yet.
[[nodiscard]] bool TranslateProviderConfigured(const QString &id);

// Fires when any of the settings below changes: the provider, a key, the LLM
// base URL / model / prompt, or the Telegram fallback toggle.
[[nodiscard]] rpl::producer<> TranslateProviderChanges();

// Defaults for the OpenAI-compatible provider, public so that the settings
// rows can prefill and display them without duplicating the literals.
[[nodiscard]] QString DefaultLlmBaseUrl();  // https://api.openai.com/v1
[[nodiscard]] QString DefaultLlmModel();    // gpt-4o-mini
[[nodiscard]] QString DefaultLlmPrompt();   // contains the {lang} placeholder

// API keys live in Store::Private, one key per provider id.
//
// Lumina::Settings::set() defaults to Store::Prefs, and passing that default
// for a key that lives in Store::Private MOVES the value into the plaintext
// pref file. SetProviderApiKey() is therefore the only supported way to write
// one: never call Settings::set() on a "translateKey_*" key anywhere else.
[[nodiscard]] QString ProviderApiKey(const QString &providerId);
void SetProviderApiKey(const QString &providerId, const QString &key);

// Getters never answer an empty string: an unset or blank value reads back as
// the corresponding default, and setting a value back to the default drops the
// key instead of pinning today's default into the pref file forever.
[[nodiscard]] QString LlmBaseUrl();
void SetLlmBaseUrl(const QString &value);
[[nodiscard]] QString LlmModel();
void SetLlmModel(const QString &value);
[[nodiscard]] QString LlmPrompt();
void SetLlmPrompt(const QString &value);

// When the selected provider fails, retry once through Telegram's own engine
// rather than showing the user nothing. On by default, as on Android.
[[nodiscard]] bool TranslateFallbackToTelegram();
void SetTranslateFallbackToTelegram(bool value);

// Language codes. Everything here takes and returns a code, never a
// LanguageId, and every function tolerates the ids Telegram's language packs
// actually use ("zh-hant-raw", "pt-br", "classic-zh-tw") as well as plain
// BCP-47 tags.

// Lower-cased, '_' folded to '-', vendor prefixes and pack suffixes dropped,
// region upper-cased, and Chinese resolved to exactly "zh-TW" or "zh-CN".
// Empty for a code that carries no language subtag at all.
[[nodiscard]] QString NormalizeLanguageCode(const QString &code);

// The language subtag alone: NormalizeLanguageCode() minus the region.
[[nodiscard]] QString BaseLanguageCode(const QString &code);

// The interface language of this build, normalised.
[[nodiscard]] QString InterfaceLanguageCode();

// The tdesktop boundary: turn the LanguageId that Ui::TranslateProvider was
// handed back into as complete a code as we can. The region is recovered from,
// in order, the LuminaGram read-language override and the interface language -
// each only when it is the same language, so that a target of English never
// picks up the region of a Chinese override. Falls back to the bare two-letter
// code, which is exactly what tdesktop would have used anyway.
[[nodiscard]] QString TargetLanguageCode(LanguageId id);

// Per-provider dialect spellings.
[[nodiscard]] QString GoogleLanguageCode(const QString &code);
[[nodiscard]] QString DeepLLanguageCode(const QString &code);

// The English language name an LLM prompt reads best with, so that a zh-TW
// target asks for "Traditional Chinese" rather than for "Chinese".
[[nodiscard]] QString LanguageEnglishName(const QString &code);

enum class TranslateError : uchar {
	None,
	Unavailable, // No provider could run the request at all.
	NoKey,       // The provider needs an API key that is not set.
	RateLimited, // HTTP 429 / 456 / 403, or a Telegram quota error.
	Network,     // Transport failure, timeout, or any other HTTP error.
	BadResponse, // The provider answered with something unparseable.
};

struct TranslateResult {
	QString text;
	QString detectedFrom; // Empty when the provider does not report it.
	TranslateError error = TranslateError::None;

	[[nodiscard]] bool failed() const {
		return (error != TranslateError::None) || text.isEmpty();
	}
};

// The LuminaGram-native translation interface.
//
// Lifetime contract, which is the same one tdesktop's own
// Ui::CreateUrlTranslateProvider() has, because it is the same mechanism:
// destroying an engine cancels every request it still has in flight, and their
// callbacks then never run. That is what makes it safe to hold an engine as a
// member of something shorter-lived than the session.
//
// The other side of that: `done` runs inside the engine's own network reply,
// so `done` must not destroy the engine. Drop it from a later main-thread turn
// if the result is what tells you the engine is no longer needed - or use
// TranslateText() below, which does exactly that for you.
//
// `done` is invoked exactly once, on the main thread. `toCode` is a language
// code, not a LanguageId; pass TargetLanguageCode(id) when all you have is a
// LanguageId.
class TranslateEngine {
public:
	virtual ~TranslateEngine() = default;

	[[nodiscard]] virtual QString id() const = 0;
	virtual void translate(
		const QString &text,
		const QString &toCode,
		Fn<void(TranslateResult)> done) = 0;

};

// Null for an unknown id, and for TelegramProviderId() without a session.
[[nodiscard]] std::unique_ptr<TranslateEngine> MakeTranslateEngine(
	const QString &providerId,
	Main::Session *session);

// The selected provider, wrapped in the Telegram fallback when that is on and
// a session is available. Null when the selected provider cannot be built.
[[nodiscard]] std::unique_ptr<TranslateEngine> MakeCurrentTranslateEngine(
	Main::Session *session);

// One-shot translation for call sites that have nowhere to keep an engine.
// Owns an engine for the duration of the request and releases it safely from a
// later main-thread turn, so `done` may do anything it likes.
//
// Note that this cannot be cancelled: `done` runs whatever happened to the
// caller in the meantime, so guard it (crl::guard, base::weak_ptr) exactly as
// you would an api().request() callback. Prefer holding a
// MakeCurrentTranslateEngine() result when a call site translates repeatedly -
// this builds a fresh engine, and therefore a fresh network stack, per call.
void TranslateText(
	Main::Session *session,
	const QString &text,
	const QString &toCode,
	Fn<void(TranslateResult)> done);

// The tdesktop routing hook, called from Ui::CreateTranslateProvider().
//
// Returns null when the user selected Telegram's own engine, which leaves
// tdesktop on exactly the path it uses today.
//
// The returned provider reports supportsMessageId() == false, and it has to:
// Ui::PrepareTranslateProviderRequest() only fills TranslateProviderRequest
// with the message text for a provider in that weaker tier. Reporting true
// would leave the request carrying nothing but a message id, which our engines
// cannot use and which the Telegram fallback would then be handed with no text
// in it. It also correctly disables the instant-view rich translation path,
// which is server-side only.
[[nodiscard]] std::unique_ptr<Ui::TranslateProvider> CreateTranslateProvider(
	not_null<Main::Session*> session);

} // namespace Lumina
