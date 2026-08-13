/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "base/basic_types.h"
#include "ui/text/text_entity.h"

#include <memory>

class HistoryItem;
class Painter;
class ClickHandler;

namespace Ui {
struct ChatPaintContext;
} // namespace Ui

namespace HistoryView {
class Element;
} // namespace HistoryView

namespace Lumina {

// Dual-language bubble rendering: the ORIGINAL stays the main (full-strength)
// bubble text and the TRANSLATION is drawn under it as a small dimmed line.
//
// Two directions, one component:
//
//  * incoming - stock tdesktop REPLACES the bubble text with the translation
//    (history_view_element.cpp calls translatedTextWithLocalEntities()). Here
//    the main text is taken from HistoryItem::originalText() instead and the
//    sub-line is HistoryItem::translatedText();
//  * outgoing translate-before-send - the item's own text IS the translation
//    that went out, so the data flow is inverted: the sub-line is the item's
//    own text and the MAIN text comes from W2-B's originals store, through
//    the lookup installed below.
//
// Both are inert unless BOTH Lumina::TranslationFeatureEnabled() (the master
// opt-in, default false) and Lumina::DualLanguageDisplay() (default false) are
// on. With either off nothing is stored, measured or painted, and every entry
// point below is an O(1) no-op on an empty table.

// The pre-translation original of an OUTGOING translate-before-send message.
//
// W2-B (lumina/lumina_translate_originals) owns that store. It installs the
// lookup here instead of this file including that header, so the renderer and
// the store do not depend on each other's build order and either can land
// first. Until a lookup is installed the outgoing half is simply inert - the
// incoming half works on its own.
//
// The lookup must be cheap: it is consulted during layout for every plain
// outgoing text message. Returning an empty string means "no original".
void SetOutgoingOriginalLookup(Fn<QString(not_null<const HistoryItem*>)> lookup);

// True while an outgoing-original lookup is installed. Only useful for W2-B's
// own wiring assertions and for tests.
[[nodiscard]] bool HasOutgoingOriginalLookup();

// W2-B calls this whenever the stored original for one message appears,
// changes or moves - in particular at the local-id -> server-id re-key, which
// is the desktop equivalent of the Android bug where the original was keyed by
// random_id alone and vanished on reload. A bubble that is already laid out
// resolved its original a moment ago and will not look again on its own.
// No-op while the feature is off, and for a message with no loaded view.
void RefreshDualLanguage(not_null<HistoryItem*> item);

// Re-evaluate this view's sub-line. Called from Element::validateText().
//
// `changed` is the answer to "does Element::_text have to be rebuilt?", and it
// is what defeats the early-out at the top of validateText():
// `_text.isEmpty() == text.empty()` stays true across a toggle flip, so
// without this the swap between the original and the translation would never
// reach an already laid-out bubble.
//
// When `overrideMainText` is set, `mainText` is what the bubble must show
// instead of translatedTextWithLocalEntities().
struct DualLanguageUpdate {
	TextWithEntities mainText;
	bool overrideMainText = false;
	bool changed = false;
};

[[nodiscard]] DualLanguageUpdate ValidateDualLanguage(
	not_null<const HistoryView::Element*> view,
	HistoryItem *textItem);

// Drop any sub-line for a view whose text is now something else entirely - a
// summary, an expired story, an album whose text item went away. Cheap no-op
// when the view has no sub-line.
void ClearDualLanguage(not_null<const HistoryView::Element*> view);

// Called from ~Element. Nothing else may keep a raw Element pointer alive.
void ForgetDualLanguage(not_null<const HistoryView::Element*> view);

// True when this view currently shows a sub-line, i.e. when its main text is
// the original rather than what stock tdesktop would have shown. Message uses
// it to suppress the bottom-info skip block (the sub-line takes the place of
// the last text line) and to keep reply-quoting off an outgoing bubble whose
// shown text no longer matches the item's own text.
[[nodiscard]] bool DualLanguageShown(
	not_null<const HistoryView::Element*> view);

// True only for the OUTGOING branch, where Element::text() is the stored
// original while HistoryItem::_text is the sent translation. Any code that
// maps a selection in the shown text onto the item's own text is wrong for
// exactly these bubbles.
[[nodiscard]] bool DualLanguageMainTextIsExternal(
	not_null<const HistoryView::Element*> view);

// Bubble width contribution, already including the horizontal message
// paddings, so it can go straight into accumulate_max(maxWidth, ...). Zero
// when there is no sub-line.
[[nodiscard]] int DualLanguageMaxWidth(
	not_null<const HistoryView::Element*> view);

// Lays the sub-line out at `width` (the inner text width) and returns the
// height it reserves inside the bubble: a top skip, the wrapped line, and -
// when `reserveBottomInfoRow` - one more `st::msgDateFont->height` row so the
// timestamp gets a row of its own instead of painting over the translation.
// Android needed the same extra row (ChatMessageCell, dp(14)).
int DualLanguageResizeToWidth(
	not_null<const HistoryView::Element*> view,
	int width,
	bool reserveBottomInfoRow);

// The height last returned by DualLanguageResizeToWidth(), zero when there is
// no sub-line.
[[nodiscard]] int DualLanguageHeight(
	not_null<const HistoryView::Element*> view);

// Paints the sub-line. (x, y) is the top-left of the reserved block, so the
// top skip is applied here and not by the caller.
void PaintDualLanguage(
	Painter &p,
	not_null<const HistoryView::Element*> view,
	const Ui::ChatPaintContext &context,
	int x,
	int y,
	int w);

// ---------------------------------------------------------------------------
// Fold original (long bilingual messages).
//
// The ORIGINAL is the bubble's MAIN text in both directions (incoming: the
// item's own text; outgoing: the stored original installed as the main text
// through overrideMainText), so folding it means folding Element::_text. Only
// the height changes: the bubble width, the sub-line and the timestamp row are
// untouched. Everything here is transient per view - stored in the same table
// the sub-line uses, cleared by ForgetDualLanguage() - so a fold that the user
// expanded reverts on reload or restart, exactly like Android's
// luminaOriginalExpanded and tdesktop's own collapsible quotes.
//
// Every entry point is a no-op for a view that has no sub-line, so nothing here
// touches an ordinary (non dual-language) bubble.

// Height reserved by the "show original" affordance row that sits between the
// single folded line and the sub-line. Constant.
[[nodiscard]] int OriginalFoldAffordanceHeight();

// Called from resizeContentGetHeight() once the main text is laid out and the
// bubble is NOT mid appear-animation. `mainTextHeight` is the full laid-out
// height of the main text at `width`, `lineHeight` its per-line height and
// `lineCount` the number of laid-out lines. Decides whether the original folds
// (setting on, not already expanded, more than the line threshold), stores the
// decision and geometry, and returns the number of pixels to REMOVE from the
// bubble height (full height minus the one-line-plus-affordance height); zero
// when it does not fold.
[[nodiscard]] int ResolveOriginalFold(
	not_null<HistoryView::Element*> view,
	int width,
	int mainTextHeight,
	int lineHeight,
	int lineCount);

// Forget any fold decision for this view (used while its text is appearing, so
// a stale "folded" flag never outlives the animation).
void ClearOriginalFold(not_null<const HistoryView::Element*> view);

// True while this view's original is folded to a single line.
[[nodiscard]] bool OriginalFolded(not_null<const HistoryView::Element*> view);

// Height of the single shown original line while folded (0 otherwise). The
// affordance row sits directly beneath it.
[[nodiscard]] int OriginalFoldedLineHeight(
	not_null<const HistoryView::Element*> view);

// The click handler that expands a folded original, null unless folded. It sets
// the transient expanded flag and requests a resize; the next layout unfolds.
[[nodiscard]] std::shared_ptr<ClickHandler> OriginalFoldExpandHandler(
	not_null<const HistoryView::Element*> view);

// Paint the "show original" affordance (a chevron and a localised label in the
// message's link colour). (x, y) is its top-left; it is
// OriginalFoldAffordanceHeight() tall and `w` wide.
void PaintOriginalFoldAffordance(
	Painter &p,
	not_null<const HistoryView::Element*> view,
	const Ui::ChatPaintContext &context,
	int x,
	int y,
	int w);

} // namespace Lumina
