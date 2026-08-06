/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_dialogs_badges_settings.h"

#include "lumina/lumina_dialogs_badges.h"
#include "lumina/lumina_locale.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "ui/wrap/vertical_layout.h"

#include "styles/style_settings.h"

namespace Lumina {

void AddChatListDotRows(
		not_null<Ui::VerticalLayout*> container,
		not_null<Window::SessionController*>) {
	Ui::AddSkip(container);
	Ui::AddSubsectionTitle(
		container,
		TrValue(u"LuminaChatListDotsTitle"_q));

	const auto online = container->add(object_ptr<Ui::SettingsButton>(
		container,
		TrValue(u"LuminaChatListOnlineDot"_q),
		st::settingsButtonNoIcon
	))->toggleOn(ChatListOnlineDotValue());
	online->toggledChanges(
	) | rpl::on_next([](bool value) {
		SetChatListOnlineDot(value);
	}, online->lifetime());

	Ui::AddSkip(container);
	Ui::AddDividerText(
		container,
		TrValue(u"LuminaChatListOnlineDotInfo"_q));
	Ui::AddSkip(container);

	const auto recency = container->add(object_ptr<Ui::SettingsButton>(
		container,
		TrValue(u"LuminaChatListRecencyDot"_q),
		st::settingsButtonNoIcon
	))->toggleOn(ChatListRecencyDotValue());
	recency->toggledChanges(
	) | rpl::on_next([](bool value) {
		SetChatListRecencyDot(value);
	}, recency->lifetime());

	Ui::AddSkip(container);
	Ui::AddDividerText(
		container,
		TrValue(u"LuminaChatListRecencyDotInfo"_q));
}

} // namespace Lumina
