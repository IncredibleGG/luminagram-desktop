/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_dual_language_line.h"

#include "base/flat_map.h"
#include "base/debug_log.h" // DUALDIAG temporary
#include "base/flat_set.h"
#include "data/data_session.h"
#include "history/history.h"
#include "history/history_item.h"
#include "history/history_item_components.h"
#include "history/view/history_view_element.h"
#include "history/view/media/history_view_media.h"
#include "lumina/lumina_translate_gating.h"
#include "lumina/lumina_translate_settings.h"
#include "ui/chat/chat_style.h"
#include "ui/painter.h"
#include "ui/text/text.h"
#include "styles/style_chat.h"
#include "styles/style_lumina.h"
#include "styles/style_chat_helpers.h"

#include <rpl/lifetime.h>
#include <rpl/producer.h>

#include <algorithm>

namespace Lumina {
namespace {

using HistoryView::Element;

// The sub-line is a provider's plain string, never markup. Parsing links or
// mentions in it would paint them in the link colour while nothing here
// installs a click handler for them, so the line would look interactive and
// not be. Multiline only, which still lets the text engine wrap and replace
// emoji.
TextParseOptions kSubLineOptions = {
	TextParseMultiline, // flags
	0, // maxw
	0, // maxh
	Qt::LayoutDirectionAuto, // dir
};

struct Line {
	Ui::Text::String text;
	QString source;
	QString original; // Only filled on the outgoing branch, for `changed`.
	int width = 0;
	int height = 0;
	bool infoRow = false;
	bool external = false;
};

// Views that currently show a sub-line. Deliberately NOT a
// RuntimeComponent<..., Element>: the runtime composer hands out component ids
// from a single global pool of 64 for the whole application
// (Telegram/lib_base/base/runtime_composer.h, `Assert(last < 64)` inside
// RuntimeComponent::Index()), and this checkout already declares 65 component
// types across HistoryItem, Element, HistoryView::Document and LayoutItemBase.
// Claiming one more would be a coin flip on an assertion at runtime, decided
// by which components a given session happens to touch first.
[[nodiscard]] base::flat_map<const Element*, Line> &Lines() {
	static auto result = base::flat_map<const Element*, Line>();
	return result;
}

// Views that WOULD show a sub-line if both toggles were on. Maintained even
// while the feature is off, because that is what makes flipping the toggle on
// able to find the bubbles it has to invalidate: with the feature off there
// are no entries in Lines() to walk, and there is no public way to enumerate
// every loaded view (Data::Session::enumerateItemViews and the _views map are
// private, and views of the ListWidget-hosted sections are not in
// History::blocks either).
//
// This stays tiny: a view only lands here when its item already carries a used
// translation, or is an outgoing message with a stored pre-translation
// original. On a profile that never translates anything the set is empty and
// costs one predicate per plain-text bubble layout.
[[nodiscard]] base::flat_set<const Element*> &Candidates() {
	static auto result = base::flat_set<const Element*>();
	return result;
}

[[nodiscard]] auto OutgoingLookup()
-> Fn<QString(not_null<const HistoryItem*>)> & {
	static auto result = Fn<QString(not_null<const HistoryItem*>)>();
	return result;
}

[[nodiscard]] rpl::lifetime &Subscription() {
	static auto result = rpl::lifetime();
	return result;
}

[[nodiscard]] bool &LastActive() {
	static auto result = false;
	return result;
}

// Dual-language display is continuous-tier: it paints a translation for every
// message that has one, for as long as it is on, so it goes through the tier
// predicate rather than through the master switch directly. That predicate is
// the only place a licence check may ever appear.
[[nodiscard]] bool FeatureActive() {
	return ContinuousTranslationAvailable() && DualLanguageDisplay();
}

// Only plain text bubbles, mirroring Android's luminaKeepOriginalAsMain
// (MessageObject.java): no sponsored, no service, no restricted, and no media
// at all - which on desktop is the single predicate that covers Android's
// web page, game, invoice, story mention and giveaway exclusions at once,
// because every one of those is a HistoryItem::media() here.
//
// Excluding media disposes of the own-media half of
// HistoryItem::withLocalEntities(), which is the larger half of the difference
// between translatedText() and translatedTextWithLocalEntities(). The other
// two halves are NOT covered and are reproduced instead of excluded, below:
// the reply half of withLocalEntities() in WithReplyTimestampLinks(), and the
// hideLinks() link filter in WithHiddenLinks().
//
// hideLinks() is deliberately NOT a rejection here, though it reads like the
// safe choice. HistoryItem::hideLinks() is `!out() && peer->hideLinks()`, and
// PeerData::hideLinks() (data_peer.cpp) answers TRUE while barSettings() is
// still nullopt - which it is for every peer until its settings come back from
// the server (PeerBarSetting::Unknown, data_peer.h). Rejecting on it would
// therefore turn the whole incoming half of this feature off for a window
// after every chat is opened, and permanently for any peer showing the report
// spam bar - a stranger writing in a foreign language, i.e. exactly the chat
// this feature exists for. Worse, nothing would bring it back: the only
// re-layout on that transition is History::refreshHiddenLinksItems(), and that
// only touches items whose hasHiddenLinks() flag is already set, which a
// message with no links never has.
[[nodiscard]] bool Renderable(not_null<HistoryItem*> item) {
	return !item->isService()
		&& !item->isSponsored()
		&& !item->media()
		&& !item->Has<HistoryMessageLogEntryOriginal>()
		&& !item->Has<HistoryMessageFactcheck>()
		&& !item->translatedRichPage()
		&& item->computeUnavailableReason().isEmpty();
}

struct Source {
	QString sub;
	QString original; // Only filled on the outgoing branch.
	bool external = false;

	[[nodiscard]] explicit operator bool() const {
		return !sub.isEmpty();
	}
};

// The TRANSLATION, which is always what the small line shows. The two
// directions reach it from opposite sides, and keeping them apart here is the
// whole point of this function: on the incoming side the item carries the
// translation and its own text is the original, on the outgoing side the
// item's own text IS the translation and the original lives outside the item.
//
// The cheap tests come first on purpose. This runs during the layout of every
// bubble in the app, including with the whole feature switched off, so an
// ordinary untranslated message must cost a bit test and a pointer compare
// and nothing else - Renderable() and its computeUnavailableReason() call are
// only reached once a message really does have a second language to show.
//
// An outgoing message can be BOTH: sent through translate-before-send and
// then translated again by the read side when the whole chat is being
// translated. The stored pre-translation original wins, and when there is
// none the outgoing branch falls through to the translation component, so an
// own message in an auto-translated chat is rendered exactly like an incoming
// one instead of silently losing its second line.
[[nodiscard]] Source ComputeOutgoingSource(not_null<HistoryItem*> item) {
	const auto &lookup = OutgoingLookup();
	if (!lookup) {
		return {};
	}
	const auto &sent = item->originalText();
	if (sent.text.isEmpty()) {
		return {};
	}
	const auto original = lookup(item).trimmed();
	if (original.isEmpty()
		|| original == sent.text
		|| !Renderable(item)) {
		return {};
	}
	return { .sub = sent.text, .original = original, .external = true };
}

[[nodiscard]] Source ComputeSource(HistoryItem *item) {
	if (!item) {
		return {};
	} else if (item->out()) {
		if (auto outgoing = ComputeOutgoingSource(item)) {
			return outgoing;
		}
	}
	const auto &translated = item->translatedText();
	const auto &original = item->originalText();
	// original.text is what becomes the MAIN bubble text, so it has to be
	// checked for emptiness too: overriding the bubble with an empty string
	// would blank the message out entirely.
	if (&translated == &original
		|| translated.text.isEmpty()
		|| original.text.isEmpty()
		|| translated.text == original.text
		|| !Renderable(item)) {
		return {};
	}
	return { .sub = translated.text, .external = false };
}

// The half of HistoryItem::withLocalEntities() that survives the no-media
// rule, and the reason ComputeMainText() cannot just hand back originalText().
//
// withLocalEntities() is private, and it does two different things: for an
// item that carries media with a duration it makes timestamps in that item's
// own text clickable, and for an item WITHOUT media it makes timestamps
// clickable against the media of the message being replied to. Renderable()
// excludes media, so only the second half applies here - but it very much
// does apply: a plain "see 1:23" reply to a voice message gets those links
// from stock, and taking originalText() raw would silently drop them.
//
// This mirrors history_item.cpp's reply branch exactly, including resolving
// the document/webpage through the owner by id.
[[nodiscard]] TextWithEntities WithReplyTimestampLinks(
		not_null<HistoryItem*> item,
		TextWithEntities text) {
	const auto reply = item->Get<HistoryMessageReply>();
	if (!reply || !reply->resolvedMessage) {
		return text;
	}
	auto &owner = item->history()->owner();
	const auto context = reply->resolvedMessage->fullId();
	if (const auto id = reply->replyToDocumentId) {
		const auto document = owner.document(id);
		const auto duration = HistoryView::DurationForTimestampLinks(document);
		if (duration) {
			return HistoryView::AddTimestampLinks(
				std::move(text),
				duration,
				HistoryView::TimestampLinkBase(document, context));
		}
	} else if (const auto id = reply->replyToWebPageId) {
		const auto webpage = owner.webpage(id);
		const auto duration = HistoryView::DurationForTimestampLinks(webpage);
		if (duration) {
			return HistoryView::AddTimestampLinks(
				std::move(text),
				duration,
				HistoryView::TimestampLinkBase(webpage, context));
		}
	}
	return text;
}

// The link filter of HistoryItem::translatedTextWithLocalEntities(), applied
// here because Renderable() no longer rejects hideLinks() messages - see the
// note there. This mirrors history_item.cpp exactly, including the order (the
// filter runs AFTER the withLocalEntities half) and the setHasHiddenLinks()
// side effect, which is what History::refreshHiddenLinksItems() looks for when
// the peer's bar settings finally arrive and the links are allowed back.
//
// The sub-line itself needs no filtering: it is parsed with
// kSubLineOptions, which recognises no entities at all.
[[nodiscard]] TextWithEntities WithHiddenLinks(
		not_null<HistoryItem*> item,
		TextWithEntities text) {
	if (!item->hideLinks()) {
		return text;
	}
	const auto isUrl = [](const EntityInText &entity) {
		const auto type = entity.type();
		return (type == EntityType::Mention)
			|| (type == EntityType::Hashtag)
			|| (type == EntityType::Cashtag)
			|| (type == EntityType::Url)
			|| (type == EntityType::CustomUrl);
	};
	const auto from = std::remove_if(
		text.entities.begin(),
		text.entities.end(),
		isUrl);
	if (from != text.entities.end()) {
		text.entities.erase(from, text.entities.end());
		item->setHasHiddenLinks(true);
	}
	return text;
}

[[nodiscard]] TextWithEntities ComputeMainText(
		not_null<HistoryItem*> item,
		const Source &source) {
	return WithHiddenLinks(item, WithReplyTimestampLinks(item, source.external
		? TextWithEntities{ source.original }
		: item->originalText()));
}

bool DropLine(not_null<const Element*> view) {
	return Lines().remove(view.get());
}

void RefreshLoadedBubbles() {
	if (Candidates().empty()) {
		return;
	}
	auto items = base::flat_set<not_null<HistoryItem*>>();
	for (const auto &view : Candidates()) {
		items.emplace(view->data());
	}
	// requestItemTextRefresh() calls Element::itemTextUpdated(), which drops
	// the laid-out text entirely. That is what defeats validateText()'s
	// `_text.isEmpty() == text.empty()` early-out for bubbles that are
	// already on screen, and it is the same route stock tdesktop uses when a
	// translation arrives (HistoryItem::translationToggle).
	for (const auto &item : items) {
		item->history()->owner().requestItemTextRefresh(item);
	}
}

void EnsureSubscribed() {
	static auto subscribed = false;
	if (subscribed) {
		return;
	}
	subscribed = true;
	LastActive() = FeatureActive();
	// Both locals are load-bearing. Function-local statics are destroyed in
	// reverse order of construction, so the lifetime that owns this
	// subscription must be constructed AFTER every static the producer
	// touches (the settings singleton, the provider-changes stream), or at
	// exit it would unsubscribe from an event stream that is already gone.
	// Building the producer first, into a named local, forces that order;
	// the two sides of `producer | on_next(..., Subscription())` are not
	// sequenced against each other.
	//
	// The tier term is merged in rather than left to
	// TranslateSettingsChanges(), which only carries preference keys: today
	// the two agree, because the tier predicate reads one of those keys, but
	// a licence check added inside it would move FeatureActive() with nothing
	// here waking up. It costs one extra evaluation on subscribe, which the
	// LastActive() comparison below discards.
	auto changes = rpl::merge(
		TranslateSettingsChanges(),
		ContinuousTranslationAvailableValue() | rpl::to_empty);
	auto &lifetime = Subscription();
	std::move(changes) | rpl::on_next([] {
		const auto now = FeatureActive();
		if (LastActive() == now) {
			return;
		}
		LastActive() = now;
		RefreshLoadedBubbles();
	}, lifetime);
}

} // namespace

void SetOutgoingOriginalLookup(
		Fn<QString(not_null<const HistoryItem*>)> lookup) {
	OutgoingLookup() = std::move(lookup);

	// This is reached from dynamic initialisation (lumina_translate_originals'
	// Registrar), i.e. before QApplication exists. Reading a preference here
	// would construct Lumina::Settings, whose save timer is a base::Timer and
	// therefore a QObject - constructing one before the application does not
	// survive. It crashed the app before it could even open its log.
	//
	// Checking the candidate set first costs nothing and keeps the whole
	// preference layer out of static init: at that point nothing is laid out,
	// so there is never anything to refresh anyway.
	if (Candidates().empty()) {
		return;
	}
	if (FeatureActive()) {
		RefreshLoadedBubbles();
	}
}

bool HasOutgoingOriginalLookup() {
	return OutgoingLookup() != nullptr;
}

void RefreshDualLanguage(not_null<HistoryItem*> item) {
	if (!FeatureActive()) {
		return;
	}
	item->history()->owner().requestItemTextRefresh(item);
}

DualLanguageUpdate ValidateDualLanguage(
		not_null<const Element*> view,
		HistoryItem *textItem) {
	EnsureSubscribed();

	const auto source = ComputeSource(textItem);
	if (!source) {
		Candidates().remove(view.get());
		return { .changed = DropLine(view) };
	}
	Candidates().emplace(view.get());
	if (!FeatureActive()) {
		return { .changed = DropLine(view) };
	}
	const auto item = not_null<HistoryItem*>(textItem);
	auto &lines = Lines();
	const auto i = lines.find(view.get());
	if (i == lines.end()) {
		auto line = Line();
		line.source = source.sub;
		line.original = source.original;
		line.external = source.external;
		line.text.setText(st::luminaTranslationTextStyle, source.sub, kSubLineOptions);
		lines.emplace(view.get(), std::move(line));
		return {
			.mainText = ComputeMainText(item, source),
			.overrideMainText = true,
			.changed = true,
		};
	}
	auto &line = i->second;
	auto changed = false;
	if (line.source != source.sub || line.external != source.external) {
		line.source = source.sub;
		line.external = source.external;
		line.width = 0;
		line.height = 0;
		line.text.setText(st::luminaTranslationTextStyle, source.sub, kSubLineOptions);
		changed = true;
	}
	// The main text of an outgoing bubble comes from outside the item, so
	// nothing else in validateText() can notice that it changed. Without this
	// the bubble keeps whatever original it resolved the first time, which is
	// exactly the shape of the Android randoms_v2 bug: W2-B re-keys the store
	// at the local-id -> server-id swap and the already laid out bubble never
	// looks again. RefreshDualLanguage() is still the prompt path; this is the
	// backstop that makes any later relayout self-correcting.
	if (line.original != source.original) {
		line.original = source.original;
		changed = true;
	}
	return {
		.mainText = ComputeMainText(item, source),
		.overrideMainText = true,
		.changed = changed,
	};
}

void ClearDualLanguage(not_null<const Element*> view) {
	if (!Lines().empty()) {
		Lines().remove(view.get());
	}
	if (!Candidates().empty()) {
		Candidates().remove(view.get());
	}
}

void ForgetDualLanguage(not_null<const Element*> view) {
	ClearDualLanguage(view);
}

bool DualLanguageShown(not_null<const Element*> view) {
	return !Lines().empty() && Lines().contains(view.get());
}

bool DualLanguageMainTextIsExternal(not_null<const Element*> view) {
	if (Lines().empty()) {
		return false;
	}
	const auto i = Lines().find(view.get());
	return (i != Lines().end()) && i->second.external;
}

int DualLanguageMaxWidth(not_null<const Element*> view) {
	if (Lines().empty()) {
		return 0;
	}
	const auto i = Lines().find(view.get());
	if (i == Lines().end()) {
		return 0;
	}
	return st::msgPadding.left()
		+ i->second.text.maxWidth()
		+ st::msgPadding.right();
}

int DualLanguageResizeToWidth(
		not_null<const Element*> view,
		int width,
		bool reserveBottomInfoRow) {
	if (Lines().empty()) {
		return 0;
	}
	const auto i = Lines().find(view.get());
	if (i == Lines().end()) {
		return 0;
	}
	auto &line = i->second;
	if (width < 1) {
		line.width = 0;
		line.height = 0;
		return 0;
	} else if (line.width != width || line.infoRow != reserveBottomInfoRow) {
		line.width = width;
		line.infoRow = reserveBottomInfoRow;
		line.height = st::mediaInBubbleSkip
			+ line.text.countHeight(width)
			+ (reserveBottomInfoRow ? st::msgDateFont->height : 0);
		LOG(("DUALDIAG line: width=%1 reserveRow=%2 countHeight=%3 textMaxWidth=%4 "
			"textMinHeight=%5 height=%6").arg(width).arg(reserveBottomInfoRow ? 1 : 0)
			.arg(line.text.countHeight(width)).arg(line.text.maxWidth())
			.arg(line.text.minHeight()).arg(line.height));
	}
	return line.height;
}

int DualLanguageHeight(not_null<const Element*> view) {
	if (Lines().empty()) {
		return 0;
	}
	const auto i = Lines().find(view.get());
	return (i != Lines().end()) ? i->second.height : 0;
}

void PaintDualLanguage(
		Painter &p,
		not_null<const Element*> view,
		const Ui::ChatPaintContext &context,
		int x,
		int y,
		int w) {
	if (Lines().empty()) {
		return;
	}
	const auto i = Lines().find(view.get());
	if (i == Lines().end()) {
		return;
	}
	const auto &line = i->second;
	if (!line.height || w < 1) {
		return;
	}
	const auto stm = context.messageStyle();
	LOG(("DUALDIAG paint: w=%1 lineWidth=%2 lineHeight=%3 countHeightAtW=%4")
		.arg(w).arg(line.width).arg(line.height).arg(line.text.countHeight(w)));
	p.setPen(stm->msgDateFg);
	line.text.draw(p, {
		.position = { x, y + st::mediaInBubbleSkip },
		.outerWidth = w,
		.availableWidth = w,
		.palette = &stm->textPalette,
		.now = context.now,
	});
}

} // namespace Lumina
