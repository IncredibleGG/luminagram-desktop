/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_message_actions_settings.h"

#include "lumina/lumina_forward_actions.h"
#include "lumina/lumina_locale.h"
#include "lumina/lumina_message_details.h"
#include "lumina/lumina_save_to_saved.h"
#include "ui/widgets/buttons.h"
#include "ui/wrap/vertical_layout.h"
#include "ui/vertical_list.h"

#include "styles/style_settings.h"

namespace Lumina {
namespace {

// Recomputed from the store rather than only from the click, so a value
// restored from a backup, or changed in a second window, is reflected.
void AddToggle(
		not_null<Ui::VerticalLayout*> container,
		rpl::producer<QString> label,
		Fn<bool()> checked,
		Fn<void(bool)> toggle,
		rpl::producer<> changes) {
	const auto button = container->add(object_ptr<Ui::SettingsButton>(
		container,
		std::move(label),
		st::settingsButtonNoIcon
	))->toggleOn(rpl::single(
		checked()
	) | rpl::then(std::move(changes) | rpl::map([=] {
		return checked();
	})));
	button->toggledChanges(
	) | rpl::on_next([=](bool value) {
		toggle(value);
	}, button->lifetime());
}

} // namespace

void AddMessageActionsRows(
		not_null<Ui::VerticalLayout*> container,
		not_null<Window::SessionController*> controller) {
	Ui::AddSkip(container);
	Ui::AddSubsectionTitle(container, TrValue(u"LuminaMessageActions"_q));

	AddToggle(
		container,
		TrValue(u"LuminaForwardNoAuthorTitle"_q),
		ForwardNoAuthorRow,
		SetForwardNoAuthorRow,
		ForwardNoAuthorRowChanges());
	AddToggle(
		container,
		TrValue(u"LuminaForwardNoCaptionTitle"_q),
		ForwardNoCaptionRow,
		SetForwardNoCaptionRow,
		ForwardNoCaptionRowChanges());
	AddToggle(
		container,
		TrValue(u"LuminaSaveToCloudTitle"_q),
		SaveToSavedRow,
		SetSaveToSavedRow,
		SaveToSavedRowChanges());
	AddToggle(
		container,
		TrValue(u"LuminaShowMessageDetails"_q),
		MessageDetailsRow,
		SetMessageDetailsRow,
		MessageDetailsRowChanges());

	Ui::AddSkip(container);
	Ui::AddDividerText(container, TrValue(u"LuminaMessageActionsInfo"_q));
}

} // namespace Lumina
