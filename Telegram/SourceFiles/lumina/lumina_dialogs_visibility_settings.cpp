/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_dialogs_visibility_settings.h"

#include "lumina/lumina_dialogs_visibility.h"
#include "lumina/lumina_locale.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "ui/wrap/vertical_layout.h"

#include "styles/style_settings.h"

namespace Lumina {

void AddDialogsVisibilityRows(
		not_null<Ui::VerticalLayout*> container,
		not_null<Window::SessionController*>) {
	Ui::AddSkip(container);
	Ui::AddSubsectionTitle(
		container,
		TrValue(u"LuminaChatListVisibilityTitle"_q));

	const auto folders = container->add(object_ptr<Ui::SettingsButton>(
		container,
		TrValue(u"LuminaHideTabs"_q),
		st::settingsButtonNoIcon
	))->toggleOn(HideChatFoldersValue());
	folders->toggledChanges(
	) | rpl::on_next([](bool value) {
		SetHideChatFolders(value);
	}, folders->lifetime());

	const auto stories = container->add(object_ptr<Ui::SettingsButton>(
		container,
		TrValue(u"LuminaHideStories"_q),
		st::settingsButtonNoIcon
	))->toggleOn(HideStoriesValue());
	stories->toggledChanges(
	) | rpl::on_next([](bool value) {
		SetHideStories(value);
	}, stories->lifetime());

	Ui::AddSkip(container);
	Ui::AddDividerText(
		container,
		TrValue(u"LuminaChatListVisibilityAbout"_q));
}

} // namespace Lumina
