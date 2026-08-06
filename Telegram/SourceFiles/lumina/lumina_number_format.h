/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "base/basic_types.h"

#include <rpl/producer.h>

#include <QtCore/QString>

namespace Lumina {

// Android's disableNumberRounding (AndroidUtilities.formatWholeNumber, which
// returns formatCount(v) outright while the preference is on): a count is
// written out in full - 1,234,567 - instead of being shortened to 1.2M.
//
// WHERE THE OVERRIDE LIVES. Lang::FormatCountToShort() is the only 1.2K-style
// shortener in the app, and it is the source of every one of them: reaction
// buttons, the views and replies counters under a bubble, the comments button,
// poll and "seen by" counts, similar-channel subscriber counts, the accounts
// badge in the main menu, gift ribbons, boost counts. It is also what the
// {count:short} lang tag resolves through, so the lang strings that carry a
// count follow along without a second hook. Reading the preference there is
// the desktop equivalent of Android reading it inside formatWholeNumber, and
// it is the only hook that does not need a line changed at each of its
// twenty-odd call sites; everything else in this file exists to take surfaces
// back OUT of it.
//
// The chat list's own unread badge is NOT this and is untouched. It is built
// by Ui::FormatUnreadCounter(), which already prints the count in full and
// abbreviates to "99+" only in the narrow column - a different mechanism
// answering a different problem, and nothing a reader would call rounding.
//
// THE SEPARATOR IS LOCALE-AWARE, unlike Android's, which hard-codes ",". The
// exact form is Lang::FormatCountDecimal(), i.e. QLocale().toString(), which
// is already what the {count:decimal} tag and the exact-count tooltip on a
// shortened reaction use. Two spellings of the same number in one window would
// be worse than differing from Android here.
//
// NOTE THAT THE EXACT FORM GROUPS SMALL NUMBERS TOO. Today a count below
// 10'000 is printed by QString::number(), so 5432 reads "5432"; with the
// preference on it reads "5,432". That is deliberate and it is what Android
// does - formatCount() groups every value - and it is the only way the two
// halves of the same counter row agree with each other.
//
// WHAT IS DELIBERATELY LEFT ROUNDED, AND HOW. Android applies this everywhere,
// including chart axis captions (ChartHorizontalLinesData:159) and Stars
// amounts (StarsIntroActivity:5774). Desktop does not, by the owner's
// decision. A caller cannot be recognised from inside the formatter - there is
// no argument, no flag and no context that tells a chart ruler apart from a
// reaction button - so the exclusion is written at the call site, which reads
// FormatCountRounded() below instead of Lang::FormatCountToShort(). That keeps
// each exclusion visible where it applies rather than hidden in a list of
// special cases, and it is a one-expression change.
//
// The call sites that must use it:
//   statistics/chart_rulers_data.cpp        axis captions, laid out for four
//   statistics/view/chart_rulers_view.cpp   or five characters;
//   ui/controls/send_button.cpp             the star counter's overflow
//                                           fallback, which is only reached
//                                           because the exact form measured
//                                           too wide - handing it the exact
//                                           form again cannot help.
// Lang::FormatCreditsAmountToShort() is excluded too, but that one lives in
// lang/lang_tag.cpp and excludes itself there.

// Preference `disableNumberRounding`, Store::Prefs, default false. Same key
// name as Android. Main thread only.
[[nodiscard]] bool ExactNumbers();
void SetExactNumbers(bool value);
[[nodiscard]] rpl::producer<bool> ExactNumbersValue();
[[nodiscard]] rpl::producer<> ExactNumbersChanges();

// Whether Lang::FormatCountToShort() should return the full count right now.
// This is ExactNumbers() plus the two guards that make it safe to call from a
// function as general-purpose as a lang formatter, plus the exclusion set up
// by FormatCountRounded(). Nothing outside lang/lang_tag.cpp needs it.
[[nodiscard]] bool ExactNumbersActive();

// The shortened "1.2K" form, whatever the preference says. For the few
// surfaces whose layout is built for four or five characters, and for
// formatters that already have their own exact fallback. Replaces
// `Lang::FormatCountToShort(n).string` at such a call site, one for one.
[[nodiscard]] QString FormatCountRounded(int64 number, bool onlyK = false);

} // namespace Lumina
