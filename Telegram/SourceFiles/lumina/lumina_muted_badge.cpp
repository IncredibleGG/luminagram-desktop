/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_muted_badge.h"

#include "lumina/lumina_settings.h"

#include <QtCore/QJsonValue>

namespace Lumina {
namespace {

const auto kKeyShowMutedCount = u"showMutedCount"_q;

} // namespace

bool ShowMutedCount() {
	return Settings::Instance().getBool(kKeyShowMutedCount, false);
}

void SetShowMutedCount(bool value) {
	Settings::Instance().set(kKeyShowMutedCount, value, Store::Prefs);
}

rpl::producer<bool> ShowMutedCountValue() {
	return rpl::single(
		rpl::empty
	) | rpl::then(
		ShowMutedCountChanges()
	) | rpl::map([] {
		return ShowMutedCount();
	});
}

rpl::producer<> ShowMutedCountChanges() {
	return Settings::Instance().changesFor(kKeyShowMutedCount);
}

bool UnreadBadgeMuted(bool muted) {
	return muted && !ShowMutedCount();
}

} // namespace Lumina
