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
//   trMode / scope read semantics       lumina/lumina_translate_gating.h
//
// trMode is the one split case: Lumina::AutoTranslateEverything() in
// lumina_translate_gating.h is the reader every runtime path uses, and
// SetAutoTranslateEverything() below is the only writer.
//
// trReadLang goes the other way: the key is declared and written here, and
// lumina/lumina_translate_readlang.h only projects it onto the LanguageId
// shape stock tdesktop's read path expects.
//
// Every key this file owns defaults to stock Telegram Desktop behaviour: both
// send-side toggles off, dual-language display off, send language "auto", read
// language unset, mode "manual", both scopes on (and scope only ever narrows
// the mode, which is off).
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

// The read-side target language, empty for "follow the interface language".
// Stored as a code for the same dialect reason as the send language; the
// LanguageId-shaped projection tdesktop's own read path needs lives in
// lumina/lumina_translate_readlang.h. Setting an empty or blank code removes
// the key rather than storing an empty string.
[[nodiscard]] QString TranslateReadLanguage();
void SetTranslateReadLanguage(const QString &code);

// The read-side scope, mirroring Android's user / chat dialog split.
// Lumina::TranslateScopeAllows() in lumina_translate_gating.h is what the read
// path consults; these two exist for the rows that write them.
//
// Read side ONLY on desktop, and the page says so: the "Where it applies"
// divider promises "Scope only limits \"In every chat\"", which is Android's
// own help text (LuminaTranslateScopeInfo). Android's code then contradicts
// that string - ChatActivityEnterView.luminaTbsInScope() gates
// translate-before-send on the same two keys as well, so a user with
// "Groups" off silently loses send translation in groups without any UI
// saying so. Do not port that. If W2-A decides it wants a send-side scope
// after all, it needs its own pair of keys, or this divider text has to
// change with it.
[[nodiscard]] bool TranslateScopePrivate();
void SetTranslateScopePrivate(bool value);
[[nodiscard]] bool TranslateScopeGroup();
void SetTranslateScopeGroup(bool value);

// Writes trMode. Read it back through Lumina::AutoTranslateEverything().
void SetAutoTranslateEverything(bool value);

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
void AddTranslateRows(
	not_null<Ui::VerticalLayout*> container,
	not_null<Window::SessionController*> controller);

} // namespace Lumina
