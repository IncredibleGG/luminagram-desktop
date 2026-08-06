/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "base/basic_types.h"

#include <rpl/producer.h>

#include <memory>

class DocumentData;
class PeerData;

namespace Ui {
class PopupMenu;
class Show;
} // namespace Ui

namespace Lumina {

// W5-G. "Save sticker" in the sticker panel's context menu.
//
// Android: LuminaConfig "saveStickers", ContentPreviewViewer action 9, which
// adds R.string.SaveToDownloads to the sticker long-press menu and routes it
// through MediaController.saveFile(). The key name is shared with the phone.
//
// Defaults OFF here, unlike the phone, where it defaults on: with the pref at
// its default the sticker panel menu is byte-for-byte the stock one.
[[nodiscard]] bool StickerSaveRow();
void SetStickerSaveRow(bool value);
[[nodiscard]] rpl::producer<> StickerSaveRowChanges();

// The row itself, appended by chat_helpers/stickers_list_widget.cpp.
//
// `setOwner` is the peer that owns the sticker set the sticker is being shown
// under, and is null for every ordinary set. It is non-null only for a group's
// own sticker set section, which is the one place in the panel where the set
// carries a peer's save restriction: a group with "Restrict saving content"
// must not gain a save route through its own stickers, because that is the
// parked grey feature (Settings::allowSaveRestricted) and not this one.
void AddSaveStickerMenuRow(
	not_null<Ui::PopupMenu*> menu,
	std::shared_ptr<Ui::Show> show,
	not_null<DocumentData*> document,
	PeerData *setOwner);

} // namespace Lumina
