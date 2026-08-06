/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_dialogs_badges.h"

#include "lumina/lumina_settings.h"

#include "base/timer.h"
#include "base/unixtime.h"
#include "core/application.h"
#include "data/data_changes.h"
#include "data/data_peer_values.h"
#include "data/data_session.h"
#include "data/data_user.h"
#include "main/main_account.h"
#include "main/main_domain.h"
#include "main/main_session.h"

#include "styles/style_dialogs.h"

#include <QtCore/QJsonValue>

namespace Lumina {
namespace {

const auto kKeyOnlineDot = u"chatListOnlineDot"_q;
const auto kKeyRecencyDot = u"chatListRecencyDot"_q;

constexpr auto kWithinHour = TimeId(60 * 60);
constexpr auto kWithinDay = TimeId(24 * 60 * 60);

constexpr auto kMinRefresh = crl::time(1000);
constexpr auto kMaxRefresh = crl::time(60 * 60 * 1000);

// Transcribed from DialogCell.getRecencyDotColor(). Deliberately not palette
// entries: the dot means "an hour ago" / "a day ago" in the same way a traffic
// light does, the two platforms are meant to be comparable at a glance, and
// lib_ui's palette has no yellow or orange to borrow. The ring around the dot
// is punched out of the frame with CompositionMode_Source, so a fixed colour
// still separates cleanly from any row background, active or not.
[[nodiscard]] QColor WithinHourColor() {
	return QColor(0xFF, 0xC1, 0x07);
}

[[nodiscard]] QColor WithinDayColor() {
	return QColor(0xFF, 0x98, 0x00);
}

struct Watcher {
	base::flat_map<not_null<UserData*>, TimeId> next;
	base::Timer timer;
};

[[nodiscard]] auto Watchers()
-> base::flat_map<not_null<Main::Session*>, std::unique_ptr<Watcher>> & {
	static auto result = base::flat_map<
		not_null<Main::Session*>,
		std::unique_ptr<Watcher>>();
	return result;
}

// Repaints every private chat-list row of every logged in account, by firing
// the update that Dialogs::InnerWidget::setupOnlineStatusCheck() already
// listens to. Only ever called when one of the two preferences is toggled: the
// dot the rows are showing was decided by a value that has just changed, and
// the cached row frames have no other reason to notice.
//
// Not a plain Ui::ForceFullRepaint(): the online preference also changes which
// corner-badge LAYER a row is on, and one repaint would freeze that transition
// half way through its fade. Going through the update instead reaches
// InnerWidget::updateRowCornerStatusShown(), which drops the row image caches,
// re-runs the layer decision and hands the animation a repaint callback.
void RefreshChatListRows() {
	for (const auto &[_, account] : Core::App().domain().accounts()) {
		const auto session = account->maybeSession();
		if (!session) {
			continue;
		}
		auto &owner = session->data();
		owner.enumerateUsers([&](not_null<UserData*> user) {
			if (owner.historyLoaded(user.get())) {
				session->changes().peerUpdated(
					user,
					Data::PeerUpdate::Flag::OnlineStatus);
			}
		});
	}
}

// The hot path asks for these once per visible row per frame, so they are kept
// unpacked here rather than looked up by string in the JSON store every time.
//
// This cache is never the answer to "what is the preference right now" - the
// getters below always read the store, because a settings row subscribed to
// changesFor() may well be notified before this cache is, and a row that reads
// back a stale value flicks itself off again in front of the user. The cache
// only feeds painting, which cannot run before the notification has been
// delivered in full.
struct CachedState {
	bool onlineDot = true;
	bool recencyDot = false;
	rpl::lifetime lifetime;
};

[[nodiscard]] const CachedState &Cached() {
	static auto result = CachedState();
	[[maybe_unused]] static const auto init = [] {
		result.onlineDot = ChatListOnlineDot();
		result.recencyDot = ChatListRecencyDot();
		Settings::Instance().changes(
		) | rpl::on_next([](const QString &key) {
			if (key == kKeyOnlineDot) {
				result.onlineDot = ChatListOnlineDot();
			} else if (key == kKeyRecencyDot) {
				result.recencyDot = ChatListRecencyDot();
			} else {
				return;
			}
			RefreshChatListRows();
		}, result.lifetime);
		return true;
	}();
	return result;
}

// The unixtime at which `user`'s dot is due to change colour, or 0 when it has
// no further changes coming.
//
// The bounds are the first instant at which UserChatListDot() would answer
// something else, so they are one second past that function's inclusive
// comparisons. Getting that wrong in the other direction is not a one second
// error: a refresh fired while the answer is still "within an hour" schedules
// the day boundary next and leaves the dot yellow for the rest of the day.
[[nodiscard]] TimeId NextDotChange(not_null<UserData*> user, TimeId now) {
	if (user->isSelf()
		|| user->isBot()
		|| user->isServiceUser()
		|| user->isSupport()) {
		return 0;
	}
	const auto till = user->lastseen().onlineTill();
	if (till <= 0) {
		return 0;
	} else if (till > now) {
		return till;
	} else if (now - till <= kWithinHour) {
		return till + kWithinHour + 1;
	} else if (now - till <= kWithinDay) {
		return till + kWithinDay + 1;
	}
	return 0;
}

void Reschedule(not_null<Watcher*> watcher, TimeId now) {
	auto nearest = TimeId(0);
	for (const auto &entry : watcher->next) {
		if (!nearest || entry.second < nearest) {
			nearest = entry.second;
		}
	}
	if (!nearest) {
		watcher->timer.cancel();
		return;
	}
	const auto delay = std::clamp(
		crl::time(nearest - now) * crl::time(1000),
		kMinRefresh,
		kMaxRefresh);
	if (watcher->timer.isActive()
		&& watcher->timer.remainingTime() <= delay) {
		return;
	}
	watcher->timer.callOnce(delay);
}

void CheckWatched(not_null<Main::Session*> session) {
	auto &watchers = Watchers();
	const auto i = watchers.find(session);
	if (i == end(watchers)) {
		return;
	}
	const auto watcher = i->second.get();
	const auto now = base::unixtime::now();
	auto due = std::vector<not_null<UserData*>>();
	for (auto j = begin(watcher->next); j != end(watcher->next);) {
		if (j->second > now) {
			++j;
			continue;
		}
		due.push_back(j->first);
		j = watcher->next.erase(j);
	}
	Reschedule(watcher, now);
	for (const auto &user : due) {
		session->changes().peerUpdated(
			user,
			Data::PeerUpdate::Flag::OnlineStatus);
	}
}

} // namespace

bool ChatListOnlineDot() {
	return Settings::Instance().getBool(kKeyOnlineDot, true);
}

void SetChatListOnlineDot(bool value) {
	Settings::Instance().set(kKeyOnlineDot, value, Store::Prefs);
}

rpl::producer<bool> ChatListOnlineDotValue() {
	return rpl::single(
		rpl::empty
	) | rpl::then(
		Settings::Instance().changesFor(kKeyOnlineDot)
	) | rpl::map([] {
		return ChatListOnlineDot();
	});
}

bool ChatListRecencyDot() {
	return Settings::Instance().getBool(kKeyRecencyDot, false);
}

void SetChatListRecencyDot(bool value) {
	Settings::Instance().set(kKeyRecencyDot, value, Store::Prefs);
}

rpl::producer<bool> ChatListRecencyDotValue() {
	return rpl::single(
		rpl::empty
	) | rpl::then(
		Settings::Instance().changesFor(kKeyRecencyDot)
	) | rpl::map([] {
		return ChatListRecencyDot();
	});
}

ChatListDot UserChatListDot(not_null<UserData*> user, TimeId now) {
	if (!now) {
		now = base::unixtime::now();
	}
	const auto &cached = Cached();
	if (Data::IsUserOnline(user, now)) {
		return cached.onlineDot ? ChatListDot::Online : ChatListDot::None;
	} else if (!cached.recencyDot
		|| user->isSelf()
		|| user->isBot()
		|| user->isServiceUser()
		|| user->isSupport()) {
		return ChatListDot::None;
	}
	const auto till = user->lastseen().onlineTill();
	if (till <= 0 || till > now) {
		return ChatListDot::None;
	}
	const auto delta = now - till;
	return (delta <= kWithinHour)
		? ChatListDot::WithinHour
		: (delta <= kWithinDay)
		? ChatListDot::WithinDay
		: ChatListDot::None;
}

bool CornerBadgeDotShown(not_null<UserData*> user, TimeId now) {
	return (UserChatListDot(user, now) != ChatListDot::None);
}

uint64 CornerBadgeDotCacheKeyPart(PeerData *peer) {
	if (!Cached().recencyDot) {
		return 0;
	}
	const auto user = peer ? peer->asUser() : nullptr;
	if (!user) {
		return 0;
	}
	switch (UserChatListDot(user)) {
	case ChatListDot::WithinHour: return (uint64(1) << 32);
	case ChatListDot::WithinDay: return (uint64(2) << 32);
	default: break;
	}
	return 0;
}

QBrush CornerBadgeDotBrush(PeerData *peer, bool active) {
	const auto stock = [&] {
		return active
			? st::dialogsOnlineBadgeFgActive->b
			: st::dialogsOnlineBadgeFg->b;
	};
	if (!Cached().recencyDot) {
		return stock();
	}
	const auto user = peer ? peer->asUser() : nullptr;
	if (!user) {
		return stock();
	}
	switch (UserChatListDot(user)) {
	case ChatListDot::WithinHour: return QBrush(WithinHourColor());
	case ChatListDot::WithinDay: return QBrush(WithinDayColor());
	default: break;
	}
	return stock();
}

void WatchCornerBadgeDot(not_null<UserData*> user, TimeId now) {
	if (!Cached().recencyDot) {
		return;
	} else if (!now) {
		now = base::unixtime::now();
	}
	const not_null<Main::Session*> session = &user->session();
	const auto when = NextDotChange(user, now);
	auto &watchers = Watchers();
	auto i = watchers.find(session);
	if (!when) {
		if (i != end(watchers)) {
			i->second->next.remove(user);
		}
		return;
	} else if (i == end(watchers)) {
		i = watchers.emplace(session, std::make_unique<Watcher>()).first;
		i->second->timer.setCallback([=] { CheckWatched(session); });
		session->lifetime().add([=] { Watchers().remove(session); });
	}
	const auto watcher = i->second.get();
	const auto j = watcher->next.find(user);
	if (j != end(watcher->next)) {
		if (j->second == when) {
			return;
		}
		j->second = when;
	} else {
		watcher->next.emplace(user, when);
	}
	Reschedule(watcher, now);
}

} // namespace Lumina
