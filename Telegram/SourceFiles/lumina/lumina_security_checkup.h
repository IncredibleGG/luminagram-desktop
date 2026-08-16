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

// The account security checkup, ported from Android: one page that reads the
// account's 2FA, privacy and logged-in-device state from Telegram's own APIs
// and states each on its own row, next to a status the user can act on by
// tapping through to the matching official settings page.
//
// It only reports and navigates - it never changes a setting itself. Every row
// deep-links to the stock page that owns that setting (the cloud-password flow
// for two-step verification and its recovery email, Privacy & Security for the
// who-can-reach-you rules, Active sessions for the devices), so the checkup is
// a map of the account's posture, not a second place to edit it.
//
// State is read live through cloud password state, Api::UserPrivacy and
// Api::Authorizations. Until a value arrives - or if it never does, offline -
// the row shows "Tap to view" rather than guessing, and nothing here can
// crash on a value it did not get.
//
// Called once from Settings::LuminaSecurityCheckup::setupContent().
void AddSecurityCheckupRows(
	not_null<Ui::VerticalLayout*> container,
	not_null<Window::SessionController*> controller);

} // namespace Lumina
