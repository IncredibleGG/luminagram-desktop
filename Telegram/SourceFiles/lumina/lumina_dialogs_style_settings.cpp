/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_dialogs_style_settings.h"

#include "lumina/lumina_dialogs_style.h"
#include "lumina/lumina_locale.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "ui/wrap/vertical_layout.h"

#include "styles/style_settings.h"

namespace Lumina {

void AddCompactListRows(
		not_null<Ui::VerticalLayout*> container,
		not_null<Window::SessionController*>) {
	Ui::AddSkip(container);
	Ui::AddSubsectionTitle(
		container,
		TrValue(u"LuminaChatListDensityTitle"_q));

	const auto compact = container->add(object_ptr<Ui::SettingsButton>(
		container,
		TrValue(u"LuminaCompactListRows"_q),
		st::settingsButtonNoIcon
	))->toggleOn(CompactListRowsValue());
	compact->toggledChanges(
	) | rpl::on_next([](bool value) {
		SetCompactListRows(value);
	}, compact->lifetime());

	Ui::AddSkip(container);
	Ui::AddDividerText(
		container,
		TrValue(u"LuminaCompactListRowsInfo"_q));
}

} // namespace Lumina
