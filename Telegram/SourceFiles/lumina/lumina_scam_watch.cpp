/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_scam_watch.h"

#include "data/data_peer.h"
#include "data/data_session.h"
#include "data/data_user.h"
#include "dialogs/dialogs_key.h"
#include "history/history.h"
#include "history/history_item.h"
#include "lumina/lumina_settings.h"
#include "main/main_session.h"
#include "ui/toast/toast.h"
#include "window/window_session_controller.h"

#include <QtCore/QJsonValue>

namespace Lumina {
namespace {

const auto kKeyEnabled = u"scamKeywordWarning"_q;

const auto kHintText = u"⚠ This message mentions money or verification and "
	u"comes from someone not in your contacts — be careful of scams."_q;

// Android shows its bulletin for 4 seconds
// (ChatActivity.luminaCheckScamKeywordWarning). Keep that, and keep the hint
// free of links: Ui::Toast::Widget only drops WA_TransparentForMouseEvents
// when the text carries a link or spoiler (toast_widget.cpp:422-426), so a
// hint with a "dismiss" link would sit on top of the composer swallowing every
// click for the whole duration. A passive warning must never do that.
constexpr auto kToastDuration = crl::time(4000);

// Transcribed verbatim from Android's LUMINA_SCAM_KEYWORDS
// (ChatActivity.java). Android is the source of truth for this list and it is
// deliberately short: the failure mode of a keyword warning is a false
// positive on ordinary conversation, and a hint that fires on everything is
// worse than no hint at all. Do not grow this list without a matching change
// on Android, and do not add single common words.
[[nodiscard]] const std::array<QString, 39> &Phrases() {
	static const auto result = std::array<QString, 39>{
		u"money transfer"_q,
		u"wire transfer"_q,
		u"bank transfer"_q,
		u"western union"_q,
		u"moneygram"_q,
		u"send me money"_q,
		u"transfer the money"_q,
		u"gift card"_q,
		u"itunes card"_q,
		u"google play card"_q,
		u"steam card"_q,
		u"amazon card"_q,
		u"investment opportunity"_q,
		u"crypto investment"_q,
		u"bitcoin investment"_q,
		u"guaranteed profit"_q,
		u"guaranteed return"_q,
		u"double your money"_q,
		u"high returns"_q,
		u"trading signal"_q,
		u"mining pool"_q,
		u"verification fee"_q,
		u"processing fee"_q,
		u"release fee"_q,
		u"activation fee"_q,
		u"customs fee"_q,
		u"unlock fee"_q,
		u"send the code"_q,
		u"send me the code"_q,
		u"verification code"_q,
		u"one-time code"_q,
		u"otp code"_q,
		u"seed phrase"_q,
		u"recovery phrase"_q,
		u"private key"_q,
		u"soulmate"_q,
		u"widower"_q,
		u"oil rig"_q,
		u"stranded abroad"_q,
	};
	return result;
}

// A phrase counts only when it is not glued to a longer word on either side.
// Android uses a bare String.contains(), which makes "private key" fire inside
// "private keyboard" and "gift card" inside "regift cardboard"; those are
// exactly the false positives that would make the feature not worth shipping.
//
// The one deliberate relaxation is a trailing "s" that itself ends the word,
// so the singular phrases in the table still match their plurals -
// "guaranteed returns", "gift cards", "trading signals" - without needing a
// second row per phrase.
[[nodiscard]] bool BoundedContains(
		const QString &text,
		const QString &phrase) {
	const auto size = text.size();
	const auto length = phrase.size();
	if (!length || length > size) {
		return false;
	}
	for (auto from = qsizetype(0); from + length <= size;) {
		const auto at = text.indexOf(phrase, from);
		if (at < 0) {
			return false;
		}
		const auto till = at + length;
		const auto openStart = !at || !text.at(at - 1).isLetterOrNumber();
		const auto openEnd = (till >= size)
			|| !text.at(till).isLetterOrNumber()
			|| ((text.at(till) == QChar(u's'))
				&& ((till + 1 >= size)
					|| !text.at(till + 1).isLetterOrNumber()));
		if (openStart && openEnd) {
			return true;
		}
		from = at + 1;
	}
	return false;
}

[[nodiscard]] bool SuspiciousPeer(not_null<PeerData*> peer) {
	const auto user = peer->asUser();
	return user
		&& !user->isSelf()
		&& !user->isServiceUser()
		&& !user->isSupport()
		&& !user->isContact();
}

class ScamWatch final {
public:
	explicit ScamWatch(not_null<Window::SessionController*> controller);

private:
	void check(not_null<HistoryItem*> item);
	void showPendingForActiveChat();
	void showHint();

	const not_null<Window::SessionController*> _controller;
	base::flat_set<PeerId> _flagged;
	base::flat_set<PeerId> _warned;
	rpl::lifetime _lifetime;

};

ScamWatch::ScamWatch(not_null<Window::SessionController*> controller)
: _controller(controller) {
	controller->session().data().newItemAdded(
	) | rpl::on_next([=](not_null<HistoryItem*> item) {
		check(item);
	}, _lifetime);

	controller->activeChatChanges(
	) | rpl::on_next([=](const Dialogs::Key &) {
		showPendingForActiveChat();
	}, _lifetime);

	Settings::Instance().changesFor(
		kKeyEnabled
	) | rpl::on_next([=] {
		if (!ScamKeywordWarningEnabled()) {
			_flagged.clear();
		}
	}, _lifetime);
}

void ScamWatch::check(not_null<HistoryItem*> item) {
	if (!ScamKeywordWarningEnabled()) {
		return;
	}
	const auto history = item->history();
	const auto peer = history->peer;
	if (!SuspiciousPeer(peer)
		|| _warned.contains(peer->id)
		|| _flagged.contains(peer->id)) {
		return;
	} else if (!item->isRegular()
		|| item->out()
		|| item->isService()
		|| !item->unread(history)) {
		return;
	}
	const auto &text = item->originalText().text;
	if (text.isEmpty() || !LooksLikeScamText(text)) {
		return;
	}
	_flagged.emplace(peer->id);
	showPendingForActiveChat();
}

void ScamWatch::showPendingForActiveChat() {
	if (_flagged.empty()) {
		return;
	}
	const auto peer = _controller->activeChatCurrent().peer();
	if (!peer || !_flagged.contains(peer->id)) {
		return;
	}
	_flagged.remove(peer->id);
	_warned.emplace(peer->id);
	showHint();
}

// Both call sites reach this from inside somebody else's mutation: check()
// runs inside History::newItemAdded() (history.cpp:1667), before that function
// has finished updating unread counts and dialog entries, and on a reconnect
// it runs hundreds of times in a row inside Updates::feedDifference();
// showPendingForActiveChat() also runs inside an activeChatChanges() emission,
// i.e. in the middle of a chat switch. Building and showing a widget there is
// exactly the re-entrancy the toolkit itself defers (see the crl::on_main in
// FlatLabel::dragActionFinish and its comment), so hand the toast to the next
// main-loop pass. The guard is the controller, and the lambda deliberately
// captures only that - never `this` - so nothing here outlives its owner.
void ScamWatch::showHint() {
	const auto controller = _controller;
	crl::on_main(controller, [=] {
		controller->showToast(Ui::Toast::Config{
			.text = TextWithEntities{ kHintText },
			.duration = kToastDuration,
		});
	});
}

} // namespace

bool ScamKeywordWarningEnabled() {
	return Settings::Instance().getBool(kKeyEnabled, false);
}

rpl::producer<bool> ScamKeywordWarningEnabledValue() {
	return rpl::single(
		rpl::empty
	) | rpl::then(
		Settings::Instance().changesFor(kKeyEnabled)
	) | rpl::map([] {
		return ScamKeywordWarningEnabled();
	});
}

void SetScamKeywordWarningEnabled(bool value) {
	Settings::Instance().set(kKeyEnabled, value, Store::Prefs);
}

bool LooksLikeScamText(const QString &text) {
	if (text.isEmpty()) {
		return false;
	}
	const auto normalized = text.toLower().simplified();
	if (normalized.isEmpty()) {
		return false;
	}
	for (const auto &phrase : Phrases()) {
		if (BoundedContains(normalized, phrase)) {
			return true;
		}
	}
	return false;
}

void SetupScamWatch(not_null<Window::SessionController*> controller) {
	controller->lifetime().make_state<ScamWatch>(controller);
}

} // namespace Lumina
