/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_panic_wipe_settings.h"

#include "lang/lang_keys.h"
#include "lumina/lumina_locale.h"
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

void PanicWipeBox(not_null<Ui::GenericBox*> box) {
	box->setTitle(TrValue(u"LuminaSecurityPanicConfirmTitle"_q));

	box->addRow(object_ptr<Ui::FlatLabel>(
		box,
		TrValue(u"LuminaSecurityPanicConfirmText"_q),
		st::boxLabel));

	const auto acknowledged = box->addRow(
		object_ptr<Ui::Checkbox>(
			box,
			TrValue(u"LuminaSecurityPanicConfirmAck"_q),
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
	// confirm button first, and that inversion is load-bearing.
	// BoxLayerWidget::updateButtonsPositions() lays buttons out with
	// moveToRight() in the order they were added, so the first one added owns
	// the right edge of the box. Adding "Wipe now" first would put it exactly
	// where Cancel sat a moment earlier: tick the box, click again without
	// moving the mouse, and the click that was aimed at Cancel wipes the
	// device. Pinning Cancel to the right edge means no control ever changes
	// meaning under a stationary cursor, and "Wipe now" only ever appears in
	// space that was empty.
	acknowledged->checkedValue(
	) | rpl::on_next([=](bool checked) {
		box->clearButtons();
		box->addButton(tr::lng_cancel(), [=] {
			box->closeBox();
		});
		if (checked) {
			box->addButton(
				TrValue(u"LuminaSecurityPanicConfirmButton"_q),
				[=] {
					box->closeBox();
					PerformPanicWipe();
				},
				st::attentionBoxButton);
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
	Ui::AddSubsectionTitle(
		container,
		TrValue(u"LuminaSecurityPanicHeader"_q));
	container->add(object_ptr<Ui::SettingsButton>(
		container,
		TrValue(u"LuminaSecurityPanicWipe"_q),
		st::settingsAttentionButton
	))->setClickedCallback([=] {
		ShowPanicWipeConfirm(controller);
	});
	Ui::AddSkip(container);
	Ui::AddDividerText(
		container,
		TrValue(u"LuminaSecurityPanicWipeAbout"_q));
}

} // namespace Lumina
