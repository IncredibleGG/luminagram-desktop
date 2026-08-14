/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "spellcheck/spellcheck_types.h" // LanguageId.

#include <rpl/producer.h>

#include <optional>
#include <vector>

class History;

namespace Main {
class Session;
} // namespace Main

namespace Lumina {

// Where LuminaGram's translation is tiered, and where the read side decides
// what its language detector is allowed to hide.
//
// The one key read here is `translateEnabled`, in Store::Prefs, written by the
// single switch on the translate settings sub-page
// (lumina_translate_settings.cpp).
//
// The keys `trMode`, `trScopePrivate` and `trScopeGroup` were read here and are
// not any more. They existed to decide WHICH chats translate automatically, and
// the user now decides that per chat, by hand, in the chat itself. Nothing
// reads them, nothing writes them, and a profile that still carries them is
// simply a profile with three unread keys in its pref file - see the migration
// note on TranslateOfferSkip() below for what such a profile experiences.
//
// The other input, the selected provider, is not read directly: it belongs to
// lumina/lumina_translate_providers.h, and Lumina::UsingOwnProvider() there is
// the single predicate for "this request does not reach Telegram's servers".
// Declaring a second copy of it here would be one definition too many, and the
// two would drift.
//
// That file's DefaultProviderId() is "google_web", so on a fresh profile
// UsingOwnProvider() is already true and the Premium relaxation below is
// already in effect: a non-Premium account sees the translate bar, the
// Settings > Language switch and the per-chat row that stock hides. No
// translation request is made until the user asks for one - the tracker only
// recognises languages locally - but the visible difference from stock at
// default settings is real, and it is the direct consequence of shipping a
// keyless engine as the default. It is a product decision that belongs to the
// provider file, not something this file may re-decide quietly.

// The master opt-in for LuminaGram's translation, key `translateEnabled`,
// default FALSE.
//
// It exists because the default provider is the keyless google_web engine:
// without an explicit opt-in a fresh profile would already satisfy
// UsingOwnProvider(), and a non-Premium account would open the app to a
// translate bar, an unlocked "Translate chats" switch and a per-chat row that
// stock does not show. Nothing was asked for, so nothing should change. With
// it on, the free engine works immediately, with no key to obtain.
//
// WITH THE TOGGLE OFF THIS FORK IS UPSTREAM WITH ONE EXCEPTION, and it is not
// this file's to hide. Lumina::CreateOnDemandTranslateProvider()
// (lumina/lumina_translate_selection.cpp) deliberately bypasses the tier when
// it resolves the engine for a one-off lookup, so a selected-text or
// single-message translation runs through the Service row even here - which
// on a profile that never opted in means the keyless google_web default, not
// upstream's MTProto request. The free tier stays available either way; what
// moves is WHERE one lookup goes. That file argues the case and names its
// costs. If the "off means upstream, byte for byte" reading is the one that
// has to hold, the fix belongs there - drop the bypass - and not here.
[[nodiscard]] bool TranslationFeatureEnabled();
[[nodiscard]] rpl::producer<bool> TranslationFeatureEnabledValue();

// ---------------------------------------------------------------------------
// THE TIER BOUNDARY. One predicate. Do not copy its body anywhere.
// ---------------------------------------------------------------------------
//
// LuminaGram's translation is tiered the way Telegram tiers its own, so that a
// user does not have to learn a second concept:
//
//  * FREE, and NOT gated on anything here - translating a selected piece of
//    text, and translating one message from its context menu. One user action,
//    one request, nothing kept. That is what stock Telegram gives everyone, and
//    the policy for it lives in lumina/lumina_translate_selection.h, which is
//    deliberately a different file with a different answer.
//  * CONTINUOUS, gated on THIS - translating a whole conversation (the top bar
//    button in history/view/history_view_top_bar_widget.cpp, the chat menu row
//    in window/window_peer_menu.cpp, both through
//    lumina/lumina_translate_toggle.h), translate-before-send
//    (lumina/lumina_translate_send.h) and dual-language display
//    (lumina/lumina_dual_language_line.h). Each of those keeps running after
//    the one action that started it, one provider request per message,
//    indefinitely, against the user's own quota. That is the tier Telegram
//    charges Premium for.
//
// Today the answer is the master opt-in and nothing else. A licence or
// entitlement check goes HERE, inside this function, and NOWHERE ELSE. Every
// continuous entry point already consults it, so adding the term here is the
// whole of the work; adding it at a call site instead is how a fork ends up
// with a switch that turns on and a feature that then does nothing, which is
// the exact failure this file's ChatTranslationUnlocked() note below describes.
//
// It is deliberately session-free. The account-shaped question - may whole-chat
// translation run for this account at all without taking Telegram's paid
// feature for free - is ChatTranslationUnlocked() below, and it is a different
// question with a different answer. Both hold for whole-chat translation.
[[nodiscard]] bool ContinuousTranslationAvailable();
[[nodiscard]] rpl::producer<bool> ContinuousTranslationAvailableValue();

// Whether whole-chat translation may run for this account.
//
// Telegram's own whole-chat translation is a paid feature, and tdesktop gates
// it on Premium in three places: the tracker that decides whether to detect a
// chat's language at all (history/view/history_view_translate_tracker.cpp),
// the "Translate chats" switch in Settings > Language (boxes/language_box.cpp)
// and the per-chat row in the chat menu (window/window_peer_menu.cpp). All
// three consult this, and they have to keep consulting the same predicate - a
// relaxation applied to only some of them leaves the user a switch that turns
// on and a feature that then does nothing.
//
// The gate is relaxed ONLY when the request will not reach Telegram's servers,
// that is only when Lumina::UsingOwnProvider() (lumina_translate_providers.h)
// is true. With Telegram selected as the provider the Premium requirement
// stays exactly as upstream wrote it. That distinction is what keeps this
// fork's translation ToS-safe; it must not be blurred into a plain
// "translation is free now" switch.
[[nodiscard]] bool ChatTranslationUnlocked(not_null<Main::Session*> session);
[[nodiscard]] rpl::producer<bool> ChatTranslationUnlockedValue(
	not_null<Main::Session*> session);

// The read side's language-detection policy: what the detector is allowed to
// hide, and how long it may wait before it says anything.
//
// Upstream only ever OFFERS to translate a chat once two independent guesses
// agree that the user cannot read it: the recognised language must not be in
// Core::Settings::skipTranslationLanguages() - the languages tdesktop assumes
// the user knows, which by default is the interface language plus the system
// one - and enough of the loaded messages must be in it
// (kEnoughForTranslation in history/view/history_view_translate_tracker.cpp).
// Both guesses are right for a translate BAR that appears on its own.
//
// Neither survives a per-chat switch. The offer is not decoration: History has
// no HistoryTranslation until something offers a source language, and
// History::translateTo() returns on its first line while there is none - so a
// chat the detector declines to offer is a chat whose top bar button, chat menu
// row and language panel all write a choice that is dropped on the floor. A
// peer writing the user's own interface language is exactly such a chat, and it
// is also exactly the chat someone reaches for the button in. That is the same
// local detector hiding the same feature for the fifth time, and it does not
// get to.
//
// So while the continuous tier is available for this chat, this returns the
// skip list the detector must use INSTEAD of the user's own, and its presence
// is also the signal to offer from the first recognised message rather than
// waiting for the count threshold. It is nullopt otherwise, and there the
// detector uses upstream's inputs untouched.
//
// The list it returns is not the empty one: bypassing "languages you know" is
// about the languages the user did not choose, and the read language itself is
// not one of them. Offering a chat's language when it already IS the target
// would buy one provider request per message to translate German into German.
// The target is resolved exactly the way Ui::ChooseTranslateTo() resolves it -
// the explicit read-language override when there is one, else
// Core::Settings::translateTo(), which itself falls back to the interface
// language - so the detector and the thing that acts on it cannot disagree
// about what "the read language" is.
//
// GROUPS AND CHANNELS. On a multi-user peer (peer->isChat() || isChannel()),
// and only while the `groupSkipMyLanguages` toggle is on - Store::Prefs,
// default true - the languages the user already reads are added to this list as
// well: the interface language (Lang::Id()) and the trReadLang override. A
// group message written in one of them is then left as its original instead of
// being offered, so only what the user cannot read is offered - the desktop
// port of Android's "groups only translate languages I don't read". A
// one-to-one chat keeps the read-language-only list above unchanged, and with
// the toggle off every peer does.
//
// MIGRATION. This is where a profile carrying the removed trMode = "all" lands.
// Nothing reads that key any more, so nothing translates itself: such a profile
// opens on a launch where no chat is translating and every chat waits to be
// switched on. That is a smaller step than it sounds, because whole-chat
// translation was never durable in the first place - HistoryTranslation holds
// _translatedTo in memory only, nothing serialises it, so every launch already
// started from nothing and "all" was what filled it back in. What the user gets
// in exchange is that the offer now arrives for chats the skip list used to
// swallow, so the button is there to press in chats where nothing was ever
// offered. Nothing is rewritten and no stored value changes meaning.
[[nodiscard]] std::optional<std::vector<LanguageId>> TranslateOfferSkip(
	not_null<History*> history);

// Fires when anything TranslateOfferSkip() reads may have changed: the tier
// predicate above, the read language, the language the read language falls back
// to (Core::Settings::translateTo(), which every stock translate-to picker
// writes) and the selected provider. A consumer must re-read rather than
// assume, because the provider stream covers API key edits as well.
//
// The tier term is taken from ContinuousTranslationAvailableValue() rather than
// from the preference key behind it, so that a licence check added inside that
// predicate reaches every consumer of this stream without a second edit here.
//
// This stream is NOT the whole of what TranslateOfferSkip() reads, and a
// caller must not treat it as such. Three inputs are per-account or per-peer
// and cannot be expressed here: Core::Settings::translateChatEnabled(),
// ChatTranslationUnlocked() (its Premium half) and
// PeerData::translationFlag(). Each of them has its own stream on the read
// side, so no offer is ever computed from a stale answer.
//
// The trap is one level up. A consumer that feeds this into
// rpl::distinct_until_changed() is remembering the last value THIS stream
// carried, and one of those three moving the answer behind its back leaves
// that memory holding a value the predicate no longer returns - after which
// the next change that maps back to the remembered value is dropped as "not a
// change", and the switch the user just turned off does nothing. So a consumer
// that de-duplicates must merge those three streams in as well; see
// TranslateTracker::setup() in history/view/history_view_translate_tracker.cpp,
// which does exactly that. Duplicating them inside this function instead would
// not work: two of the three need a Main::Session and the third a PeerData,
// and this producer is per-application.
[[nodiscard]] rpl::producer<> TranslateOfferPolicyChanges();

} // namespace Lumina
