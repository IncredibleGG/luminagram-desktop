/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_number_format_settings.h"

#include "lumina/lumina_locale.h"
#include "lumina/lumina_number_format.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "ui/wrap/vertical_layout.h"

#include "styles/style_settings.h"

namespace Lumina {

void AddExactNumbersRows(
		not_null<Ui::VerticalLayout*> container,
		not_null<Window::SessionController*>) {
	Ui::AddSkip(container);

	const auto exact = container->add(object_ptr<Ui::SettingsButton>(
		container,
		TrValue(u"LuminaExactNumbers"_q),
		st::settingsButtonNoIcon
	))->toggleOn(ExactNumbersValue());
	exact->toggledChanges(
	) | rpl::on_next([](bool value) {
		SetExactNumbers(value);
	}, exact->lifetime());

	Ui::AddSkip(container);
	Ui::AddDividerText(
		container,
		TrValue(u"LuminaExactNumbersInfo"_q));
}

} // namespace Lumina
