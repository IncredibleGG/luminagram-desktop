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

// The crypto-address paste guard's rows on the LuminaGram > Privacy sub-page,
// in the F-02 sub-page shape: settings_lumina_privacy.cpp holds one call to
// this and nothing else of ours.
//
// The preference and everything that acts on it live in
// lumina/lumina_clipboard_guard.h; this file only renders the switch. It
// defaults off, so the page is behaviour-neutral until the user touches it.
void AddCryptoGuardRows(
	not_null<Ui::VerticalLayout*> container,
	not_null<Window::SessionController*> controller);

} // namespace Lumina
