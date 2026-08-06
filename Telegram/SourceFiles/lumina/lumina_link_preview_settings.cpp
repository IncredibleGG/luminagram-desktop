/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_link_preview_settings.h"

#include "lumina/lumina_link_preview.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "ui/wrap/vertical_layout.h"

#include "styles/style_settings.h"

namespace Lumina {

void AddLinkPreviewRows(
		not_null<Ui::VerticalLayout*> container,
		not_null<Window::SessionController*> controller) {
	Ui::AddSkip(container);
	const auto button = container->add(object_ptr<Ui::SettingsButton>(
		container,
		rpl::single(u"Disable link preview by default"_q),
		st::settingsButtonNoIcon
	))->toggleOn(LinkPreviewOffByDefaultValue());
	button->toggledChanges(
	) | rpl::on_next([](bool value) {
		SetLinkPreviewOffByDefault(value);
	}, button->lifetime());
	Ui::AddSkip(container);
	Ui::AddDividerText(
		container,
		rpl::single(u"Messages you write from scratch are sent without a link "
			u"preview, and the composer never asks Telegram to look a pasted "
			u"link up while you type. A preview that is already attached, and "
			u"a message you edit, are left as they are. To attach a preview "
			u"to one message anyway, give \"Toggle link preview\" a key in "
			u"Settings > Keyboard shortcuts and press it while composing."_q));
}

} // namespace Lumina
