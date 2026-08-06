/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include <rpl/producer.h>

#include <QtCore/QDateTime>
#include <QtCore/QString>

namespace Lumina {

// Android's `timeWithSeconds` (LuminaChatActivity item 9): the time under a
// message reads 14:07:23 instead of 14:07.
//
// ANDROID APPLIES IT IN ONE PLACE. LocaleController::getFormatterDay() returns
// the "HH:mm:ss" / "h:mm:ss a" formatter instead of the "HH:mm" / "h:mm a" one
// while the preference is on, the toggle calls recreateFormatters(), and the
// recycler rebinds every cell afterwards, so nothing else has to be touched.
//
// DESKTOP HAS NO SINGLE FORMATTER. Every timestamp here is its own
// QLocale().toString(..., QLocale::ShortFormat) call site, so the preference
// is applied by routing the message-timestamp call sites through the helpers
// below. They are the whole feature, and with the preference off each one
// returns exactly the string the stock call returned.
//
// THE SECONDS ARE ADDED TO THE LOCALE'S PATTERN, NOT APPENDED TO THE RESULT.
// Appending ":ss" to the formatted string would write "3:07 PM:42" in a
// 12-hour locale and would use the wrong separator in the locales that write
// 15.07. So the locale's own short pattern is rewritten instead: the minute
// field is located outside quoted literals and the seconds field is inserted
// directly after it, reusing whatever character that locale already puts
// between hours and minutes. A pattern that already carries seconds, or that
// has no minute field at all, is left exactly as it is.
//
// THE RE-MEASURE IS THE PART ANDROID DOES NOT NEED. tdesktop measures the
// bubble timestamp once, in BottomInfo::layoutDateText(), and then keeps that
// view - and that width - alive for as long as the history stays loaded, not
// merely while it is on screen. A view measured for "14:07" clips "14:07:23",
// and turning the preference back off leaves the gap behind, so the change
// would otherwise appear only in content scrolled into view afterwards.
// RefreshMessageTimeLayouts() re-runs the measurement for every message view
// of every loaded history in every logged-in account, and SetTimeWithSeconds()
// calls it.
//
// BEHAVIOUR NEUTRALITY. The preference defaults to false, which is Android's
// default too, and every helper below is a pass-through while it is.

// Preference `timeWithSeconds`, Store::Prefs, default false. Same key name as
// Android. Main thread only.
[[nodiscard]] bool TimeWithSeconds();

// Writes the preference and re-measures every message view. Any other writer
// of the key - a settings import, say - must call RefreshMessageTimeLayouts()
// itself, or already-loaded histories keep their old widths.
void SetTimeWithSeconds(bool value);

[[nodiscard]] rpl::producer<bool> TimeWithSecondsValue();
[[nodiscard]] rpl::producer<> TimeWithSecondsChanges();

// Replaces QLocale().toString(time, QLocale::ShortFormat) - the time of day on
// its own, which is what the bubble shows.
[[nodiscard]] QString FormatMessageTime(QTime time);

// Replaces QLocale().toString(dateTime, QLocale::ShortFormat) - the short date
// and time together, which is what the copied text and the screen-reader text
// for a forwarded original use.
[[nodiscard]] QString FormatMessageDateTime(const QDateTime &dateTime);

// Replaces Ui::FormatDateTimeSavedFrom(dateTime), the "today at ..." form the
// bubble uses for a forwarded message's original date. With the preference off
// it simply calls that function; ui/text/format_values.cpp is shared with the
// media viewer and is not this feature's to change, so only the on case is
// reproduced here, from the same lang keys.
[[nodiscard]] QString FormatSavedFromDateTime(const QDateTime &dateTime);

// Re-measures every message view of every loaded history in every logged-in
// account. Cheap to call when nothing is loaded, and a no-op before the
// application object exists. Main thread only.
void RefreshMessageTimeLayouts();

} // namespace Lumina
