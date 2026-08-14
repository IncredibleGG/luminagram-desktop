/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include <rpl/producer.h>

#include <QtCore/QString>

namespace Lumina {

// Offline, name-only impersonation hint, ported from Android's LuminaHomoglyph.
//
// containsSuspicious() flags a display name that mixes visually confusable
// characters the way an impersonator does: a Cyrillic or Greek letter shaped
// like a Latin one dropped into an otherwise Latin word ("Apple" with a
// Cyrillic A), two writing systems inside one word, full-width Latin or digit
// forms, or a 0/1 wedged between letters to stand in for O/l. It is a pure
// function of the string it is handed - it reads no preference, touches no
// session, allocates nothing beyond locals and is safe to call from any
// thread, including during static initialisation. The preference below gates
// the WARNING that is shown, never the detector.
//
// It deliberately does NOT fire on a legitimate name written wholly in one
// non-Latin script - an all-Cyrillic "Ivan", a Greek or CJK name - because a
// single-script name is exactly what a real speaker of that language types.
// The signal is the MIX, not the mere presence of a non-Latin letter.
[[nodiscard]] bool containsSuspicious(const QString &name);

// The `homoglyphWarn` preference (Store::Prefs). Defaults to TRUE: the whole
// point is a warning that protects a user who never opened the settings, and
// the check is a purely local string comparison, so leaving it on costs
// nothing and asks nothing of Telegram. The default is registered in
// Lumina::Settings::Defaults(); these accessors are the single funnel for it.
[[nodiscard]] bool HomoglyphWarnEnabled();
[[nodiscard]] rpl::producer<bool> HomoglyphWarnEnabledValue();
[[nodiscard]] rpl::producer<> HomoglyphWarnChanges();
void SetHomoglyphWarnEnabled(bool value);

} // namespace Lumina
