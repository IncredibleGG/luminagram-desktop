/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_fake_crash_settings.h"

#include "core/application.h"
#include "core/core_settings.h"
#include "lang/lang_keys.h"
#include "lumina/lumina_fake_crash.h"
#include "lumina/lumina_locale.h"
#include "main/main_domain.h"
#include "settings/settings_common.h"
#include "storage/storage_domain.h"
#include "ui/boxes/confirm_box.h"
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

namespace Lumina {
namespace {

// Long enough for a passphrase, short enough that the row label stays honest.
constexpr auto kCodeMaxLength = 64;

// Every label, toggle and paragraph on the block is recomputed from the store
// on any change to it, and on any change to the local passcode - the second
// one matters because two of the warnings below are about the passcode, and
// the passcode is edited on a completely different page. LangChanges() is
// merged in because every text they produce is itself a translated string.
[[nodiscard]] rpl::producer<> Changes() {
	return rpl::merge(
		FakeCrashChanges(),
		Core::App().domain().local().localPasscodeChanged(),
		LangChanges());
}

[[nodiscard]] rpl::producer<QString> LabelValue(Fn<QString()> compute) {
	return rpl::single(
		rpl::empty
	) | rpl::then(
		Changes()
	) | rpl::map([compute = std::move(compute)] {
		return compute();
	});
}

[[nodiscard]] rpl::producer<bool> FlagValue(Fn<bool()> compute) {
	return rpl::single(
		rpl::empty
	) | rpl::then(
		Changes()
	) | rpl::map([compute = std::move(compute)] {
		return compute();
	});
}

[[nodiscard]] bool HasLocalPasscode() {
	const auto &domain = Core::App().domain();
	return domain.started() && domain.local().hasLocalPasscode();
}

// True when `code` would unlock the app.
//
// Storage::Domain::checkPasscode() asserts on key material that only exists
// once the domain has started (storage/storage_domain.cpp:246-252), and there
// is nothing to collide with when no passcode lock is set at all - hence both
// guards, not just the obvious one.
[[nodiscard]] bool CodeIsLocalPasscode(const QString &code) {
	if (code.isEmpty() || !HasLocalPasscode()) {
		return false;
	}
	// Same conversion the lock screen applies to what the user typed
	// (window/window_lock_widgets.cpp, PasscodeLockWidget::submit).
	return Core::App().domain().local().checkPasscode(code.toUtf8());
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

// `Settings` names Lumina::Settings inside this namespace, so the settings
// section helpers have to be reached through the global namespace.
void AddValueRow(
		not_null<Ui::VerticalLayout*> container,
		rpl::producer<QString> label,
		Fn<QString()> value,
		Fn<void()> activate) {
	::Settings::AddButtonWithLabel(
		container,
		std::move(label),
		LabelValue(std::move(value)),
		st::settingsButtonNoIcon
	)->setClickedCallback(std::move(activate));
}

void EditCodeBox(not_null<Ui::GenericBox*> box) {
	box->setTitle(TrValue(u"LuminaSecurityFakeCrashCodeDialogTitle"_q));

	// Shown in the clear, deliberately, and unlike Android - which masks the
	// field. The row behind this box only says "Set", so without this there is
	// no way to find out what the code actually is, and a duress code the user
	// is no longer sure of is a duress code they will not use. It is the same
	// trade the translation page already makes for API keys: masked in the
	// row, legible in the editor. Anyone who can read this box is looking at
	// an unlocked LuminaGram anyway.
	const auto field = box->addRow(object_ptr<Ui::InputField>(
		box,
		st::defaultInputField,
		Ui::InputField::Mode::SingleLine,
		TrValue(u"LuminaSecurityFakeCrashCodeHint"_q),
		FakeCrashCode()));
	field->setMaxLength(kCodeMaxLength);
	box->setFocusCallback([=] {
		field->setFocusFast();
	});

	const auto submit = [=] {
		const auto entered = field->getLastText().trimmed();

		// The duress code must not be the passcode. The lock screen would
		// still behave sanely if it were - it only consults the duress code
		// after the passcode has already been rejected, so a collision costs
		// the feature, not the account - but a code that silently never fires
		// is worth refusing at the one moment we can explain why.
		if (CodeIsLocalPasscode(entered)) {
			box->uiShow()->showBox(Ui::MakeInformBox(
				Tr(u"LuminaSecurityFakeCrashCodeSameAsPasscode"_q)));
			return;
		}
		SetFakeCrashCode(entered);
		box->closeBox();
	};
	field->submits() | rpl::on_next(submit, field->lifetime());

	box->addButton(tr::lng_settings_save(), submit);
	box->addButton(tr::lng_cancel(), [=] {
		box->closeBox();
	});
}

// One reactive paragraph rather than a stack of conditional dividers: the
// warnings come and go as the code, the toggle and the passcode change, and a
// block of dividers appearing and disappearing under the rows reads far worse
// than a paragraph that grows a sentence.
[[nodiscard]] QString AboutText() {
	auto lines = QStringList();
	lines.append(Tr(u"LuminaSecurityFakeCrashInfo"_q));

	// The decision recorded in the port plan (§6 Q3): a fingerprint opens the
	// app without the passcode field ever being touched, and would therefore
	// walk straight past the duress code. Said here because the toggle that
	// causes it is here, and the system-unlock switch that stops working is
	// on a different page entirely.
	lines.append(Tr(u"LuminaSecurityFakeCrashBiometricInfo"_q));

	if (FakeCrashEnabled() && FakeCrashCode().isEmpty()) {
		lines.append(Tr(u"LuminaSecurityFakeCrashNoCodeInfo"_q));
	}
	if (FakeCrashEnabled() && !HasLocalPasscode()) {
		lines.append(Tr(u"LuminaSecurityFakeCrashNoPasscodeInfo"_q));
	}
	if (FakeCrashArmed() && CodeIsLocalPasscode(FakeCrashCode())) {
		// Only reachable by changing the passcode lock to the duress code
		// after setting it, which the passcode page knows nothing about.
		lines.append(Tr(u"LuminaSecurityFakeCrashCodeClashInfo"_q));
	}
	return lines.join(u" "_q);
}

} // namespace

void AddFakeCrashRows(
		not_null<Ui::VerticalLayout*> container,
		not_null<Window::SessionController*> controller) {
	Ui::AddSkip(container);
	Ui::AddSubsectionTitle(
		container,
		TrValue(u"LuminaSecurityFakeCrashHeader"_q));
	AddToggleRow(
		container,
		TrValue(u"LuminaSecurityFakeCrashEnable"_q),
		[] { return FakeCrashEnabled(); },
		[](bool value) { SetFakeCrashEnabled(value); });

	// Built once and toggled reactively, like the provider blocks on the
	// translation page: switching the toggle never rebuilds the block and
	// never invalidates a pointer another row is holding.
	const auto codeBlock = container->add(
		object_ptr<Ui::SlideWrap<Ui::VerticalLayout>>(
			container,
			object_ptr<Ui::VerticalLayout>(container))
	)->toggleOn(
		FlagValue([] { return FakeCrashEnabled(); })
	)->finishAnimating()->entity();
	AddValueRow(
		codeBlock,
		TrValue(u"LuminaSecurityFakeCrashCode"_q),
		[] {
			return Tr(FakeCrashCode().isEmpty()
				? u"LuminaSecurityFakeCrashCodeNotSet"_q
				: u"LuminaSecurityFakeCrashCodeSet"_q);
		},
		[=] { controller->show(Box(EditCodeBox)); });

	Ui::AddSkip(container);
	Ui::AddDividerText(container, LabelValue([] { return AboutText(); }));
}

} // namespace Lumina
