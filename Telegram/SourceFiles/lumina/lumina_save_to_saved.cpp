/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_save_to_saved.h"

#include "data/data_peer.h"
#include "data/data_types.h"
#include "history/history.h"
#include "history/history_item.h"
#include "lumina/lumina_forward_actions.h"
#include "lumina/lumina_locale.h"
#include "lumina/lumina_message_menu.h"
#include "lumina/lumina_settings.h"
#include "ui/widgets/popup_menu.h"
#include "window/window_peer_menu.h"
#include "window/window_session_controller.h"

#include "styles/style_menu_icons.h"

namespace Lumina {
namespace {

const auto kKey = u"saveToCloud"_q;

} // namespace

bool SaveToSavedRow() {
	return Settings::Instance().getBool(kKey, false);
}

void SetSaveToSavedRow(bool value) {
	Settings::Instance().set(kKey, value);
}

rpl::producer<> SaveToSavedRowChanges() {
	return Settings::Instance().changesFor(kKey);
}

void AddSaveToSavedMenuRow(
		not_null<Ui::PopupMenu*> menu,
		const MessageMenuContext &context) {
	if (!SaveToSavedRow()) {
		return;
	}

	// ResolveForwardMenuTarget() is what keeps this inside the forwarding
	// permission rather than beside it - it returns nothing at all unless
	// item->allowsForward() holds for the whole album. Saving to Saved
	// Messages IS a forward, to yourself, and it goes out over the same
	// messages.forwardMessages call; offering it where forwarding is
	// restricted would be a save-from-restricted-chat bypass wearing a
	// friendlier label.
	const auto target = ResolveForwardMenuTarget(context);
	if (target.ids.empty()) {
		return;
	}

	// Saved Messages itself: the destination is where we already are.
	if (context.item->history()->peer->isSelf()) {
		return;
	}
	const auto show = context.controller->uiShow();
	const auto ids = target.ids;

	// NoSenderNames matches Android's forwardFromMyName = true. Where the
	// server will not let the author be stripped, ApiWrap::forwardMessages()
	// normalises it back to PreserveInfo and the copy still lands in Saved
	// Messages, header and all - this row promises a copy, not the absence
	// of a header.
	menu->addAction(Tr(u"LuminaSaveToCloud"_q), [=] {
		Window::ForwardToSelf(show, Data::ForwardDraft{
			.ids = ids,
			.options = Data::ForwardOptions::NoSenderNames,
		});
	}, &st::menuIconSavedMessages);
}

} // namespace Lumina
