/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_message_menu.h"

#include "lumina/lumina_select_author.h"
#include "lumina/lumina_save_to_saved.h"
#include "lumina/lumina_message_details.h"
#include "lumina/lumina_forward_actions.h"
#include "lumina/lumina_bookmarks.h"
#include "lumina/lumina_voice_to_text.h"
#include "lumina/lumina_explain.h"
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
	AddVoiceToTextRow(menu, context);
	AddExplainRow(menu, context);
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
	AddForwardNoAuthorMenuRow(menu, context);
}

void AddForwardNoCaptionRow(
		not_null<Ui::PopupMenu*> menu,
		const MessageMenuContext &context) {
	AddForwardNoCaptionMenuRow(menu, context);
}

void AddSaveToSavedRow(
		not_null<Ui::PopupMenu*> menu,
		const MessageMenuContext &context) {
	AddSaveToSavedMenuRow(menu, context);
}

void AddMessageDetailsRow(
		not_null<Ui::PopupMenu*> menu,
		const MessageMenuContext &context) {
	AddMessageDetailsMenuRow(menu, context);
}

void AddBookmarkRow(
		not_null<Ui::PopupMenu*> menu,
		const MessageMenuContext &context) {
	AddBookmarkMenuRow(menu, context);
}

void AddSelectFromAuthorRow(
		not_null<Ui::PopupMenu*> menu,
		const MessageMenuContext &context) {
	FillSelectFromAuthorRow(menu, context);
}

void AddVoiceToTextRow(
		not_null<Ui::PopupMenu*> menu,
		const MessageMenuContext &context) {
	AddVoiceToTextMenuRow(menu, context);
}

void AddExplainRow(
		not_null<Ui::PopupMenu*> menu,
		const MessageMenuContext &context) {
	AddExplainMenuRow(menu, context);
}

} // namespace Lumina
