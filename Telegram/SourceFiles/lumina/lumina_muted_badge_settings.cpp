/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_muted_badge_settings.h"

#include "lumina/lumina_locale.h"
#include "lumina/lumina_muted_badge.h"
#include "ui/ui_utility.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "ui/wrap/vertical_layout.h"

#include "styles/style_settings.h"

namespace Lumina {

void AddMutedBadgeRows(
		not_null<Ui::VerticalLayout*> container,
		not_null<Window::SessionController*>) {
	Ui::AddSkip(container);

	const auto count = container->add(object_ptr<Ui::SettingsButton>(
		container,
		TrValue(u"LuminaShowMutedCount"_q),
		st::settingsButtonNoIcon
	))->toggleOn(ShowMutedCountValue());
	count->toggledChanges(
	) | rpl::on_next([](bool value) {
		SetShowMutedCount(value);
	}, count->lifetime());

	// The chat list is a sibling of this page rather than a child of it, so
	// nothing repaints it when the colour it draws unread badges with changes:
	// the rows keep the pixels they were last painted with until something
	// else happens to dirty them. Android has the same problem and answers it
	// with NotificationCenter.reloadInterface; the desktop equivalent for a
	// pure colour change is one forced repaint of the whole window, which is
	// also what the palette switch does. Driven from the preference rather than
	// from the toggle so that a value restored by a settings import repaints
	// too, as long as this page is open.
	ShowMutedCountChanges(
	) | rpl::on_next([=] {
		Ui::ForceFullRepaint(container->window());
	}, container->lifetime());

	Ui::AddSkip(container);
	Ui::AddDividerText(
		container,
		TrValue(u"LuminaShowMutedCountInfo"_q));
}

} // namespace Lumina
