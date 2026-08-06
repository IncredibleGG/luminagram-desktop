/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include <rpl/producer.h>

#include <QtCore/QString>

namespace Lumina {

// Fake-crash duress unlock.
//
// A SECOND, purely local code that is typed into the passcode lock screen and,
// instead of unlocking, ends the process on the spot: the window disappears,
// nothing is shown, nothing is written, and to anyone watching the app simply
// died. Android calls it the same thing and fakes an "app has stopped" dialog
// (PasscodeView.showFakeCrash()); on desktop a crashing app leaves no dialog
// of its own, so the faithful port is the silent disappearance and nothing
// else - see TriggerFakeCrash() for why no dialog and no real crash.
//
// Both keys live in Store::Private. The plan's F-01 table only names
// `fakeCrashCode` as sensitive, but `fakeCrashEnabled` is moved with it: the
// flag alone tells a reader of the pref file that a duress code exists, which
// is most of what it should not learn. Every write goes through the setters
// below, which is what keeps a bare set() from relocating either key into the
// ordinary plaintext pref file.
//
// Everything here defaults off, and with it off the passcode screen behaves
// exactly as stock Telegram Desktop.

[[nodiscard]] bool FakeCrashEnabled();
void SetFakeCrashEnabled(bool value);

// Already trimmed, empty when unset. Never logged, never shown in a toast,
// never sent anywhere.
[[nodiscard]] QString FakeCrashCode();
void SetFakeCrashCode(const QString &code); // Empty removes the key.

// Enabled AND a non-empty code stored: the state in which the code can
// actually fire. The toggle on its own does nothing, so this - not
// FakeCrashEnabled() - is what other behaviour keys off.
[[nodiscard]] bool FakeCrashArmed();

// True only when `entered` is exactly the armed duress code. Exact match on
// the trimmed text: the stored code is trimmed when it is saved, and what the
// user typed is trimmed the same way before it is compared.
[[nodiscard]] bool FakeCrashCodeMatches(const QString &entered);

// Ends the process immediately. Does not return, does not unwind, does not
// save, does not log.
[[noreturn]] void TriggerFakeCrash();

// Fires for any change to either key, including a whole-file restore through
// Settings::importAll().
[[nodiscard]] rpl::producer<> FakeCrashChanges();

} // namespace Lumina
