/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "data/data_types.h"

#include <rpl/producer.h>

namespace Ui {
class PopupMenu;
} // namespace Ui

namespace Lumina {

struct MessageMenuContext;

// What a forward-family row would act on: the tapped message, or its whole
// album when the menu was opened on an album as a whole.
//
// Empty `ids` means "do not offer the row at all" and folds together every
// reason there is: a live multi-selection, a message that may not be
// forwarded, one part of an album that may not be forwarded, and ids that no
// longer resolve to messages. Rows test `ids.empty()` and nothing else.
//
// `items` is handed out alongside because both the CanHideForwardAuthor() and
// the ItemsForwardCaptionsCount() guards want the resolved list, and resolving
// it twice per row on a context-menu build is pure waste.
struct ForwardMenuTarget {
	MessageIdsList ids;
	HistoryItemsList items;
};

[[nodiscard]] ForwardMenuTarget ResolveForwardMenuTarget(
	const MessageMenuContext &context);

// Two extra entries next to the stock "Forward" row, each of which opens the
// ordinary forward box with one of tdesktop's forward options preselected
// instead of "Preserve info".
//
// Android: LuminaConfig "forwardNoAuthor" / "forwardNoCaption", read in
// ChatActivity.createMenu(); the key names are shared with the phone so a
// backup restored from it lands on the same two preferences.
//
// Both default OFF, so with a fresh profile the message menu is byte for byte
// the stock one.
[[nodiscard]] bool ForwardNoAuthorRow();
void SetForwardNoAuthorRow(bool value);
[[nodiscard]] rpl::producer<> ForwardNoAuthorRowChanges();

[[nodiscard]] bool ForwardNoCaptionRow();
void SetForwardNoCaptionRow(bool value);
[[nodiscard]] rpl::producer<> ForwardNoCaptionRowChanges();

// The F-04 rows. Signatures match the AddForwardNoAuthorRow /
// AddForwardNoCaptionRow stubs in lumina/lumina_message_menu.h, which is where
// the aggregator forwards to them from.
//
// The "no captions" row is deliberately NOT labelled the way Android labels
// it. Data::ForwardOptions has three states, not four
// (data/data_types.h:391-395): dropping captions is only expressible as
// NoNamesAndCaptions, which drops the author as well. Telling the user the
// author survives would be a lie about the flag that actually goes out.
void AddForwardNoAuthorMenuRow(
	not_null<Ui::PopupMenu*> menu,
	const MessageMenuContext &context);
void AddForwardNoCaptionMenuRow(
	not_null<Ui::PopupMenu*> menu,
	const MessageMenuContext &context);

} // namespace Lumina
