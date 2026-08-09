/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_stories_off.h"

#include "lumina/lumina_dialogs_visibility.h"
#include "lumina/lumina_settings.h"

#include <rpl/map.h>
#include <rpl/merge.h>
#include <rpl/producer.h>
#include <rpl/range.h>
#include <rpl/then.h>

#include <QtCore/QJsonValue>

namespace Lumina {
namespace {

const auto kKeyStoriesFullyOff = u"storiesFullyOff"_q;
const auto kKeyStoriesHidePostEntry = u"storiesHidePostEntry"_q;

struct Cache {
	bool fullyOff = false;
	bool hidePostEntry = false;
	rpl::lifetime lifetime;
};

// StoriesFullyOff() is asked once per avatar per repaint - from
// UserData::hasActiveStories() and ChannelData::hasActiveStories(), which the
// chat list, the profile top bar and every participant list go through - so it
// must not walk the preference store each time. The value is mirrored here and
// refreshed from the change stream instead.
//
// Built on first use and never during static initialisation: the first
// Settings::Instance() constructs a base::Timer, which is a QObject, and doing
// that before QApplication exists kills the app before it can open its log.
[[nodiscard]] Cache &Data() {
	static auto result = Cache();
	[[maybe_unused]] static const auto init = [] {
		auto &settings = Settings::Instance();
		result.fullyOff = settings.getBool(kKeyStoriesFullyOff, false);
		result.hidePostEntry = settings.getBool(
			kKeyStoriesHidePostEntry,
			false);
		settings.changes(
		) | rpl::on_next([](const QString &key) {
			// A backup restore fires this per key just as a toggle does, so
			// there is no separate path to keep in step with this one.
			if (key == kKeyStoriesFullyOff) {
				result.fullyOff = Settings::Instance().getBool(
					kKeyStoriesFullyOff,
					false);
			} else if (key == kKeyStoriesHidePostEntry) {
				result.hidePostEntry = Settings::Instance().getBool(
					kKeyStoriesHidePostEntry,
					false);
			}
		}, result.lifetime);
		return true;
	}();
	return result;
}

} // namespace

bool StoriesFullyOff() {
	return Data().fullyOff;
}

void SetStoriesFullyOff(bool value) {
	Settings::Instance().set(kKeyStoriesFullyOff, value, Store::Prefs);
}

rpl::producer<bool> StoriesFullyOffValue() {
	return rpl::single(
		rpl::empty
	) | rpl::then(
		StoriesFullyOffChanges()
	) | rpl::map([] {
		return StoriesFullyOff();
	});
}

rpl::producer<> StoriesFullyOffChanges() {
	return Settings::Instance().changesFor(kKeyStoriesFullyOff);
}

bool StoriesHidePostEntry() {
	return Data().hidePostEntry;
}

void SetStoriesHidePostEntry(bool value) {
	Settings::Instance().set(kKeyStoriesHidePostEntry, value, Store::Prefs);
}

rpl::producer<bool> StoriesHidePostEntryValue() {
	return rpl::single(
		rpl::empty
	) | rpl::then(
		Settings::Instance().changesFor(kKeyStoriesHidePostEntry)
	) | rpl::map([] {
		return StoriesHidePostEntry();
	});
}

bool StoriesRowHidden() {
	return HideStories() || StoriesFullyOff();
}

rpl::producer<> StoriesRowHiddenChanges() {
	return rpl::merge(HideStoriesChanges(), StoriesFullyOffChanges());
}

bool StoriesPostEntryHidden() {
	const auto &data = Data();
	return data.fullyOff && data.hidePostEntry;
}

rpl::producer<> StoriesPostEntryHiddenChanges() {
	return rpl::merge(
		StoriesFullyOffChanges(),
		Settings::Instance().changesFor(kKeyStoriesHidePostEntry));
}

} // namespace Lumina
