/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include <rpl/producer.h>

#include <QtCore/QString>

namespace Window {
class SessionController;
} // namespace Window

namespace Lumina {

// A local, offline-only "this looks like a scam" hint.
//
// Android's version (ChatActivity.luminaCheckScamKeywordWarning) shows one
// bulletin per opened chat when an incoming message from a NON-CONTACT in a
// one-on-one chat contains one of a fixed list of English scam phrases. It
// never blocks, never edits, never reports and never sends anything anywhere;
// the whole check is a substring scan of text already on this machine. This
// port keeps all of that, including the phrase list verbatim.
//
// Key, in Store::Prefs:
//
//   scamKeywordWarning   bool   default FALSE
//
// The watcher is installed unconditionally, because the preference can be
// turned on while the app is running. With the default it does nothing beyond
// holding two rpl subscriptions whose handlers return on the first line, so
// the app behaves exactly as upstream: nothing is blocked, edited, stored,
// logged or sent anywhere at any setting.

[[nodiscard]] bool ScamKeywordWarningEnabled();
[[nodiscard]] rpl::producer<bool> ScamKeywordWarningEnabledValue();
void SetScamKeywordWarningEnabled(bool value);

// The whole detector, exposed so it can be reasoned about (and tested) without
// a session. `text` is the plain message text; it is never stored or logged.
//
// Matching is case-insensitive, whitespace-collapsed, and - unlike Android's
// bare String.contains() - anchored on word boundaries, so "private key" no
// longer fires inside "private keyboard". A single trailing "s" is allowed, so
// "guaranteed returns" still matches "guaranteed return".
[[nodiscard]] bool LooksLikeScamText(const QString &text);

// Installs a per-window watcher owned by `controller->lifetime()`.
//
// WHY A WATCHER AND NOT A ONE-SHOT TOAST ON ARRIVAL - the open verification
// this item was given, answered by reading every caller:
//
// Data::Session::notifyNewItemAdded() is fired from exactly one function,
// History::newItemAdded(), which is reached from History::addNewItem()
// (history.cpp:848), History::addNewToBack() (:1222) - both behind
// `if (unread)` - and History::newItemAdded() called directly by
// history_streamed_drafts.cpp:330. That `unread` is true only when
// History::addNewMessage() was handed NewMessageType::Unread, or when the item
// is a LOCAL one (addNewLocalMessage / addSponsoredMessage pass a literal
// true).
//
// Every history-backfill path in the tree - messages.getHistory, the replies
// list, saved sublists, forum topics, search, the calendar, stories, statistics
// and the discussion lookup - passes NewMessageType::Existing, and the
// dialog-list top messages pass NewMessageType::Last. Neither reaches
// newItemAdded(). So opening an old chat or scrolling up CANNOT fire this, and
// the spray the plan feared from that direction does not exist.
//
// It does, however, fire as a bulk vector from Updates::feedDifference()
// (api_updates.cpp:620) and Updates::feedChannelDifference() (:457), which both
// call processMessages(..., NewMessageType::Unread). That is the offline
// catch-up: every message missed while the client was disconnected arrives in
// one batch on reconnect. Those messages are genuinely new, but there can be
// hundreds of them and the user is looking at one chat, if any.
//
// Three gates keep that honest, and all three are needed:
//   1. the item must still be unread (HistoryItem::unread), so a batch that
//      only replays what was already read on another device is silent - this
//      is the "genuinely new" gate, and it fails OPEN when the read horizon is
//      not known yet, so a real new message is never missed;
//   2. at most one hint per peer per app run;
//   3. the hint is only shown while that peer's chat is the active chat of
//      this window. A peer flagged while the user is elsewhere is remembered
//      and the hint appears the moment that chat is opened - which is also
//      where Android shows it, and the only place it is any use.
void SetupScamWatch(not_null<Window::SessionController*> controller);

} // namespace Lumina
