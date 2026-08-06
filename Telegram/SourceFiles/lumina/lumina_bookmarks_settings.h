/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "base/basic_types.h"

namespace Ui {
class VerticalLayout;
} // namespace Ui

namespace Window {
class SessionController;
} // namespace Window

namespace Lumina {

// The bookmarks settings rows, in the F-02 sub-page shape: the sub-page holds
// this one call and nothing else.
//
// Two rows - the list, which pushes the Settings::LuminaBookmarks sub-page,
// and the `showBookmarks` switch that decides whether the message context menu
// carries a Bookmark row at all. The Android surveys found these two in
// different activities (LuminaGramSettingsActivity and LuminaChatActivity); on
// desktop they are one feature with one entry point, because splitting them
// across two sub-pages would mean two files calling into this one.
//
// Every preference behind these rows is declared and defaulted in
// lumina/lumina_bookmarks.h, which is also where the message menu reads them.
void AddBookmarksRows(
	not_null<Ui::VerticalLayout*> container,
	not_null<Window::SessionController*> controller);

} // namespace Lumina
