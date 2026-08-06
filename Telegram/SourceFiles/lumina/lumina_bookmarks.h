/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "data/data_msg_id.h"

#include <rpl/producer.h>

#include <QtCore/QString>

#include <vector>

class HistoryItem;

namespace Main {
class Session;
} // namespace Main

namespace Ui {
class PopupMenu;
} // namespace Ui

namespace Lumina {

struct MessageMenuContext;

// Message bookmarks - a local, client-side alternative to Saved Messages.
//
// A bookmark is a REFERENCE, not a copy: (session, peer, message id) plus a
// short snippet and the moment it was made. Nothing is ever sent to Telegram,
// no message is forwarded anywhere, and the bookmarked message itself is left
// completely untouched - which is the whole reason the feature exists next to
// Saved Messages rather than on top of it.
//
//
// KEY
//
// (session unique id, peer id, message id). The session half is not optional
// on desktop: Android has one account per process and keys by dialogId alone,
// while here two logged-in accounts share one preference file, hand out local
// message ids from the same per-account counter, and can both be members of
// the same chat - so a key without the session would show account B's
// bookmarks in account A's list and let a removal from one delete the other's
// entry. Hence Main::Session::uniqueId() in the key, and BookmarksFor() takes
// the session it is listing for.
//
//
// WHAT IS PERSISTED
//
// Store::Bookmarks, key `bookmarks` (tdata/luminagram_bookmarks.json). That
// store exists precisely for this: an array that grows without bound and is
// rewritten on every toggle must not force a rewrite of the ordinary pref
// file. One array entry per bookmark, oldest first, matching Android's
// insertion order:
//
//   { "session": "<uint64>", "peer": "<uint64>", "msg": "<int64>",
//     "snippet": "<text>", "date": <TimeId seconds> }
//
// The three ids are strings because PeerId and MsgId are 64-bit values with
// high tag bits, and JSON numbers are doubles - a channel peer id round-trips
// through a double as a different channel. `date` is seconds since the epoch
// (TimeId), not Android's milliseconds; every other timestamp on desktop is a
// TimeId and mixing the two units in one file is how somebody eventually
// renders a bookmark as 1970.
//
// A client-side message id is never stored. Data::Session hands those out from
// a counter that restarts at StartClientMsgId on every launch, so an id
// persisted in one run names a completely different message in the next one -
// the list would navigate to a stranger. The menu row is only ever built for
// items the F-04 seam already accepted (isRegular(), so nothing local and
// nothing scheduled), and the store re-checks it anyway on save and on load.
//
// The snippet is the only CONTENT in the file, and luminagram_bookmarks.json
// is plaintext - see the encryption-at-rest question in the port plan. Two
// consequences are deliberate:
//
//  * A message from a chat with copy restrictions is bookmarked WITHOUT its
//    snippet. The bookmark itself is only a pointer to a message the user can
//    already open, so it is not a restriction bypass; a stored copy of the
//    text on disk would be one. Such a row falls back to the chat name and the
//    date, which is exactly the degraded rendering a deleted message gets.
//  * The store is included in Settings::exportAll(), so the W6-E backup
//    carries the snippets too.
//
//
// RETENTION
//
// Bookmarks are explicit user artifacts, so nothing here ages them out the way
// the translation-originals store does - a two-year-old bookmark is the point
// of the feature. The only bound is a hard cap of kMaxBookmarks entries across
// all accounts, and reaching it REFUSES the new bookmark (Full below) instead
// of silently dropping the oldest one. Android has no cap at all;
// dropping data the user explicitly asked to keep, without telling them, is
// worse than declining to add one more.
//
//
// BEHAVIOUR NEUTRALITY
//
// `showBookmarks` is opt-OUT and defaults to true, which the F-01 defaults
// table already expresses (lumina_settings.cpp) and which matches Android
// (LuminaConfig.getBoolean("showBookmarks", true)). It is the one preference
// in this item that is not off by default: W4-A's entire user-visible surface
// is the menu row and the list, so defaulting it off would ship the feature
// switched off for everyone. With it off, AddBookmarkMenuRow() returns without
// adding anything and the store is never even loaded.
struct Bookmark {
	uint64 session = 0;
	FullMsgId id;
	QString snippet;
	TimeId date = 0;
};

// How much of a message is kept as the row's preview. Android's number.
inline constexpr auto kBookmarkSnippetMaxLength = 140;

// Across every account, because one file holds them all.
inline constexpr auto kMaxBookmarks = 1000;

[[nodiscard]] bool BookmarksEnabled();
void SetBookmarksEnabled(bool value);
[[nodiscard]] rpl::producer<> BookmarksEnabledChanges();

// Fires whenever the stored array changes, including a W6-E backup import.
[[nodiscard]] rpl::producer<> BookmarkChanges();

[[nodiscard]] bool IsBookmarked(not_null<const HistoryItem*> item);
[[nodiscard]] bool IsBookmarked(
	not_null<Main::Session*> session,
	FullMsgId id);

// The preview text this item would be bookmarked with: its message text, or
// what the chat list would show for its media ("Photo", "Voice message", ...).
// Whitespace-collapsed and cut to kBookmarkSnippetMaxLength without splitting
// a surrogate pair.
[[nodiscard]] QString BookmarkSnippet(not_null<const HistoryItem*> item);

enum class BookmarkToggleResult {
	Added,
	Removed,
	Full, // kMaxBookmarks reached; the row is not offered in that state.
	Unsupported, // No stable message id. Unreachable through the menu row.
};

// Pass an empty snippet to store a reference with no preview text - which is
// what a copy-restricted chat gets.
BookmarkToggleResult ToggleBookmark(
	not_null<const HistoryItem*> item,
	const QString &snippet);

void RemoveBookmark(not_null<Main::Session*> session, FullMsgId id);

// Newest first, which is the order the list shows them in.
[[nodiscard]] std::vector<Bookmark> BookmarksFor(
	not_null<Main::Session*> session);
[[nodiscard]] int BookmarksCount(not_null<Main::Session*> session);

// The F-04 message-menu row. Its signature is the one lumina_message_menu.h
// declares for the AddBookmarkRow() stub, which forwards here; that file is
// shared by three items and is not ours to edit.
void AddBookmarkMenuRow(
	not_null<Ui::PopupMenu*> menu,
	const MessageMenuContext &context);

} // namespace Lumina
