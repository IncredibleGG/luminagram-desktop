/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include <rpl/producer.h>

namespace Lumina {

// Android's showMutedCount (DialogCell::isCounterMuted, which returns false
// outright while the preference is on): a muted chat keeps drawing its unread
// badge in the accent colour instead of the muted grey, so a counter that is
// merely quiet is still readable at a glance.
//
// WHERE THE OVERRIDE MAY LIVE, AND WHERE IT MAY NOT. Desktop takes the badge
// colour from BadgesState::unreadMuted, and the very same struct, built by the
// very same Dialogs::BadgesForUnread(), is what decides which chats are
// counted into the application badge - the number on the Windows taskbar, the
// macOS dock tile and the Linux launcher. Clearing the flag there would not
// recolour anything at all; it would silently change that number. So the
// override lives in the PAINT path only, at the top of
// Dialogs::Ui::PaintBadges(), which takes its BadgesState by value - nothing
// outside that one paint call ever observes the modified flag.
//
// SCOPE. Only the unread badge: the counter, and the counter-less "unread
// mark" dot, which is the same badge drawn with an empty label. The mention,
// reaction and poll badges carry their own muted flags and are deliberately
// left alone - Android recolours those only on folder rows, and on desktop
// they are another feature's colours.
//
// The date on the right and the pinned icon have no muted variant on desktop
// at all - Android's getTimeTextPaint() and dialogs_pinnedDrawable2Accent have
// no counterpart here - so for them there is nothing to un-mute.
//
// BEHAVIOUR NEUTRALITY. The preference defaults to false, and while it is off
// UnreadBadgeMuted() returns its argument unchanged, so the paint path is
// exactly the one that was there before.

// Preference `showMutedCount`, Store::Prefs, default false. Same key name as
// Android. Main thread only.
[[nodiscard]] bool ShowMutedCount();
void SetShowMutedCount(bool value);
[[nodiscard]] rpl::producer<bool> ShowMutedCountValue();
[[nodiscard]] rpl::producer<> ShowMutedCountChanges();

// The muted flag the unread badge should be PAINTED with, given the flag the
// row actually carries. This runs once per row per frame, so the caller must
// only ask for rows that actually draw an unread badge, and the short circuit
// below keeps an unmuted row from touching the preference store at all.
[[nodiscard]] bool UnreadBadgeMuted(bool muted);

} // namespace Lumina
