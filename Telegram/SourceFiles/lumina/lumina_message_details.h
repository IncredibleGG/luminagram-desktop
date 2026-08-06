/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "base/basic_types.h"

#include <rpl/producer.h>

#include <QtCore/QString>

class HistoryItem;

namespace Ui {
class PopupMenu;
} // namespace Ui

namespace Lumina {

struct MessageMenuContext;

// A read-only "Details" popup for one message: exact timestamp, message id,
// author, and - when the message was forwarded - where it came from and when
// it was originally posted.
//
// Android: LuminaConfig "showMessageDetails", ChatActivity OPTION_DETAILS and
// showMessageDetailsDialog(). The key name is shared with the phone.
//
// Everything shown is already on the message this client has synced. Nothing
// is requested, nothing is stored, and the message's own text is deliberately
// not part of it.
//
// Defaults OFF, so a fresh profile keeps the stock message menu. (Android
// defaults it on; see the report accompanying this file.)
[[nodiscard]] bool MessageDetailsRow();
void SetMessageDetailsRow(bool value);
[[nodiscard]] rpl::producer<> MessageDetailsRowChanges();

// The text the popup shows, one "Label: value" per line. Exposed so that the
// popup and its Copy button cannot drift apart.
[[nodiscard]] QString MessageDetailsText(not_null<HistoryItem*> item);

// The F-04 row. Signature matches the AddMessageDetailsRow stub in
// lumina/lumina_message_menu.h.
void AddMessageDetailsMenuRow(
	not_null<Ui::PopupMenu*> menu,
	const MessageMenuContext &context);

} // namespace Lumina
