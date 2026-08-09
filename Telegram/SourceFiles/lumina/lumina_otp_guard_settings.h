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

// The OTP guard's row on the LuminaGram privacy sub-page. One toggle over the
// `otpGuardEnabled` key, which - unlike every other row on the page - defaults
// to TRUE, because it is a protection and not a preference. The detector, and
// every accessor for that key, live in lumina/lumina_otp_guard.h.
//
// Sits directly after the scam keyword warning, the same neighbour it has on
// Android's privacy page: one row for incoming scam text, one for the outgoing
// login code.
//
// Called once from Settings::LuminaPrivacy::setupContent().
void AddOtpGuardRows(
	not_null<Ui::VerticalLayout*> container,
	not_null<Window::SessionController*> controller);

} // namespace Lumina
