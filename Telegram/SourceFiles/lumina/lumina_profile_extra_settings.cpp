/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_profile_extra_settings.h"

#include "lumina/lumina_locale.h"
#include "lumina/lumina_profile_chat_date.h"
#include "lumina/lumina_profile_dc_id.h"
#include "lumina/lumina_registration_date.h"
#include "ui/widgets/buttons.h"
#include "ui/wrap/vertical_layout.h"
#include "ui/vertical_list.h"

#include "styles/style_settings.h"

namespace Lumina {
namespace {

// Recomputed from the store rather than from the toggle, so the row also
// follows a value written by a backup restore through Settings::importAll()
// and by the panic wipe, which removes the key outright.
void AddToggle(
		not_null<Ui::VerticalLayout*> container,
		const QString &labelKey,
		Fn<bool()> enabled,
		rpl::producer<> changes,
		Fn<void(bool)> setEnabled) {
	auto value = rpl::single(
		rpl::empty
	) | rpl::then(
		std::move(changes)
	) | rpl::map([enabled] {
		return enabled();
	});
	const auto button = container->add(object_ptr<Ui::SettingsButton>(
		container,
		TrValue(labelKey),
		st::settingsButtonNoIcon
	))->toggleOn(std::move(value));
	button->toggledChanges(
	) | rpl::on_next([setEnabled](bool value) {
		setEnabled(value);
	}, button->lifetime());
}

} // namespace

void AddProfileExtraRows(
		not_null<Ui::VerticalLayout*> container,
		not_null<Window::SessionController*>) {
	Ui::AddSkip(container);
	Ui::AddSubsectionTitle(
		container,
		TrValue(u"LuminaProfileInfoHeader"_q));
	AddToggle(
		container,
		u"LuminaProfileShowRegistrationDate"_q,
		RegistrationDateRowEnabled,
		RegistrationDateRowChanges(),
		SetRegistrationDateRowEnabled);
	AddToggle(
		container,
		u"LuminaProfileShowDcId"_q,
		ProfileDcIdRowEnabled,
		ProfileDcIdRowChanges(),
		SetProfileDcIdRowEnabled);
	AddToggle(
		container,
		u"LuminaProfileShowChatDate"_q,
		ChatCreationDateRowEnabled,
		ChatCreationDateRowChanges(),
		SetChatCreationDateRowEnabled);
	Ui::AddSkip(container);
	Ui::AddDividerText(container, TrValue(u"LuminaProfileInfoInfo"_q));
}

} // namespace Lumina
