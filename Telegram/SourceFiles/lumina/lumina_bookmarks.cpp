/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_bookmarks.h"

#include "base/unixtime.h"
#include "data/data_session.h"
#include "history/history.h"
#include "history/history_item.h"
#include "lumina/lumina_locale.h"
#include "lumina/lumina_message_menu.h"
#include "lumina/lumina_settings.h"
#include "main/main_session.h"
#include "ui/widgets/popup_menu.h"
#include "window/window_session_controller.h"

#include "styles/style_menu_icons.h"

#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
#include <crl/crl_on_main.h>

#include <algorithm>

namespace Lumina {
namespace {

[[nodiscard]] QString StoreKey() {
	return u"bookmarks"_q;
}

[[nodiscard]] QString EnabledKey() {
	return u"showBookmarks"_q;
}

struct State {
	std::vector<Bookmark> list;
	rpl::lifetime lifetime;
	bool loaded = false;
	bool subscribed = false;
};

[[nodiscard]] State &Current() {
	static auto result = State();
	return result;
}

[[nodiscard]] auto Find(
		std::vector<Bookmark> &list,
		uint64 session,
		FullMsgId id) {
	return std::find_if(list.begin(), list.end(), [&](const Bookmark &b) {
		return (b.session == session) && (b.id == id);
	});
}

// A client-side id is reassigned from StartClientMsgId on every launch, so an
// entry under one names a different message in the next run.
[[nodiscard]] bool Storable(uint64 session, FullMsgId id) {
	return session && id.peer && id.msg && !IsClientMsgId(id.msg);
}

void Save() {
	auto &state = Current();
	auto array = QJsonArray();
	for (const auto &entry : state.list) {
		if (!Storable(entry.session, entry.id)) {
			continue;
		}
		auto object = QJsonObject();
		object.insert(u"session"_q, QString::number(entry.session));
		object.insert(u"peer"_q, QString::number(entry.id.peer.value));
		object.insert(u"msg"_q, QString::number(entry.id.msg.bare));
		object.insert(u"snippet"_q, entry.snippet);
		object.insert(u"date"_q, entry.date);
		array.append(object);
	}
	if (array.isEmpty()) {
		Settings::Instance().remove(StoreKey());
	} else {
		Settings::Instance().set(StoreKey(), array, Store::Bookmarks);
	}
}

void EnsureLoaded() {
	auto &state = Current();
	if (state.loaded) {
		return;
	}
	state.loaded = true;
	state.list.clear();

	// Any write through Lumina::Settings fires changes() - ours as well as a
	// W6-E backup import - and the answer to both is the same: forget what is
	// in memory and reparse on the next read. Doing it this way keeps the
	// file, not this vector, as the single source of truth, at the cost of one
	// reparse after each toggle.
	if (!state.subscribed) {
		state.subscribed = true;
		Settings::Instance().changesFor(
			StoreKey()
		) | rpl::on_next([] {
			Current().loaded = false;
		}, state.lifetime);
	}

	const auto array = Settings::Instance().getArray(StoreKey());

	// Two different reasons to stop reading an entry, and they must not be
	// rewritten the same way. `dropped` means the entry was UNUSABLE - bad
	// JSON, a duplicate, an id that can never name a message again - and
	// rewriting the file without it is a cleanup. `truncated` means the entry
	// was perfectly good and only did not fit under kMaxBookmarks, which
	// happens to a store imported from a build or a platform with a larger cap
	// (Android has none at all). Rewriting the file then would DELETE
	// bookmarks the user explicitly asked to keep, which is exactly what
	// lumina_bookmarks.h promises this cap does not do - so the file is left
	// alone and the extra entries survive on disk.
	auto dropped = false;
	auto truncated = false;
	for (auto i = 0, count = int(array.size()); i != count; ++i) {
		const auto object = array.at(i).toObject();
		auto sessionOk = false;
		auto peerOk = false;
		auto msgOk = false;
		const auto session = uint64(
			object.value(u"session"_q).toString().toULongLong(&sessionOk));
		const auto peer = PeerId(
			object.value(u"peer"_q).toString().toULongLong(&peerOk));
		const auto msg = MsgId(
			object.value(u"msg"_q).toString().toLongLong(&msgOk));
		const auto id = FullMsgId(peer, msg);
		if (!sessionOk
			|| !peerOk
			|| !msgOk
			|| !Storable(session, id)
			|| (Find(state.list, session, id) != state.list.end())) {
			dropped = true;
			continue;
		} else if (int(state.list.size()) >= kMaxBookmarks) {
			truncated = true;
			break;
		}
		state.list.push_back(Bookmark{
			.session = session,
			.id = id,
			.snippet = object.value(u"snippet"_q).toString(),
			.date = TimeId(object.value(u"date"_q).toInt()),
		});
	}
	if (dropped && !truncated) {
		// Rewriting the file is never urgent - the entries it drops are
		// already gone from the vector that answers every lookup - and this
		// runs from inside a context-menu build, where Settings::set() firing
		// changes() synchronously would invalidate the vector underneath the
		// caller. So it goes to the next main-thread turn.
		//
		// `!truncated` because Save() writes the whole in-memory vector, which
		// is the one that stops at the cap: cleaning up an unusable entry must
		// not become an excuse to also delete the good ones behind it. A store
		// that is over the cap therefore keeps its unusable entries too, and
		// they are re-skipped on every load, which costs nothing. (A later
		// toggle still calls Save() and truncates then - that is a write the
		// user asked for, and the alternative is refusing to bookmark at all.)
		crl::on_main([] {
			EnsureLoaded();
			Save();
		});
	}
}

} // namespace

bool BookmarksEnabled() {
	return Settings::Instance().getBool(EnabledKey(), true);
}

void SetBookmarksEnabled(bool value) {
	Settings::Instance().set(EnabledKey(), value, Store::Prefs);
}

rpl::producer<> BookmarksEnabledChanges() {
	return Settings::Instance().changesFor(EnabledKey());
}

rpl::producer<> BookmarkChanges() {
	return Settings::Instance().changesFor(StoreKey());
}

bool IsBookmarked(not_null<Main::Session*> session, FullMsgId id) {
	auto &state = Current();
	EnsureLoaded();
	return Find(state.list, session->uniqueId(), id) != state.list.end();
}

bool IsBookmarked(not_null<const HistoryItem*> item) {
	return IsBookmarked(&item->history()->session(), item->fullId());
}

QString BookmarkSnippet(not_null<const HistoryItem*> item) {
	auto result = item->notificationText().text.simplified();
	if (result.size() > kBookmarkSnippetMaxLength) {
		result = result.left(kBookmarkSnippetMaxLength);
		// Qt 5's non-const QString::back() yields a QCharRef, which has
		// no isHighSurrogate(); at() yields a QChar and does.
		if (!result.isEmpty()
			&& result.at(result.size() - 1).isHighSurrogate()) {
			result.chop(1);
		}
		result = result.trimmed();
	}
	return result;
}

BookmarkToggleResult ToggleBookmark(
		not_null<const HistoryItem*> item,
		const QString &snippet) {
	auto &state = Current();
	EnsureLoaded();

	const auto session = item->history()->session().uniqueId();
	const auto id = item->fullId();
	const auto i = Find(state.list, session, id);
	if (i != state.list.end()) {
		state.list.erase(i);
		Save();
		return BookmarkToggleResult::Removed;
	} else if (!Storable(session, id)) {
		return BookmarkToggleResult::Unsupported;
	} else if (int(state.list.size()) >= kMaxBookmarks) {
		return BookmarkToggleResult::Full;
	}
	state.list.push_back(Bookmark{
		.session = session,
		.id = id,
		.snippet = snippet,
		.date = base::unixtime::now(),
	});
	Save();
	return BookmarkToggleResult::Added;
}

void RemoveBookmark(not_null<Main::Session*> session, FullMsgId id) {
	auto &state = Current();
	EnsureLoaded();

	const auto i = Find(state.list, session->uniqueId(), id);
	if (i == state.list.end()) {
		return;
	}
	state.list.erase(i);
	Save();
}

std::vector<Bookmark> BookmarksFor(not_null<Main::Session*> session) {
	auto &state = Current();
	EnsureLoaded();

	const auto unique = session->uniqueId();
	auto result = std::vector<Bookmark>();
	for (auto i = state.list.rbegin(); i != state.list.rend(); ++i) {
		if (i->session == unique) {
			result.push_back(*i);
		}
	}
	return result;
}

int BookmarksCount(not_null<Main::Session*> session) {
	auto &state = Current();
	EnsureLoaded();

	const auto unique = session->uniqueId();
	return int(std::count_if(
		state.list.begin(),
		state.list.end(),
		[&](const Bookmark &b) { return b.session == unique; }));
}

void AddBookmarkMenuRow(
		not_null<Ui::PopupMenu*> menu,
		const MessageMenuContext &context) {
	if (!BookmarksEnabled() || context.hasSelection) {
		return;
	}
	const auto item = context.item;
	const auto session = &item->history()->session();
	const auto itemId = item->fullId();
	const auto bookmarked = IsBookmarked(session, itemId);
	if (!bookmarked && !Storable(session->uniqueId(), itemId)) {
		return;
	} else if (!bookmarked
		&& (int(Current().list.size()) >= kMaxBookmarks)) {
		// Offering "Bookmark" and then refusing every time is worse than
		// not offering it. The row stays for a message that is already
		// bookmarked - removing one is how the user gets back under the cap.
		return;
	}
	const auto restricted = context.copyRestricted;
	const auto controller = context.controller;

	// The two menu builders own the menu they hand us, so they outlive it and
	// the raw captures are safe - this is how every upstream row in them is
	// written. The message is resolved again on click because it can be
	// deleted while the menu is open, and the bookmark state is re-read for
	// the same reason.
	menu->addAction(Tr(bookmarked
		? u"LuminaBookmarkRemove"_q
		: u"LuminaBookmark"_q
	), crl::guard(controller, [=] {
		const auto item = session->data().message(itemId);
		if (!item) {
			// Deleted while the menu was open. A bookmark for it is still
			// removable - that is the only reason the row was showing.
			if (IsBookmarked(session, itemId)) {
				RemoveBookmark(session, itemId);
				controller->showToast(Tr(u"LuminaBookmarkRemoved"_q));
			}
			return;
		}
		const auto result = ToggleBookmark(
			item,
			restricted ? QString() : BookmarkSnippet(item));
		switch (result) {
		case BookmarkToggleResult::Added:
			controller->showToast(Tr(u"LuminaBookmarkAdded"_q));
			break;
		case BookmarkToggleResult::Removed:
			controller->showToast(Tr(u"LuminaBookmarkRemoved"_q));
			break;
		case BookmarkToggleResult::Full:
			controller->showToast(Tr(u"LuminaBookmarksFull"_q));
			break;
		case BookmarkToggleResult::Unsupported:
			break;
		}
	}), bookmarked ? &st::menuIconUnfave : &st::menuIconFave);
}

} // namespace Lumina
