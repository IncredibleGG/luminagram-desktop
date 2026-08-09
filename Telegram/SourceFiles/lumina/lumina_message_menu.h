/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

class HistoryItem;
class HistoryInner;

namespace Ui {
class PopupMenu;
} // namespace Ui

namespace Window {
class SessionController;
} // namespace Window

namespace HistoryView {
class ListWidget;
struct ContextMenuRequest;
} // namespace HistoryView

namespace Lumina {

// tdesktop builds the message context menu in two completely independent
// places: HistoryView::AddMessageActions() for every ListWidget-based section
// (replies, pinned, scheduled, saved sublists, ...) and
// HistoryInner::showContextMenu() for the main history. Each of them gets
// exactly one FillMessageMenu() call; everything LuminaGram adds to the menu
// hangs off that single seam, so no ported feature ever has to touch those two
// very hot upstream files again.
//
// MessageMenuContext is what the seam resolves for the rows. It exists so that
// the guards which every row would otherwise re-derive - and which the two
// builders express differently - are written down once, here.
struct MessageMenuContext {
	not_null<Window::SessionController*> controller;
	not_null<HistoryItem*> item;

	// Exactly one of the two is non-null: the widget that owns the menu.
	// Rows that need list-specific behaviour (selection, scrolling) branch on
	// which one it is; rows that only need the item ignore both.
	HistoryView::ListWidget *list = nullptr;
	HistoryInner *inner = nullptr;

	// Act on the whole album rather than on the single tapped part.
	bool asGroup = true;

	// A multi-item selection is active, so `item` is only the tapped one and
	// the menu is already showing the "selected" variants of its actions.
	bool hasSelection = false;

	// item->allowsForward().
	bool allowsForward = false;

	// Copying the text out of this message is restricted.
	bool copyRestricted = false;

	// Saving the media of this message is restricted.
	bool copyMediaRestricted = false;
};

// The seam inserted at the tail of HistoryView::AddMessageActions().
void FillMessageMenu(
	not_null<Ui::PopupMenu*> menu,
	const HistoryView::ContextMenuRequest &request,
	not_null<HistoryView::ListWidget*> list);

// HistoryInner keeps hasCopyRestriction() / hasCopyMediaRestriction() private,
// so its side of the seam resolves them at the call site (which is a member of
// HistoryInner) and hands the results over.
struct HistoryInnerMenuRequest {
	not_null<Window::SessionController*> controller;
	HistoryItem *item = nullptr;
	bool asGroup = true;
	bool hasSelection = false;
	bool copyRestricted = false;
	bool copyMediaRestricted = false;
};

// The seam inserted at the equivalent tail of HistoryInner::showContextMenu().
//
// Build the request as a named local and pass it by name. A bare braced
// designated-initializer-list as the second argument would have to be matched
// against the ListWidget overload above as well, which is exactly the corner
// of [over.ics.list] that compilers disagree about.
void FillMessageMenu(
	not_null<Ui::PopupMenu*> menu,
	const HistoryInnerMenuRequest &request,
	not_null<HistoryInner*> inner);

// Rows, in the order they appear in the menu.
//
// Every one of these is a real, compilable no-op today - it returns without
// adding anything. A later wave fills in exactly one of them and no other file
// in the tree changes. Any row-specific guard (for example "hide when the chat
// is already Saved Messages") belongs in the row, not in the aggregator; only
// guards that would otherwise be duplicated across rows live in the context.
void AddForwardNoAuthorRow(
	not_null<Ui::PopupMenu*> menu,
	const MessageMenuContext &context);
void AddForwardNoCaptionRow(
	not_null<Ui::PopupMenu*> menu,
	const MessageMenuContext &context);
void AddSaveToSavedRow(
	not_null<Ui::PopupMenu*> menu,
	const MessageMenuContext &context);
void AddMessageDetailsRow(
	not_null<Ui::PopupMenu*> menu,
	const MessageMenuContext &context);
void AddBookmarkRow(
	not_null<Ui::PopupMenu*> menu,
	const MessageMenuContext &context);
void AddSelectFromAuthorRow(
	not_null<Ui::PopupMenu*> menu,
	const MessageMenuContext &context);
void AddVoiceToTextRow(
	not_null<Ui::PopupMenu*> menu,
	const MessageMenuContext &context);

} // namespace Lumina
