/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include <rpl/producer.h>

#include <QtCore/QString>

namespace Ui {
class VerticalLayout;
} // namespace Ui

namespace Window {
class SessionController;
} // namespace Window

namespace Lumina {

// LuminaGram's login guard - the desktop port of Android's
// messenger/LuminaSessionGuard.java + ui/LuminaSessionAlertBox.java.
//
// The threat it answers is the 2026 account-takeover playbook: somebody sends
// you a QR code, calls it a "verification", and the moment it is scanned they
// hold a live authorized session that can read every chat you have. Almost
// nobody opens the "Devices" screen on their own, so the new session sits
// there unnoticed for weeks.
//
// The whole feature is a local diff:
//
//  1. When the application comes to the foreground - throttled to one network
//     round-trip per 30 minutes per account - it asks for the user's OWN
//     authorization list.
//  2. It compares the returned session hashes against a list kept in
//     Lumina::Settings and shows one alert per session it has not seen
//     before, with the device / app / IP / country / time it can show.
//
// ANDROID PARITY, AND THE ONE BEHAVIOUR THAT IS NOT PORTED. Android has a
// second half: a confirmation shown before its QR-login camera opens, because
// scanning a login QR someone sent you hands them a session. Telegram Desktop
// has no such flow at all. Its QR screen (intro/intro_qr.cpp) is the opposite
// direction - the desktop DISPLAYS a code for a phone to scan, using
// auth.exportLoginToken / auth.importLoginToken, so the device being
// authorized is this one and the user is by definition sitting in front of
// it. auth.acceptLoginToken, the request the scanning side sends, appears
// only in mtproto/scheme/api.tl and is called from nowhere in the tree, and
// there is no camera or barcode decoder anywhere in it either. So there is no
// point to warn at, and the LuminaSessionQrConfirm* strings are carried in
// lumina_locale.cpp for key parity with Android but have no caller here.
//
// TOS BOUNDARIES (deliberate, do not "optimise" away):
//
//  * only official API is used, and only through Api::Authorizations - the
//    very object the built-in Settings -> Devices page drives. No raw MTP
//    request is issued from this file;
//  * a session is NEVER terminated automatically. Api::Authorizations
//    ::requestTerminate() is reached from exactly one place: the user
//    pressing "Not me" in the alert. Acting on the account behind the user's
//    back would breach Telegram's ToS 1.4;
//  * nothing is uploaded, logged or shared. The known-session list lives in
//    tdata/luminagram_private.json and never leaves the machine.
//
// The first successful check per account only SEEDS the baseline (every
// session that already exists is marked known), so a fresh install never
// opens a wall of alerts for devices the user has had for years.
//
// Keys, spelled exactly as Android spells them:
//
//   sessionGuardEnabled    Store::Prefs    bool    default TRUE
//   sessionGuardKnown      Store::Private  object  accountId -> [hash, ...]
//   sessionGuardLastCheck  Store::Private  object  accountId -> "<ms>"
//
// Android indexes the last two by its 0..3 account slot; a desktop profile has
// no such slot, so the account id is the logged-in user id and both keys hold
// one object keyed by it rather than one key per account.

[[nodiscard]] bool SessionGuardEnabled();
void SetSessionGuardEnabled(bool value);
[[nodiscard]] rpl::producer<bool> SessionGuardEnabledValue();

// Installs the foreground watcher for `controller`'s account. Idempotent per
// Main::Session: a second window on the same account reuses the first
// window's guard, so two windows cannot produce two alerts for one login.
// The guard dies with the session. Never throws.
void SetupSessionGuard(not_null<Window::SessionController*> controller);

enum class SessionGuardOutcome {
	NewLogins, // At least one unknown session; its alert is already queued.
	NoNew,     // Checked, everything present was already approved.
	Busy,      // A check was already running; this call started nothing.
	Failed,    // The request could not be completed at all.
};

// The manual "check now" path. Ignores the 30-minute throttle and ignores the
// preference - the user asked for this one explicitly. Never throws; `done`
// is always called exactly once, on the main thread.
void SessionGuardCheckNow(
	not_null<Window::SessionController*> controller,
	Fn<void(SessionGuardOutcome)> done);

// True from the moment a check starts until its outcome is delivered.
//
// It exists because the row alone cannot tell that story. A check reports
// itself only when it finishes, the answer arrives over the network, and a
// request that is never answered takes the full 30-second timeout to fail -
// so a press was followed by nothing at all for up to half a minute, which is
// exactly what a broken button looks like. The row watches this instead and
// says so while the check runs, whoever started it: the foreground watcher
// runs the same checks with no callback of its own, and a manual press landing
// on top of one of those gets SessionGuardOutcome::Busy rather than a "no new
// logins" it did not earn.
//
// Never throws. Emits its current value on subscription, and completes if the
// account goes away.
[[nodiscard]] rpl::producer<bool> SessionGuardRunningValue(
	not_null<Window::SessionController*> controller);

// The rows on the LuminaGram security sub-page: the toggle, the manual check,
// and the explanation.
void AddSessionGuardRows(
	not_null<Ui::VerticalLayout*> container,
	not_null<Window::SessionController*> controller);

} // namespace Lumina
