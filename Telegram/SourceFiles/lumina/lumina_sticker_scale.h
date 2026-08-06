/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include <rpl/producer.h>

#include <array>

namespace Lumina {

// Android's `stickerScale`: a percentage that resizes the stickers drawn in a
// chat. Written by LuminaAppearanceActivity (four fixed choices, 75 / 100 /
// 125 / 150), read in ChatMessageCell.java's sticker layout branch, where it
// multiplies photoWidth/photoHeight and is clamped to 50..250 on the way in.
// 100 means "exactly the size the app drew before this existed".
//
// WHAT IT RESIZES, AND WHAT IT DELIBERATELY DOES NOT. Android applies the
// percentage inside the plain-sticker branch only: the animated-emoji and dice
// branch above it computes its size from animatedEmojisZoom and is left alone.
// Desktop has the same split - HistoryView::Sticker::EmojiSize() serves emoji
// and dice, Sticker::Size() serves plain stickers - so the percentage is
// applied to Sticker::Size() and EmojiSize() is not touched.
//
// Sticker::Size() is also the final box that Sticker::initSize() downscales
// every OTHER sticker-ish media into (gift box, gift theme, a sticker set
// preview inside a web page, a custom emoji). Those all pass an explicit
// custom size that is far below the box, so at every percentage this fork
// offers (75 and up, box 168px and up against a 140px largest custom size)
// they come out unchanged. Only a value below ~63%, which the settings page
// cannot produce and only a hand-edited preference or a restored backup can,
// would start clipping them - and clipping is then the honest reading of
// "draw stickers smaller".
//
// WHY THE APPLIED VALUE IS LATCHED FOR THE RUN. Android can afford to apply
// this live because its RecyclerView re-binds and re-measures every visible
// cell. Desktop cannot: HistoryView::Sticker caches its measured `_size` in
// the media object, that object outlives scrolling, and NOTHING in the tree
// clears it - not Data::Session::requestViewResize(), not
// History::forceFullResize(), not Data::Session::unloadHeavyViewParts(), all
// of which only re-run a measurement that returns the cached value. Applying a
// new percentage immediately would therefore resize the stickers that happen
// to be drawn from then on and leave every already-measured sticker at its old
// size, in the same history, forever.
//
// So the value is read once per run: every sticker in one run of the app is
// laid out with the same percentage, and a change takes effect on the next
// start. That is exactly how tdesktop handles its own layout metrics of this
// class - the interface scale slider offers a restart rather than applying
// itself, and both `use-small-msg-bubble-radius` and
// `unlimited-message-width` are declared `.restartRequired = true`. The
// settings page offers the same restart while the two values diverge.
//
// ONE KNOWN LIMIT, AT THE LARGEST SIZE IN THE SMALLEST WINDOW. Unwrapped media
// reports its own size and is never clipped to the width the bubble was given
// (HistoryView::UnwrappedMedia::countCurrentSize), which has never mattered
// because a sticker could not exceed 224px while the narrowest possible chat
// column is around 308px. At 150% a sticker is 336px, so a window within about
// 30px of tdesktop's 380px minimum width draws it slightly past the bubble.
// Bounding the percentage by the minimum window instead would silently shrink
// the setting for every normally sized window, which is the worse trade.
//
// Main thread only.

// The four values the settings page offers, matching Android's radio list.
inline constexpr auto kStickerScaleChoices = std::array{ 75, 100, 125, 150 };

inline constexpr auto kStickerScaleDefault = 100;

// The defensive bounds, same as Android's Math.max(50, Math.min(250, ...)).
// They exist for values the settings page cannot produce: a restored backup or
// a hand-edited luminagram.json. 0 would make stickers invisible and a huge
// value would try to allocate a huge frame.
inline constexpr auto kStickerScaleMin = 50;
inline constexpr auto kStickerScaleMax = 250;

// The stored preference `stickerScale`, Store::Prefs, default 100, clamped to
// kStickerScaleMin..kStickerScaleMax. This is what the settings page shows and
// writes; it is NOT what the running app is drawing with - see
// AppliedStickerScale().
[[nodiscard]] int StickerScale();
void SetStickerScale(int percent);
[[nodiscard]] rpl::producer<int> StickerScaleValue();
[[nodiscard]] rpl::producer<> StickerScaleChanges();

// The percentage this run of the app is laying stickers out with. Latched from
// StickerScale() at the first call and constant afterwards, so no two stickers
// can ever be measured with different percentages.
//
// Cheap enough for a layout path. Do not call it before QApplication exists:
// the first call reads a preference.
[[nodiscard]] int AppliedStickerScale();

// True while the stored preference differs from the one in effect, i.e. while
// a restart would change what is on screen.
//
// It does NOT latch: while no sticker has been measured yet - which is the
// state the whole app is in until a sticker is actually drawn - the stored
// value is still the one the run will use, so this is false and the settings
// page correctly offers no restart.
[[nodiscard]] bool StickerScaleNeedsRestart();

// `length` scaled by AppliedStickerScale(), rounded the way every other length
// in the app is rounded - style::ConvertScale's own rule, with the percentage
// standing in for the interface scale. Returns `length` untouched at 100%.
//
// Feed it a length that has NOT been through style::ConvertScale yet wherever
// one is available: the percentage belongs before the interface scale, not
// after, so that the two roundings do not stack. An already-converted length
// (a style `px` value, which the style loader converted for us) is fine too -
// it is then simply the one remaining conversion.
[[nodiscard]] int ScaleStickerLength(int length);

} // namespace Lumina
