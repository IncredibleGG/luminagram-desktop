/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_forward_actions.h"

#include "data/data_groups.h"
#include "data/data_session.h"
#include "data/data_types.h"
#include "history/view/controls/history_view_forward_panel.h"
#include "history/history.h"
#include "history/history_item.h"
#include "history/history_item_helpers.h"
#include "lumina/lumina_locale.h"
#include "lumina/lumina_message_menu.h"
#include "lumina/lumina_settings.h"
#include "main/main_session.h"
#include "ui/widgets/popup_menu.h"
#include "window/window_peer_menu.h"
#include "window/window_session_controller.h"

#include "styles/style_menu_icons.h"

namespace Lumina {
namespace {

const auto kKeyNoAuthor = u"forwardNoAuthor"_q;
const auto kKeyNoCaption = u"forwardNoCaption"_q;

} // namespace

ForwardMenuTarget ResolveForwardMenuTarget(
		const MessageMenuContext &context) {
	auto result = ForwardMenuTarget();

	// A multi-item selection is live, so the menu is already showing the
	// "forward selected" variants and `context.item` is only the message the
	// pointer happened to be over. A row that quietly acted on that one
	// message instead of on the selection would be a trap.
	if (context.hasSelection || !context.allowsForward) {
		return result;
	}
	const auto item = context.item;
	const auto owner = &item->history()->owner();
	if (context.asGroup) {
		if (const auto group = owner->groups().find(item)) {
			if (!ranges::all_of(group->items, &HistoryItem::allowsForward)) {
				return result;
			}
		}
	}
	auto ids = context.asGroup
		? owner->itemOrItsGroup(item)
		: MessageIdsList{ 1, item->fullId() };
	auto items = owner->idsToItems(ids);
	if (items.empty()) {
		return result;
	}
	result.ids = std::move(ids);
	result.items = std::move(items);
	return result;
}

bool ForwardNoAuthorRow() {
	return Settings::Instance().getBool(kKeyNoAuthor, false);
}

void SetForwardNoAuthorRow(bool value) {
	Settings::Instance().set(kKeyNoAuthor, value);
}

rpl::producer<> ForwardNoAuthorRowChanges() {
	return Settings::Instance().changesFor(kKeyNoAuthor);
}

bool ForwardNoCaptionRow() {
	return Settings::Instance().getBool(kKeyNoCaption, false);
}

void SetForwardNoCaptionRow(bool value) {
	Settings::Instance().set(kKeyNoCaption, value);
}

rpl::producer<> ForwardNoCaptionRowChanges() {
	return Settings::Instance().changesFor(kKeyNoCaption);
}

void AddForwardNoAuthorMenuRow(
		not_null<Ui::PopupMenu*> menu,
		const MessageMenuContext &context) {
	if (!ForwardNoAuthorRow()) {
		return;
	}
	const auto target = ResolveForwardMenuTarget(context);
	if (target.ids.empty()) {
		return;
	}

	// Without this the row is offered, the box opens, and the forward goes
	// out with the author intact: ShowForwardMessagesBox() and
	// ApiWrap::forwardMessages() both run NormalizeForwardOptions(), which
	// silently downgrades an impossible request to PreserveInfo.
	const auto session = &context.item->history()->session();
	const auto canHideAuthor = HistoryView::Controls::CanHideForwardAuthor(
		session,
		target.items);
	if (!canHideAuthor) {
		return;
	}
	const auto show = context.controller->uiShow();
	const auto ids = target.ids;
	menu->addAction(Tr(u"LuminaForwardNoAuthor"_q), [=] {
		Window::ShowForwardMessagesBox(show, Data::ForwardDraft{
			.ids = ids,
			.options = Data::ForwardOptions::NoSenderNames,
		});
	}, &st::menuIconForward);
}

void AddForwardNoCaptionMenuRow(
		not_null<Ui::PopupMenu*> menu,
		const MessageMenuContext &context) {
	if (!ForwardNoCaptionRow()) {
		return;
	}
	const auto target = ResolveForwardMenuTarget(context);
	if (target.ids.empty()) {
		return;
	}
	const auto session = &context.item->history()->session();
	const auto canHideAuthor = HistoryView::Controls::CanHideForwardAuthor(
		session,
		target.items);
	if (!canHideAuthor) {
		return;
	}

	// Nothing to drop: NoNamesAndCaptions would then differ from
	// NoSenderNames in name only, and the menu would carry two rows that do
	// exactly the same thing.
	if (!ItemsForwardCaptionsCount(target.items)) {
		return;
	}
	const auto show = context.controller->uiShow();
	const auto ids = target.ids;
	menu->addAction(Tr(u"LuminaForwardNoCaption"_q), [=] {
		Window::ShowForwardMessagesBox(show, Data::ForwardDraft{
			.ids = ids,
			.options = Data::ForwardOptions::NoNamesAndCaptions,
		});
	}, &st::menuIconForward);
}

} // namespace Lumina
