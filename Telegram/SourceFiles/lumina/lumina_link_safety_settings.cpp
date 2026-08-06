/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_link_safety_settings.h"

#include "lumina/lumina_link_safety.h"
#include "ui/widgets/buttons.h"
#include "ui/wrap/vertical_layout.h"
#include "ui/vertical_list.h"

#include "styles/style_settings.h"

namespace Lumina {
namespace {

// Recomputed from the store rather than from the toggle, so the row also
// follows a value written by a backup restore through Settings::importAll().
[[nodiscard]] rpl::producer<bool> EnabledValue() {
	return rpl::single(
		rpl::empty
	) | rpl::then(
		LinkSafetyChanges()
	) | rpl::map([] {
		return LinkSafetyEnabled();
	});
}

} // namespace

void AddLinkSafetyRows(
		not_null<Ui::VerticalLayout*> container,
		not_null<Window::SessionController*>) {
	Ui::AddSkip(container);
	const auto button = container->add(object_ptr<Ui::SettingsButton>(
		container,
		rpl::single(u"Link safety inspector"_q),
		st::settingsButtonNoIcon
	))->toggleOn(EnabledValue());
	button->toggledChanges(
	) | rpl::on_next([](bool value) {
		SetLinkSafetyEnabled(value);
	}, button->lifetime());
	Ui::AddSkip(container);
	Ui::AddDividerText(
		container,
		rpl::single(u"Ask before opening a link that hides its real "
			"destination behind text before an @ sign, or that goes through "
			"a known link shortener. The confirmation shows the full "
			"address. Telegram Desktop already asks about look-alike and "
			"punycode domains on its own. Nothing is checked online - no "
			"address you open leaves this device."_q));
}

} // namespace Lumina
