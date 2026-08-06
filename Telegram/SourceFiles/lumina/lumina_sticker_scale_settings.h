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

// The sticker-size rows on the LuminaGram appearance sub-page, in the F-02
// sub-page shape: settings/sections/settings_lumina_appearance.cpp holds this
// one call and nothing else for this feature.
//
// Four radio rows, 75 / 100 / 125 / 150 per cent, exactly the choices Android
// offers, with 100 - "leave stickers the size they have always been" - the
// default. Picking a different one saves immediately and then offers a
// restart, because the size is latched for the run; why it has to be is
// written out in lumina/lumina_sticker_scale.h.
void AddStickerScaleRows(
	not_null<Ui::VerticalLayout*> container,
	not_null<Window::SessionController*> controller);

} // namespace Lumina
