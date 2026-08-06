/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_contact_notes_settings.h"

#include "lang/lang_keys.h"
#include "lumina/lumina_contact_notes.h"
#include "lumina/lumina_locale.h"
#include "settings/settings_common.h"
#include "ui/boxes/confirm_box.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "ui/wrap/slide_wrap.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"

#include "styles/style_layers.h"
#include "styles/style_settings.h"

namespace Lumina {
namespace {

[[nodiscard]] rpl::producer<bool> EnabledValue() {
	return rpl::single(
		rpl::empty
	) | rpl::then(
		ContactNotesEnabledChanges()
	) | rpl::map([] {
		return ContactNotesEnabled();
	});
}

[[nodiscard]] rpl::producer<bool> HasStoredValue() {
	return rpl::single(
		rpl::empty
	) | rpl::then(
		ContactNoteChanges()
	) | rpl::map([] {
		return (ContactNotesCount() > 0);
	});
}

[[nodiscard]] rpl::producer<QString> CountValue() {
	return rpl::single(
		rpl::empty
	) | rpl::then(
		rpl::merge(ContactNoteChanges(), LangChanges())
	) | rpl::map([] {
		const auto count = ContactNotesCount();
		return count
			? QString::number(count)
			: Tr(u"LuminaContactNotesNone"_q);
	});
}

void ConfirmClear(not_null<Window::SessionController*> controller) {
	controller->show(Ui::MakeConfirmBox({
		.text = TrValue(u"LuminaContactNotesClearText"_q),
		.confirmed = [=](Fn<void()> close) {
			ClearContactNotes();
			close();
		},
		.confirmText = tr::lng_box_delete(),
		.confirmStyle = &st::attentionBoxButton,
		.title = TrValue(u"LuminaContactNotesClearTitle"_q),
	}));
}

} // namespace

void AddContactNotesRows(
		not_null<Ui::VerticalLayout*> container,
		not_null<Window::SessionController*> controller) {
	Ui::AddSkip(container);
	Ui::AddSubsectionTitle(
		container,
		TrValue(u"LuminaContactNotesTitle"_q));

	const auto toggle = container->add(object_ptr<Ui::SettingsButton>(
		container,
		TrValue(u"LuminaContactNotesToggle"_q),
		st::settingsButtonNoIcon
	))->toggleOn(EnabledValue());
	toggle->toggledChanges(
	) | rpl::on_next([](bool value) {
		SetContactNotesEnabled(value);
	}, toggle->lifetime());

	// The stored notes outlive the preference on purpose - switching the rows
	// off must not destroy what the user wrote - so deleting them is a row of
	// its own. It appears only while there is something to delete, and it is
	// deliberately not tied to the toggle above: with the feature switched off
	// there is no profile row left to reach the data through, which is exactly
	// when somebody who has changed their mind comes looking for it.
	const auto wrap = container->add(
		object_ptr<Ui::SlideWrap<Ui::VerticalLayout>>(
			container,
			object_ptr<Ui::VerticalLayout>(container)));
	wrap->toggleOn(HasStoredValue());
	wrap->finishAnimating();
	::Settings::AddButtonWithLabel(
		wrap->entity(),
		TrValue(u"LuminaContactNotesClear"_q),
		CountValue(),
		st::settingsAttentionButton
	)->setClickedCallback([=] {
		ConfirmClear(controller);
	});

	Ui::AddSkip(container);
	Ui::AddDividerText(container, TrValue(u"LuminaContactNotesAbout"_q));
}

} // namespace Lumina
