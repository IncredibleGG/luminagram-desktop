/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_contact_note_box.h"

#include "lang/lang_keys.h"
#include "lumina/lumina_contact_notes.h"
#include "lumina/lumina_locale.h"
#include "ui/layers/generic_box.h"
#include "ui/vertical_list.h"
#include "ui/widgets/fields/input_field.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"

#include "styles/style_layers.h"
#include "styles/style_widgets.h"

#include <algorithm>

namespace Lumina {

void ContactNoteBox(
		not_null<Ui::GenericBox*> box,
		not_null<UserData*> user) {
	const auto withNote = ContactNoteFieldAvailable(user);
	const auto stored = ContactNoteFor(user);

	box->setWidth(st::boxWideWidth);
	box->setTitle(TrValue(withNote
		? u"LuminaContactNote"_q
		: u"LuminaContactTags"_q));

	const auto note = withNote
		? box->addRow(object_ptr<Ui::InputField>(
			box,
			st::defaultInputField,
			Ui::InputField::Mode::MultiLine,
			TrValue(u"LuminaContactNoteHint"_q),
			stored.note))
		: nullptr;
	if (note) {
		// NOT the bare cap. InputField::setMaxLength() CHOPS whatever is
		// already in the field (input_field.cpp:2500-2517), and nothing on the
		// write path clamps a note - SetContactNote() only trims, and
		// ContactNoteFor() reads the stored string verbatim. So a note longer
		// than the cap (a hand-edited store, a cross-platform import once the
		// v1 container is accepted) would be cut down the instant this box
		// opened, and the next Save - or the Save the user presses without
		// having touched the note at all - would make that permanent, with no
		// way back. The cap exists to bound what one paste can ADD, so it is
		// raised to whatever is already there: an over-long note can still be
		// read and shortened, it just cannot be grown.
		note->setMaxLength(std::max(
			kContactNoteMaxLength,
			int(stored.note.size())));

		// Mode::MultiLine alone is not enough: InputField defaults to
		// SubmitSettings::Enter, and in that mode keyPressEventInner() turns
		// a plain Return into a _submits emission INSTEAD of a newline. With
		// nothing subscribed to submits() that swallows the key outright, so
		// a "multi-line" note could never be given a second line by typing.
		note->setSubmitSettings(
			Ui::InputField::SubmitSettings::CtrlEnter);
	}

	const auto tags = box->addRow(object_ptr<Ui::InputField>(
		box,
		st::defaultInputField,
		Ui::InputField::Mode::SingleLine,
		TrValue(u"LuminaContactTagsHint"_q),
		stored.tags));
	// Same reasoning as the note field above.
	tags->setMaxLength(std::max(
		kContactTagsMaxLength,
		int(stored.tags.size())));

	Ui::AddSkip(box->verticalLayout());
	Ui::AddDividerText(
		box->verticalLayout(),
		TrValue(u"LuminaContactNoteAbout"_q));

	box->setFocusCallback([=] {
		if (note) {
			note->setFocusFast();
		} else {
			tags->setFocusFast();
		}
	});

	// `stored.note` is empty whenever the note field is absent - the field is
	// absent only for a contact who has no local note - so carrying it through
	// here changes nothing today. It is written that way so that a future
	// widening of the policy cannot turn a tags-only save into a silent wipe
	// of a note the box never showed.
	const auto save = [=] {
		SetContactNote(user, ContactNote{
			.note = note ? note->getLastText() : stored.note,
			.tags = tags->getLastText(),
		});
		box->closeBox();
	};
	tags->submits() | rpl::on_next(save, tags->lifetime());
	if (note) {
		note->submits() | rpl::on_next(save, note->lifetime());
	}

	box->addButton(tr::lng_settings_save(), save);
	box->addButton(tr::lng_cancel(), [=] {
		box->closeBox();
	});
	if (!stored.empty()) {
		box->addLeftButton(tr::lng_box_delete(), [=] {
			SetContactNote(user, ContactNote());
			box->closeBox();
		}, st::attentionBoxButton);
	}
}

void ShowContactNoteBox(
		not_null<Window::SessionController*> controller,
		not_null<UserData*> user) {
	controller->show(Box(ContactNoteBox, user));
}

} // namespace Lumina
