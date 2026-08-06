/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_sticker_save.h"

#include "data/data_document.h"
#include "data/data_file_click_handler.h"
#include "data/data_file_origin.h"
#include "data/data_peer.h"
#include "lumina/lumina_locale.h"
#include "lumina/lumina_settings.h"
#include "ui/layers/show.h"
#include "ui/widgets/popup_menu.h"

#include "styles/style_menu_icons.h"

#include <QtCore/QDir>
#include <QtCore/QFileInfo>

namespace Lumina {
namespace {

const auto kKey = u"saveStickers"_q;

} // namespace

bool StickerSaveRow() {
	return Settings::Instance().getBool(kKey, false);
}

void SetStickerSaveRow(bool value) {
	Settings::Instance().set(kKey, value);
}

rpl::producer<> StickerSaveRowChanges() {
	return Settings::Instance().changesFor(kKey);
}

void AddSaveStickerMenuRow(
		not_null<Ui::PopupMenu*> menu,
		std::shared_ptr<Ui::Show> show,
		not_null<DocumentData*> document,
		PeerData *setOwner) {
	if (!StickerSaveRow()
		|| document->isNull()
		|| (setOwner && !setOwner->allowsForwarding())) {
		return;
	}

	// Mode::ToFile, not ToNewFile: the phone's row is "Save to downloads" and
	// puts the file where every other download goes, so this one asks
	// DocumentSaveClickHandler for exactly the destination the rest of the app
	// would pick - the configured download folder, or a save dialog when
	// "Ask download path" is on. Nothing here builds a path of its own.
	const auto origin = document->stickerSetOrigin();
	const auto saved = [=] {
		// Either the file is already on disk, or a download to a known
		// destination has just started. When neither holds there is nothing
		// truthful to name, so nothing is shown.
		const auto path = document->filepath(true);
		const auto target = path.isEmpty()
			? document->loadingFilePath()
			: path;
		if (target.isEmpty()) {
			return;
		}
		show->showToast(Tr(
			u"LuminaStickerSavedTo"_q,
			QDir::toNativeSeparators(QFileInfo(target).absolutePath())));
	};
	menu->addAction(Tr(u"LuminaSaveSticker"_q), [=] {
		DocumentSaveClickHandler::Save(
			origin,
			document,
			DocumentSaveClickHandler::Mode::ToFile,
			saved);
	}, &st::menuIconDownload);
}

} // namespace Lumina
