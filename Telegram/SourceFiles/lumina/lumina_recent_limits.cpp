/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_recent_limits.h"

#include "lumina/lumina_settings.h"

namespace Lumina {
namespace {

const auto kKeyEnabled = u"moreRecentStickers"_q;

[[nodiscard]] int Boost(int stockLimit, int minimum) {
	return (stockLimit >= minimum || !MoreRecentEnabled())
		? stockLimit
		: minimum;
}

} // namespace

bool MoreRecentEnabled() {
	return Settings::Instance().getBool(kKeyEnabled, false);
}

void SetMoreRecentEnabled(bool value) {
	Settings::Instance().set(kKeyEnabled, value);
}

rpl::producer<> MoreRecentChanges() {
	return Settings::Instance().changesFor(kKeyEnabled);
}

int RecentStickersLimit(int stockLimit) {
	return Boost(stockLimit, kRecentStickersFloor);
}

int SavedGifsLimit(int stockLimit) {
	return Boost(stockLimit, kSavedGifsFloor);
}

} // namespace Lumina
