/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_voice_confirm.h"

#include "base/flat_map.h"
#include "base/flat_set.h"
#include "base/weak_qptr.h"
#include "chat_helpers/compose/compose_show.h"
#include "lang/lang_keys.h"
#include "lumina/lumina_locale.h"
#include "lumina/lumina_settings.h"
#include "ui/boxes/confirm_box.h"
#include "ui/layers/box_content.h"
#include "ui/layers/generic_box.h"

#include <QtCore/QJsonValue>

namespace Lumina {
namespace {

const auto kConfirmKey = u"confirmSendVoiceVideo"_q;

struct PendingConfirm {
	base::weak_qptr<Ui::BoxContent> box;
	std::shared_ptr<bool> answered;
};

struct ConfirmState {
	base::flat_map<QObject*, PendingConfirm> pending;

	// Owners whose confirmation has just been accepted and whose send is
	// being re-run. RequestVoiceSendConfirm() returns false for them, which is
	// what lets the second pass fall through to the real send.
	base::flat_set<QObject*> confirming;
};

[[nodiscard]] ConfirmState &Pending() {
	static auto result = ConfirmState();
	return result;
}

[[nodiscard]] QString ConfirmText(bool round, bool discardOnCancel) {
	if (discardOnCancel) {
		return round
			? Tr(u"LuminaVoiceConfirmSendVideoDiscard"_q)
			: Tr(u"LuminaVoiceConfirmSendVoiceDiscard"_q);
	}
	return round
		? Tr(u"LuminaVoiceConfirmSendVideo"_q)
		: Tr(u"LuminaVoiceConfirmSendVoice"_q);
}

} // namespace

bool ConfirmVoiceSend() {
	return Settings::Instance().getBool(kConfirmKey);
}

void SetConfirmVoiceSend(bool value) {
	Settings::Instance().set(kConfirmKey, value, Store::Prefs);
}

rpl::producer<> ConfirmVoiceSendChanges() {
	return Settings::Instance().changesFor(kConfirmKey);
}

bool RequestVoiceSendConfirm(VoiceSendConfirmRequest &&request) {
	const auto owner = request.owner.get();
	if (Pending().confirming.contains(owner)) {
		return false;
	} else if (Pending().pending.contains(owner)) {
		return true;
	} else if (!request.show
		|| !request.show->valid()
		|| !ConfirmVoiceSend()) {
		return false;
	}

	const auto answered = std::make_shared<bool>(false);
	const auto finish = [
		=,
		send = std::move(request.send),
		cancel = std::move(request.cancel)
	](bool sending) {
		if (*answered) {
			return;
		}
		*answered = true;
		Pending().pending.remove(owner);
		if (!sending) {
			if (cancel) {
				cancel();
			}
		} else if (send) {
			Pending().confirming.emplace(owner);
			send();
			Pending().confirming.remove(owner);
		}
	};

	// close() runs last in both callbacks: closing the box can take the
	// running lambda's owner with it, and every in-tree confirm box that
	// closes itself from its own callback does it in that order.
	const auto weak = request.show->show(Ui::MakeConfirmBox({
		.text = ConfirmText(request.round, request.discardOnCancel),
		.confirmed = [=](Fn<void()> close) {
			finish(true);
			close();
		},
		.cancelled = [=](Fn<void()> close) {
			finish(false);
			close();
		},
		.confirmText = tr::lng_send_button(),
		.title = Tr(u"LuminaVoiceConfirmBoxTitle"_q),
	}));

	// Nothing is recorded as pending until the box is actually up, and both
	// checks here fail open. If show() answered the box already - a layer
	// stack that closed it on the spot - the outcome has run and this owner
	// must not be left marked pending, or every later send for this bar would
	// be swallowed and both Enter filters would stay disabled for good. If no
	// box appeared at all, the send goes through unchanged, which is what
	// Android does when it has no context to put the dialog on.
	if (*answered) {
		return true;
	} else if (!weak) {
		return false;
	}
	Pending().pending.emplace(owner, PendingConfirm{
		.box = weak,
		.answered = answered,
	});
	return true;
}

bool VoiceSendConfirmPending(not_null<QObject*> owner) {
	return Pending().pending.contains(owner.get());
}

void CancelVoiceSendConfirm(not_null<QObject*> owner) {
	const auto entry = Pending().pending.take(owner.get());
	if (!entry) {
		return;
	}

	// Marked answered, and taken out of the map, before the box is closed, so
	// that the boxClosing this triggers finds nothing left to answer for.
	*entry->answered = true;
	if (const auto box = entry->box.get()) {
		box->closeBox();
	}
}

} // namespace Lumina
