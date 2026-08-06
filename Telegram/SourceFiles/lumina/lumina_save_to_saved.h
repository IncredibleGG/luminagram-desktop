/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "base/basic_types.h"

#include <rpl/producer.h>

namespace Ui {
class PopupMenu;
} // namespace Ui

namespace Lumina {

struct MessageMenuContext;

// One-tap copy of a message into Saved Messages, without the "Forwarded from"
// header.
//
// Android: LuminaConfig "saveToCloud", ChatActivity OPTION_SAVE_TO_CLOUD,
// which sends to the client user id with forwardFromMyName = true. The key
// name is shared with the phone.
//
// Defaults OFF, so a fresh profile keeps the stock message menu.
[[nodiscard]] bool SaveToSavedRow();
void SetSaveToSavedRow(bool value);
[[nodiscard]] rpl::producer<> SaveToSavedRowChanges();

// The F-04 row. Signature matches the AddSaveToSavedRow stub in
// lumina/lumina_message_menu.h.
//
// This row sits *inside* the forwarding permission, never beside it: a chat
// with forwarding restricted must not gain a one-tap route out of itself. That
// is the parked grey feature, and it is not what this is.
void AddSaveToSavedMenuRow(
	not_null<Ui::PopupMenu*> menu,
	const MessageMenuContext &context);

} // namespace Lumina
