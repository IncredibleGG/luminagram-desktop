/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_dialogs_visibility.h"

#include "lumina/lumina_settings.h"

#include <QtCore/QJsonValue>

namespace Lumina {
namespace {

const auto kKeyHideChatFolders = u"hideChatFolders"_q;
const auto kKeyHideStories = u"hideStories"_q;

} // namespace

bool HideChatFolders() {
	return Settings::Instance().getBool(kKeyHideChatFolders, false);
}

void SetHideChatFolders(bool value) {
	Settings::Instance().set(kKeyHideChatFolders, value, Store::Prefs);
}

rpl::producer<bool> HideChatFoldersValue() {
	return rpl::single(
		rpl::empty
	) | rpl::then(
		HideChatFoldersChanges()
	) | rpl::map([] {
		return HideChatFolders();
	});
}

rpl::producer<> HideChatFoldersChanges() {
	return Settings::Instance().changesFor(kKeyHideChatFolders);
}

bool HideStories() {
	return Settings::Instance().getBool(kKeyHideStories, false);
}

void SetHideStories(bool value) {
	Settings::Instance().set(kKeyHideStories, value, Store::Prefs);
}

rpl::producer<bool> HideStoriesValue() {
	return rpl::single(
		rpl::empty
	) | rpl::then(
		HideStoriesChanges()
	) | rpl::map([] {
		return HideStories();
	});
}

rpl::producer<> HideStoriesChanges() {
	return Settings::Instance().changesFor(kKeyHideStories);
}

} // namespace Lumina
