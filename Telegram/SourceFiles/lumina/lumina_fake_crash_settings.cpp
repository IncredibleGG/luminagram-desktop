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
// the passcode is edited on a completely different page.
[[nodiscard]] rpl::producer<> Changes() {
	return rpl::merge(
		FakeCrashChanges(),
		Core::App().domain().local().localPasscodeChanged());
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
		const QString &label,
		Fn<bool()> checked,
		Fn<void(bool)> save) {
	const auto button = container->add(object_ptr<Ui::SettingsButton>(
		container,
		rpl::single(label),
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
		const QString &label,
		Fn<QString()> value,
		Fn<void()> activate) {
	::Settings::AddButtonWithLabel(
		container,
		rpl::single(label),
		LabelValue(std::move(value)),
		st::settingsButtonNoIcon
	)->setClickedCallback(std::move(activate));
}

void EditCodeBox(not_null<Ui::GenericBox*> box) {
	box->setTitle(rpl::single(u"Fake-crash code"_q));

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
		rpl::single(u"Code"_q),
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
				u"This code can't be the same as your passcode lock."_q));
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
	lines.append(u"When this is on, typing the fake-crash code on the lock "
		"screen closes LuminaGram straight away instead of unlocking it, "
		"with no message and nothing left on screen - to anyone watching, "
		"the app crashed. The code is separate from your passcode lock, it "
		"is kept on this device only, and it is never sent anywhere."_q);

	// The decision recorded in the port plan (§6 Q3): a fingerprint opens the
	// app without the passcode field ever being touched, and would therefore
	// walk straight past the duress code. Said here because the toggle that
	// causes it is here, and the system-unlock switch that stops working is
	// on a different page entirely.
	lines.append(u"While this is on and a code is set, unlocking with Touch "
		"ID, Windows Hello or your system password is turned off: a "
		"fingerprint opens the app without ever asking for the passcode, so "
		"it would go straight past this code. Clearing the code, or turning "
		"this off, brings it back."_q);

	if (FakeCrashEnabled() && FakeCrashCode().isEmpty()) {
		lines.append(u"No code is set yet, so nothing will happen on the "
			"lock screen."_q);
	}
	if (FakeCrashEnabled() && !HasLocalPasscode()) {
		lines.append(u"LuminaGram has no passcode lock, so the lock screen "
			"never appears and this code is never asked for. Turn on the "
			"passcode lock in Privacy and Security first."_q);
	}
	if (FakeCrashArmed() && CodeIsLocalPasscode(FakeCrashCode())) {
		// Only reachable by changing the passcode lock to the duress code
		// after setting it, which the passcode page knows nothing about.
		lines.append(u"This code is now the same as your passcode lock, so it "
			"will never fire - the passcode unlocks the app instead. Choose a "
			"different code."_q);
	}
	return lines.join(u" "_q);
}

} // namespace

void AddFakeCrashRows(
		not_null<Ui::VerticalLayout*> container,
		not_null<Window::SessionController*> controller) {
	Ui::AddSkip(container);
	Ui::AddSubsectionTitle(container, rpl::single(u"Fake-crash unlock"_q));
	AddToggleRow(
		container,
		u"Enable fake-crash unlock"_q,
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
		u"Fake-crash code"_q,
		[] { return FakeCrashCode().isEmpty() ? u"Not set"_q : u"Set"_q; },
		[=] { controller->show(Box(EditCodeBox)); });

	Ui::AddSkip(container);
	Ui::AddDividerText(container, LabelValue([] { return AboutText(); }));
}

} // namespace Lumina
