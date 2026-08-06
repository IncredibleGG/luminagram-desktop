/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_clipboard_guard_settings.h"

#include "lumina/lumina_clipboard_guard.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "ui/wrap/vertical_layout.h"

#include "styles/style_settings.h"

namespace Lumina {

void AddCryptoGuardRows(
		not_null<Ui::VerticalLayout*> container,
		not_null<Window::SessionController*> controller) {
	Ui::AddSkip(container);
	const auto button = container->add(object_ptr<Ui::SettingsButton>(
		container,
		rpl::single(u"Crypto address paste guard"_q),
		st::settingsButtonNoIcon
	))->toggleOn(CryptoClipboardGuardValue());
	button->toggledChanges(
	) | rpl::on_next([](bool value) {
		SetCryptoClipboardGuard(value);
	}, button->lifetime());
	Ui::AddSkip(container);
	Ui::AddDividerText(
		container,
		rpl::single(u"Ask before pasting what looks like a crypto wallet "
			"address into a message. Clipboard-hijacking malware can swap a "
			"copied address for a scammer's without you noticing, and the "
			"paste is the last moment you can catch it. The check runs on "
			"this device and nothing is sent anywhere."_q));
}

} // namespace Lumina
