/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

namespace Ui {
class VerticalLayout;
} // namespace Ui

namespace Window {
class SessionController;
} // namespace Window

namespace Lumina {

// The impersonation-name warning's row on the LuminaGram privacy sub-page. One
// toggle over the `homoglyphWarn` key, which - like the OTP guard beside it -
// defaults to TRUE, because it is a protection and not a preference. The
// detector, and every accessor for that key, live in lumina/lumina_homoglyph.h.
//
// Sits directly after the OTP guard: both are offline, on-device warnings that
// never block or change anything.
//
// Called once from Settings::LuminaPrivacy::setupContent().
void AddHomoglyphWarnRows(
	not_null<Ui::VerticalLayout*> container,
	not_null<Window::SessionController*> controller);

} // namespace Lumina
