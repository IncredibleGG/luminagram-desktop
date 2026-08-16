/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include <rpl/producer.h>

#include <QtCore/QString>

class History;
class PeerData;

namespace Lumina::ChatLock {

// Single-chat lock / private folder, a port of Android's per-conversation lock.
//
// A locked conversation is taken out of the chat list and out of every search
// surface (local name filter, message search, global peer search) until the
// secret code is typed into the ordinary search field. Reveal then lasts for
// the lifetime of the process only - it is deliberately NOT persisted, so a
// restart re-hides everything.
//
// *** THIS IS A DISPLAY-LAYER FILTER, AND ONLY THAT (ToS 1.4) ***
//
// A hidden conversation keeps receiving messages and keeps its unread state:
// nothing here calls removeFromChatList(), touches read receipts, typing,
// online / last-seen, or any network path. The only thing that changes while a
// row is hidden is its LAYOUT height in the dialogs list (collapsed to zero)
// and whether it is offered as a search result. Unread counters, folder badges
// and the app badge are computed elsewhere and are untouched.
//
// *** FAIL OPEN ***
//
// Every predicate below returns "not hidden" for anything it is unsure about -
// a null peer, an empty locked set, a settings read that came back empty, or
// the revealed flag being on. Losing sight of a conversation is far worse than
// briefly showing one, so the safe direction is always to show.
//
// The locked set is a JSON array of peer-id strings under Store::Private key
// `lockedChats`; the optional dedicated code is Store::Private key
// `chatLockCode`. Both are the same key names Android uses, so a backup carries
// across. Main thread only.

// Is this peer in the locked set (independent of the reveal flag)? The context
// menu uses this to choose between "add" and "remove".
[[nodiscard]] bool IsLocked(PeerData *peer);

// Add / remove a peer to the locked set. Persisted. Fires Changes().
void Lock(PeerData *peer);
void Unlock(PeerData *peer);

// Process-global reveal. Not persisted.
[[nodiscard]] bool Revealed();
void Reveal();

// Should this conversation be hidden from the list and search right now?
// IsLocked() AND not revealed. Fail-open (see above).
[[nodiscard]] bool Hidden(PeerData *peer);
[[nodiscard]] bool Hidden(History *history);

// The secret code: the dedicated `chatLockCode` if one is set, otherwise the
// disguise vault's decoy unlock code as a fallback. May be empty.
[[nodiscard]] QString EffectiveCode();
[[nodiscard]] bool HasSecretCode();

// If there are hidden chats and `text` (trimmed) equals the secret code,
// reveals and returns true so the caller can swallow the query. Otherwise
// returns false and the text is an ordinary search as before.
bool MaybeRevealFromQuery(const QString &text);

// Fires when the locked set OR the reveal flag changes (also on a whole-file
// settings restore, via changesFor).
[[nodiscard]] rpl::producer<> Changes();

} // namespace Lumina::ChatLock
