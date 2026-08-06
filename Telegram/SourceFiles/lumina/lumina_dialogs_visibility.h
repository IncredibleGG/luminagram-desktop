/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include <rpl/producer.h>

namespace Lumina {

// Android's hideTabs / hideStories (DialogsActivity::updateFilterTabs and
// DialogsActivity::updateStoriesVisibility): take the chat folder bar, and the
// stories row above the chat list, out of the chat list.
//
// FOLDERS - WHY THIS IS MORE THAN HIDING ONE WIDGET. Desktop draws the folder
// bar in two entirely separate places, and which of them is used is recomputed
// on every window resize: the vertical Window::FiltersMenu owned by
// SessionController, and the horizontal strip owned by Dialogs::Widget. So the
// suppression lives inside both toggleFiltersMenu() implementations rather
// than in the one lambda that usually picks between them - the strip is also
// created straight from the Dialogs::Widget constructor, and the menu straight
// from Window::Controller::setupSideBar(), and neither of those goes through
// that chooser.
//
// Hiding the bar is still not enough on its own. Unlike Android, tdesktop
// keeps the open folder in SessionController::_activeChatsFilter, which
// happily survives the bar disappearing - and for a Premium account whose
// first tab is a folder the app even starts inside one, because
// Data::ChatFilters::defaultId() is the id of the first tab and not
// necessarily "All chats". With the bar hidden and that value left alone the
// user is looking at a filtered chat list with no visible way back out. The
// invariant enforced here is therefore stronger than "do not draw the bar":
// while the preference is on, the active filter is always 0. That is applied
// where the filter is set, so the keyboard shortcuts and the swipe gesture -
// which live in files this feature does not own - cannot put the user back
// inside a folder either.
//
// STORIES. Hidden by adding one term to the `hiddenInstant` expression in
// Dialogs::Widget::updateStoriesVisibility(), which is the same route the row
// already takes when there are no stories to show. NEVER by skipping the
// construction of Dialogs::Widget::_stories: the scroll area chooses its
// overscroll types from whether that pointer exists, once, in the constructor,
// so a list that was never built permanently turns the elastic pull-down into
// an ordinary one - a change that turning the preference back off cannot undo.
//
// BEHAVIOUR NEUTRALITY. Both preferences default to false, and every hook is a
// plain no-op while they are.

// Preference `hideChatFolders`, Store::Prefs, default false.
//
// The key keeps this fork's own spelling rather than Android's `hideTabs`: it
// is the key the old flat LuminaGram settings page already wrote, and
// Lumina::Settings still carries named accessors for it. Reusing it means a
// user who ticked that row before the feature existed now gets the behaviour
// the row promised, instead of a second parallel key and a dead first one.
//
// Main thread only.
[[nodiscard]] bool HideChatFolders();
void SetHideChatFolders(bool value);
[[nodiscard]] rpl::producer<bool> HideChatFoldersValue();
[[nodiscard]] rpl::producer<> HideChatFoldersChanges();

// Preference `hideStories`, Store::Prefs, default false. Same key name as
// Android. Main thread only.
[[nodiscard]] bool HideStories();
void SetHideStories(bool value);
[[nodiscard]] rpl::producer<bool> HideStoriesValue();
[[nodiscard]] rpl::producer<> HideStoriesChanges();

} // namespace Lumina
