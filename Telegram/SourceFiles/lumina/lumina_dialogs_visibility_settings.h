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

// The "hide folder tabs" / "hide stories" block of the LuminaGram Chat list
// sub-page, in the F-02 sub-page shape: the section .cpp holds this one call
// and nothing else. The preferences themselves are declared by
// lumina/lumina_dialogs_visibility.h, which is also what the chat list reads.
void AddDialogsVisibilityRows(
	not_null<Ui::VerticalLayout*> container,
	not_null<Window::SessionController*> controller);

} // namespace Lumina
