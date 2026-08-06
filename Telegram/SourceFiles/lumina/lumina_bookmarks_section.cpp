/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_bookmarks_section.h"

#include "base/unique_qptr.h"
#include "base/unixtime.h"
#include "data/data_peer.h"
#include "data/data_session.h"
#include "lang/lang_keys.h"
#include "lumina/lumina_bookmarks.h"
#include "lumina/lumina_locale.h"
#include "main/main_session.h"
#include "settings/settings_common.h"
#include "ui/boxes/confirm_box.h"
#include "ui/rp_widget.h"
#include "ui/ui_utility.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/popup_menu.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"

#include "styles/style_layers.h"
#include "styles/style_menu_icons.h"
#include "styles/style_settings.h"
#include "styles/style_widgets.h"

#include <QtGui/QCursor>

namespace Settings {
namespace {

// How much of a bookmark fits on a row before it is elided. A character count,
// not a dimension - nothing here is measured in pixels. It is deliberately far
// shorter than the stored snippet: Settings::AddButtonWithLabel() gives its
// right-hand label whatever width the left-hand text leaves over, so a long
// title would squeeze the label out of existence entirely.
constexpr auto kPreviewMaxLength = 40;

[[nodiscard]] QString Preview(const QString &text) {
	auto result = text.simplified();
	if (result.size() > kPreviewMaxLength) {
		result = result.left(kPreviewMaxLength);
		if (!result.isEmpty() && result.back().isHighSurrogate()) {
			result.chop(1);
		}
		result = result.trimmed() + QChar(0x2026);
	}
	return result;
}

[[nodiscard]] QString DateText(TimeId date) {
	return date ? langDateTime(base::unixtime::parse(date)) : QString();
}

// A bookmark points at a message this client may never have loaded, and may
// point at one that no longer exists at all. Both are ordinary states here, so
// the jump is the same one every "go to message" in the tree performs
// (JumpToMessageClickHandler, history/history_item_helpers.cpp:803): show the
// item when it happens to be in memory, otherwise ask the history to open at
// that id, which loads around it and settles on the nearest position when the
// message is gone.
//
// An unknown peer is the one case that cannot degrade into anything useful -
// Data::Session::peer() would mint an empty stub and open a blank chat - so it
// is refused with a toast instead, and the row stays in the list for the user
// to delete.
void OpenBookmark(
		not_null<Window::SessionController*> controller,
		FullMsgId id) {
	const auto owner = &controller->session().data();
	const auto peer = owner->peerLoaded(id.peer);
	if (!peer) {
		controller->showToast(Lumina::Tr(u"LuminaBookmarkGone"_q));
		return;
	}
	auto params = Window::SectionShow(Window::SectionShow::Way::Forward);
	params.allowDuplicateInStack = true;
	if (const auto item = owner->message(id)) {
		controller->showMessage(item, params);
	} else {
		controller->showPeerHistory(peer, params, id.msg);
	}
}

void ConfirmRemove(
		not_null<Window::SessionController*> controller,
		const Lumina::Bookmark &bookmark) {
	const auto session = &controller->session();
	const auto id = bookmark.id;
	auto text = bookmark.snippet.isEmpty()
		? DateText(bookmark.date)
		: bookmark.snippet;
	controller->show(Ui::MakeConfirmBox({
		.text = std::move(text),
		.confirmed = [=](Fn<void()> close) {
			Lumina::RemoveBookmark(session, id);
			close();
		},
		.confirmText = tr::lng_box_delete(),
		.confirmStyle = &st::attentionBoxButton,
		.title = Lumina::Tr(u"LuminaBookmarkDeleteTitle"_q),
	}));
}

void AppendBookmarkRow(
		not_null<Ui::VerticalLayout*> container,
		not_null<Window::SessionController*> controller,
		const Lumina::Bookmark &bookmark) {
	const auto owner = &controller->session().data();
	const auto peer = owner->peerLoaded(bookmark.id.peer);
	const auto id = bookmark.id;
	const auto title = peer
		? peer->name()
		: Lumina::Tr(u"LuminaBookmarkChatUnavailable"_q);

	// Android shows the snippet with the date underneath, and falls back to
	// the date when there is no snippet. Desktop has one line, and the chat a
	// message came from is the thing a user actually navigates by, so the chat
	// takes the line and the snippet takes the label - with Android's fallback
	// to the date kept exactly where it was.
	const auto label = bookmark.snippet.isEmpty()
		? DateText(bookmark.date)
		: Preview(bookmark.snippet);

	const auto button = AddButtonWithLabel(
		container,
		rpl::single(title),
		rpl::single(label),
		st::settingsButtonNoIcon);
	button->setClickedCallback([=] {
		OpenBookmark(controller, id);
	});

	// Android deletes on long press, behind a confirmation. Desktop's long
	// press is a right click, and the menu is parented to the row, so the list
	// rebuilding underneath it takes the menu with it.
	const auto menu = button->lifetime().make_state<
		base::unique_qptr<Ui::PopupMenu>>();
	button->events(
	) | rpl::filter([](not_null<QEvent*> e) {
		return e->type() == QEvent::ContextMenu;
	}) | rpl::on_next([=](not_null<QEvent*> e) {
		*menu = base::make_unique_q<Ui::PopupMenu>(
			button,
			st::popupMenuWithIcons);
		(*menu)->addAction(Lumina::Tr(u"LuminaBookmarkRemove"_q), [=] {
			ConfirmRemove(controller, bookmark);
		}, &st::menuIconDelete);
		(*menu)->popup(QCursor::pos());
		e->accept();
	}, button->lifetime());
}

} // namespace

Type LuminaBookmarksId() {
	return LuminaBookmarks::Id();
}

LuminaBookmarks::LuminaBookmarks(
	QWidget *parent,
	not_null<Window::SessionController*> controller)
: Section(parent, controller) {
	const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);
	setupContent(content);
	Ui::ResizeFitChild(this, content);
}

LuminaBookmarks::~LuminaBookmarks() = default;

rpl::producer<QString> LuminaBookmarks::title() {
	return Lumina::TrValue(u"LuminaBookmarksTitle"_q);
}

void LuminaBookmarks::setupContent(not_null<Ui::VerticalLayout*> container) {
	const auto controller = this->controller();
	const auto session = &controller->session();

	// The list is rebuilt wholesale, so it gets a layout of its own rather
	// than clearing the one the section was handed.
	const auto list = container->add(
		object_ptr<Ui::VerticalLayout>(container));

	const auto rebuild = container->lifetime().make_state<Fn<void()>>();
	*rebuild = [=] {
		const auto width = list->width();
		list->clear();

		const auto bookmarks = Lumina::BookmarksFor(session);
		Ui::AddSkip(list);
		if (bookmarks.empty()) {
			Ui::AddDividerText(
				list,
				Lumina::TrValue(u"LuminaBookmarksEmpty"_q));
		} else {
			for (const auto &bookmark : bookmarks) {
				AppendBookmarkRow(list, controller, bookmark);
			}
			Ui::AddSkip(list);
			Ui::AddDividerText(
				list,
				Lumina::TrValue(u"LuminaBookmarksListAbout"_q));
		}
		list->resizeToWidth(width);
	};
	(*rebuild)();

	// Driven by the store, not by whoever wrote to it, so a bookmark removed
	// from the message context menu while this page is open updates it too.
	// Always deferred: Ui::VerticalLayout::clear() deletes its children
	// immediately, and the click that caused the write may still be on the
	// stack inside one of the rows about to go.
	rpl::merge(
		Lumina::BookmarkChanges(),
		Lumina::LangChanges()
	) | rpl::on_next([=] {
		Ui::PostponeCall(list, [=] {
			(*rebuild)();
		});
	}, list->lifetime());
}

} // namespace Settings
