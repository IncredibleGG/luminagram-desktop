/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_otp_guard.h"

#include "base/unixtime.h"
#include "data/data_peer.h"
#include "data/data_session.h"
#include "history/history.h"
#include "history/history_item.h"
#include "lumina/lumina_locale.h"
#include "lumina/lumina_settings.h"
#include "main/main_session.h"
#include "ui/boxes/confirm_box.h"
#include "ui/layers/box_content.h"
#include "window/window_session_controller.h"

#include "styles/style_layers.h"

#include <QtCore/QJsonValue>
#include <crl/crl_on_main.h>

namespace Lumina {
namespace {

const auto kKeyEnabled = u"otpGuardEnabled"_q;

// A code is only "in flight" for a short while after Telegram sent it. Same
// ten minutes as Android.
constexpr auto kRecentWindowSeconds = TimeId(10 * 60);

// Tolerance for a service message dated slightly ahead of us (clock skew).
constexpr auto kFutureSkewSeconds = TimeId(60);

// Never scan an unbounded paste; a login code is always near the start anyway.
constexpr auto kMaxScanChars = 4096;

[[nodiscard]] bool IsAsciiDigit(QChar ch) {
	return (ch >= QChar('0')) && (ch <= QChar('9'));
}

[[nodiscard]] bool IsAsciiLetter(QChar ch) {
	return ((ch >= QChar('a')) && (ch <= QChar('z')))
		|| ((ch >= QChar('A')) && (ch <= QChar('Z')));
}

[[nodiscard]] bool IsNumberSeparator(QChar ch) {
	switch (ch.unicode()) {
	case '.':
	case ',':
	case '-':
	case '/':
	case '+':
	case ':':
	case '\'':
		return true;
	}
	return false;
}

// Token boundaries are whitespace AND CJK (U+2E80 and above), so a Chinese
// sentence written without spaces still isolates the digits instead of
// swallowing the whole message into one "URL" token.
[[nodiscard]] bool IsTokenBreak(QChar ch) {
	return ch.isSpace() || (ch.unicode() >= 0x2E80);
}

// Symbols that, glued in front of a number, make it an amount of money.
[[nodiscard]] const QString &CurrencyPrefixes() {
	static const auto result = u"$＄¥￥€£￡₹₽₩￦฿₫₴"_q;
	return result;
}

// Characters that, glued after a number, make it an amount of money or a
// count.
[[nodiscard]] const QString &CurrencySuffixes() {
	static const auto result = u"元塊块圓円币幣원$¥€"_q;
	return result;
}

[[nodiscard]] bool IsCurrencyPrefix(QChar ch) {
	return !ch.isNull() && (CurrencyPrefixes().indexOf(ch) >= 0);
}

[[nodiscard]] bool IsCurrencySuffix(QChar ch) {
	return !ch.isNull() && (CurrencySuffixes().indexOf(ch) >= 0);
}

// Nearest non-space character at or before `from`, or a null QChar at the
// start of the text.
[[nodiscard]] QChar PreviousNonSpace(const QString &text, int from) {
	auto i = from;
	while ((i >= 0) && (text.at(i) == QChar(' '))) {
		--i;
	}
	return (i >= 0) ? text.at(i) : QChar();
}

// Nearest non-space character at or after `from`, or a null QChar at the end
// of the text.
[[nodiscard]] QChar NextNonSpace(const QString &text, int from, int n) {
	auto i = from;
	while ((i < n) && (text.at(i) == QChar(' '))) {
		++i;
	}
	return (i < n) ? text.at(i) : QChar();
}

// True when the run sits inside a token that looks like a URL / path / e-mail
// / filename.
[[nodiscard]] bool InsideUrlLikeToken(
		const QString &text,
		int start,
		int end,
		int n) {
	auto tokenStart = start;
	while ((tokenStart > 0) && !IsTokenBreak(text.at(tokenStart - 1))) {
		--tokenStart;
	}
	auto tokenEnd = end;
	while ((tokenEnd < n) && !IsTokenBreak(text.at(tokenEnd))) {
		++tokenEnd;
	}
	for (auto i = tokenStart; i != tokenEnd; ++i) {
		const auto ch = text.at(i);
		switch (ch.unicode()) {
		case '/':
		case '\\':
		case '@':
		case '?':
		case '&':
		case '=':
		case '%':
		case '#':
			return true;
		}
		// "example.com", "photo.jpg", "v1.2" - a dot glued to a Latin letter.
		if ((ch == QChar('.'))
			&& (i + 1 < tokenEnd)
			&& IsAsciiLetter(text.at(i + 1))) {
			return true;
		}
	}
	return false;
}

// False-positive filters, applied to one maximal digit run [start, end).
// The rule of thumb is "rather miss a code than nag on every price": every
// ambiguous shape is rejected. Ported one for one from Android's
// LuminaOtpGuard.looksLikeLoginCode(), which 26 cases were checked against.
[[nodiscard]] bool LooksLikeLoginCode(
		const QString &text,
		int start,
		int end,
		int n) {
	const auto before = (start > 0) ? text.at(start - 1) : QChar();
	const auto after = (end < n) ? text.at(end) : QChar();

	// The same neighbours with runs of plain spaces skipped, so that a
	// spaced-out amount ("45678 元", "$ 12345") or a spaced phone group
	// ("0912 345678") is still recognised.
	const auto beforeSpaced = PreviousNonSpace(text, start - 1);
	const auto afterSpaced = NextNonSpace(text, end, n);

	// Glued to Latin letters / underscore => an identifier, hash, filename or
	// password fragment ("abc123456", "123456th"), not something a user reads
	// out as a code.
	if (IsAsciiLetter(before) || (before == QChar('_'))) {
		return false;
	} else if (IsAsciiLetter(after) || (after == QChar('_'))) {
		return false;
	}

	// Money ("$12345", "12345元", "45678 元") and percentages.
	if (IsCurrencyPrefix(beforeSpaced)
		|| IsCurrencySuffix(afterSpaced)
		|| (afterSpaced == QChar('%'))) {
		return false;
	}

	// Part of a longer structured number: a separator with digits on the far
	// side means decimal / thousands group / phone group / date / version /
	// order id, e.g. "12.34567", "1,234,567", "2024-123456", "0912 345678".
	if (IsNumberSeparator(before)
		&& (start - 2 >= 0)
		&& IsAsciiDigit(text.at(start - 2))) {
		return false;
	} else if (IsNumberSeparator(after)
		&& (end + 1 < n)
		&& IsAsciiDigit(text.at(end + 1))) {
		return false;
	} else if ((before == QChar(' ')) && IsAsciiDigit(beforeSpaced)) {
		return false;
	} else if ((after == QChar(' ')) && IsAsciiDigit(afterSpaced)) {
		return false;
	}

	// Inside a URL, path, query string, e-mail or file name.
	return !InsideUrlLikeToken(text, start, end, n);
}

// Nothing here can throw: it is a bounds-checked scan of a QString and a
// null-checked read of two already-loaded pointers. There is no equivalent of
// Android's `catch (Throwable)` wrapper to add, and none is needed - every
// step that could be unsure answers false, which is the fail-open answer.
[[nodiscard]] bool ShouldWarn(
		not_null<History*> history,
		const QString &text) {
	if (!OtpGuardEnabled() || text.isEmpty()) {
		return false;
	}
	const auto peer = history->peer;

	// Sending "back" to Telegram itself, or to Saved Messages, leaks nothing.
	if ((peer->id == PeerData::kServiceNotificationsId) || peer->isSelf()) {
		return false;
	}

	// Cheap test first: most sends contain no 5-6 digit run at all, and this
	// one touches nothing but the string.
	if (!ContainsLoginCode(text)) {
		return false;
	}
	return ReceivedServiceMessageRecently(&history->session());
}

} // namespace

bool OtpGuardEnabled() {
	return Settings::Instance().getBool(kKeyEnabled, true);
}

void SetOtpGuardEnabled(bool value) {
	Settings::Instance().set(kKeyEnabled, value, Store::Prefs);
}

rpl::producer<> OtpGuardChanges() {
	return Settings::Instance().changesFor(kKeyEnabled);
}

bool ContainsLoginCode(const QString &text) {
	const auto n = std::min(int(text.size()), kMaxScanChars);
	auto i = 0;
	while (i != n) {
		if (!IsAsciiDigit(text.at(i))) {
			++i;
			continue;
		}
		const auto start = i;
		while ((i != n) && IsAsciiDigit(text.at(i))) {
			++i;
		}
		const auto length = i - start;
		if ((length >= 5)
			&& (length <= 6)
			&& LooksLikeLoginCode(text, start, i, n)) {
			return true;
		}
	}
	return false;
}

// WHY THE HISTORY AND NOT A TIMESTAMP OF OUR OWN.
//
// Android reads MessagesController's in-memory dialog state. The same answer
// on desktop is Data::Session's already-loaded History for peer 777000, and it
// was chosen over recording our own "last service message at" for the same
// reasons: it needs no hook in any message-receive path (so no shared hot file
// is touched), it is restored for free with the dialog list, and it is a plain
// main-thread field read with no query and no I/O.
//
// historyLoaded() deliberately does NOT create the History: before the dialog
// list has arrived there is nothing to read, and "no warning" is the right
// answer for a client that cannot yet tell whether a code is in flight.
//
// Two dates are taken, as on Android: chatListTimeId() is the dialog-list
// date, which can lag on an entry rebuilt from cache, and lastMessage() is the
// usually fresher cached top message for the same chat.
bool ReceivedServiceMessageRecently(not_null<Main::Session*> session) {
	const auto history = session->data().historyLoaded(
		PeerData::kServiceNotificationsId);
	if (!history) {
		return false;
	}
	auto date = history->chatListTimeId();
	if (const auto item = history->lastMessage()) {
		date = std::max(date, item->date());
	}
	if (date <= 0) {
		return false;
	}

	// The server-corrected clock, so a device with a wrong local time still
	// compares against the same seconds the code was stamped with.
	const auto now = base::unixtime::now();
	if (now <= 0) {
		return false;
	}
	const auto age = now - date;
	return (age <= kRecentWindowSeconds) && (age >= -kFutureSkewSeconds);
}

bool OtpGuardIntercept(
		not_null<History*> history,
		const TextWithTags &text,
		Api::SendOptions,
		Fn<void()> proceed) {
	Expects(proceed != nullptr);

	// Scheduled sends are warned about too: the code leaves the device either
	// way, and Android's guard sits above its own schedule branch as well.
	if (!ShouldWarn(history, text.text)) {
		return true;
	}
	const auto controller = history->session().tryResolveWindow(history->peer);
	if (!controller) {
		// No UI to warn with - fail open and send exactly what the user asked
		// for, which is what Android does when it has no context to put the
		// dialog on.
		return true;
	}

	// Every outcome funnels through here, so the send is dispatched at most
	// once no matter how the box is dismissed.
	const auto answered = std::make_shared<bool>(false);
	const auto resolve = [=](bool send) {
		if (*answered) {
			return;
		}
		*answered = true;
		if (send) {
			proceed();
		}
		// Not sending means simply dropping `proceed`, which is how
		// lumina/lumina_send_pipeline.h spells "cancel". The composer has not
		// cleared its field yet at this point, so the text is still sitting
		// there for the user to edit - exactly like Android, where cancelling
		// leaves the message in the composer.
	};

	// The safe choice is the emphasised one. tdesktop puts `confirmText` on
	// the primary button, so "Don't send" is the confirm and "send anyway" is
	// the red secondary - the same emphasis Android gives them.
	//
	// strictCancel is load-bearing: without it, boxClosing() fires the cancel
	// button, so pressing Escape or having the layer torn down would count as
	// "send anyway" and push the login code out. With it, a box that goes away
	// unanswered answers nothing and the send is dropped.
	//
	// close() runs last in both callbacks: closing the box can take the
	// running lambda's owner with it.
	const auto weak = controller->show(Ui::MakeConfirmBox({
		.text = Tr(u"LuminaOtpGuardMessage"_q),
		.confirmed = [=](Fn<void()> close) { resolve(false); close(); },
		.cancelled = [=](Fn<void()> close) { resolve(true); close(); },
		.confirmText = Tr(u"LuminaOtpGuardCancel"_q),
		.cancelText = Tr(u"LuminaOtpGuardSendAnyway"_q),
		.cancelStyle = &st::attentionBoxButton,
		.title = Tr(u"LuminaOtpGuardTitle"_q),
		.strictCancel = true,
	}));
	if (!*answered && !weak) {
		// show() put nothing up - a layer stack that refused it. The user will
		// never be asked, so asking is not what should happen to the message:
		// fail open and send it. Deferred rather than called here, because
		// this returns false and the composer is still inside its own send.
		crl::on_main([=] { resolve(true); });
	}
	return false;
}

} // namespace Lumina
