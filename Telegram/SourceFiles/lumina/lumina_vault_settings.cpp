/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_vault_settings.h"

#include "lang/lang_keys.h"
#include "lumina/lumina_locale.h"
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
// page having to know which row owns which key. LangChanges() is merged in
// because every text these produce is itself a translated string.
[[nodiscard]] rpl::producer<QString> TextValue(Fn<QString()> compute) {
	return rpl::single(
		rpl::empty
	) | rpl::then(
		rpl::merge(VaultChanges(), LangChanges())
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
	return Tr((CurrentVaultMode() == VaultMode::DecoyApp)
		? u"LuminaVaultModeDecoyApp"_q
		: u"LuminaVaultModePasswordDoor"_q);
}

[[nodiscard]] QString VaultSkinName() {
	return Tr((CurrentVaultSkin() == VaultSkin::Calculator)
		? u"LuminaVaultSkinCalculator"_q
		: u"LuminaVaultSkinNotepad"_q);
}

[[nodiscard]] QString VaultModeInfo() {
	return Tr((CurrentVaultMode() == VaultMode::DecoyApp)
		? u"LuminaVaultModeDecoyAppInfo"_q
		: u"LuminaVaultModePasswordDoorInfo"_q);
}

// Keyed on the skin the gate will really show, not the one that is selected:
// a calculator whose code cannot be typed on its keypad is replaced by the
// notepad, and printing the calculator instructions there would send the user
// to a decoy they will never see.
[[nodiscard]] QString VaultSkinHint() {
	return Tr((EffectiveVaultSkin() == VaultSkin::Calculator)
		? u"LuminaVaultSkinHintCalculator"_q
		: u"LuminaVaultSkinHintNotepad"_q);
}

[[nodiscard]] QString VaultInfo() {
	auto lines = QStringList();
	lines.push_back(Tr(u"LuminaVaultInfo"_q));
	if (!VaultEnabled()) {
		return lines.join(u"\n\n"_q);
	}
	lines.push_back(VaultModeInfo());
	lines.push_back(VaultSkinHint());
	if (!VaultCodeIsSet()) {
		lines.push_back(Tr(u"LuminaVaultNoCodeInfo"_q));
	} else if ((CurrentVaultSkin() == VaultSkin::Calculator)
		&& !CalculatorCanType(VaultCode())) {
		// The password door accepts any code, but the calculator keypad can
		// only produce digits, a dot and the four operators - and it
		// normalises even those. A code it cannot type would leave the decoy
		// with no way in at all, which in VaultMode::DecoyApp is a lock-out,
		// so the gate substitutes the notepad. Say so: the alternative is a
		// user who was promised a calculator and is handed a notepad with no
		// explanation.
		lines.push_back(Tr(u"LuminaVaultCalculatorCodeInfo"_q));
	}
	return lines.join(u"\n\n"_q);
}

void ShowModePicker(not_null<Window::SessionController*> controller) {
	const auto options = std::vector<QString>{
		Tr(u"LuminaVaultModePasswordDoor"_q),
		Tr(u"LuminaVaultModeDecoyApp"_q),
	};
	const auto selected = (CurrentVaultMode() == VaultMode::DecoyApp) ? 1 : 0;
	controller->show(Box([=](not_null<Ui::GenericBox*> box) {
		SingleChoiceBox(box, {
			.title = TrValue(u"LuminaVaultMode"_q),
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
		Tr(u"LuminaVaultSkinNotepad"_q),
		Tr(u"LuminaVaultSkinCalculator"_q),
	};
	const auto selected = (CurrentVaultSkin() == VaultSkin::Calculator)
		? 1
		: 0;
	controller->show(Box([=](not_null<Ui::GenericBox*> box) {
		SingleChoiceBox(box, {
			.title = TrValue(u"LuminaVaultSkin"_q),
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
	box->setTitle(TrValue(u"LuminaVaultSecretCodeDialogTitle"_q));

	const auto field = box->addRow(object_ptr<Ui::InputField>(
		box,
		st::defaultInputField,
		Ui::InputField::Mode::SingleLine,
		TrValue(u"LuminaVaultSecretCode"_q),
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
	Ui::AddSubsectionTitle(container, TrValue(u"LuminaVaultHeader"_q));
	AddToggleRow(
		container,
		TrValue(u"LuminaVaultEnable"_q),
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
		TrValue(u"LuminaVaultMode"_q),
		[] { return VaultModeName(); },
		[=] { ShowModePicker(controller); });
	AddValueRow(
		details,
		TrValue(u"LuminaVaultSkin"_q),
		[] { return VaultSkinName(); },
		[=] { ShowSkinPicker(controller); });

	// The row shows only whether a code exists; the code itself is legible
	// inside the editor box, which is the same trade the fake-crash page
	// makes for its duress code. A vault code the owner is no longer sure of
	// is a vault they cannot open, and anyone who can reach this page is
	// already looking at an unlocked LuminaGram.
	AddValueRow(
		details,
		TrValue(u"LuminaVaultSecretCode"_q),
		[] {
			// The two "Set" / "Not set" values are Android's own keys for
			// this very row, so the two platforms cannot drift apart.
			return Tr(VaultCodeIsSet()
				? u"LuminaDisguiseDecoyCodeSet"_q
				: u"LuminaDisguiseDecoyCodeNotSet"_q);
		},
		[=] { controller->show(Box(EditCodeBox)); });

	Ui::AddSkip(container);
	Ui::AddDividerText(container, TextValue([] { return VaultInfo(); }));
	Ui::AddSkip(container);
	Ui::AddDividerText(container, TrValue(u"LuminaVaultDisclaimer"_q));
}

} // namespace Lumina
