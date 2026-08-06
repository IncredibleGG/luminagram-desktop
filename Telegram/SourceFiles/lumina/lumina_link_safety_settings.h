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

// The link safety inspector's rows on the LuminaGram privacy sub-page, in the
// F-02 sub-page shape: settings/sections/settings_lumina_privacy.cpp holds
// this one call and nothing else for this feature.
//
// One toggle. The preference itself, and every word of what the inspector
// does or does not check, lives in lumina/lumina_link_safety.h.
void AddLinkSafetyRows(
	not_null<Ui::VerticalLayout*> container,
	not_null<Window::SessionController*> controller);

} // namespace Lumina
