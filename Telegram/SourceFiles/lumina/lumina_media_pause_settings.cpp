/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_media_pause_settings.h"

#include "lumina/lumina_locale.h"
#include "lumina/lumina_media_pause.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "ui/wrap/vertical_layout.h"

#include "styles/style_settings.h"

namespace Lumina {

void AddMediaPauseRows(
		not_null<Ui::VerticalLayout*> container,
		not_null<Window::SessionController*>) {
	Ui::AddSkip(container);
	Ui::AddSubsectionTitle(container, TrValue(u"LuminaMediaTitle"_q));

	// Toggled from the store rather than only from the click, so a value that
	// arrived through a backup import, or was changed in a second window, is
	// reflected here.
	const auto button = container->add(object_ptr<Ui::SettingsButton>(
		container,
		TrValue(u"LuminaMediaAutoPauseBgVideo"_q),
		st::settingsButtonNoIcon
	))->toggleOn(AutoPauseBackgroundVideoValue());
	button->toggledChanges(
	) | rpl::on_next([](bool value) {
		SetAutoPauseBackgroundVideo(value);
	}, button->lifetime());

	Ui::AddSkip(container);
	Ui::AddDividerText(
		container,
		TrValue(u"LuminaMediaAutoPauseBgVideoInfo"_q));
}

} // namespace Lumina
