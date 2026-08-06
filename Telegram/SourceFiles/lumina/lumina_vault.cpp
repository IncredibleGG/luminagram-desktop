/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_vault.h"

#include "lumina/lumina_settings.h"
#include "lumina/lumina_vault_calculator.h"
#include "lumina/lumina_vault_door.h"
#include "lumina/lumina_vault_notepad.h"

#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>
#include <QtWidgets/QWidget>

namespace Lumina {
namespace {

const auto kKeyEnabled = u"vaultEnabled"_q;
const auto kKeyMode = u"vaultMode"_q;
const auto kKeySkin = u"decoySkin"_q;
const auto kKeyCode = u"decoyUnlockCode"_q;
const auto kKeyNotepadContent = u"decoyNotepadContent"_q;

const auto kModePasswordDoor = u"passwordDoor"_q;
const auto kModeDecoyApp = u"decoyApp"_q;
const auto kSkinNotepad = u"notepad"_q;
const auto kSkinCalculator = u"calculator"_q;

constexpr auto kIconSize = 64;
constexpr auto kIconInset = 2;
constexpr auto kIconRadius = 12;
constexpr auto kIconGlyphSize = 36;

[[nodiscard]] QIcon GenerateIcon(QChar glyph, QColor color) {
	const auto side = kIconSize - 2 * kIconInset;
	auto pixmap = QPixmap(kIconSize, kIconSize);
	pixmap.fill(Qt::transparent);

	auto p = QPainter(&pixmap);
	p.setRenderHint(QPainter::Antialiasing, true);
	p.setPen(Qt::NoPen);
	p.setBrush(color);
	const auto box = QRectF(kIconInset, kIconInset, side, side);
	p.drawRoundedRect(box, kIconRadius, kIconRadius);
	if (!glyph.isNull()) {
		auto font = QFont();
		font.setPixelSize(kIconGlyphSize);
		font.setBold(true);
		p.setFont(font);
		p.setPen(QColor(255, 255, 255));
		p.drawText(box, Qt::AlignCenter, QString(glyph));
	}
	p.end();

	return QIcon(pixmap);
}

// Nothing in this tree ever calls setQuitOnLastWindowClosed(), so it is Qt's
// default, which is true - and every gate window is the only window that
// exists while it is up. Qt reacts to the last one going away by calling
// QCoreApplication::exit(), and exit() does two things that are fatal here: it
// exits every event loop currently on this thread's loop stack, and it leaves
// QThreadData::quitNow set, which makes every LATER QEventLoop::exec() on this
// thread return immediately without processing anything.
//
// Both of the gate's window-to-window handovers hide the outgoing window, and
// both then destroy it once the run function returns:
//
//   * password door -> decoy, after a wrong code. If the hide or the destroy
//     trips that path, the decoy's own loop.exec() returns at once, the decoy
//     is never operable, and RunGate() reports Closed - a wrong code quits the
//     app instead of showing the decoy it exists to show;
//   * any unlock. The gate returns true, Application::run() carries on, and
//     the main event loop it eventually returns to has already been told to
//     exit - the app disappears moments after the correct code was accepted.
//
// Whether a plain hide() is enough to trip it depends on the Qt version, and
// that is exactly the kind of dependency this must not have: the failure is
// silent, it is version-dependent, and one of its two forms costs the owner
// their unlock. So the gate does not rely on the answer. It suppresses the
// behaviour for its whole duration - including the destruction of the last
// gate window, which happens inside the Run... functions - and restores the
// previous value on the way out.
//
// Nothing is lost by suppressing it: the caller quits explicitly. RunGate()
// returning false makes Core::Application::run() call Core::Quit(), which
// reaches Sandbox::QuitWhenStarted() and the same QCoreApplication::exit(0)
// (core/application.cpp:279-282, core/sandbox.cpp:230-243). Sandbox::_started
// is already true by then; it is set at core/sandbox.cpp:180, immediately
// before the exec() whose event loop is running us.
class QuitOnLastWindowSuppression final {
public:
	QuitOnLastWindowSuppression()
	: _was(QGuiApplication::quitOnLastWindowClosed()) {
		QGuiApplication::setQuitOnLastWindowClosed(false);
	}
	QuitOnLastWindowSuppression(const QuitOnLastWindowSuppression &) = delete;
	QuitOnLastWindowSuppression &operator=(
		const QuitOnLastWindowSuppression &) = delete;
	~QuitOnLastWindowSuppression() {
		QGuiApplication::setQuitOnLastWindowClosed(_was);
	}

private:
	const bool _was = false;

};

[[nodiscard]] VaultOutcome RunSkin(VaultSkin skin) {
	try {
		return (skin == VaultSkin::Calculator)
			? RunVaultCalculator()
			: RunVaultNotepad();
	} catch (...) {
		return VaultOutcome::Failed;
	}
}

[[nodiscard]] VaultOutcome RunDoor() {
	try {
		return RunVaultDoor();
	} catch (...) {
		return VaultOutcome::Failed;
	}
}

[[nodiscard]] bool RunGate() {
	if (!VaultArmed()) {
		return true;
	} else if (!QGuiApplication::primaryScreen()) {
		LOG(("Lumina Warning: No screen for the vault, opening the app."));
		return true;
	}
	const auto suppression = QuitOnLastWindowSuppression();
	const auto skin = EffectiveVaultSkin();
	if (skin != CurrentVaultSkin()) {
		LOG(("Lumina Warning: Vault code cannot be entered on the chosen "
			"decoy, showing the notepad instead."));
	}
	if (CurrentVaultMode() == VaultMode::PasswordDoor) {
		switch (RunDoor()) {
		case VaultOutcome::Unlock:
			return true;
		case VaultOutcome::Closed:
			return false;
		case VaultOutcome::Failed:
			LOG(("Lumina Error: Vault door failed, opening the app."));
			return true;
		case VaultOutcome::Decoy:
			break;
		}

		// A wrong code was entered. Unlike everywhere else, a decoy that
		// cannot be built must NOT open the real app here: whoever is at the
		// keyboard has already shown they do not know the code, and the owner
		// still has the door on the next launch, so nobody is locked out.
		if (RunSkin(skin) == VaultOutcome::Unlock) {
			return true;
		}
		return false;
	}
	switch (RunSkin(skin)) {
	case VaultOutcome::Unlock:
		return true;
	case VaultOutcome::Failed:
		LOG(("Lumina Error: Vault decoy failed, opening the app."));
		return true;
	default:
		return false;
	}
}

} // namespace

bool VaultEnabled() {
	return Settings::Instance().getBool(kKeyEnabled, false);
}

// Flushed rather than left to the ~500ms coalescing timer, here and in the
// other three setters below. All four are read by the gate once, at the very
// start of the NEXT launch, and the two ways this application ends without
// draining that timer are both reachable from the screen the vault is
// configured on: Lumina::TriggerFakeCrash() calls std::_Exit(), which runs no
// destructor, and a real crash does the same.
//
// Losing any of them fails open in the sense that nothing is destroyed, but
// "fails open" is not the same as "harmless", and it is worst for the mode:
// a user who picked VaultMode::DecoyApp picked it precisely so that no
// password box ever appears, and a lost write hands them the password door
// instead - the disguise inverted, silently, with every switch still showing
// what they chose. Three small JSON files per toggle is not a price worth
// haggling over for that.
void SetVaultEnabled(bool value) {
	Settings::Instance().set(kKeyEnabled, value);
	Settings::Instance().saveNow();
}

VaultMode CurrentVaultMode() {
	const auto stored = Settings::Instance().getString(
		kKeyMode,
		kModePasswordDoor);
	return (stored == kModeDecoyApp)
		? VaultMode::DecoyApp
		: VaultMode::PasswordDoor;
}

void SetVaultMode(VaultMode mode) {
	Settings::Instance().set(
		kKeyMode,
		(mode == VaultMode::DecoyApp) ? kModeDecoyApp : kModePasswordDoor);
	Settings::Instance().saveNow();
}

VaultSkin CurrentVaultSkin() {
	const auto stored = Settings::Instance().getString(
		kKeySkin,
		kSkinNotepad);
	return (stored == kSkinCalculator)
		? VaultSkin::Calculator
		: VaultSkin::Notepad;
}

void SetVaultSkin(VaultSkin skin) {
	Settings::Instance().set(
		kKeySkin,
		(skin == VaultSkin::Calculator) ? kSkinCalculator : kSkinNotepad);
	Settings::Instance().saveNow();
}

// The password door accepts any code, but the calculator keypad can only
// produce digits, a dot and + - * / and normalises even those, so a code it
// cannot type leaves that skin with no unlock in it at all. In
// VaultMode::DecoyApp that is a permanent lock-out: the decoy is the only
// thing the owner ever sees, and the only way back into the account is to
// find and delete the pref file by hand. The settings page warns about the
// combination, but a warning the user scrolled past must not cost them their
// account, so the notepad - which takes any code at all as its note body -
// stands in instead.
VaultSkin EffectiveVaultSkin() {
	const auto skin = CurrentVaultSkin();
	if ((skin == VaultSkin::Calculator) && !CalculatorCanType(VaultCode())) {
		return VaultSkin::Notepad;
	}
	return skin;
}

QString VaultCode() {
	return Settings::Instance().getString(kKeyCode).trimmed();
}

void SetVaultCode(const QString &code) {
	const auto trimmed = code.trimmed();
	if (trimmed.isEmpty()) {
		Settings::Instance().remove(kKeyCode);
	} else {
		Settings::Instance().set(kKeyCode, trimmed, Store::Private);
	}
	Settings::Instance().saveNow();
}

bool VaultCodeIsSet() {
	return !VaultCode().isEmpty();
}

bool VaultCodeMatches(const QString &entered) {
	const auto code = VaultCode();
	return !code.isEmpty() && (entered.trimmed() == code);
}

QString DecoyNotepadContent() {
	return Settings::Instance().getString(kKeyNotepadContent);
}

void SetDecoyNotepadContent(const QString &content) {
	if (content.isEmpty()) {
		Settings::Instance().remove(kKeyNotepadContent);
	} else {
		Settings::Instance().set(kKeyNotepadContent, content, Store::Private);
	}
}

bool VaultArmed() {
	return VaultEnabled() && VaultCodeIsSet();
}

rpl::producer<> VaultChanges() {
	return Settings::Instance().changes(
	) | rpl::filter([](const QString &key) {
		return (key == kKeyEnabled)
			|| (key == kKeyMode)
			|| (key == kKeySkin)
			|| (key == kKeyCode);
	}) | rpl::to_empty;
}

bool VaultGatePassed() {
	try {
		return RunGate();
	} catch (...) {
		LOG(("Lumina Error: Vault gate threw, opening the app."));
		return true;
	}
}

namespace VaultDetail {

bool ShowGateWindow(
		not_null<QWidget*> window,
		const GateWindowArgs &args) {
	window->setWindowTitle(args.title);
	if (args.iconColor.isValid()) {
		window->setWindowIcon(GenerateIcon(args.iconGlyph, args.iconColor));
	}
	window->resize(args.width, args.height);
	window->setMinimumSize(args.width / 2, args.height / 2);
	if (const auto screen = QGuiApplication::primaryScreen()) {
		const auto available = screen->availableGeometry();
		window->move(
			available.x() + (available.width() - args.width) / 2,
			available.y() + (available.height() - args.height) / 2);
	}
	window->show();
	window->raise();
	window->activateWindow();
	return window->isVisible();
}

} // namespace VaultDetail
} // namespace Lumina
