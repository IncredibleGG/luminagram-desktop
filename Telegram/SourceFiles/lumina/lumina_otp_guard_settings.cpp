/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_otp_guard_settings.h"

#include "lumina/lumina_locale.h"
#include "lumina/lumina_otp_guard.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "ui/wrap/vertical_layout.h"

#include "styles/style_settings.h"

namespace Lumina {
namespace {

// Recomputed from the store rather than from the toggle, so the row also
// follows a value written by a backup restore through Settings::importAll()
// and by the panic wipe, which removes the key outright - and removing this
// key restores the protection rather than dropping it, because its default is
// true.
[[nodiscard]] rpl::producer<bool> EnabledValue() {
	return rpl::single(
		rpl::empty
	) | rpl::then(
		OtpGuardChanges()
	) | rpl::map([] {
		return OtpGuardEnabled();
	});
}

} // namespace

void AddOtpGuardRows(
		not_null<Ui::VerticalLayout*> container,
		not_null<Window::SessionController*>) {
	Ui::AddSkip(container);
	const auto button = container->add(object_ptr<Ui::SettingsButton>(
		container,
		TrValue(u"LuminaOtpGuard"_q),
		st::settingsButtonNoIcon
	))->toggleOn(EnabledValue());
	button->toggledChanges(
	) | rpl::on_next([](bool value) {
		SetOtpGuardEnabled(value);
	}, button->lifetime());
	Ui::AddSkip(container);
	Ui::AddDividerText(container, TrValue(u"LuminaOtpGuardInfo"_q));
}

} // namespace Lumina
