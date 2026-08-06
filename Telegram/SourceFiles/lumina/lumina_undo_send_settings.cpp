/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_undo_send_settings.h"

#include "lumina/lumina_locale.h"
#include "lumina/lumina_undo_send.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "ui/wrap/vertical_layout.h"

#include "styles/style_settings.h"

namespace Lumina {
namespace {

// The one "{1}" in LuminaUndoSendWindowInfo is the length of the window, so
// that the wording and the timer in lumina_undo_send.cpp cannot drift apart.
[[nodiscard]] rpl::producer<QString> AboutValue() {
	return rpl::single(
		rpl::empty
	) | rpl::then(
		LangChanges()
	) | rpl::map([] {
		return Tr(
			u"LuminaUndoSendWindowInfo"_q,
			QString::number(UndoSendWindowSeconds()));
	});
}

} // namespace

void AddUndoSendRows(
		not_null<Ui::VerticalLayout*> container,
		not_null<Window::SessionController*>) {
	Ui::AddSkip(container);
	Ui::AddSubsectionTitle(container, TrValue(u"LuminaUndoSendTitle"_q));

	// Recomputed from the store rather than only from the click, so a value
	// restored from a backup, or changed in a second window, is reflected.
	const auto button = container->add(object_ptr<Ui::SettingsButton>(
		container,
		TrValue(u"LuminaUndoSendWindow"_q),
		st::settingsButtonNoIcon
	))->toggleOn(rpl::single(
		UndoSendWindow()
	) | rpl::then(UndoSendWindowChanges() | rpl::map([] {
		return UndoSendWindow();
	})));
	button->toggledChanges(
	) | rpl::on_next([=](bool value) {
		SetUndoSendWindow(value);
	}, button->lifetime());

	Ui::AddSkip(container);
	Ui::AddDividerText(container, AboutValue());
}

} // namespace Lumina
