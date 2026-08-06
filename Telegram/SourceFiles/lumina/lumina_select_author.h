/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "lumina/lumina_message_menu.h"

#include <rpl/producer.h>

class HistoryItem;
class PeerData;

namespace Ui {
class VerticalLayout;
} // namespace Ui

namespace Lumina {

// "Select all from author" (W4-C).
//
// Android: ChatActivity.OPTION_SELECT_AUTHOR (:33307), offered by the message
// menu at :46072 behind LuminaConfig "selectFromAuthor". The key name is
// shared with the phone so a backup restored from it lands on the same
// preference.
//
// WHAT "ALL" MEANS. Android walks its own `messages` list, which holds only
// what the chat has paged in, and never asks the server for more; this port
// keeps that meaning. "All" is therefore every message from that sender that
// this window has ALREADY LOADED - the History blocks in the main chat, the
// current slice in a ListWidget section - and nothing else. Scrolling further
// back and repeating the action reaches more. That is a deliberate choice, not
// an omission: a chat can hold hundreds of thousands of messages from one
// member, the selection is capped at MaxSelectedItems (100) anyway, and a
// backfill would have to page the whole history to be honest about the word
// "all". The settings text says so, and the toast below says so again the
// moment the cap actually truncates a selection.
//
// Default OFF. With the preference unset nothing consults it, the menu row is
// not built, and both message lists behave exactly as stock Telegram Desktop's
// do. Android defaults it on; see the item report for why this side does not.
[[nodiscard]] bool SelectFromAuthorEnabled();
void SetSelectFromAuthorEnabled(bool value);

// Reactive form of the getter, for the settings row. Fires on a whole-file
// restore through Settings::importAll() as well, so a value changed in a
// second window or brought back from a backup is reflected.
[[nodiscard]] rpl::producer<bool> SelectFromAuthorEnabledValue();

// What one pass over one message list produced.
struct SelectFromAuthorResult {
	// How many messages the pass added to the selection. Zero means nothing
	// changed, and the caller must not repaint or push a selection update.
	int added = 0;

	// At least one message from the same author had to be left out because
	// the selection is capped at MaxSelectedItems. Only its truth matters -
	// album parts make an exact count meaningless, so there is not one.
	bool limited = false;
};

// The sender the row would select, or nullptr when the row does not apply to
// this message at all. This is the whole visibility rule, in one place, and it
// is checked twice: once while the menu is built and once when the action is
// actually invoked, because the menu can sit open while the message goes away
// or the preference is turned off in another window.
//
// It is Android's guard, translated: the preference is on, the message is a
// selectable non-sponsored one, the chat is a group or a channel (in a
// one-to-one chat every message is from one of two people, so the action is
// meaningless), and the sender is a USER. That last clause is what Android's
// `getFromChatId() > 0` says - a channel post, or a post by an anonymous group
// admin, is attributed to the chat itself, and "every message from this chat"
// is just "select everything".
[[nodiscard]] PeerData *SelectFromAuthorTarget(not_null<HistoryItem*> item);

// The per-message filter the two list widgets apply while walking their loaded
// messages: `item` is another selectable message from the same sender as
// `anchor`, and one the selection can actually hold alongside it.
//
// The last clause is what keeps the "selection is full" report honest.
// tdesktop refuses to mix ephemeral messages with ordinary ones in one
// selection (HistoryItem::inSameSelectionGroup), and a message refused for
// that reason is not a message the cap left out - it is a message this action
// was never about, so it must be skipped here rather than offered and
// rejected.
//
// It deliberately does NOT re-check the preference or the chat type: those
// were decided once, by SelectFromAuthorTarget(), before the walk began.
[[nodiscard]] bool ItemFromSameAuthor(
	not_null<HistoryItem*> item,
	not_null<HistoryItem*> anchor);

// The message-menu row. lumina_message_menu.cpp's AddSelectFromAuthorRow()
// stub forwards here; the signature is the stub's.
void FillSelectFromAuthorRow(
	not_null<Ui::PopupMenu*> menu,
	const MessageMenuContext &context);

// The "select all from author" block of the LuminaGram Chats sub-page, in the
// F-02 sub-page shape: the section .cpp holds this one call and nothing else.
void AddSelectAuthorRows(
	not_null<Ui::VerticalLayout*> container,
	not_null<Window::SessionController*> controller);

} // namespace Lumina
