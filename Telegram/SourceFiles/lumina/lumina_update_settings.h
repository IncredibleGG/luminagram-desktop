/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

namespace Ui {
class VerticalLayout;
} // namespace Ui

namespace Lumina {

// The LuminaGram update block: which build is running, which channel it
// follows, and a way to ask our feed for a newer one right now.
//
// It drives Core::UpdateChecker, the same object stock Settings > Advanced
// drives, rather than a second checker of its own - there is one Updater per
// process and every UpdateChecker is a handle onto it, so a check started here
// is the same check that page would report, and its progress, result and
// failure arrive on both.
//
// Two consequences of that shared updater are visible in this block:
//
//  * Updater::start() returns immediately unless cAutoUpdate() is set, so the
//    "check now" row would do nothing with automatic updates off. The row
//    therefore lives behind the same toggle stock uses, and the toggle is
//    shown here too - it is the one control that makes the rest of the block
//    work.
//  * the toggle's initial value is read once, when the rows are built. Stock
//    does the same, and both pages rebuild when they are opened, so the only
//    way to see a stale value is to have both open at once.
//
// A feed that cannot be reached is not an error state to act on: the checker
// fails, the row says so, and the app carries on - which is what stock does
// and what this fork must keep doing, since our feed is one small bucket.
//
// Nothing here can install anything but LuminaGram: the feed prefix is pinned
// in storage/localstorage.cpp and every package is verified against the single
// public key in config.h.
void AddUpdateRows(not_null<Ui::VerticalLayout*> container);

} // namespace Lumina
