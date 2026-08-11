/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_select_author.h"

#include "config.h" // MaxSelectedItems
#include "data/data_peer.h"
#include "data/data_session.h"
#include "data/data_user.h"
#include "history/view/history_view_list_widget.h"
#include "history/history.h"
#include "history/history_inner_widget.h"
#include "history/history_item.h"
#include "lumina/lumina_locale.h"
#include "lumina/lumina_settings.h"
#include "main/main_session.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/popup_menu.h"
#include "ui/wrap/vertical_layout.h"
#include "ui/vertical_list.h"
#include "window/window_session_controller.h"

#include "styles/style_menu_icons.h"
#include "styles/style_settings.h"

#include <QtCore/QJsonValue>

namespace Lumina {
namespace {

const auto kKeyEnabled = u"selectFromAuthor"_q;

} // namespace

bool SelectFromAuthorEnabled() {
	return Settings::Instance().getBool(kKeyEnabled, false);
}

void SetSelectFromAuthorEnabled(bool value) {
	Settings::Instance().set(kKeyEnabled, value, Store::Prefs);
}

rpl::producer<bool> SelectFromAuthorEnabledValue() {
	return rpl::single(
		rpl::empty
	) | rpl::then(
		Settings::Instance().changesFor(kKeyEnabled)
	) | rpl::map([] {
		return SelectFromAuthorEnabled();
	});
}

PeerData *SelectFromAuthorTarget(not_null<HistoryItem*> item) {
	if (!SelectFromAuthorEnabled()
		|| item->isSponsored()
		|| !item->canBeSelected()
		|| item->history()->peer->isUser()) {
		return nullptr;
	}
	return item->from()->asUser();
}

bool ItemFromSameAuthor(
		not_null<HistoryItem*> item,
		not_null<HistoryItem*> anchor) {
	return (item != anchor)
		&& (item->from() == anchor->from())
		&& item->canBeSelected()
		&& !item->isSponsored()
		&& item->inSameSelectionGroup(anchor);
}

void FillSelectFromAuthorRow(
		not_null<Ui::PopupMenu*> menu,
		const MessageMenuContext &context) {
	const auto item = context.item;
	if (!SelectFromAuthorTarget(item)) {
		return;
	}
	const auto list = context.list;
	const auto inner = context.inner;
	const auto allowed = list
		? list->luminaCanSelectFromAuthor(item)
		: (inner && inner->luminaCanSelectFromAuthor(item));
	if (!allowed) {
		return;
	}

	// Both widgets own the menu they are handed, so they outlive it and the
	// raw captures below are safe - this is how every upstream row in these
	// two builders is written. The message is still resolved again on click,
	// because it can be deleted while the menu is open.
	const auto session = &item->history()->session();
	const auto itemId = item->fullId();
	const auto controller = context.controller;
	menu->addAction(Tr(u"LuminaSelectFromAuthor"_q), [=] {
		const auto item = session->data().message(itemId);
		if (!item || !SelectFromAuthorTarget(item)) {
			return;
		}
		const auto result = list
			? list->luminaSelectFromAuthor(item)
			: inner->luminaSelectFromAuthor(item);
		if (result.limited) {
			controller->showToast(Tr(
				u"LuminaSelectFromAuthorLimit"_q,
				QString::number(int(MaxSelectedItems))));
		}
	}, &st::menuIconSelect);
}

void AddSelectAuthorRows(
		not_null<Ui::VerticalLayout*> container,
		not_null<Window::SessionController*>) {
	// As with the link-preview block above it: the chats page titles every
	// block, so an untitled one reads as a stray row belonging to the block
	// before it rather than as a setting of its own.
	Ui::AddSkip(container);
	Ui::AddSubsectionTitle(container, TrValue(u"LuminaSelectionHeader"_q));
	const auto button = container->add(object_ptr<Ui::SettingsButton>(
		container,
		TrValue(u"LuminaSelectFromAuthorTitle"_q),
		st::settingsButtonNoIcon
	))->toggleOn(SelectFromAuthorEnabledValue());
	button->toggledChanges(
	) | rpl::on_next([](bool value) {
		SetSelectFromAuthorEnabled(value);
	}, button->lifetime());

	Ui::AddSkip(container);
	Ui::AddDividerText(
		container,
		TrValue(u"LuminaSelectFromAuthorAbout"_q));
}

} // namespace Lumina
