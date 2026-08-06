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

// The W5-G switch, on the LuminaGram > Chats sub-page, under the same "Media"
// subsection title the phone puts it under (LuminaChatActivity).
//
// The switch only decides whether the "Save sticker" row is offered in the
// sticker panel's context menu; it changes nothing else. It defaults off, so
// the page is behaviour-neutral until the user touches it.
void AddStickerSaveRows(
	not_null<Ui::VerticalLayout*> container,
	not_null<Window::SessionController*> controller);

} // namespace Lumina
