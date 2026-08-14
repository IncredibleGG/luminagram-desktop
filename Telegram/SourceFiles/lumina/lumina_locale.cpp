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
		{ u"LuminaTranslateSendInfo"_q, u"This switch only makes the feature "
			"available. Nothing is translated until you turn it on for a chat: "
			"hold its send button and switch on \"Translate before sending\" "
			"there. Outgoing messages are then translated into the language "
			"above, and the original is kept alongside the translation. With "
			"\"Recipient's language\" LuminaGram asks once per chat which "
			"language to use there, then remembers it. \"Confirm before "
			"sending\" shows the translation next to the original first, so you "
			"can send either one; with it off the translation goes out straight "
			"away."_q },
		{ u"LuminaTranslateReceiveHeader"_q, u"Receiving"_q },
		{ u"LuminaDualLanguageDisplay"_q,
			u"Show original and translation together"_q },
		{ u"LuminaFoldOriginalLongMessages"_q,
			u"Fold original in long messages"_q },
		{ u"LuminaFoldOriginalLongMessagesInfo"_q,
			u"In a long bilingual message the original folds down to a "
			"single line so it does not flood the chat; tap it to expand. "
			"The translation is always shown in full."_q },
		{ u"LuminaExpandOriginal"_q, u"Show original"_q },
		{ u"LuminaTranslateReadLang"_q, u"Read in"_q },
		{ u"LuminaTranslateReadLangFollow"_q, u"Interface language"_q },
		{ u"LuminaTranslateReceiveInfo"_q, u"With both languages shown, an "
			"incoming message keeps its original text at full size and its "
			"translation appears underneath; otherwise you see the "
			"translation on its own. Choose which chats are translated in "
			"each chat, from the translate button in its title bar."_q },
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

		// The provider test. Every failure has to name the thing to go and
		// change: "it doesn't translate" covers a service that was never
		// reachable, a key that was never saved, a key the service turned
		// down and a quota that ran out, and those are four different
		// afternoons.
		{ u"LuminaTranslateTest"_q, u"Test translation"_q },
		{ u"LuminaTranslateTestRunning"_q, u"Testing..."_q },
		{ u"LuminaTranslateTestSuccess"_q, u"Translation works."_q },
		{ u"LuminaTranslateTestFailed"_q, u"Test failed"_q },
		{ u"LuminaTranslateNoKey"_q, u"API key required"_q },
		{ u"LuminaTranslateTestKeyRejected"_q, u"The service could not be "
			"reached, or it turned this API key down. Check the key first, "
			"then the base URL and your connection."_q },
		{ u"LuminaTranslateTestNetwork"_q, u"The service could not be "
			"reached. Check your internet connection and any proxy, then try "
			"again."_q },
		{ u"LuminaTranslateTestNoChange"_q, u"The service returned the sample unchanged, so nothing was translated. Check the key, the model and the system prompt."_q },
		{ u"LuminaTranslateTestQuota"_q, u"The service refused the request - "
			"too many requests, or this key's quota is used up. Try again "
			"later."_q },
		{ u"LuminaTranslateTestQuotaKeyed"_q, u"The service refused the "
			"request - it turned this API key down, the key's quota is used "
			"up, or there were too many requests. Check the key first, then "
			"try again later."_q },
		{ u"LuminaTranslateTestBadResponse"_q, u"The service answered with "
			"something LuminaGram could not read, or with nothing at all. "
			"Check the base URL and the model."_q },
		{ u"LuminaTranslateTestUnavailable"_q, u"This service cannot run "
			"here. Choose another one above."_q },

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

		// The per-chat translate button in the chat top bar. Both texts name
		// what pressing the button will do, not what the chat is doing now,
		// because that is what a screen reader reads out before the press.
		{ u"LuminaTranslateChatToggle"_q, u"Translate this chat"_q },
		{ u"LuminaTranslateChatShowOriginal"_q, u"Show original"_q },

		// Pressing that button: the one place that states this conversation's
		// language pair, and the shortcut to the two rows on the translation
		// settings page that hold it. Both halves name their side first -
		// "them" and "me" rather than "incoming" and "outgoing" - because the
		// complaint these strings answer is that the app never said whose
		// messages were meant, and a direction word does not say it.
		//
		// The language at the end is the whole content of each row, so it is
		// always a language a reader recognises: never a code, and never an
		// empty value standing in for one.
		// One string per side per state rather than substituting the "not
		// translated" entry into {1}: that reads as "translated into not
		// translated" in English and breaks outright in languages that place
		// the verb elsewhere.
		{ u"LuminaChatLangThem"_q, u"Them, translated into {1}"_q },
		{ u"LuminaChatLangThemOff"_q, u"Them, not translated"_q },
		{ u"LuminaChatLangMe"_q, u"Me, translated into {1}"_q },
		{ u"LuminaChatLangMeOff"_q, u"Me, not translated"_q },
		{ u"LuminaChatLangThemTitle"_q, u"Their messages, translated into"_q },
		{ u"LuminaChatLangMeTitle"_q, u"My messages, translated into"_q },
		{ u"LuminaChatLangNone"_q, u"Not translated"_q },

		// The third row of that menu, and the box it opens: who this person is
		// to you, and therefore how a translation of this chat should sound.
		// The relationship is the one thing neither language can state, and it
		// is what a translation that is otherwise correct gets wrong first.
		//
		// Every entry carries a sentence of its own, because the names alone
		// do not separate them: "Client" and "Colleague" differ only in what
		// they do to the translation. The last two strings are the caveats -
		// what the selected engine can actually honour, said out loud rather
		// than left to look like it worked.
		//
		// The instruction the model is actually sent is NOT here. It is
		// English on purpose, it is read by the model rather than by the user,
		// and it lives in lumina/lumina_register.cpp.
		{ u"LuminaChatRegister"_q, u"Tone: {1}"_q },
		{ u"LuminaChatRegisterOff"_q, u"Tone: not set"_q },
		{ u"LuminaChatRegisterTitle"_q, u"Tone for this chat"_q },
		{ u"LuminaChatRegisterNone"_q, u"Not set"_q },
		{ u"LuminaChatRegisterNoneInfo"_q, u"Translate as usual, with no tone "
			"instruction."_q },
		{ u"LuminaChatRegisterClient"_q, u"Client"_q },
		{ u"LuminaChatRegisterClientInfo"_q, u"Polite, professional business "
			"language; formal honorifics where the language has them."_q },
		{ u"LuminaChatRegisterColleague"_q, u"Colleague"_q },
		{ u"LuminaChatRegisterColleagueInfo"_q, u"Everyday polite workplace "
			"language — courteous, but not stiff."_q },
		{ u"LuminaChatRegisterFriend"_q, u"Friend"_q },
		{ u"LuminaChatRegisterFriendInfo"_q, u"Casual, informal speech, the "
			"way you talk to a friend."_q },
		{ u"LuminaChatRegisterFamily"_q, u"Family"_q },
		{ u"LuminaChatRegisterFamilyInfo"_q, u"Warm, familiar language of the "
			"kind used at home."_q },
		{ u"LuminaChatRegisterElder"_q, u"Elder"_q },
		{ u"LuminaChatRegisterElderInfo"_q, u"Respectful and deferential, but "
			"still warm rather than corporate."_q },
		{ u"LuminaChatRegisterRomance"_q, u"Someone special"_q },
		{ u"LuminaChatRegisterRomanceInfo"_q, u"Warm and playful, with a light "
			"flirtatious touch."_q },
		{ u"LuminaChatRegisterCustom"_q, u"Custom…"_q },
		{ u"LuminaChatRegisterCustomInfo"_q, u"Describe the relationship in "
			"your own words."_q },
		{ u"LuminaChatRegisterCustomTitle"_q, u"Describe this relationship"_q },
		{ u"LuminaChatRegisterCustomHint"_q, u"e.g. my thesis advisor — "
			"respectful but not stiff"_q },
		{ u"LuminaChatRegisterUnsupported"_q, u"The current translation engine "
			"ignores tone"_q },
		{ u"LuminaChatRegisterDeepL"_q, u"DeepL: formal/informal only, some "
			"languages"_q },

		// The tray menu and the taskbar button's jump list. Upstream builds
		// both from lng_open_from_tray / lng_quit_from_tray, whose text is
		// "Open Telegram" and "Quit Telegram" in every language Telegram
		// serves - so the one menu a user reaches by right-clicking our
		// taskbar button named someone else's product.
		{ u"LuminaTrayOpen"_q, u"Open {1}"_q },
		{ u"LuminaTrayQuit"_q, u"Quit {1}"_q },

		// Telegram's own AI editor, which carries a Translate tab of its own.
		// These were reached through TrOrValue() with the English written at
		// the call site, so the settings page they build was the one section
		// of LuminaGram that stayed English in every other language.
		{ u"LuminaAiEditorHeader"_q, u"Telegram AI editor"_q },
		{ u"LuminaAiEditorKeep"_q, u"Keep Telegram's AI editor"_q },
		{ u"LuminaAiEditorInfo"_q, u"Telegram has an AI editor of its own in "
			"the composer, with a Translate tab that overlaps LuminaGram's "
			"translation. While LuminaGram translation is on, its button and "
			"its keyboard shortcut are not offered, so there is only ever one "
			"translation tool in front of you. Turn this on to keep "
			"Telegram's editor available anyway. With LuminaGram translation "
			"off, Telegram's editor is always there and this setting changes "
			"nothing."_q },

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
		{ u"LuminaAppearanceStickerSizeHeader"_q, u"Sticker size"_q },
		{ u"LuminaBackupCryptoFailed"_q, u"Encryption is not available on this system."_q },
		{ u"LuminaBackupDamaged"_q, u"The backup file is damaged and could not be restored."_q },
		{ u"LuminaBackupExport"_q, u"Export encrypted backup"_q },
		{ u"LuminaBackupExportDone"_q, u"Backup saved."_q },
		{ u"LuminaBackupExportFailed"_q, u"Could not create the backup file."_q },
		{ u"LuminaBackupExportInfo"_q, u"Save everything LuminaGram keeps on this device - bookmarks, notes, reply templates, text replacements and every setting - into one file, encrypted with a passphrase you choose. The file also carries the private values: translation API keys, the vault and fake-crash codes, the decoy note. Nothing is sent to Telegram. Pick a long passphrase and keep it somewhere safe - without it the file cannot be opened, and there is no way to recover it."_q },
		{ u"LuminaBackupExportPassphraseTitle"_q, u"Set a passphrase"_q },
		{ u"LuminaBackupFileFilter"_q, u"LuminaGram backup (*.lgbak)"_q },
		{ u"LuminaBackupImport"_q, u"Import backup"_q },
		{ u"LuminaBackupImportFailed"_q, u"Could not read the backup file."_q },
		{ u"LuminaBackupImportInfo"_q, u"Pick a backup file and enter its passphrase to restore your LuminaGram data. Settings the backup carries replace the ones on this device; anything it does not carry is left alone. A file that cannot be verified is refused before anything is written, so a wrong passphrase can never leave you half-restored."_q },
		{ u"LuminaBackupImportSuccess"_q, u"Backup restored. Restart LuminaGram to apply everything."_q },
		{ u"LuminaBackupInvalidFile"_q, u"This is not a valid LuminaGram backup file."_q },
		{ u"LuminaBackupNewerFormat"_q, u"This backup was made by a newer version of LuminaGram."_q },
		{ u"LuminaBackupOpenCaption"_q, u"Open LuminaGram backup"_q },
		{ u"LuminaBackupPassphraseHint"_q, u"Passphrase"_q },
		{ u"LuminaBackupPassphraseMismatch"_q, u"The two passphrases are different."_q },
		{ u"LuminaBackupPassphraseRepeatHint"_q, u"Repeat passphrase"_q },
		{ u"LuminaBackupPassphraseTitle"_q, u"Enter passphrase"_q },
		{ u"LuminaBackupPassphraseTooShort"_q, u"Choose a passphrase of at least 4 characters."_q },
		{ u"LuminaBackupSaveCaption"_q, u"Save LuminaGram backup"_q },
		{ u"LuminaBackupTitle"_q, u"Encrypted backup"_q },
		{ u"LuminaBackupUnauthenticated"_q, u"This backup uses the old unprotected format, which cannot be checked for a wrong passphrase or for tampering. Make a new backup from an up-to-date LuminaGram."_q },
		{ u"LuminaBackupWrongPassphrase"_q, u"Wrong passphrase, or the file has been changed."_q },
		{ u"LuminaBookmark"_q, u"Bookmark"_q },
		{ u"LuminaBookmarkAdded"_q, u"Added to bookmarks"_q },
		{ u"LuminaBookmarkChatUnavailable"_q, u"Chat unavailable"_q },
		{ u"LuminaBookmarkDeleteTitle"_q, u"Delete bookmark?"_q },
		{ u"LuminaBookmarkGone"_q, u"This chat is no longer available on this device."_q },
		{ u"LuminaBookmarkRemove"_q, u"Remove bookmark"_q },
		{ u"LuminaBookmarkRemoved"_q, u"Removed from bookmarks"_q },
		{ u"LuminaBookmarksAbout"_q, u"Bookmarks are pointers to messages, kept only on this device and never sent to Telegram. Removing a bookmark leaves the message itself untouched."_q },
		{ u"LuminaBookmarksEmpty"_q, u"You haven't bookmarked any messages yet."_q },
		{ u"LuminaBookmarksFull"_q, u"The bookmark list is full. Remove a bookmark to add another one."_q },
		{ u"LuminaBookmarksList"_q, u"Bookmarked messages"_q },
		{ u"LuminaBookmarksListAbout"_q, u"Click a bookmark to open the message, or right-click it to remove it. A bookmark stays in this list even after its message is deleted."_q },
		{ u"LuminaBookmarksNone"_q, u"None"_q },
		{ u"LuminaBookmarksTitle"_q, u"Bookmarks"_q },
		{ u"LuminaChatListDensityTitle"_q, u"Chat list density"_q },
		{ u"LuminaCompactListRows"_q, u"Compact rows"_q },
		{ u"LuminaCompactListRowsInfo"_q, u"Fit more chats on the screen by shortening every chat-list row. Avatars and message previews stay visible."_q },
		{ u"LuminaChatListDotsTitle"_q, u"Avatar dots"_q },
		{ u"LuminaChatListOnlineDot"_q, u"Online dot"_q },
		{ u"LuminaChatListOnlineDotInfo"_q, u"Shows a small green dot on the avatar of one-on-one chats whose contact is currently online."_q },
		{ u"LuminaChatListRecencyDot"_q, u"Last-seen recency dot"_q },
		{ u"LuminaChatListRecencyDotInfo"_q, u"Colors the avatar dot in one-on-one chats by how recently the contact was last seen: green when online now, yellow within an hour, orange within a day. No dot for older or hidden last-seen. The online dot toggle still controls the green (online) case."_q },
		{ u"LuminaChatListVisibilityAbout"_q, u"Take the folder bar out of the chat list — both the vertical bar beside it and the horizontal one above it. While folder tabs are hidden you always see all of your chats: any folder you were inside is left, and the folder shortcuts and the swipe between folders do nothing. Hiding stories only removes the row above the chat list; stories themselves, and every other way of opening them, are unchanged."_q },
		{ u"LuminaChatListVisibilityTitle"_q, u"Folders and stories"_q },
		{ u"LuminaContactNote"_q, u"Private note"_q },
		{ u"LuminaContactNoteAbout"_q, u"Only you can see this. It stays on this device, is never sent to Telegram and does not sync to your other devices."_q },
		{ u"LuminaContactNoteEmpty"_q, u"Click to add a private note"_q },
		{ u"LuminaContactNoteHint"_q, u"Note (only you can see this)"_q },
		{ u"LuminaContactNotesAbout"_q, u"Adds a private note and a list of tags to a person's profile. Both stay on this device, are never sent to Telegram and do not sync to your other devices. Telegram's own contact notes are left alone: the private note is offered where they are not available - on bots, and on people who are not in your contacts - while tags are offered everywhere."_q },
		{ u"LuminaContactNotesClear"_q, u"Delete all private notes"_q },
		{ u"LuminaContactNotesClearText"_q, u"Delete every private note and tag stored on this device? This cannot be undone."_q },
		{ u"LuminaContactNotesClearTitle"_q, u"Delete private notes"_q },
		{ u"LuminaContactNotesNone"_q, u"None"_q },
		{ u"LuminaContactNotesTitle"_q, u"Private contact notes"_q },
		{ u"LuminaContactNotesToggle"_q, u"Private notes and tags on profiles"_q },
		{ u"LuminaContactTags"_q, u"Tags"_q },
		{ u"LuminaContactTagsEmpty"_q, u"Click to add tags"_q },
		{ u"LuminaContactTagsHint"_q, u"Tags, comma separated"_q },
		{ u"LuminaDetailsDate"_q, u"Date"_q },
		{ u"LuminaDetailsForwardedFrom"_q, u"Forwarded from"_q },
		{ u"LuminaDetailsFrom"_q, u"From"_q },
		{ u"LuminaDetailsMessageId"_q, u"Message ID"_q },
		{ u"LuminaDetailsOriginalDate"_q, u"Original date"_q },
		{ u"LuminaExactNumbers"_q, u"Show exact numbers (no 1.2K rounding)"_q },
		{ u"LuminaExactNumbersInfo"_q, u"Show full counts like 1,234,567 instead of shortened forms such as 1.2M. Counters already drawn on screen keep their current form until the app is restarted."_q },
		{ u"LuminaForwardNoAuthor"_q, u"Forward without author"_q },
		{ u"LuminaForwardNoAuthorTitle"_q, u"Forward without author"_q },
		{ u"LuminaForwardNoCaption"_q, u"Forward without author and captions"_q },
		{ u"LuminaForwardNoCaptionTitle"_q, u"Forward without author and captions"_q },
		{ u"LuminaHideStories"_q, u"Hide stories"_q },
		{ u"LuminaHideTabs"_q, u"Hide folder tabs"_q },
		{ u"LuminaLinkSafetyDestination"_q, u"Real destination"_q },
		{ u"LuminaLinkSafetyInfo"_q, u"Ask before opening a link that hides its real destination behind text before an @ sign, that spells its domain in punycode, or that goes through a known link shortener. The confirmation shows the real destination host and the full address. Telegram Desktop already asks on its own about look-alike domains written in other alphabets. Nothing is checked online — no address you open leaves this device."_q },
		{ u"LuminaLinkSafetyRow"_q, u"Link safety inspector"_q },
		{ u"LuminaLinkSafetyTitle"_q, u"Open external link?"_q },
		{ u"LuminaLinkSafetyWarnMismatch"_q, u"This link hides its real destination behind text before the “@” sign."_q },
		{ u"LuminaLinkSafetyWarnPunycode"_q, u"This address uses encoded (punycode) characters that can imitate a well-known site."_q },
		{ u"LuminaLinkSafetyWarnShortener"_q, u"This is a link shortener — the real destination stays hidden until you open it."_q },
		{ u"LuminaMediaAutoPauseBgVideo"_q, u"Pause video when app goes to background"_q },
		{ u"LuminaMediaAutoPauseBgVideoInfo"_q, u"Automatically pause the playing video when you leave LuminaGram. Minimising the viewer pauses it too. A video you deliberately put in a window of its own, or in picture-in-picture, keeps playing, and music and voice messages keep playing while you are away."_q },
		{ u"LuminaMediaTitle"_q, u"Media"_q },
		{ u"LuminaMessageActionsInfo"_q, u"These entries only appear in a message's context menu. Each one uses Telegram's own forwarding, so a chat that restricts forwarding stays restricted. \"Forward without author and captions\" is named for what actually goes out: Telegram cannot drop captions while keeping the sender, so choosing it drops both. \"Details\" reads only what this device has already synced - nothing is requested and nothing is stored."_q },
		{ u"LuminaMessageDetails"_q, u"Details"_q },
		{ u"LuminaMessageDisplayHeader"_q, u"Message display"_q },
		{ u"LuminaOnboardingDualName"_q, u"Both languages at once"_q },
		{ u"LuminaOnboardingDualText"_q, u"Keep the original text on screen next to its translation, for messages you receive and for the ones you send."_q },
		{ u"LuminaOnboardingFooter"_q, u"Open a section on the LuminaGram settings page to set any of this up. You can read this card again from Tools."_q },
		{ u"LuminaOnboardingGotIt"_q, u"Got it"_q },
		{ u"LuminaOnboardingHeader"_q, u"About"_q },
		{ u"LuminaOnboardingIntro"_q, u"LuminaGram adds its own tools on top of Telegram Desktop. Every option below is stored on this computer only and is never synced to Telegram."_q },
		{ u"LuminaOnboardingRow"_q, u"What LuminaGram adds"_q },
		{ u"LuminaOnboardingRowAbout"_q, u"Shows the welcome card that appears the first time you open LuminaGram settings. It only explains what is here and turns nothing on."_q },
		{ u"LuminaOnboardingSafetyName"_q, u"Safety checks"_q },
		{ u"LuminaOnboardingSafetyText"_q, u"Warn before opening a link whose address is not what it looks like, catch a swapped crypto address when you paste one, and remove the location from photos you send."_q },
		{ u"LuminaOnboardingTitle"_q, u"Welcome to LuminaGram"_q },
		{ u"LuminaOnboardingTranslateText"_q, u"Translate incoming messages with your own translation service, and translate what you type before it is sent."_q },
		{ u"LuminaOnboardingVaultName"_q, u"Disguise vault"_q },
		{ u"LuminaOnboardingVaultText"_q, u"Hide LuminaGram behind a calculator or a notepad that opens the real app only for your secret code."_q },
		{ u"LuminaProfileCardAbout"_q, u"A local card about yourself - what you speak, what you are into - that you can copy and paste into a chat. It stays on this device."_q },
		{ u"LuminaProfileCardBio"_q, u"Short bio"_q },
		{ u"LuminaProfileCardBioHint"_q, u"A few words about yourself"_q },
		{ u"LuminaProfileCardCopied"_q, u"Card copied"_q },
		{ u"LuminaProfileCardCopy"_q, u"Copy to clipboard"_q },
		{ u"LuminaProfileCardCopyInfo"_q, u"Builds a plain-text summary of your card. Paste it into any chat when you want to share it."_q },
		{ u"LuminaProfileCardEdit"_q, u"My card"_q },
		{ u"LuminaProfileCardEmptyShare"_q, u"Fill in your card first"_q },
		{ u"LuminaProfileCardHeader"_q, u"My card"_q },
		{ u"LuminaProfileCardInfo"_q, u"This card is stored only on this device and is never sent to Telegram. It does not change your Telegram profile."_q },
		{ u"LuminaProfileCardInterests"_q, u"Interests / tags"_q },
		{ u"LuminaProfileCardInterestsHint"_q, u"e.g. music, hiking, coding"_q },
		{ u"LuminaProfileCardLanguages"_q, u"Languages I speak"_q },
		{ u"LuminaProfileCardLanguagesHint"_q, u"e.g. English, 中文, Español"_q },
		{ u"LuminaProfileCardNotSet"_q, u"Not set"_q },
		{ u"LuminaProfileCardTagline"_q, u"Tagline"_q },
		{ u"LuminaProfileCardTaglineHint"_q, u"A short headline about you"_q },
		{ u"LuminaProfileCardTitle"_q, u"Profile card"_q },
		{ u"LuminaProfileChatCreated"_q, u"Created"_q },
		{ u"LuminaProfileDcId"_q, u"Datacenter"_q },
		{ u"LuminaProfileUserId"_q, u"User ID"_q },
		{ u"LuminaProfileDcIdValue"_q, u"DC{1}"_q },
		{ u"LuminaProfileInfoHeader"_q, u"Profile"_q },
		{ u"LuminaProfileInfoInfo"_q, u"Extra lines on profile pages, all worked out on this device — nothing is asked of Telegram. Telegram itself supplies the registration date for some accounts; for the rest it is estimated from the account number and shown with a \"~\". The datacenter is the one storing the profile photo. The creation date is when a group or channel was made."_q },
		{ u"LuminaProfileRegistrationApprox"_q, u"~ {1}"_q },
		{ u"LuminaProfileRegistrationDate"_q, u"Registration date"_q },
		{ u"LuminaProfileShowChatDate"_q, u"Show creation date"_q },
		{ u"LuminaProfileShowDcId"_q, u"Show datacenter"_q },
		{ u"LuminaProfileShowRegistrationDate"_q, u"Show registration date"_q },
		{ u"LuminaRecentLimitsInfo"_q, u"Stop this device from discarding recent stickers and saved GIFs earlier than it has to, using the same limits as LuminaGram on Android: up to 200 recent stickers and 500 saved GIFs. Telegram's servers still decide how many are actually stored in your account and synced to your other devices, so this cannot give you a bigger cloud list — the next time this device syncs, anything the server no longer holds disappears here too, usually within seconds. While this is on, Telegram Desktop stops removing a saved GIF on its own, so its Premium reminder about the saved-GIF limit stays quiet. The sticker panel still shows only the first 20 recent stickers unless you also turn on \"Unlimited recent stickers\" in Settings › Advanced › Experimental settings."_q },
		{ u"LuminaRecentLimitsRow"_q, u"Keep more recent stickers and GIFs"_q },
		{ u"LuminaReplyTemplatesAbout"_q, u"Short pieces of text you keep on this device and drop into the message field. Right-click the emoji button in a chat to pick one. Templates are never sent to Telegram."_q },
		{ u"LuminaReplyTemplatesAdd"_q, u"Add template"_q },
		{ u"LuminaReplyTemplatesEdit"_q, u"Edit template"_q },
		{ u"LuminaReplyTemplatesEmpty"_q, u"No templates yet. Add one, then right-click the emoji button in a chat to insert it."_q },
		{ u"LuminaReplyTemplatesFull"_q, u"The list is full — delete a template to add another."_q },
		{ u"LuminaReplyTemplatesInfo"_q, u"Right-click the emoji button in a chat to insert a template. Right-click a template here to move it up or down; open one to edit or delete it."_q },
		{ u"LuminaReplyTemplatesList"_q, u"Templates"_q },
		{ u"LuminaReplyTemplatesManage"_q, u"Manage templates…"_q },
		{ u"LuminaReplyTemplatesMoveDown"_q, u"Move down"_q },
		{ u"LuminaReplyTemplatesMoveUp"_q, u"Move up"_q },
		{ u"LuminaReplyTemplatesNone"_q, u"None"_q },
		{ u"LuminaReplyTemplatesPlaceholder"_q, u"Template text"_q },
		{ u"LuminaReplyTemplatesShow"_q, u"Offer templates in chats"_q },
		{ u"LuminaReplyTemplatesTitle"_q, u"Reply templates"_q },
		{ u"LuminaSaveSticker"_q, u"Save sticker"_q },
		{ u"LuminaSaveStickers"_q, u"Save stickers"_q },
		{ u"LuminaSaveStickersInfo"_q, u"Adds a \"Save sticker\" row to the sticker panel's context menu. Stickers go wherever your other downloads go, and the row is hidden for a group's own sticker set when that group restricts saving."_q },
		{ u"LuminaSaveToCloud"_q, u"Save to Saved Messages"_q },
		{ u"LuminaSaveToCloudTitle"_q, u"Save to Saved Messages"_q },
		{ u"LuminaSecurityPanicConfirmAck"_q, u"I understand this cannot be undone"_q },
		{ u"LuminaSecurityPanicConfirmButton"_q, u"Wipe now"_q },
		{ u"LuminaSecurityPanicConfirmText"_q, u"Every account on this device will be logged out. The local message database, drafts and cached media will be erased, together with LuminaGram's own settings, bookmarks, saved translations and API keys.\n\nYour accounts are not deleted. They stay on Telegram's servers, and so do your messages — you can sign in again from anywhere.\n\nFiles that were already downloaded are NOT deleted. LuminaGram does not touch your download folder, because it is usually your ordinary Downloads folder and holds unrelated files. Move or delete anything sensitive there yourself.\n\nThis cannot be undone."_q },
		{ u"LuminaSecurityPanicConfirmTitle"_q, u"Panic wipe?"_q },
		{ u"LuminaSecurityPanicHeader"_q, u"Panic wipe"_q },
		{ u"LuminaSecurityPanicWipe"_q, u"Panic wipe (Kaboom)"_q },
		{ u"LuminaSecurityPanicWipeAbout"_q, u"Logs every account on this device out and erases the local message database, drafts and cached media, together with LuminaGram's own settings, bookmarks, saved translations and API keys. Your accounts and your messages stay on Telegram's servers. Files you already downloaded are left where they are. This cannot be undone."_q },
		{ u"LuminaSelectFromAuthor"_q, u"Select all from author"_q },
		{ u"LuminaSelectFromAuthorAbout"_q, u"Adds a message menu entry in groups that selects every message from the sender you clicked. It only reaches messages this window has already loaded — scroll further back and repeat it to include older ones. Messages attributed to the chat itself, such as channel posts and posts by anonymous admins, do not get the entry."_q },
		{ u"LuminaSelectFromAuthorLimit"_q, u"Only {1} messages can be selected at once."_q },
		{ u"LuminaSelectFromAuthorTitle"_q, u"Select all from author"_q },
		{ u"LuminaSelectionHeader"_q, u"Selection"_q },
		{ u"LuminaSendOriginalCaption"_q, u"Send original caption"_q },
		{ u"LuminaShowBookmarks"_q, u"Show bookmark menu item"_q },
		{ u"LuminaShowMessageDetails"_q, u"Message details"_q },
		{ u"LuminaShowMutedCount"_q, u"Always show unread count"_q },
		{ u"LuminaShowMutedCountInfo"_q, u"Draws the unread badge of muted chats in the normal accent color instead of muted gray."_q },
		{ u"LuminaStickerSavedTo"_q, u"Sticker saved to {1}"_q },
		{ u"LuminaStickerSizeChoice"_q, u"{1}%"_q },
		{ u"LuminaStickerSizeChoiceDefault"_q, u"{1}% (default)"_q },
		{ u"LuminaStickerSizeInfo"_q, u"How large stickers are drawn in chats — the ones you send and the ones you receive. Animated emoji, dice and gift stickers keep their own sizes. Telegram Desktop measures a sticker once and remembers it, so a new size takes effect the next time the app starts."_q },
		{ u"LuminaStickersHeader"_q, u"Stickers and GIFs"_q },
		{ u"LuminaTimeWithSeconds"_q, u"Show seconds in message time"_q },
		{ u"LuminaTimeWithSecondsAbout"_q, u"The time under a message, the time in copied text and the time a screen reader announces all include seconds."_q },
		{ u"LuminaUndoSendBulletin"_q, u"Sending message…"_q },
		{ u"LuminaUndoSendTitle"_q, u"Undo send"_q },
		{ u"LuminaUndoSendUndo"_q, u"Undo"_q },
		{ u"LuminaUndoSendWindow"_q, u"Undo send window"_q },
		{ u"LuminaUndoSendWindowInfo"_q, u"Hold a plain text message for {1} seconds behind an Undo button before it is sent. Your text stays in the message box the whole time and the box clears only once the message really goes out, so Undo simply leaves it where it is - nothing is taken away and put back. Sending again, opening another chat, or quitting sends the held message straight away. Media, voice, edited, forwarded and scheduled messages are never held, and neither are messages sent from a forum topic or a comment thread."_q },
		// clipboard guard + scam watch
		{ u"LuminaPrivacyCryptoClipboardGuard"_q, u"Crypto address paste guard"_q },
		{ u"LuminaPrivacyCryptoClipboardGuardInfo"_q, u"Ask before pasting what looks like a crypto wallet address into a message. Clipboard-hijacking malware can swap a copied address for a scammer's without you noticing, and the paste is the last moment you can catch it. The check runs on this device and nothing is sent anywhere."_q },
		{ u"LuminaPrivacyScamKeywordWarning"_q, u"Scam keyword warning"_q },
		{ u"LuminaPrivacyScamKeywordWarningInfo"_q, u"When a message from someone not in your contacts mentions common scam tactics — money transfers, gift cards, crypto “investments”, verification fees or asking for codes — show a one-time reminder to stay cautious. The message is never blocked or changed. Checks run offline on your device."_q },
		// otp guard
		{ u"LuminaOtpGuardTitle"_q, u"Warning"_q },
		{ u"LuminaOtpGuardMessage"_q, u"You are about to send your Telegram login code to someone else. Whoever receives it can sign in to your account and read all of your messages. Telegram will never ask you for this code."_q },
		{ u"LuminaOtpGuardCancel"_q, u"Don't send"_q },
		{ u"LuminaOtpGuardSendAnyway"_q, u"I understand the risk, send anyway"_q },
		{ u"LuminaOtpGuard"_q, u"Login code protection"_q },
		{ u"LuminaOtpGuardInfo"_q, u"Warn you before sending a message that contains a login code, if Telegram has just sent you one. The check runs entirely on your device — nothing is uploaded, and your message is never changed."_q },
		// link preview + voice confirm
		{ u"LuminaPrivacyDisableLinkPreview"_q, u"Disable link preview by "
			"default"_q },
		{ u"LuminaLinkPreviewInfo"_q, u"Messages you write from scratch are "
			"sent without a link preview, and the composer never asks "
			"Telegram to look a pasted link up while you type. A preview that "
			"is already attached, and a message you edit, are left as they "
			"are. To attach a preview to one message anyway, give \"Toggle "
			"link preview\" a key in Settings > Keyboard shortcuts and press "
			"it while composing."_q },
		{ u"LuminaLinkPreviewHeader"_q, u"Link previews"_q },
		{ u"LuminaVoiceConfirmTitle"_q, u"Voice and video messages"_q },
		{ u"LuminaVoiceConfirmRow"_q, u"Confirm before sending"_q },
		{ u"LuminaVoiceConfirmInfo"_q, u"Ask before a recorded voice or round "
			"video message is sent. Backing out of that question discards the "
			"recording, except in the listen preview, where it leaves the "
			"recording alone. Scheduled sends are never asked about."_q },
		// exif strip + text replace
		{ u"LuminaExifStripRow"_q, u"Remove photo location before sending"_q },
		{ u"LuminaExifStripInfo"_q, u"Erase the GPS coordinates a camera stored inside a JPEG photo before it is uploaded — both when you send the photo the ordinary compressed way and when you send it as a file. Only the location tags go: orientation, camera and date tags are left exactly as they were, so a photo sent as a file still shows the right way up. Your own copy of the file on disk is never modified. This does not cover videos, a location written into a photo by XMP or by a camera's own maker notes, or the second copy of the picture that some phones tuck into the same file."_q },
		{ u"LuminaReplacerTitle"_q, u"Text replacer"_q },
		{ u"LuminaReplacerHeader"_q, u"Rules"_q },
		{ u"LuminaReplacerAdd"_q, u"Add rule"_q },
		{ u"LuminaReplacerEdit"_q, u"Edit rule"_q },
		{ u"LuminaReplacerFrom"_q, u"Replace"_q },
		{ u"LuminaReplacerTo"_q, u"With"_q },
		{ u"LuminaReplacerEnable"_q, u"Replace text in messages I send"_q },
		{ u"LuminaReplacerNone"_q, u"None"_q },
		{ u"LuminaReplacerNothing"_q, u"(nothing)"_q },
		{ u"LuminaReplacerEmpty"_q, u"No rules yet. Add one to start replacing text in the messages you send."_q },
		{ u"LuminaReplacerFull"_q, u"Rules are applied in order, from the top. The list is full - delete a rule to add another."_q },
		{ u"LuminaReplacerListInfo"_q, u"Rules are applied in order, from the top, and each one works on what the one above it produced."_q },
		{ u"LuminaReplacerAbout"_q, u"Swap a shorthand for the phrase you meant in every message you send - \"brb\" becomes \"be right back\". Matching is case-sensitive and matches whole words. A message that carries any formatting is sent exactly as typed, and links, mentions, hashtags and bot commands are never rewritten. Rules stay on this device and are never sent to Telegram."_q },
		// vault + fake crash
		{ u"LuminaVaultHeader"_q, u"Disguise vault"_q },
		{ u"LuminaVaultEnable"_q, u"Enable vault"_q },
		{ u"LuminaVaultInfo"_q, u"Everything here stays on this device. The vault hides LuminaGram behind a harmless-looking app, and only the secret code gets you back into the real one."_q },
		{ u"LuminaVaultMode"_q, u"Vault mode"_q },
		{ u"LuminaVaultModePasswordDoor"_q, u"Password door"_q },
		{ u"LuminaVaultModeDecoyApp"_q, u"Decoy app"_q },
		{ u"LuminaVaultModePasswordDoorInfo"_q, u"Starting LuminaGram asks for a password. The secret code opens the real app; anything else opens the decoy, and never says that it was wrong."_q },
		{ u"LuminaVaultModeDecoyAppInfo"_q, u"Starting LuminaGram opens the decoy straight away. Enter your secret code in it to reach the real app."_q },
		{ u"LuminaVaultSkin"_q, u"Decoy app style"_q },
		{ u"LuminaVaultSkinNotepad"_q, u"Notepad"_q },
		{ u"LuminaVaultSkinCalculator"_q, u"Calculator"_q },
		{ u"LuminaVaultSkinHintCalculator"_q, u"To unlock from the calculator, type the secret code and press the equals key."_q },
		{ u"LuminaVaultSkinHintNotepad"_q, u"To unlock from the notepad, make the secret code the whole note, then double-click the Notes title bar."_q },
		{ u"LuminaVaultSecretCode"_q, u"Secret code"_q },
		{ u"LuminaVaultSecretCodeDialogTitle"_q, u"Secret code"_q },
		{ u"LuminaDisguiseDecoyCodeSet"_q, u"Set"_q },
		{ u"LuminaDisguiseDecoyCodeNotSet"_q, u"Not set"_q },
		{ u"LuminaVaultNoCodeInfo"_q, u"The vault stays off until you set a secret code."_q },
		{ u"LuminaVaultCalculatorCodeInfo"_q, u"This code cannot be typed on the calculator keypad, so the notepad decoy is shown instead. Use only digits, a dot and + - * / if you want the calculator."_q },
		{ u"LuminaVaultDisclaimer"_q, u"This defeats a glance over your shoulder, not an informed inspector: the program is still called LuminaGram in the task manager, and anyone who can read this device's files can read your account. The vault appears the next time LuminaGram starts, and closing the decoy quits LuminaGram. If you ever forget the code, delete tdata/luminagram.json and tdata/luminagram_private.json from your LuminaGram data folder: the vault then turns itself off and LuminaGram starts normally."_q },
		{ u"LuminaSecurityFakeCrashHeader"_q, u"Fake-crash unlock"_q },
		{ u"LuminaSecurityFakeCrashEnable"_q, u"Enable fake-crash unlock"_q },
		{ u"LuminaSecurityFakeCrashCode"_q, u"Fake-crash code"_q },
		{ u"LuminaSecurityFakeCrashCodeDialogTitle"_q, u"Fake-crash code"_q },
		{ u"LuminaSecurityFakeCrashCodeHint"_q, u"Code"_q },
		{ u"LuminaSecurityFakeCrashCodeSet"_q, u"Set"_q },
		{ u"LuminaSecurityFakeCrashCodeNotSet"_q, u"Not set"_q },
		{ u"LuminaSecurityFakeCrashCodeSameAsPasscode"_q, u"This code can't be the same as your passcode lock."_q },
		{ u"LuminaSecurityFakeCrashInfo"_q, u"When this is on, typing the fake-crash code on the lock screen closes LuminaGram straight away instead of unlocking it, with no message and nothing left on screen - to anyone watching, the app crashed. The code is separate from your passcode lock, it is kept on this device only, and it is never sent anywhere."_q },
		{ u"LuminaSecurityFakeCrashBiometricInfo"_q, u"While this is on and a code is set, unlocking with Touch ID, Windows Hello or your system password is turned off: a fingerprint opens the app without ever asking for the passcode, so it would go straight past this code. Clearing the code, or turning this off, brings it back."_q },
		{ u"LuminaSecurityFakeCrashNoCodeInfo"_q, u"No code is set yet, so nothing will happen on the lock screen."_q },
		{ u"LuminaSecurityFakeCrashNoPasscodeInfo"_q, u"LuminaGram has no passcode lock, so the lock screen never appears and this code is never asked for. Turn on the passcode lock in Privacy and Security first."_q },
		{ u"LuminaSecurityFakeCrashCodeClashInfo"_q, u"This code is now the same as your passcode lock, so it will never fire - the passcode unlocks the app instead. Choose a different code."_q },
		// login guard (D-32). Keys and texts are Android's; LuminaSessionQr* has no
		// caller on desktop - see lumina_session_guard.h.
		{ u"LuminaSessionGuardHeader"_q, u"Login guard"_q },
		{ u"LuminaSessionGuard"_q, u"New-login alerts"_q },
		{ u"LuminaSessionGuardInfo"_q, u"Check your linked devices when the app opens and warn you about any session you have not approved. Scanning a login QR code hands whoever showed it a live session that can read all of your chats. Checks run at most once every 30 minutes, use Telegram's own API and never leave your device."_q },
		{ u"LuminaSessionGuardCheckNow"_q, u"Check linked devices now"_q },
		{ u"LuminaSessionGuardNoNew"_q, u"No new logins. Every linked device is one you already approved."_q },
		{ u"LuminaSessionGuardCheckFailed"_q, u"Could not check your linked devices. Check your connection and try again."_q },
		{ u"LuminaSessionGuardChecking"_q, u"Checking..."_q },
		{ u"LuminaSessionGuardBusy"_q, u"A check is already running. Its result will appear in a moment."_q },
		{ u"LuminaSessionAlertTitle"_q, u"New login detected"_q },
		{ u"LuminaSessionAlertIntro"_q, u"A device you have not approved is signed in to your account. If this was not you, it can read all of your chats right now."_q },
		{ u"LuminaSessionAlertDevice"_q, u"Device"_q },
		{ u"LuminaSessionAlertApp"_q, u"App"_q },
		{ u"LuminaSessionAlertIp"_q, u"IP address"_q },
		{ u"LuminaSessionAlertLocation"_q, u"Location"_q },
		{ u"LuminaSessionAlertTime"_q, u"Signed in"_q },
		{ u"LuminaSessionAlertNotMe"_q, u"Not me — terminate now"_q },
		{ u"LuminaSessionAlertItsMe"_q, u"That was me"_q },
		{ u"LuminaSessionTerminated"_q, u"Session terminated"_q },
		{ u"LuminaSessionTerminateFailed"_q, u"Could not terminate that session. Open Settings → Devices and remove it there."_q },
		{ u"LuminaSession2FAMessage"_q, u"That device has been signed out. Turn on Two-Step Verification so nobody can sign in again with just a code."_q },
		{ u"LuminaSession2FAButton"_q, u"Set up now"_q },
		{ u"LuminaSessionLater"_q, u"Later"_q },
		{ u"LuminaSessionQrConfirmTitle"_q, u"Authorise a new device?"_q },
		{ u"LuminaSessionQrConfirmMessage"_q, u"You are about to authorise a new device to sign in to your account. Unless you are sitting at that computer yourself, whoever controls it will be able to read all of your messages. Never scan a login QR code that someone sent you."_q },
		{ u"LuminaSessionQrConfirmContinue"_q, u"I am at that device — continue"_q },

		// Stories, fully off. Key names and texts are Android's, verbatim.
		{ u"LuminaStoriesHeader"_q, u"Stories"_q },
		{ u"LuminaStoriesFullyOff"_q, u"Turn stories off completely"_q },
		{ u"LuminaStoriesFullyOffInfo"_q, u"Removes stories from the whole app: the row above the chat list and the one in Archive, the colored rings on avatars everywhere, the stories block and Stories tab on profiles, and story notifications. Nothing is changed on Telegram's servers — your stories and everyone else's stay exactly as they are, they are simply never shown here."_q },
		{ u"LuminaStoriesHidePostEntry"_q, u"Also hide the post button"_q },
		{ u"LuminaStoriesHidePostEntryInfo"_q, u"Hides the camera button used to post your own story, on the chat list and on profiles."_q },

		// Voice to text (F-32), keys shared with Android's
		// LuminaLocale / LuminaVoiceToTextActivity.
		{ u"LuminaVoiceToTextTitle"_q, u"Voice to Text"_q },
		{ u"LuminaSttEnable"_q, u"Enable voice to text"_q },
		{ u"LuminaSttEngine"_q, u"Engine"_q },
		{ u"LuminaSttEngineWhisper"_q, u"OpenAI Whisper (your own key)"_q },
		{ u"LuminaSttEngineGoogle"_q, u"Google (your own key)"_q },
		{ u"LuminaSttKey"_q, u"API key"_q },
		{ u"LuminaSttBaseUrl"_q, u"Base URL"_q },
		{ u"LuminaSttModel"_q, u"Model"_q },
		{ u"LuminaSttNotReady"_q, u"Voice to text is on, but the engine selected below has no API key yet, so it cannot run. Save a key under Engine, or turn this off."_q },
		{ u"LuminaSttInfoDesktop"_q, u"Transcription runs on your own API key and your own quota "
			u"- LuminaGram never provides a key. The voice message is "
			u"uploaded to the service you pick below, so only turn this "
			u"on for a service you are willing to send your messages "
			u"to."_q },
		{ u"LuminaSttVoskUnsupported"_q, u"The offline engine (Vosk) is not supported on desktop "
			u"yet: it needs a native library and a voice model packaged "
			u"separately for Windows, macOS and Linux. Until then, "
			u"transcription on desktop is cloud-only."_q },
		{ u"LuminaExplainMenuItem"_q, u"Explain"_q },
		{ u"LuminaExplainTitle"_q, u"Explain this message"_q },
		{ u"LuminaExplainWorking"_q, u"Explaining..."_q },
		{ u"LuminaExplainError"_q, u"Could not explain this message. Check your "
			u"LLM key and connection, then try again."_q },
		{ u"LuminaExplainNoKey"_q, u"No LLM API key set. Add one in Settings, "
			u"LuminaGram, Translation, then try again."_q },
		{ u"LuminaSttUiMenuItem"_q, u"Transcribe"_q },
		{ u"LuminaSttUiPickTitle"_q, u"Voice to text"_q },
		{ u"LuminaSttUiFetching"_q, u"Downloading the voice message..."_q },
		{ u"LuminaSttUiTranscribing"_q, u"Transcribing..."_q },
		{ u"LuminaSttUiTranslation"_q, u"Translation"_q },
		{ u"LuminaSttUiError"_q, u"Transcription failed"_q },
		{ u"LuminaSttUiNoText"_q, u"No speech detected"_q },
		{ u"LuminaSttUiNoKey"_q, u"No API key set. Add one in Settings, LuminaGram, Voice to "
			u"Text."_q },
		{ u"LuminaSttUiTooLarge"_q, u"This voice message is too large for the selected engine."_q },
		{ u"LuminaSttUiRoundUnsupported"_q, u"Google Speech cannot read video messages. Choose Whisper "
			u"for round videos."_q },
		{ u"LuminaSttAutoTranslate"_q, u"Translate after transcribing"_q },
		{ u"LuminaSttAutoTranslateInfo"_q, u"When a voice message is transcribed into a language you "
			u"do not read, LuminaGram translates it straight away and "
			u"shows the transcript first, then the translation. Nothing "
			u"is translated when the transcript is already in your "
			u"reading language, so no quota is spent, and a translation "
			u"that fails still leaves the transcript on screen."_q },
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

rpl::producer<QString> TrValue(const QString &key, const QString &arg1) {
	return rpl::single(
		rpl::empty
	) | rpl::then(
		LangChanges()
	) | rpl::map([key, arg1] {
		return Tr(key, arg1);
	});
}

} // namespace Lumina
