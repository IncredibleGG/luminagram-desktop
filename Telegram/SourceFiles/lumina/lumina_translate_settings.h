/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include <rpl/producer.h>

#include <QtCore/QString>

#include <vector>

namespace Ui {
class VerticalLayout;
} // namespace Ui

namespace Window {
class SessionController;
} // namespace Window

namespace Lumina {

// The LuminaGram translation settings sub-page, and the preferences that have
// no other owner.
//
// The page presents two directions, deliberately not one "translate to"
// setting:
//
//  * send side - TranslateBeforeSend() rewrites an outgoing message into
//    TranslateSendLanguage(), which defaults to "auto", meaning "whatever the
//    peer writes in";
//  * read side - DualLanguageDisplay() shows an incoming message together with
//    its translation into the read language, which defaults to unset, meaning
//    "follow the interface language".
//
// On both sides the original text stays the large one and the translation the
// small one; that is a rendering decision and lives with the renderer.
//
// Where a value already has an owner, this file does not declare a second
// accessor for it - it renders the row and writes through that owner:
//
//   provider, API keys, LLM base URL / model / prompt, Telegram fallback
//                                       lumina/lumina_translate_providers.h
//   the master switch, and the tier it governs
//                                       lumina/lumina_translate_gating.h
//
// trReadLang goes the other way: the key is declared and written here, and
// lumina/lumina_translate_readlang.h only projects it onto the LanguageId
// shape stock tdesktop's read path expects.
//
// THE PAGE IS A SWITCH, AN ENGINE AND TWO DEFAULT LANGUAGES. It used to carry a
// mode (every chat / only chats I turn on) and a scope (private chats, groups
// and channels) as well. Both existed only to decide WHICH chats translate
// automatically, and the user now makes that decision per chat, by hand, in the
// chat itself - so both were two more things that could go wrong for no
// benefit. Their keys - trMode, trScopePrivate, trScopeGroup - are no longer
// written and no longer read; a profile that still has them keeps three unread
// entries in its pref file and nothing else. Do not re-add a preference here
// that answers a question a chat can answer about itself.
//
// The scope keys were the only ones whose removal a user can feel. They also
// narrowed translate-before-send, so a profile that had turned "Groups and
// channels" off and translate-before-send on now translates outgoing messages
// in groups too. Both scope keys defaulted to on and translate-before-send
// defaults to off, so that is the intersection of two deliberate choices; the
// send-menu row and the per-chat send language are where it is controlled now.
//
// Every key this file owns defaults to stock Telegram Desktop behaviour: both
// send-side toggles off, dual-language display off, send language "auto" and
// read language unset.
//
// The page is NOT behaviour-neutral overall, and the reason is not here: the
// provider row reads Lumina::CurrentProviderId(), whose unset value resolves
// through Lumina::DefaultProviderId() to "google_web", not to "telegram"
// (lumina/lumina_translate_providers.cpp). On a fresh profile the Service row
// therefore reads "Google", the "fall back to Telegram" toggle is visible, and
// Lumina::UsingOwnProvider() is already true - which relaxes the Premium gate
// in lumina/lumina_translate_gating.h before the user has chosen anything.
// That is W1-A's product decision and is documented there and in the gating
// header; this file only renders it. If it is ever reverted to "telegram", no
// change is needed here.

[[nodiscard]] bool TranslateBeforeSend();
void SetTranslateBeforeSend(bool value);

// Show the translation beside the original before it goes out, so the user can
// send the translation, send the original, or back out. Off sends the
// translation straight away. Android: LuminaConfig.translateBeforeSendConfirm,
// consumed in ChatActivityEnterView.luminaHandleTranslateResult().
//
// This is NOT the per-dialog send-language lock. That confirm is unconditional
// - the send pipeline asks once per chat when the send language is "auto" and
// nothing is locked yet (Android: luminaConfirmSendLang) - and has no
// preference of its own. W2-A must not wire this key to it.
[[nodiscard]] bool TranslateBeforeSendConfirm();
void SetTranslateBeforeSendConfirm(bool value);

// "auto" means "detect the language the peer writes in". Never empty. Stored
// as a code rather than a LanguageId, because LanguageId is a bare
// QLocale::Language and cannot express zh-TW versus zh-CN.
[[nodiscard]] QString TranslateSendLanguage();
void SetTranslateSendLanguage(const QString &code);
[[nodiscard]] bool TranslateSendLanguageIsAuto();

[[nodiscard]] bool DualLanguageDisplay();
void SetDualLanguageDisplay(bool value);

// Fold the ORIGINAL of a long bilingual message down to one tappable line, so a
// wall of untranslated text does not push the chat around. Default ON, and only
// ever consulted while a message already shows both languages, so with dual
// display off it changes nothing. Its own change stream is exposed because
// flipping it does not move TranslationFeatureEnabled() or DualLanguageDisplay()
// and so is invisible to TranslateSettingsChanges()'s active-tier watchers.
[[nodiscard]] bool FoldOriginalLongMessages();
void SetFoldOriginalLongMessages(bool value);
[[nodiscard]] rpl::producer<> FoldOriginalLongMessagesChanges();

// The read-side target language, empty for "follow the interface language".
// Stored as a code for the same dialect reason as the send language; the
// LanguageId-shaped projection tdesktop's own read path needs lives in
// lumina/lumina_translate_readlang.h. Setting an empty or blank code removes
// the key rather than storing an empty string.
[[nodiscard]] QString TranslateReadLanguage();
void SetTranslateReadLanguage(const QString &code);

// The language picker list, shared by the send-side and read-side rows. Codes
// are the ISO tags a provider is handed unchanged, so the list carries the
// dialects a bare two-letter code cannot express - the zh-TW / zh-CN split in
// particular, which this fork's own interface language needs.
struct TranslateLanguage {
	QString code;
	QString name;
};

[[nodiscard]] const std::vector<TranslateLanguage> &TranslateLanguages();

// Display name for a stored code, falling back to the raw code when it is not
// in the table - a code written by another client or restored from a backup is
// still shown rather than silently dropped.
[[nodiscard]] QString TranslateLanguageName(const QString &code);

// Fires once for any change to any preference this page shows, including an
// API key and including a whole-file restore through Settings::importAll().
// Cheaper and harder to get wrong than subscribing to a dozen changesFor()
// streams; a consumer that only cares about one value should still compare
// before acting.
[[nodiscard]] rpl::producer<> TranslateSettingsChanges();

// The whole sub-page, in the F-02 sub-page shape: the section .cpp holds this
// one call and nothing else.
//
// Three sections under the master switch: sending, receiving, service. Android
// puts a mode section after sending and a scope section after receiving; both
// are deliberately absent here, for the reason given at the top of this file.
//
// The service section ends in a test action that really does translate a
// sample through the selected engine - deliberately without the Telegram
// fallback, which would report success for a service that never answered - and
// names the failure it got. It is the only thing on this page that answers "is
// my key actually working", so it is the one row that must not be dropped in a
// later cleanup.
void AddTranslateRows(
	not_null<Ui::VerticalLayout*> container,
	not_null<Window::SessionController*> controller);

// The language list this page's two language rows open, shared so that the
// translate button in a chat's title bar edits the very same two values
// through the very same list. A second list built somewhere else is a second
// set of languages to keep in step, and a shortcut that offers different
// choices than the page it is a shortcut to is not a shortcut.
//
// `firstOption` is the special entry at the top - "Recipient's language" on
// the send side, "Interface language" on the read side, "Not translated" from
// the chat - and `firstValue` the code it stores.
void ShowLanguagePicker(
	not_null<Window::SessionController*> controller,
	const QString &title,
	const QString &firstOption,
	const QString &firstValue,
	const QString &current,
	Fn<void(QString)> save);

} // namespace Lumina
