/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_time_format_settings.h"

#include "lumina/lumina_locale.h"
#include "lumina/lumina_time_format.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "ui/wrap/vertical_layout.h"

#include "styles/style_settings.h"

namespace Lumina {

// The title also covers AddExactNumbersRows(), which follows this block on the
// appearance page and carries no title of its own. Without one here, both rows
// hung under whatever block happened to precede them.
void AddTimeFormatRows(
		not_null<Ui::VerticalLayout*> container,
		not_null<Window::SessionController*>) {
	Ui::AddSkip(container);
	Ui::AddSubsectionTitle(
		container,
		TrValue(u"LuminaMessageDisplayHeader"_q));
	const auto button = container->add(object_ptr<Ui::SettingsButton>(
		container,
		TrValue(u"LuminaTimeWithSeconds"_q),
		st::settingsButtonNoIcon
	))->toggleOn(TimeWithSecondsValue());
	button->toggledChanges(
	) | rpl::on_next([](bool value) {
		SetTimeWithSeconds(value);
	}, button->lifetime());
	Ui::AddSkip(container);
	Ui::AddDividerText(
		container,
		TrValue(u"LuminaTimeWithSecondsAbout"_q));
}

} // namespace Lumina
