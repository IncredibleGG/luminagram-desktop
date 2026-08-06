/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_vault_settings.h"

#include "lang/lang_keys.h"
#include "lumina/lumina_vault.h"
#include "lumina/lumina_vault_calculator.h"
#include "settings/settings_common.h"
#include "ui/boxes/single_choice_box.h"
#include "ui/layers/generic_box.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/fields/input_field.h"
#include "ui/wrap/slide_wrap.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"

#include "styles/style_layers.h"
#include "styles/style_settings.h"
#include "styles/style_widgets.h"

#include <QtCore/QStringList>

namespace Lumina {
namespace {

constexpr auto kCodeMaxLength = 128;

// Every label and toggle is recomputed from the store on any change, so a
// value restored from a backup or written elsewhere is reflected without this
// page having to know which row owns which key.
[[nodiscard]] rpl::producer<QString> TextValue(Fn<QString()> compute) {
	return rpl::single(
		rpl::empty
	) | rpl::then(
		VaultChanges()
	) | rpl::map([compute = std::move(compute)] {
		return compute();
	});
}

[[nodiscard]] rpl::producer<bool> FlagValue(Fn<bool()> compute) {
	return rpl::single(
		rpl::empty
	) | rpl::then(
		VaultChanges()
	) | rpl::map([compute = std::move(compute)] {
		return compute();
	});
}

void AddToggleRow(
		not_null<Ui::VerticalLayout*> container,
		rpl::producer<QString> label,
		Fn<bool()> checked,
		Fn<void(bool)> save) {
	const auto button = container->add(object_ptr<Ui::SettingsButton>(
		container,
		std::move(label),
		st::settingsButtonNoIcon
	))->toggleOn(FlagValue(std::move(checked)));
	button->toggledChanges(
	) | rpl::on_next([save = std::move(save)](bool value) {
		save(value);
	}, button->lifetime());
}

// Sibling files in namespace Lumina have a Lumina::Settings of their own, so
// the settings-section helpers are spelled ::Settings:: throughout lumina/.
void AddValueRow(
		not_null<Ui::VerticalLayout*> container,
		rpl::producer<QString> label,
		Fn<QString()> value,
		Fn<void()> activate) {
	::Settings::AddButtonWithLabel(
		container,
		std::move(label),
		TextValue(std::move(value)),
		st::settingsButtonNoIcon
	)->setClickedCallback(std::move(activate));
}

[[nodiscard]] QString VaultModeName() {
	return (CurrentVaultMode() == VaultMode::DecoyApp)
		? u"Decoy app"_q
		: u"Password door"_q;
}

[[nodiscard]] QString VaultSkinName() {
	return (CurrentVaultSkin() == VaultSkin::Calculator)
		? u"Calculator"_q
		: u"Notepad"_q;
}

[[nodiscard]] QString VaultModeInfo() {
	return (CurrentVaultMode() == VaultMode::DecoyApp)
		? u"Starting LuminaGram opens the decoy straight away. Enter your "
			"secret code in it to reach the real app."_q
		: u"Starting LuminaGram asks for a password. The secret code opens "
			"the real app; anything else opens the decoy, and never says "
			"that it was wrong."_q;
}

// Keyed on the skin the gate will really show, not the one that is selected:
// a calculator whose code cannot be typed on its keypad is replaced by the
// notepad, and printing the calculator instructions there would send the user
// to a decoy they will never see.
[[nodiscard]] QString VaultSkinHint() {
	return (EffectiveVaultSkin() == VaultSkin::Calculator)
		? u"To unlock from the calculator, type the secret code and press "
			"the equals key."_q
		: u"To unlock from the notepad, make the secret code the whole note, "
			"then double-click the Notes title bar."_q;
}

[[nodiscard]] QString VaultInfo() {
	auto lines = QStringList();
	lines.push_back(u"Everything here stays on this device. The vault hides "
		"LuminaGram behind a harmless-looking app, and only the secret code "
		"gets you back into the real one."_q);
	if (!VaultEnabled()) {
		return lines.join(u"\n\n"_q);
	}
	lines.push_back(VaultModeInfo());
	lines.push_back(VaultSkinHint());
	if (!VaultCodeIsSet()) {
		lines.push_back(u"The vault stays off until you set a secret code."_q);
	} else if ((CurrentVaultSkin() == VaultSkin::Calculator)
		&& !CalculatorCanType(VaultCode())) {
		// The password door accepts any code, but the calculator keypad can
		// only produce digits, a dot and the four operators - and it
		// normalises even those. A code it cannot type would leave the decoy
		// with no way in at all, which in VaultMode::DecoyApp is a lock-out,
		// so the gate substitutes the notepad. Say so: the alternative is a
		// user who was promised a calculator and is handed a notepad with no
		// explanation.
		lines.push_back(u"This code cannot be typed on the calculator keypad, "
			"so the notepad decoy is shown instead. Use only digits, a dot "
			"and + - * / if you want the calculator."_q);
	}
	return lines.join(u"\n\n"_q);
}

[[nodiscard]] QString VaultDisclaimer() {
	return u"This defeats a glance over your shoulder, not an informed "
		"inspector: the program is still called LuminaGram in the task "
		"manager, and anyone who can read this device's files can read your "
		"account. The vault appears the next time LuminaGram starts, and "
		"closing the decoy quits LuminaGram. If you ever forget the code, "
		"delete tdata/luminagram.json and tdata/luminagram_private.json from "
		"your LuminaGram data folder: the vault then turns itself off and "
		"LuminaGram starts normally."_q;
}

void ShowModePicker(not_null<Window::SessionController*> controller) {
	const auto options = std::vector<QString>{
		u"Password door"_q,
		u"Decoy app"_q,
	};
	const auto selected = (CurrentVaultMode() == VaultMode::DecoyApp) ? 1 : 0;
	controller->show(Box([=](not_null<Ui::GenericBox*> box) {
		SingleChoiceBox(box, {
			.title = rpl::single(u"Vault mode"_q),
			.options = options,
			.initialSelection = selected,
			.callback = [=](int index) {
				SetVaultMode((index == 1)
					? VaultMode::DecoyApp
					: VaultMode::PasswordDoor);
			},
		});
	}));
}

void ShowSkinPicker(not_null<Window::SessionController*> controller) {
	const auto options = std::vector<QString>{
		u"Notepad"_q,
		u"Calculator"_q,
	};
	const auto selected = (CurrentVaultSkin() == VaultSkin::Calculator)
		? 1
		: 0;
	controller->show(Box([=](not_null<Ui::GenericBox*> box) {
		SingleChoiceBox(box, {
			.title = rpl::single(u"Decoy app style"_q),
			.options = options,
			.initialSelection = selected,
			.callback = [=](int index) {
				SetVaultSkin((index == 1)
					? VaultSkin::Calculator
					: VaultSkin::Notepad);
			},
		});
	}));
}

void EditCodeBox(not_null<Ui::GenericBox*> box) {
	box->setTitle(rpl::single(u"Secret code"_q));

	const auto field = box->addRow(object_ptr<Ui::InputField>(
		box,
		st::defaultInputField,
		Ui::InputField::Mode::SingleLine,
		rpl::single(u"Secret code"_q),
		VaultCode()));
	field->setMaxLength(kCodeMaxLength);
	box->setFocusCallback([=] {
		field->setFocusFast();
	});

	const auto submit = [=] {
		SetVaultCode(field->getLastText().trimmed());
		box->closeBox();
	};
	field->submits() | rpl::on_next(submit, field->lifetime());
	box->addButton(tr::lng_settings_save(), submit);
	box->addButton(tr::lng_cancel(), [=] {
		box->closeBox();
	});
}

} // namespace

void AddVaultRows(
		not_null<Ui::VerticalLayout*> container,
		not_null<Window::SessionController*> controller) {
	Ui::AddSkip(container);
	Ui::AddSubsectionTitle(container, rpl::single(u"Disguise vault"_q));
	AddToggleRow(
		container,
		rpl::single(u"Enable vault"_q),
		[] { return VaultEnabled(); },
		[](bool value) { SetVaultEnabled(value); });

	// Built once and toggled reactively, so turning the vault on never
	// rebuilds the page and never invalidates a pointer another row holds.
	const auto details = container->add(
		object_ptr<Ui::SlideWrap<Ui::VerticalLayout>>(
			container,
			object_ptr<Ui::VerticalLayout>(container))
	)->toggleOn(
		FlagValue([] { return VaultEnabled(); })
	)->finishAnimating()->entity();

	AddValueRow(
		details,
		rpl::single(u"Vault mode"_q),
		[] { return VaultModeName(); },
		[=] { ShowModePicker(controller); });
	AddValueRow(
		details,
		rpl::single(u"Decoy app style"_q),
		[] { return VaultSkinName(); },
		[=] { ShowSkinPicker(controller); });

	// The row shows only whether a code exists; the code itself is legible
	// inside the editor box, which is the same trade the fake-crash page
	// makes for its duress code. A vault code the owner is no longer sure of
	// is a vault they cannot open, and anyone who can reach this page is
	// already looking at an unlocked LuminaGram.
	AddValueRow(
		details,
		rpl::single(u"Secret code"_q),
		[] { return VaultCodeIsSet() ? u"Set"_q : u"Not set"_q; },
		[=] { controller->show(Box(EditCodeBox)); });

	Ui::AddSkip(container);
	Ui::AddDividerText(container, TextValue([] { return VaultInfo(); }));
	Ui::AddSkip(container);
	Ui::AddDividerText(container, rpl::single(VaultDisclaimer()));
}

} // namespace Lumina
