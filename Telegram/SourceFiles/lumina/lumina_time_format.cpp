/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_time_format.h"

#include "base/unixtime.h"
#include "base/weak_ptr.h"
#include "core/application.h"
#include "data/data_channel.h"
#include "data/data_peer.h"
#include "data/data_session.h"
#include "data/data_user.h"
#include "history/history.h"
#include "history/history_item.h"
#include "history/view/history_view_element.h"
#include "lang/lang_keys.h"
#include "lumina/lumina_settings.h"
#include "main/main_account.h"
#include "main/main_domain.h"
#include "main/main_session.h"
#include "ui/text/format_values.h"

#include <QtCore/QJsonValue>
#include <QtCore/QLocale>
#include <crl/crl_on_main.h>

#include <vector>

namespace Lumina {
namespace {

constexpr auto kSecondsInYear = 365 * 24 * 60 * 60;

const auto kKeyTimeWithSeconds = u"timeWithSeconds"_q;

// Inserts a seconds field into a Qt date/time pattern, right after the minute
// field, and reuses the character the pattern already puts in front of the
// minutes as the separator - so "HH:mm" becomes "HH:mm:ss", "h:mm AP" becomes
// "h:mm:ss AP" rather than "h:mm AP:ss", and the locales that write "H.mm"
// get "H.mm.ss". Text inside single quotes is a literal in these patterns and
// is skipped, so a locale that spells out a word containing "m" or "s" cannot
// be mistaken for a field. A pattern that already has a seconds field, or that
// has no minute field at all, comes back unchanged.
[[nodiscard]] QString AddSecondsToPattern(const QString &pattern) {
	auto quoted = false;
	auto minutesFrom = -1;
	auto minutesTill = -1;
	for (auto i = 0, count = int(pattern.size()); i != count; ++i) {
		const auto ch = pattern[i];
		if (ch == QChar('\'')) {
			quoted = !quoted;
		} else if (quoted) {
			continue;
		} else if (ch == QChar('s')) {
			return pattern;
		} else if (ch == QChar('m')) {
			if (minutesTill != i) {
				minutesFrom = i;
			}
			minutesTill = i + 1;
		}
	}
	if (minutesFrom < 0) {
		return pattern;
	}
	const auto previous = (minutesFrom > 0)
		? pattern[minutesFrom - 1]
		: QChar(':');
	const auto separator = (previous.isLetter() || previous == QChar('\''))
		? QString(QChar(':'))
		: QString(previous);
	return pattern.mid(0, minutesTill)
		+ separator
		+ u"ss"_q
		+ pattern.mid(minutesTill);
}

[[nodiscard]] QString SecondsTimePattern() {
	static auto cachedFrom = QString();
	static auto cachedTo = QString();
	const auto pattern = QLocale().timeFormat(QLocale::ShortFormat);
	if (pattern != cachedFrom) {
		cachedFrom = pattern;
		cachedTo = AddSecondsToPattern(pattern);
	}
	return cachedTo;
}

[[nodiscard]] QString SecondsDateTimePattern() {
	static auto cachedFrom = QString();
	static auto cachedTo = QString();
	const auto pattern = QLocale().dateTimeFormat(QLocale::ShortFormat);
	if (pattern != cachedFrom) {
		cachedFrom = pattern;
		cachedTo = AddSecondsToPattern(pattern);
	}
	return cachedTo;
}

} // namespace

bool TimeWithSeconds() {
	return Settings::Instance().getBool(kKeyTimeWithSeconds, false);
}

void SetTimeWithSeconds(bool value) {
	if (TimeWithSeconds() == value) {
		return;
	}
	Settings::Instance().set(kKeyTimeWithSeconds, value, Store::Prefs);

	// This is normally reached from inside a settings toggle's own click
	// handling. Walking every message view from there fires resize and repaint
	// requests back into a widget tree that is still finishing that press, so
	// the re-measure goes to the next main-thread turn instead.
	crl::on_main([] {
		RefreshMessageTimeLayouts();
	});
}

rpl::producer<bool> TimeWithSecondsValue() {
	return rpl::single(
		rpl::empty
	) | rpl::then(
		TimeWithSecondsChanges()
	) | rpl::map([] {
		return TimeWithSeconds();
	});
}

rpl::producer<> TimeWithSecondsChanges() {
	return Settings::Instance().changesFor(kKeyTimeWithSeconds);
}

QString FormatMessageTime(QTime time) {
	if (!TimeWithSeconds()) {
		return QLocale().toString(time, QLocale::ShortFormat);
	}
	return QLocale().toString(time, SecondsTimePattern());
}

QString FormatMessageDateTime(const QDateTime &dateTime) {
	if (!TimeWithSeconds()) {
		return QLocale().toString(dateTime, QLocale::ShortFormat);
	}
	return QLocale().toString(dateTime, SecondsDateTimePattern());
}

QString FormatSavedFromDateTime(const QDateTime &dateTime) {
	if (!TimeWithSeconds()) {
		return Ui::FormatDateTimeSavedFrom(dateTime);
	}
	const auto current = QDate::currentDate();
	const auto date = dateTime.date();
	const auto time = FormatMessageTime(dateTime.time());
	if (date == current) {
		return tr::lng_mediaview_today(tr::now, lt_time, time);
	} else if (date == current.addDays(-1)) {
		return tr::lng_mediaview_yesterday(tr::now, lt_time, time);
	}
	const auto now = base::unixtime::now();
	const auto serialized = base::unixtime::serialize(dateTime);
	const auto diff = (now > serialized)
		? (now - serialized)
		: (serialized - now);
	const auto dateText = (diff < kSecondsInYear)
		? tr::lng_month_day(
			tr::now,
			lt_month,
			Lang::MonthSmall(date.month())(tr::now),
			lt_day,
			QString::number(date.day()))
		: langDayOfMonthFull(date);
	return tr::lng_mediaview_date_time(
		tr::now,
		lt_date,
		dateText,
		lt_time,
		time);
}

void RefreshMessageTimeLayouts() {
	if (!Core::IsAppLaunched() || !Core::App().domain().started()) {
		return;
	}

	// The views are collected first and refreshed afterwards, through weak
	// pointers, because refreshing one of them resizes the history it belongs
	// to, which can load more messages and grow (or, for an item that turns
	// out to be gone, shrink) the very blocks we would otherwise be walking.
	auto views = std::vector<base::weak_ptr<HistoryView::Element>>();
	for (const auto &[index, account] : Core::App().domain().accounts()) {
		const auto session = account->maybeSession();
		if (!session) {
			continue;
		}
		auto &owner = session->data();
		const auto collect = [&](not_null<PeerData*> peer) {
			const auto history = owner.historyLoaded(peer);
			if (!history) {
				return;
			}
			for (const auto &block : history->blocks) {
				for (const auto &view : block->messages) {
					views.emplace_back(view.get());
				}
			}
		};
		owner.enumerateUsers([&](not_null<UserData*> user) {
			collect(user);
		});
		owner.enumerateGroups(collect);
		owner.enumerateBroadcasts([&](not_null<ChannelData*> channel) {
			collect(channel);
		});
	}

	// Three calls per view, and each one is load-bearing.
	//
	// itemDataChanged() is what re-runs BottomInfo::layoutDateText(), which is
	// where the timestamp is formatted and measured. Nothing else does.
	//
	// requestViewResize() is not redundant with it. Message::itemDataChanged()
	// asks for a resize only when the bottom info's *current* size changed,
	// and a narrow bubble can wrap its way to the same current size out of a
	// wider maxWidth - and it is the optimal size that the message text's skip
	// block, the gap the text leaves for the timestamp, is built from. Asking
	// unconditionally sets Flag::NeedsResize, which makes the next layout pass
	// go through initDimensions() and rebuild that gap.
	//
	// notifyItemDataChange() covers the views this walk cannot see. Only the
	// item's main view lives in History::blocks; a pinned-message list, a
	// forum topic, a saved sublist and the scheduled section each own another
	// view of the same item inside their own list widget, and those widgets
	// refresh their view when this fires.
	for (const auto &weak : views) {
		const auto view = weak.get();
		if (!view) {
			continue;
		}
		const auto item = view->data();
		auto &owner = item->history()->owner();
		view->itemDataChanged();
		owner.requestViewResize(view);
		owner.notifyItemDataChange(item);
	}
}

} // namespace Lumina
