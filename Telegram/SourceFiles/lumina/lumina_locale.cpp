/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_locale.h"

#include "core/application.h"
#include "lang/lang_instance.h"
#include "lang/lang_keys.h"

#include <vector>

namespace Lumina {
namespace {

// Spelled as calls rather than as namespace-scope constants: a QString built
// during static initialisation would not be there yet for a caller that is
// itself running from static initialisation.
[[nodiscard]] QString EnglishCode() {
	return u"en"_q;
}

[[nodiscard]] QString ArgumentPlaceholder() {
	return u"{1}"_q;
}

struct Registration {
	const char *code = nullptr;
	LocaleTableFactory factory = nullptr;
};

// Function-local statics only, on purpose: RegisterLocaleTable() runs during
// the dynamic initialisation of whichever language file was linked in, in an
// order nothing controls, and a namespace-scope container might not be
// constructed yet when the first registrar runs.
[[nodiscard]] std::vector<Registration> &Registrations() {
	static auto result = std::vector<Registration>();
	return result;
}

[[nodiscard]] base::flat_map<QString, LocaleTable> &BuiltTables() {
	static auto result = base::flat_map<QString, LocaleTable>();
	return result;
}

// The English table, and with it the list of every string LuminaGram adds.
// Adding a row here is what makes a key exist; a language file that does not
// carry the key falls back to the text below.
[[nodiscard]] const LocaleTable &EnglishTable() {
	static const auto result = LocaleTable{
		// Sections of the LuminaGram settings page.
		{ u"LuminaGramTitle"_q, u"LuminaGram"_q },
		{ u"LuminaTranslateTitle"_q, u"Translation"_q },
		{ u"LuminaPrivacyTitle"_q, u"Privacy"_q },
		{ u"LuminaSecurityTitle"_q, u"Security"_q },
		{ u"LuminaChatSettings"_q, u"Chats"_q },
		{ u"LuminaGramChatList"_q, u"Chat list"_q },
		{ u"LuminaAppearanceTitle"_q, u"Appearance"_q },
		{ u"LuminaToolsTitle"_q, u"Tools"_q },
		{ u"LuminaGramStoredLocallyInfo"_q, u"LuminaGram options are stored "
			"on this device only and are never synced to Telegram."_q },

		// Sub-pages that have no rows yet.
		{ u"LuminaAppearancePlaceholder"_q, u"Message, sticker and number "
			"formatting options will appear here."_q },
		{ u"LuminaChatListPlaceholder"_q, u"Chat list layout and badge "
			"options will appear here."_q },
		{ u"LuminaPrivacyPlaceholder"_q, u"Link, clipboard and "
			"outgoing-media privacy options will appear here."_q },
		{ u"LuminaSecurityPlaceholder"_q, u"Disguise vault, panic wipe and "
			"duress unlock options will appear here."_q },
		{ u"LuminaToolsPlaceholder"_q, u"Bookmarks, reply templates and "
			"local backup will appear here."_q },

		// Chats sub-page.
		{ u"LuminaMessageActions"_q, u"Message actions"_q },
		{ u"LuminaAllowSaveRestricted"_q,
			u"Allow save / copy from restricted chats"_q },
		{ u"LuminaAllowSaveRestrictedInfo"_q, u"\"Allow save / copy from "
			"restricted chats\" only affects local actions on this device. "
			"Some chats restrict saving for a reason - use responsibly."_q },

		// Translation sub-page.
		{ u"LuminaTranslateEnable"_q, u"Enable LuminaGram translation"_q },
		{ u"LuminaTranslateEnableInfo"_q, u"Translate with your own engine "
			"instead of Telegram's Premium service. The default engine needs "
			"no account and no API key. While this is off, LuminaGram leaves "
			"translation exactly as Telegram Desktop ships it."_q },
		{ u"LuminaTranslateSendHeader"_q, u"Sending"_q },
		{ u"LuminaTranslateBeforeSend"_q, u"Translate before sending"_q },
		{ u"LuminaTranslateSendLang"_q, u"Send in"_q },
		{ u"LuminaTranslateSendLangAuto"_q, u"Recipient's language"_q },
		{ u"LuminaTranslateBeforeSendConfirm"_q, u"Confirm before sending"_q },
		{ u"LuminaTranslateSendInfo"_q, u"Outgoing messages are translated "
			"into the language above, and the original is kept alongside the "
			"translation. With \"Recipient's language\" LuminaGram asks once "
			"per chat which language to use there, then remembers it. "
			"\"Confirm before sending\" shows the translation next to the "
			"original first, so you can send either one; with it off the "
			"translation goes out straight away."_q },
		{ u"LuminaTranslateReceiveHeader"_q, u"Receiving"_q },
		{ u"LuminaDualLanguageDisplay"_q,
			u"Show original and translation together"_q },
		{ u"LuminaTranslateReadLang"_q, u"Read in"_q },
		{ u"LuminaTranslateReadLangFollow"_q, u"Interface language"_q },
		{ u"LuminaTranslateModeHeader"_q, u"Translate incoming messages"_q },
		{ u"LuminaTranslateModeAll"_q, u"In every chat"_q },
		{ u"LuminaTranslateModeManual"_q, u"Only chats I turn on"_q },
		{ u"LuminaTranslateReceiveInfo"_q, u"Incoming messages keep their "
			"original text in full size, with the translation shown "
			"underneath. \"In every chat\" sends one request per message to "
			"your translation service - on a metered key, leave it on \"Only "
			"chats I turn on\"."_q },
		{ u"LuminaTranslateScopeHeader"_q, u"Where it applies"_q },
		{ u"LuminaTranslateScopePrivate"_q, u"Private chats"_q },
		{ u"LuminaTranslateScopeGroup"_q, u"Groups and channels"_q },
		{ u"LuminaTranslateScopeInfo"_q, u"Chats outside the scope are left "
			"alone in both directions: they are not translated as you read "
			"them, and messages you send there go out untranslated. You can "
			"still translate any single message by hand."_q },
		{ u"LuminaTranslateProviderHeader"_q, u"Service"_q },
		{ u"LuminaTranslateProvider"_q, u"Translation service"_q },
		// The other provider names are the services' own brands and are not
		// translated; this one names a kind of endpoint.
		{ u"LuminaTranslateProviderLlm"_q, u"LLM (OpenAI-compatible)"_q },
		{ u"LuminaTranslateApiKey"_q, u"API key"_q },
		{ u"LuminaTranslateApiKeyNotSet"_q, u"Not set"_q },
		{ u"LuminaTranslateBaseUrl"_q, u"Base URL"_q },
		{ u"LuminaTranslateModel"_q, u"Model"_q },
		{ u"LuminaTranslateSystemPrompt"_q, u"System prompt"_q },
		{ u"LuminaTranslatePromptDefault"_q, u"Default"_q },
		{ u"LuminaTranslatePromptCustom"_q, u"Custom"_q },
		{ u"LuminaTranslateFallbackTelegram"_q,
			u"Fall back to Telegram when this service fails"_q },
		{ u"LuminaTranslateProviderSecurityInfo"_q, u"Keys are kept on this "
			"device only, in a separate file from the rest of the settings, "
			"and are never sent to Telegram. Everything you translate is sent "
			"to the service selected here, so pick one you trust."_q },

		// Translate before sending: the boxes and the send menu.
		{ u"LuminaTranslateOriginalLabel"_q, u"Original"_q },
		{ u"LuminaSendTranslation"_q, u"Send translation"_q },
		{ u"LuminaSendOriginal"_q, u"Send original"_q },
		{ u"LuminaTrSendPickerTitle"_q, u"Translate messages into"_q },
		{ u"LuminaTrSendConfirmMessage"_q, u"This chat looks like it is "
			"written in {1}. Translate the messages you send here into {1}? "
			"LuminaGram will remember this for this chat."_q },
		{ u"LuminaTrSendConfirmTranslate"_q, u"Translate"_q },
		{ u"LuminaTrSendAsTyped"_q, u"Send as typed"_q },
		{ u"LuminaTrSendChooseOther"_q, u"Choose language"_q },

		// The translate-before-send preview bar above the composer.
		{ u"LuminaTranslatePreviewTranslating"_q, u"Translating..."_q },
		{ u"LuminaTranslatePreviewFailed"_q, u"Translation unavailable"_q },

		// Names of the languages LuminaGram can translate into, in the order
		// the pickers show them. These are the ones Telegram's own language
		// list cannot serve, because it names a QLocale::Language and so
		// cannot tell zh-TW from zh-CN.
		{ u"LuminaLangEn"_q, u"English"_q },
		{ u"LuminaLangZhTw"_q, u"Chinese (Traditional)"_q },
		{ u"LuminaLangZhCn"_q, u"Chinese (Simplified)"_q },
		{ u"LuminaLangJa"_q, u"Japanese"_q },
		{ u"LuminaLangKo"_q, u"Korean"_q },
		{ u"LuminaLangEs"_q, u"Spanish"_q },
		{ u"LuminaLangFr"_q, u"French"_q },
		{ u"LuminaLangDe"_q, u"German"_q },
		{ u"LuminaLangRu"_q, u"Russian"_q },
		{ u"LuminaLangPtBr"_q, u"Portuguese (Brazil)"_q },
		{ u"LuminaLangPtPt"_q, u"Portuguese (Portugal)"_q },
		{ u"LuminaLangIt"_q, u"Italian"_q },
		{ u"LuminaLangAr"_q, u"Arabic"_q },
		{ u"LuminaLangHi"_q, u"Hindi"_q },
		{ u"LuminaLangId"_q, u"Indonesian"_q },
		{ u"LuminaLangTh"_q, u"Thai"_q },
		{ u"LuminaLangVi"_q, u"Vietnamese"_q },
		{ u"LuminaLangTr"_q, u"Turkish"_q },
		{ u"LuminaLangPl"_q, u"Polish"_q },
		{ u"LuminaLangUk"_q, u"Ukrainian"_q },
		{ u"LuminaLangNl"_q, u"Dutch"_q },
		{ u"LuminaLangAf"_q, u"Afrikaans"_q },
		{ u"LuminaLangSq"_q, u"Albanian"_q },
		{ u"LuminaLangAm"_q, u"Amharic"_q },
		{ u"LuminaLangHy"_q, u"Armenian"_q },
		{ u"LuminaLangAz"_q, u"Azerbaijani"_q },
		{ u"LuminaLangEu"_q, u"Basque"_q },
		{ u"LuminaLangBe"_q, u"Belarusian"_q },
		{ u"LuminaLangBn"_q, u"Bengali"_q },
		{ u"LuminaLangBs"_q, u"Bosnian"_q },
		{ u"LuminaLangBg"_q, u"Bulgarian"_q },
		{ u"LuminaLangMy"_q, u"Burmese"_q },
		{ u"LuminaLangCa"_q, u"Catalan"_q },
		{ u"LuminaLangHr"_q, u"Croatian"_q },
		{ u"LuminaLangCs"_q, u"Czech"_q },
		{ u"LuminaLangDa"_q, u"Danish"_q },
		{ u"LuminaLangEt"_q, u"Estonian"_q },
		{ u"LuminaLangTl"_q, u"Filipino"_q },
		{ u"LuminaLangFi"_q, u"Finnish"_q },
		{ u"LuminaLangGl"_q, u"Galician"_q },
		{ u"LuminaLangKa"_q, u"Georgian"_q },
		{ u"LuminaLangEl"_q, u"Greek"_q },
		{ u"LuminaLangGu"_q, u"Gujarati"_q },
		{ u"LuminaLangHe"_q, u"Hebrew"_q },
		{ u"LuminaLangHu"_q, u"Hungarian"_q },
		{ u"LuminaLangIs"_q, u"Icelandic"_q },
		{ u"LuminaLangGa"_q, u"Irish"_q },
		{ u"LuminaLangJv"_q, u"Javanese"_q },
		{ u"LuminaLangKn"_q, u"Kannada"_q },
		{ u"LuminaLangKk"_q, u"Kazakh"_q },
		{ u"LuminaLangKm"_q, u"Khmer"_q },
		{ u"LuminaLangKu"_q, u"Kurdish"_q },
		{ u"LuminaLangKy"_q, u"Kyrgyz"_q },
		{ u"LuminaLangLo"_q, u"Lao"_q },
		{ u"LuminaLangLv"_q, u"Latvian"_q },
		{ u"LuminaLangLt"_q, u"Lithuanian"_q },
		{ u"LuminaLangMk"_q, u"Macedonian"_q },
		{ u"LuminaLangMs"_q, u"Malay"_q },
		{ u"LuminaLangMl"_q, u"Malayalam"_q },
		{ u"LuminaLangMr"_q, u"Marathi"_q },
		{ u"LuminaLangMn"_q, u"Mongolian"_q },
		{ u"LuminaLangNe"_q, u"Nepali"_q },
		{ u"LuminaLangNo"_q, u"Norwegian"_q },
		{ u"LuminaLangPs"_q, u"Pashto"_q },
		{ u"LuminaLangFa"_q, u"Persian"_q },
		{ u"LuminaLangPa"_q, u"Punjabi"_q },
		{ u"LuminaLangRo"_q, u"Romanian"_q },
		{ u"LuminaLangSr"_q, u"Serbian"_q },
		{ u"LuminaLangSi"_q, u"Sinhala"_q },
		{ u"LuminaLangSk"_q, u"Slovak"_q },
		{ u"LuminaLangSl"_q, u"Slovenian"_q },
		{ u"LuminaLangSo"_q, u"Somali"_q },
		{ u"LuminaLangSw"_q, u"Swahili"_q },
		{ u"LuminaLangSv"_q, u"Swedish"_q },
		{ u"LuminaLangTg"_q, u"Tajik"_q },
		{ u"LuminaLangTa"_q, u"Tamil"_q },
		{ u"LuminaLangTe"_q, u"Telugu"_q },
		{ u"LuminaLangUr"_q, u"Urdu"_q },
		{ u"LuminaLangUz"_q, u"Uzbek"_q },
		{ u"LuminaLangCy"_q, u"Welsh"_q },
		{ u"LuminaLangYi"_q, u"Yiddish"_q },
		{ u"LuminaLangZu"_q, u"Zulu"_q },
	};
	return result;
}

// The table for `code`, built on first use, or nullptr when this build has no
// table for it - which is the normal answer for English and for every language
// nobody has translated yet.
[[nodiscard]] const LocaleTable *TableFor(const QString &code) {
	if (code.isEmpty() || code == EnglishCode()) {
		return nullptr;
	}
	auto &built = BuiltTables();
	const auto i = built.find(code);
	if (i != built.end()) {
		return &i->second;
	}
	for (const auto &registration : Registrations()) {
		if (!registration.factory
			|| (code != QLatin1String(registration.code))) {
			continue;
		}
		built.emplace(code, registration.factory());
		const auto j = built.find(code);
		return (j != built.end()) ? &j->second : nullptr;
	}
	return nullptr;
}

[[nodiscard]] QString EnglishText(const QString &key) {
	const auto &table = EnglishTable();
	const auto i = table.find(key);
	return (i != table.end()) ? i->second : QString();
}

} // namespace

bool RegisterLocaleTable(
		const char *languageCode,
		LocaleTableFactory factory) {
	if (languageCode && *languageCode && factory) {
		Registrations().push_back({ languageCode, factory });
	}
	return true;
}

QString LocaleCode() {
	// Lang::Id() reaches the language pack through Core::App(), so an early
	// caller - anything running from dynamic initialisation - has to be told
	// English rather than allowed to build the application singleton.
	if (!Core::IsAppLaunched()) {
		return EnglishCode();
	}
	const auto id = Lang::Id().toLower();
	const auto base = Lang::GetInstance().baseId().toLower();

	// The base id is what an unofficial pack reports its language through, and
	// a Telegram language id is not always a plain BCP-47 tag: it can carry a
	// vendor prefix ("classic-zh-tw") or a pack suffix ("zh-hant-raw"). Both
	// halves are searched together, exactly as Android's LuminaLocale does.
	const auto probe = id + QChar('|') + base;
	if (probe.contains(u"zh"_q)) {
		return (probe.contains(u"hant"_q)
			|| probe.contains(u"tw"_q)
			|| probe.contains(u"hk"_q)
			|| probe.contains(u"mo"_q)
			|| probe.contains(u"traditional"_q))
			? u"zh-hant"_q
			: u"zh-hans"_q;
	} else if (id.startsWith(u"pt"_q)) {
		return u"pt-br"_q;
	}
	const auto shortened = id.left(2);
	if (shortened.isEmpty()) {
		return EnglishCode();
	}
	return (shortened == u"in"_q) ? u"id"_q : shortened;
}

QString Tr(const QString &key) {
	if (const auto table = TableFor(LocaleCode())) {
		const auto i = table->find(key);
		if (i != table->end() && !i->second.isEmpty()) {
			return i->second;
		}
	}
	return EnglishText(key);
}

QString Tr(const QString &key, const QString &arg1) {
	return Tr(key).replace(ArgumentPlaceholder(), arg1);
}

rpl::producer<> LangChanges() {
	// Two different events, because they mean two different things: the id
	// changes when another language is selected, and updated() fires when the
	// values of the current one are replaced (a custom .strings file, the test
	// languages). Either one can leave our text stale.
	if (!Core::IsAppLaunched()) {
		return rpl::never<>();
	}
	return rpl::merge(
		Lang::Updated(),
		Lang::GetInstance().idChanges() | rpl::to_empty);
}

rpl::producer<QString> TrValue(const QString &key) {
	return rpl::single(
		rpl::empty
	) | rpl::then(
		LangChanges()
	) | rpl::map([key] {
		return Tr(key);
	});
}

} // namespace Lumina
