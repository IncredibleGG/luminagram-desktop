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

// The photo-location stripper's rows on the LuminaGram privacy sub-page, in
// the F-02 sub-page shape: settings/sections/settings_lumina_privacy.cpp holds
// this one call and nothing else for this feature.
//
// One toggle, default off. What it removes, what it deliberately leaves in
// place, and why desktop needs it where Android's own string says it does not,
// all live in lumina/lumina_exif_strip.h.
void AddExifStripRows(
	not_null<Ui::VerticalLayout*> container,
	not_null<Window::SessionController*> controller);

} // namespace Lumina
