/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_panic_wipe_settings.h"

#include "lang/lang_keys.h"
#include "lumina/lumina_panic_wipe.h"
#include "ui/layers/generic_box.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/checkbox.h"
#include "ui/widgets/labels.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"

#include "styles/style_layers.h"
#include "styles/style_settings.h"

namespace Lumina {
namespace {

[[nodiscard]] QString PanicWipeAbout() {
	return u"Logs every account on this device out and erases the local "
		"message database, drafts and cached media, together with "
		"LuminaGram's own settings, bookmarks, saved translations and API "
		"keys. Your accounts and your messages stay on Telegram's servers. "
		"Files you already downloaded are left where they are. This cannot "
		"be undone."_q;
}

[[nodiscard]] QString PanicWipeConfirmText() {
	return u"Every account on this device will be logged out. The local "
		"message database, drafts and cached media will be erased, together "
		"with LuminaGram's own settings, bookmarks, saved translations and "
		"API keys.\n\n"

		"Your accounts are not deleted. They stay on Telegram's servers, and "
		"so do your messages - you can sign in again from anywhere.\n\n"

		"Files that were already downloaded are NOT deleted. LuminaGram does "
		"not touch your download folder, because it is usually your ordinary "
		"Downloads folder and holds unrelated files. Move or delete anything "
		"sensitive there yourself.\n\n"

		"This cannot be undone."_q;
}

void PanicWipeBox(not_null<Ui::GenericBox*> box) {
	box->setTitle(rpl::single(u"Panic wipe"_q));

	box->addRow(object_ptr<Ui::FlatLabel>(
		box,
		PanicWipeConfirmText(),
		st::boxLabel));

	const auto acknowledged = box->addRow(
		object_ptr<Ui::Checkbox>(
			box,
			u"I understand this cannot be undone"_q,
			false,
			st::defaultBoxCheckbox),
		style::margins(
			st::boxRowPadding.left(),
			st::boxLittleSkip,
			st::boxRowPadding.right(),
			st::boxLittleSkip));
	acknowledged->setAllowTextLines();

	// The destructive button does not exist until the box is ticked, rather
	// than existing in a disabled state: Ui::RoundButton paints a disabled
	// button exactly like an enabled one, so a greyed-out "Wipe now" would
	// read as an unresponsive app instead of as a guard. Appearing on tick is
	// the only variant of this the user cannot misread.
	//
	// Cancel is added FIRST, against the usual convention of adding the
	// confirm button first, and that inversion is load-bearing. Buttons are
	// laid out right to left in the order they are added, so the first one
	// added owns the right edge of the box. Adding "Wipe now" first would put
	// it exactly where Cancel sat a moment earlier: tick the box, click again
	// without moving the mouse, and the click that was aimed at Cancel wipes
	// the device. Pinning Cancel to the right edge means no control ever
	// changes meaning under a stationary cursor, and "Wipe now" only ever
	// appears in space that was empty.
	acknowledged->checkedValue(
	) | rpl::on_next([=](bool checked) {
		box->clearButtons();
		box->addButton(tr::lng_cancel(), [=] {
			box->closeBox();
		});
		if (checked) {
			box->addButton(rpl::single(u"Wipe now"_q), [=] {
				box->closeBox();
				PerformPanicWipe();
			}, st::attentionBoxButton);
		}
	}, box->lifetime());
}

} // namespace

void ShowPanicWipeConfirm(not_null<Window::SessionController*> controller) {
	controller->show(Box(PanicWipeBox));
}

void AddPanicWipeRows(
		not_null<Ui::VerticalLayout*> container,
		not_null<Window::SessionController*> controller) {
	Ui::AddSkip(container);
	Ui::AddSubsectionTitle(container, rpl::single(u"Panic wipe"_q));
	container->add(object_ptr<Ui::SettingsButton>(
		container,
		rpl::single(u"Panic wipe (Kaboom)"_q),
		st::settingsAttentionButton
	))->setClickedCallback([=] {
		ShowPanicWipeConfirm(controller);
	});
	Ui::AddSkip(container);
	Ui::AddDividerText(container, rpl::single(PanicWipeAbout()));
}

} // namespace Lumina
