/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include <rpl/producer.h>

namespace Lumina {

// "Disable link preview by default". Android: LuminaConfig
// "disableLinkPreview", read into ChatActivityEnterView.messageWebPageSearch
// at :776 and re-read at :6041 on every large text change. The key name is
// shared with the phone so a backup restored from it lands on the same
// preference.
//
// With it on, a message composed from scratch goes out with no_webpage set,
// and - because HistoryView::Controls::WebpageProcessor stops before it asks
// the resolver - the pasted link is never sent to Telegram to be looked up
// either. A preview can still be attached to a single message by hand: bind
// "Toggle link preview" under Settings > Keyboard shortcuts, which has no
// default binding upstream, and press it while composing.
//
// Default OFF. With the preference unset nothing consults it and the composer
// behaves exactly as stock Telegram Desktop's does.
[[nodiscard]] bool LinkPreviewOffByDefault();
void SetLinkPreviewOffByDefault(bool value);

// Reactive form of the getter, for the settings row. Fires on a whole-file
// restore through Settings::importAll() as well, so a value changed in a
// second window or brought back from a backup is reflected.
[[nodiscard]] rpl::producer<bool> LinkPreviewOffByDefaultValue();

} // namespace Lumina
