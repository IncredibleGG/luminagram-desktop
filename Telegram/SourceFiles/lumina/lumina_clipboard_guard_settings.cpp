/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_clipboard_guard_settings.h"

#include "lumina/lumina_clipboard_guard.h"
#include "lumina/lumina_locale.h"
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
		TrValue(u"LuminaPrivacyCryptoClipboardGuard"_q),
		st::settingsButtonNoIcon
	))->toggleOn(CryptoClipboardGuardValue());
	button->toggledChanges(
	) | rpl::on_next([](bool value) {
		SetCryptoClipboardGuard(value);
	}, button->lifetime());
	Ui::AddSkip(container);
	Ui::AddDividerText(
		container,
		TrValue(u"LuminaPrivacyCryptoClipboardGuardInfo"_q));
}

} // namespace Lumina
