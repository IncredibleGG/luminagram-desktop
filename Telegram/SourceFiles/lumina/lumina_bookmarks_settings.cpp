/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_bookmarks_settings.h"

#include "lumina/lumina_bookmarks.h"
#include "lumina/lumina_bookmarks_section.h"
#include "lumina/lumina_locale.h"
#include "main/main_session.h"
#include "settings/settings_common.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"

#include "styles/style_settings.h"

namespace Lumina {
namespace {

[[nodiscard]] rpl::producer<bool> EnabledValue() {
	return rpl::single(
		rpl::empty
	) | rpl::then(
		BookmarksEnabledChanges()
	) | rpl::map([] {
		return BookmarksEnabled();
	});
}

[[nodiscard]] rpl::producer<QString> CountValue(
		not_null<Main::Session*> session) {
	return rpl::single(
		rpl::empty
	) | rpl::then(
		rpl::merge(BookmarkChanges(), LangChanges())
	) | rpl::map([=] {
		const auto count = BookmarksCount(session);
		return count
			? QString::number(count)
			: Tr(u"LuminaBookmarksNone"_q);
	});
}

} // namespace

void AddBookmarksRows(
		not_null<Ui::VerticalLayout*> container,
		not_null<Window::SessionController*> controller) {
	Ui::AddSkip(container);
	Ui::AddSubsectionTitle(container, TrValue(u"LuminaBookmarksTitle"_q));

	::Settings::AddButtonWithLabel(
		container,
		TrValue(u"LuminaBookmarksList"_q),
		CountValue(&controller->session()),
		st::settingsButtonNoIcon
	)->setClickedCallback([=] {
		controller->showSettings(::Settings::LuminaBookmarksId());
	});

	const auto toggle = container->add(object_ptr<Ui::SettingsButton>(
		container,
		TrValue(u"LuminaShowBookmarks"_q),
		st::settingsButtonNoIcon
	))->toggleOn(EnabledValue());
	toggle->toggledChanges(
	) | rpl::on_next([](bool value) {
		SetBookmarksEnabled(value);
	}, toggle->lifetime());

	Ui::AddSkip(container);
	Ui::AddDividerText(container, TrValue(u"LuminaBookmarksAbout"_q));
}

} // namespace Lumina
