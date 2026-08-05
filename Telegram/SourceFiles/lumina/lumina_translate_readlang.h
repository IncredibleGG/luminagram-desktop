/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "spellcheck/spellcheck_types.h" // LanguageId.

#include <rpl/producer.h>

namespace Lumina {

// The read-side target language: the one incoming messages are translated
// INTO. Android calls it `trReadLang` and stores "" for "follow the interface
// language", which is what stock tdesktop already does, so the default costs
// nothing and changes nothing.
//
// The preference itself is NOT owned here. It is declared, defaulted, read,
// written and surfaced in the settings page by lumina/lumina_translate_settings
// (TranslateReadLanguage() / SetTranslateReadLanguage()), which is the single
// owner of every translation key. This file only projects that string onto the
// LanguageId-shaped read path that stock tdesktop already has, so there is
// exactly one key spelling and exactly one writer in the tree.
//
// This is deliberately not a second target-language mechanism.
// Ui::ChooseTranslateTo() (boxes/translate_box.cpp) already resolves a target
// per chat out of three inputs: the language the chat is offered to be
// translated from, the last language the user picked
// (Core::Settings::translateTo()) and the skip list. The override composes with
// that by taking the place of the picked language, so the "do not translate a
// chat into the language it is already written in" fallback to the skip list
// is kept, and everything downstream of ChooseTranslateTo() - the translate
// bar, the translate box, the per-chat translation itself - keeps working the
// way it does today.
//
// The stored value is a language code string rather than a LanguageId because
// LanguageId is a bare QLocale::Language and cannot express a dialect (zh-TW
// versus zh-CN), which the provider layer needs. Every LanguageId here is
// therefore a lossy projection of the stored code - good enough for tdesktop's
// own UI and for Telegram-side translation - and a stored code QLocale cannot
// parse at all resolves to "no override", i.e. to stock behaviour.
//
// Provider wire-code normalisation (Android rewrites "nb" to "no" before
// calling out) belongs to the provider layer and is not done here.

// The stored code, trimmed. Empty means "follow the interface language".
[[nodiscard]] QString ReadLanguageCode();

// Empty when following the interface language, and also when the stored code
// is not a language Qt recognises.
[[nodiscard]] LanguageId ReadLanguage();

// Returns `fallback` unchanged when no override is set. This is the single
// call the read path needs.
[[nodiscard]] LanguageId ReadLanguageOr(LanguageId fallback);

// The user picked a translate-to language in one of the stock pickers - both
// of them go through Ui::ChooseTranslateToBox(), from the translate bar menu
// and from the translate box. While an override is in force it, and not
// Core::Settings::translateTo(), is what the read path resolves to, so the
// pick has to land here as well or it would apply once and then be forgotten.
//
// Does nothing when no override is set, which leaves the pickers behaving
// exactly as they do today.
//
// A stored code that already projects to `id` is left alone, so picking
// "Chinese" in a dialect-blind picker does not quietly rewrite a stored
// "zh-TW" into "zh".
void NoteReadLanguageChosen(LanguageId id);

// Emits the current code immediately, then again on every change. Meant for
// dropping into an rpl::combine that has to recompute a target language: a
// changes-only stream would stall the whole combine until the first change.
[[nodiscard]] rpl::producer<QString> ReadLanguageCodeValue();

} // namespace Lumina
