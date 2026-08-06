/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_quick_replies_settings.h"

#include "lumina/lumina_locale.h"
#include "lumina/lumina_quick_replies.h"
#include "lumina/lumina_quick_replies_section.h"
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
		ReplyTemplateChanges()
	) | rpl::map([] {
		return ReplyTemplatesEnabled();
	});
}

[[nodiscard]] rpl::producer<QString> CountValue() {
	return rpl::single(
		rpl::empty
	) | rpl::then(
		rpl::merge(ReplyTemplateChanges(), LangChanges())
	) | rpl::map([] {
		const auto count = ReplyTemplatesCount();
		return count
			? QString::number(count)
			: Tr(u"LuminaReplyTemplatesNone"_q);
	});
}

} // namespace

void AddReplyTemplatesRows(
		not_null<Ui::VerticalLayout*> container,
		not_null<Window::SessionController*> controller) {
	Ui::AddSkip(container);
	Ui::AddSubsectionTitle(
		container,
		TrValue(u"LuminaReplyTemplatesTitle"_q));

	::Settings::AddButtonWithLabel(
		container,
		TrValue(u"LuminaReplyTemplatesList"_q),
		CountValue(),
		st::settingsButtonNoIcon
	)->setClickedCallback([=] {
		controller->showSettings(::Settings::LuminaReplyTemplatesId());
	});

	const auto toggle = container->add(object_ptr<Ui::SettingsButton>(
		container,
		TrValue(u"LuminaReplyTemplatesShow"_q),
		st::settingsButtonNoIcon
	))->toggleOn(EnabledValue());
	toggle->toggledChanges(
	) | rpl::on_next([](bool value) {
		SetReplyTemplatesEnabled(value);
	}, toggle->lifetime());

	Ui::AddSkip(container);
	Ui::AddDividerText(container, TrValue(u"LuminaReplyTemplatesAbout"_q));
}

} // namespace Lumina
