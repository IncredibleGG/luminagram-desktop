/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_message_menu.h"

#include "history/history_inner_widget.h"
#include "history/history_item.h"
#include "history/view/history_view_context_menu.h"
#include "history/view/history_view_cursor_state.h"
#include "history/view/history_view_list_widget.h"
#include "ui/widgets/popup_menu.h"

namespace Lumina {
namespace {

// The one guard shared by every row, so that no row repeats it.
//
// Sponsored messages are ads and must never grow LuminaGram actions.
// Non-regular messages (isRegular() is isHistoryEntry() && !isLocal(), so this
// covers scheduled entries, admin-log entries and anything still local) have
// no stable server id to bookmark, forward, save or describe.
[[nodiscard]] bool SkipItem(HistoryItem *item) {
	return !item || item->isSponsored() || !item->isRegular();
}

void FillRows(
		not_null<Ui::PopupMenu*> menu,
		const MessageMenuContext &context) {
	AddForwardNoAuthorRow(menu, context);
	AddForwardNoCaptionRow(menu, context);
	AddSaveToSavedRow(menu, context);
	AddMessageDetailsRow(menu, context);
	AddBookmarkRow(menu, context);
	AddSelectFromAuthorRow(menu, context);
}

} // namespace

void FillMessageMenu(
		not_null<Ui::PopupMenu*> menu,
		const HistoryView::ContextMenuRequest &request,
		not_null<HistoryView::ListWidget*> list) {
	const auto item = request.item;
	if (SkipItem(item)) {
		return;
	}
	FillRows(menu, {
		.controller = list->controller(),
		.item = item,
		.list = list,
		.asGroup = (request.pointState
			!= HistoryView::PointState::GroupPart),
		.hasSelection = !request.selectedItems.empty(),
		.allowsForward = item->allowsForward(),
		.copyRestricted = list->hasCopyRestriction(item),
		.copyMediaRestricted = list->hasCopyMediaRestriction(item),
	});
}

void FillMessageMenu(
		not_null<Ui::PopupMenu*> menu,
		const HistoryInnerMenuRequest &request,
		not_null<HistoryInner*> inner) {
	const auto item = request.item;
	if (SkipItem(item)) {
		return;
	}
	FillRows(menu, {
		.controller = request.controller,
		.item = item,
		.inner = inner,
		.asGroup = request.asGroup,
		.hasSelection = request.hasSelection,
		.allowsForward = item->allowsForward(),
		.copyRestricted = request.copyRestricted,
		.copyMediaRestricted = request.copyMediaRestricted,
	});
}

void AddForwardNoAuthorRow(
		not_null<Ui::PopupMenu*> menu,
		const MessageMenuContext &context) {
	// TODO (wave 4, W4-B): forward with Data::ForwardOptions::NoSenderNames.
	// Guard on context.allowsForward and on CanHideForwardAuthor().
}

void AddForwardNoCaptionRow(
		not_null<Ui::PopupMenu*> menu,
		const MessageMenuContext &context) {
	// TODO (wave 4, W4-B): forward with NoNamesAndCaptions, which on the wire
	// implies dropping the author too - label it accordingly. Hide it when the
	// forwarded set carries no captions at all.
}

void AddSaveToSavedRow(
		not_null<Ui::PopupMenu*> menu,
		const MessageMenuContext &context) {
	// TODO (wave 4, W4-B): copy to Saved Messages without the "Forwarded from"
	// header. Must stay behind context.allowsForward so that it cannot become
	// a save-from-restricted-chat bypass, and must hide in Saved Messages.
}

void AddMessageDetailsRow(
		not_null<Ui::PopupMenu*> menu,
		const MessageMenuContext &context) {
	// TODO (wave 4, W4-B): show id, date, author and forward origin.
}

void AddBookmarkRow(
		not_null<Ui::PopupMenu*> menu,
		const MessageMenuContext &context) {
	// TODO (wave 4, W4-A): add / remove a local bookmark for this message.
}

void AddSelectFromAuthorRow(
		not_null<Ui::PopupMenu*> menu,
		const MessageMenuContext &context) {
	// TODO (wave 4, W4-C): select every loaded message from this author.
	// Needs selection APIs that are private today, hence the context carries
	// the owning widget (context.list / context.inner) rather than only the
	// item.
}

} // namespace Lumina
