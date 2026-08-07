/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "spellcheck/spellcheck_types.h" // LanguageId.

#include <memory>

namespace Main {
class Session;
} // namespace Main

namespace Ui {
class TranslateProvider;
} // namespace Ui

namespace Lumina {

// On-demand translation: the user selected a word or a sentence, opened the
// context menu and asked what it says. One lookup, one answer, nothing kept.
//
// THIS IS THE FREE TIER. It is deliberately NOT gated on
// Lumina::ContinuousTranslationAvailable() (lumina_translate_gating.h), which
// is where a licence check will one day live. That predicate governs the things
// that keep going after one user action - a whole conversation translating
// itself, outgoing messages being rewritten, every bubble carrying a second
// line - and none of that is what happens here. One action, one request,
// nothing kept: available the way "Copy" is available, which is what stock
// Telegram gives everyone.
//
// The gate is absent from BOTH answers below, and the second one is the one
// that is easy to lose: an entry point that is offered and then translates
// through somebody else's service is not a free tier, it is a reroute.
//
// Everything on-demand goes through these two answers, and nothing decides
// either of them anywhere else in the tree.

// May we offer to translate this text at all.
//
// Stock tdesktop asks Ui::SkipTranslate() (boxes/translate_box.cpp), which
// answers "no" in two cases this fork does not want:
//
//  * Core::Settings::translateButtonEnabled() is off - and it is off by
//    default, so out of the box the row never appears for anyone;
//  * the recognised language is in Core::Settings::skipTranslationLanguages(),
//    the list of languages tdesktop assumes the user already reads. Selecting
//    a Chinese sentence with a Chinese interface therefore offers nothing at
//    all - exactly when the user has told us, by selecting it, that this
//    particular sentence is the one they cannot read.
//
// What is kept from SkipTranslate() is the part that is genuinely about the
// text: empty selections, and selections with no letters anywhere in them
// (punctuation, digits, emoji), because offering to translate "?!" is noise.
[[nodiscard]] bool OnDemandTranslateAllowed(const QString &text);

// Which engine an on-demand translation runs through.
//
// The rule is the whole rule: whatever the user chose, always. A one-off
// lookup goes to exactly the service named in Settings > Language, which is
// what that page promises, and there is no account state, no text and no
// failure mode that may quietly send a selection somewhere else.
//
// A free account is served by the default selection rather than by rerouting:
// DefaultProviderId() is the keyless Google web engine, so an untouched
// profile already translates with no Premium and no API key. And an account
// that did pick "Telegram" is not stranded either - stock tdesktop offers this
// same box, over the same MTProto request, with no Premium check anywhere on
// the path (Lumina::ChatTranslationUnlocked() gates whole-chat translation,
// which is the paid feature; a single messages.translateText is not).
//
// This does NOT go through Ui::CreateTranslateProvider() alone, and must not.
// That function reaches a LuminaGram engine only through
// Lumina::CreateTranslateProvider(), which the continuous tier gates - so with
// the switch off it would fall through to Telegram's own service, taking the
// free tier off the keyless engine that makes it free and sending the lookup
// past the service the user named. The implementation therefore resolves the
// Service row itself when that would happen. It reads the tier predicate to
// know when that is, and for nothing else: the tier never decides whether this
// is offered, only whether upstream's resolution already lands where it should.
//
// So the only thing an account-state check could do here is move a Premium
// service's traffic to Google behind the back of a user who went into Settings
// and asked for Telegram by name. It is not worth a menu row.
//
// Never returns null: Ui::CreateTranslateProvider() can answer null when the
// platform translator is selected and the OS then declines to supply one, and
// every caller dereferences this.
[[nodiscard]] auto CreateOnDemandTranslateProvider(
	not_null<Main::Session*> session)
-> std::unique_ptr<Ui::TranslateProvider>;

// The language a one-off lookup answers in.
//
// `chosen` is what Ui::ChooseTranslateTo() resolved, and that is almost always
// the answer: it already composes the LuminaGram read-language override, the
// last language picked in the translate-to picker and, when neither is set,
// the interface language - Core::Settings::translateTo() falls back to
// DefaultSkipLanguages().front(), which is Lang::Id(). So with the translation
// service switched off entirely, stock resolution already lands on "the
// language this app is in", which is the right answer for a lookup, and this
// only replaces a target that came back unusable.
[[nodiscard]] LanguageId OnDemandTranslateTo(LanguageId chosen);

} // namespace Lumina
