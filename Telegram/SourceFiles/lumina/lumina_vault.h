/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include <rpl/producer.h>

#include <QtCore/QString>
#include <QtGui/QColor>

class QWidget;

namespace Lumina {

// The disguise vault: LuminaGram hides behind a harmless-looking app, and the
// real app only opens for whoever knows the secret code.
//
// Android calls this the same thing (LuminaDecoy / LuminaVaultDoorActivity /
// LuminaCalculatorActivity / LuminaNotepadActivity), and the behaviour here is
// a port of that, minus one piece: migrateLegacyIfNeeded(). No desktop build
// ever wrote `decoyLockEnabled`, so there is nothing to migrate and the legacy
// key is not read anywhere in this tree.
//
// *** WHERE THIS RUNS ***
//
// VaultGatePassed() is called from Core::Application::run() before
// style::StartManager(), before Ui::InitTextOptions() and before the language
// pack is read. At that moment there is a QApplication and an event loop and
// essentially nothing else: no style palette, no fonts registered, no `tr::`
// values, no Ui:: widget infrastructure. Everything the gate draws is
// therefore raw QWidget / QLineEdit / QPushButton / QLabel with hard-coded
// English literals and the platform's own palette. Do not reach for st::, tr::
// or Ui:: from any code the gate can call - it will assert, crash or render
// blank.
//
// *** FAIL OPEN ***
//
// A vault that locks the owner out of their own messenger is far worse than
// one that occasionally does not appear. Every path that cannot present a
// working way to enter the code opens the real app instead: the vault
// disabled, no code configured, no screen to show a window on, an exception
// anywhere, a window that refuses to become visible. The one deliberate
// exception is documented at VaultGatePassed(): once a wrong code has been
// entered at the password door, a decoy that then fails to appear quits rather
// than revealing the app, because at that point the person at the keyboard has
// already demonstrated they do not know the code.
//
// *** WHAT IT IS NOT ***
//
// This defeats a casual look over the shoulder. It is not forensic
// protection - the process is still called LuminaGram in Task Manager, the
// window class and the .desktop / .app identity are unchanged, and anyone with
// read access to the data folder has the account regardless. The settings page
// says so in as many words; see lumina/lumina_vault_settings.cpp.

enum class VaultMode {
	// Ask for a code first; anything wrong opens the decoy.
	PasswordDoor,

	// Show the decoy straight away; the code is typed into it.
	DecoyApp,
};

enum class VaultSkin {
	Notepad,
	Calculator,
};

enum class VaultOutcome {
	// The secret code was entered - open the real app.
	Unlock,

	// Password door only: a wrong code, so show the decoy instead.
	Decoy,

	// The user closed the window - the app should quit.
	Closed,

	// The window could not be built. What that means is the caller's call:
	// for the door, and for a straight-to-decoy launch, it means fail open,
	// because the owner has been left with no way in.
	Failed,
};

[[nodiscard]] bool VaultEnabled();
void SetVaultEnabled(bool value);

[[nodiscard]] VaultMode CurrentVaultMode();
void SetVaultMode(VaultMode mode);

[[nodiscard]] VaultSkin CurrentVaultSkin();
void SetVaultSkin(VaultSkin skin);

// The skin the gate will actually show, which is not always the configured
// one. The calculator keypad cannot produce every code, and a decoy with no
// way in is a lock-out in VaultMode::DecoyApp, so a calculator that could not
// be unlocked is replaced by the notepad. The settings page uses this rather
// than CurrentVaultSkin() so the unlock instructions it prints are the ones
// that will work. See UsableVaultSkin() in lumina_vault.cpp.
[[nodiscard]] VaultSkin EffectiveVaultSkin();

// The code lives in Store::Private. These two are the only writers and the
// only readers of that key: Lumina::Settings::set() defaults to Store::Prefs,
// so a bare set() somewhere else would silently relocate the vault passcode
// into the plaintext pref file.
[[nodiscard]] QString VaultCode();
void SetVaultCode(const QString &code);
[[nodiscard]] bool VaultCodeIsSet();

// Both sides are trimmed. Never log either of them.
[[nodiscard]] bool VaultCodeMatches(const QString &entered);

// The notepad decoy's note body, so the decoy looks lived-in across launches.
// Store::Private for the same reason as the code: it is whatever the user
// typed while they thought they were using a notepad.
[[nodiscard]] QString DecoyNotepadContent();
void SetDecoyNotepadContent(const QString &content);

// Enabled AND a non-empty code. A vault with no code is inert by design: it
// would otherwise be a lock with no key.
[[nodiscard]] bool VaultArmed();

// Fires for any change to any of the preferences above, including a whole-file
// restore through Settings::importAll(). Lives here rather than with the
// settings page so that the key names stay in one file.
[[nodiscard]] rpl::producer<> VaultChanges();

// The gate. True means "carry on into the real app", false means the caller
// must quit. Never throws.
[[nodiscard]] bool VaultGatePassed();

namespace VaultDetail {

struct GateWindowArgs {
	QString title;
	int width = 0;
	int height = 0;
	QChar iconGlyph;
	QColor iconColor;
};

// Titles, sizes, centres and shows a gate window, and gives it a plain
// generated icon so the taskbar entry does not carry the Telegram one. False
// means the window never became visible, which every caller must treat the
// same way it treats a build failure.
[[nodiscard]] bool ShowGateWindow(
	not_null<QWidget*> window,
	const GateWindowArgs &args);

} // namespace VaultDetail
} // namespace Lumina
