/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_sticker_save_settings.h"

#include "lumina/lumina_locale.h"
#include "lumina/lumina_sticker_save.h"
#include "ui/widgets/buttons.h"
#include "ui/wrap/vertical_layout.h"
#include "ui/vertical_list.h"

#include "styles/style_settings.h"

namespace Lumina {

void AddStickerSaveRows(
		not_null<Ui::VerticalLayout*> container,
		not_null<Window::SessionController*> controller) {
	// This block and the "keep more recent" row that follows it are about
	// stickers and GIFs, not about media at large. It used to be headed
	// "Media", which collided with the real Media block further down the page
	// - the one over "Pause video when app goes to background" - and left the
	// page showing two identical titles with unrelated rows under them.
	Ui::AddSkip(container);
	Ui::AddSubsectionTitle(container, TrValue(u"LuminaStickersHeader"_q));

	// Recomputed from the store rather than only from the click, so a value
	// restored from a backup, or changed in a second window, is reflected.
	const auto button = container->add(object_ptr<Ui::SettingsButton>(
		container,
		TrValue(u"LuminaSaveStickers"_q),
		st::settingsButtonNoIcon
	))->toggleOn(rpl::single(
		StickerSaveRow()
	) | rpl::then(StickerSaveRowChanges() | rpl::map([] {
		return StickerSaveRow();
	})));
	button->toggledChanges(
	) | rpl::on_next([=](bool value) {
		SetStickerSaveRow(value);
	}, button->lifetime());

	Ui::AddSkip(container);
	Ui::AddDividerText(container, TrValue(u"LuminaSaveStickersInfo"_q));
}

} // namespace Lumina
