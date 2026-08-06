/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include <rpl/producer.h>

namespace style {
struct DialogRow;
} // namespace style

namespace Lumina {

// Android's LuminaChatListActivity `compactChatList`: a shorter chat-list row,
// so more chats fit on a screen, with the avatar and the message preview both
// still drawn.
//
// WHY THIS IS NOT A .style FILE. Every st::...DialogRow is codegen output from
// dialogs.style, frozen into the binary by style_dialogs.cpp at build time.
// Adding a second, compact set there would be a second frozen set, selected by
// a preference that could then only change on a restart. So the compact set is
// built at RUNTIME instead: DialogRowStyle() hands back a patched copy of the
// codegen struct, rebuilt whenever the palette version or the interface scale
// moves under it (px values in a codegen struct are already scaled, so a copy
// taken at one scale is wrong at another).
//
// WHAT COMPACT CHANGES, AND WHAT IT DELIBERATELY DOES NOT. Only the vertical
// metrics move, plus the avatar's size. The one horizontal change is
// padding.left(), and it is shifted by exactly half of the avatar's shrink, so
//
//     padding.left() + photoSize + padding.left()
//
// - the narrow chat-list column width, computed with that formula in four
// separate places, two of which are outside the dialogs module entirely
// (Window::MainWindow and Window::SessionController) - comes out bit-identical
// to the stock one. nameLeft / textLeft / padding.right() are never touched, so
// text keeps the stock left and right edges and the avatar simply sits centred
// in the same slot it always occupied. That is what lets the ~20 direct readers
// of st::defaultDialogRow outside this feature's own files stay on the stock
// object without ever disagreeing with what is painted.
//
// PIXEL IDENTITY WHEN OFF. Every entry point below short-circuits to its stock
// argument while the preference is off, so nothing is copied, patched, compared
// or rounded, and the chat list is the stock chat list.

// Preference `compactListRows`, Store::Prefs, default false. Main thread only.
[[nodiscard]] bool CompactListRows();
void SetCompactListRows(bool value);
[[nodiscard]] rpl::producer<bool> CompactListRowsValue();

// The row style to actually use in place of `original`.
//
// `original` must be one of the codegen st::...DialogRow objects; anything else
// - including a style this function already returned - is handed straight back,
// which is what makes the call idempotent and safe to apply twice on the same
// paint path. Returns `original` itself while the preference is off.
//
// THE INVARIANT THAT KEEPS HIT-TESTING AND PAINTING TOGETHER: Dialogs::Row's
// height is computed from this function in Row::recountHeight(), and every
// Dialogs::Row painted anywhere goes through RowPainter::Paint(), which runs
// its PaintContext's style through this function too. A caller that never heard
// of this preference - Dialogs::CommunityChatsList picks its own
// st::communityInfo*DialogRow by pointer identity, and is not one of this
// feature's files - therefore still paints at exactly the height its rows were
// laid out at.
[[nodiscard]] const style::DialogRow &DialogRowStyle(
	const style::DialogRow &original);

// Top of a row's right-hand button (the "open bot" / sponsored buttons), whose
// margin is a fixed style value measured from the top of a stock 62px row and
// would hang out of the bottom of a compact one. Clamped against the compact
// default row - never against the row's own style - so that painting
// (dialogs_layout.cpp) and hit-testing (dialogs_inner_widget.cpp) agree for
// tagged and forum rows as well. Returns `margin` unchanged while the
// preference is off.
[[nodiscard]] int DialogRowRightButtonTop(int margin, int buttonHeight);

} // namespace Lumina
