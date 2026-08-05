/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include <rpl/producer.h>

class History;
class PeerData;

namespace Main {
class Session;
} // namespace Main

namespace Lumina {

// Gating for read-side (incoming) translation: when it may run at all, and
// when it may start on its own.
//
// Keys read here, all in Store::Prefs, all written by the translate settings
// sub-page (lumina_translate_settings.cpp), which uses exactly these names and
// defaults:
//
//   trMode             string  default "manual"    "manual" | "all"
//   trScopePrivate     bool    default true        1:1 user chats
//   trScopeGroup       bool    default true        groups and channels
//
// With those defaults nothing here ever starts translating a chat on its own,
// which is the point: automatic translation waits for an explicit opt-in.
//
// The third input, the selected provider, is not read directly: it belongs to
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
// the toggle off this fork behaves exactly like upstream; with it on, the free
// engine works immediately, with no key to obtain.
[[nodiscard]] bool TranslationFeatureEnabled();
[[nodiscard]] rpl::producer<bool> TranslationFeatureEnabledValue();

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

// trMode == "all". "manual" is the default and has to stay the default: "all"
// costs one provider request per message against what is usually a metered API
// key, because lib_translate fans a batch out into N single requests.
[[nodiscard]] bool AutoTranslateEverything();

// The read-side scope: 1:1 user chats are gated on trScopePrivate, groups and
// channels on trScopeGroup, mirroring Android's DialogObject.isUserDialog /
// isChatDialog split. Scope narrows ONLY the automatic mode - a chat outside
// the scope can still be translated by hand, exactly as Android's own help
// text promises ("Scope only limits the Auto-translate all chats mode").
[[nodiscard]] bool TranslateScopeAllows(not_null<PeerData*> peer);

// THE predicate. Every path that would start translating a chat the user did
// not ask to translate consults this one function and nothing else. On Android
// the equivalent check was duplicated and one onResume-style entry point
// silently skipped it, so chats translated themselves while the mode said
// manual; a second copy of this logic anywhere is that same bug.
//
// It deliberately does not cover the paths where the user did ask: the
// translate bar, the per-chat row in the chat menu, and the channel-side
// automatic translation Telegram itself performs through
// ChannelDataFlag::AutoTranslation.
[[nodiscard]] bool ShouldAutoTranslate(not_null<History*> history);

} // namespace Lumina
