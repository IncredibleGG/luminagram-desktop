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

// The "keep more recent stickers and GIFs" row, in the F-02 sub-page shape:
// the settings section holds this one call and nothing else for this feature.
//
// One toggle. The preference, the two floors, the four trim points and the
// reason the premium upsell toast goes quiet all live in
// lumina/lumina_recent_limits.h.
void AddRecentLimitsRows(
	not_null<Ui::VerticalLayout*> container,
	not_null<Window::SessionController*> controller);

} // namespace Lumina
