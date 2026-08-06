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

// The "show exact numbers" row of the LuminaGram Appearance sub-page, in the
// F-02 sub-page shape: the section .cpp holds this one call and nothing else.
// The preference itself is declared by lumina/lumina_number_format.h, which is
// also what lang/lang_tag.cpp reads.
//
// No subsection title: on Android this is a plain switch in the Interface list
// rather than the head of a group, and the sub-page's other planned rows are
// separate features owned by separate files.
//
// THE ROW CANNOT REFRESH WHAT IS ALREADY DRAWN, and it does not pretend to.
// Desktop keeps a HistoryView::Element alive for as long as its chat stays
// open, and the counters that carry these numbers cache the formatted string
// rather than rebuilding it per frame - InlineList::setButtonCount() returns
// early when the count itself has not changed, and BottomInfo lays its views
// and replies text out once. So a forced repaint would redraw exactly the old
// strings, and there is no history-wide re-measure reachable from a settings
// page. The divider text says so instead, and the new form appears as chats
// are reopened.
void AddExactNumbersRows(
	not_null<Ui::VerticalLayout*> container,
	not_null<Window::SessionController*> controller);

} // namespace Lumina
