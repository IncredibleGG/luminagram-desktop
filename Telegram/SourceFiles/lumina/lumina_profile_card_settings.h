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

// The local profile card rows, in the F-02 sub-page shape: the LuminaGram
// tools sub-page holds this one call and nothing else for W6-F.
//
// One row, showing what the card currently says, that opens the card itself in
// a box - Android's LuminaProfileCardActivity is a page, and a box is what a
// page that small is on desktop. Everything the Android page carries lives in
// that box: the four fields, each editable in its own small box, and the copy
// action.
//
// There is no preference to turn any of this on or off, and nothing outside
// this box reads the card: an untouched card is four empty strings that no
// other code path ever looks at, so the app behaves exactly as it did before
// this feature existed until the user types something into it.
//
// The card itself, what it is for, and why v1 copies rather than sends are all
// documented in lumina/lumina_profile_card.h.
void AddProfileCardRows(
	not_null<Ui::VerticalLayout*> container,
	not_null<Window::SessionController*> controller);

} // namespace Lumina
