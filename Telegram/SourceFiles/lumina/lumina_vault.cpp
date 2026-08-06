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

void SetVaultEnabled(bool value) {
	Settings::Instance().set(kKeyEnabled, value);
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
