/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include <rpl/producer.h>

namespace Lumina {

// Android's storiesFullyOff / storiesHidePostEntry (LuminaConfig): one master
// switch that takes stories out of the whole app, plus a sub-option that also
// hides the ways you would publish your own.
//
// WHY A SECOND KEY INSTEAD OF WIDENING `hideStories`. The obvious shape is to
// turn the existing `hideStories` boolean into a three-way "off / row hidden /
// fully off". Android deliberately did not, because its preference file is
// type-tagged (<boolean name="hideStories" .../>) and its encrypted backup
// carries those tags: an old backup restored into a build that had rewritten
// the key as an int would come back as "not an int" and read as OFF - the
// user's setting lost, with no error anywhere to notice it by.
//
// That reasoning transfers to this fork exactly, because our store is typed in
// the same way. Lumina::Settings holds raw QJsonValues and its readers are
// type-checked: getBool() is `value.isBool() ? value.toBool() : def` and
// getInt() is `value.isDouble() ? value.toInt() : def`
// (lumina/lumina_settings.cpp). A `hideStories` written as `true` and later
// read as an int therefore yields the default, not 1. And the backup carries
// the type verbatim - lumina_backup restores through Settings::importAll(),
// which writes the QJsonValue as it found it - so every backup ever taken
// keeps the old type forever, including backups written by a phone.
//
// `storiesFullyOff` and `storiesHidePostEntry` are consequently new,
// independent booleans, with the same key names and the same meaning as
// Android, and `hideStories` keeps meaning exactly what it always meant.
// Turning the master switch on implies the row is hidden; turning it off
// leaves whatever `hideStories` was set to, untouched.
//
// PURELY LOCAL. Nothing here changes server state and nothing calls an API.
// Stories are simply never drawn, never counted and never announced. Yours and
// everyone else's are still there, and still there for every other client.
//
// Main thread only. StoriesFullyOff() is read from draw paths - every avatar,
// on every chat-list repaint - so it is answered from a cached bool rather
// than from a preference lookup, exactly as Android reads a plain static
// field there.

// Preference `storiesFullyOff`, Store::Prefs, default false. Same key name as
// Android.
[[nodiscard]] bool StoriesFullyOff();
void SetStoriesFullyOff(bool value);
[[nodiscard]] rpl::producer<bool> StoriesFullyOffValue();
[[nodiscard]] rpl::producer<> StoriesFullyOffChanges();

// Preference `storiesHidePostEntry`, Store::Prefs, default false. Same key
// name as Android. It only does anything while StoriesFullyOff(), which is
// also the only time its row is offered - read it through
// StoriesPostEntryHidden() rather than directly.
[[nodiscard]] bool StoriesHidePostEntry();
void SetStoriesHidePostEntry(bool value);
[[nodiscard]] rpl::producer<bool> StoriesHidePostEntryValue();

// Android's isStoriesRowHidden(): the stories row above the chat list, and the
// one the same widget shows while the archive is open, are hidden. Either
// preference on its own is enough.
[[nodiscard]] bool StoriesRowHidden();
[[nodiscard]] rpl::producer<> StoriesRowHiddenChanges();

// Android's isStoriesPostEntryHidden(): the entries that lead to publishing
// your own stories are hidden.
//
// Telegram Desktop cannot post a story at all - there is no camera button on
// the chat list or on a profile, and nothing in the tree calls
// stories.sendStory - so this covers less ground here than it does on the
// phone. What it does cover is the one posting-side entry desktop has: the
// stories archive of a channel you can post to, which is where a desktop user
// publishes (a story is posted to the page FROM that archive).
[[nodiscard]] bool StoriesPostEntryHidden();
[[nodiscard]] rpl::producer<> StoriesPostEntryHiddenChanges();

} // namespace Lumina
