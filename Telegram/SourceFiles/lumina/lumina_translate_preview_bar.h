/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include <rpl/producer.h>

#include <QtCore/QString>

class History;

namespace Ui {
class RpWidget;
class InputField;
} // namespace Ui

namespace Lumina {

// The live send-translation preview: a one-line-plus-one-line bar directly
// above the composer showing the text as typed and, under it, the translation
// that translate-before-send will actually put on the wire.
//
// Layout, deliberately the opposite of Android's panel: the ORIGINAL is the
// dominant primary line and the TRANSLATION the quieter secondary one,
// matching the dual-language bubble rendering on the read side. Android
// shipped it the other way round (a 13sp hint-coloured original over a 15sp
// translation) and the decision was explicitly reversed for this fork; both
// directions now agree.
//
// The size difference lands as weight plus colour rather than as point size -
// original in st::msgServiceNameFont / st::historyComposeAreaFg over
// translation in st::msgDateFont / st::historyComposeAreaFgService - because
// this item may not add a .style file (a new one has to be registered in
// Telegram/CMakeLists.txt, which belongs to the orchestrator) and the tree has
// no smaller general-purpose font constant that is not already spoken for by
// an unrelated widget. Hard-coded pixel sizes are not an option: they do not
// survive a non-100% interface scale. It reads as the same primary/secondary
// split the reply bar directly underneath it uses, which is the point.
//
// No HistoryWidget or ComposeControls member is added for it. Neither of those
// headers belongs to this item, so the bar is created as an ordinary Qt child
// of the composer widget and looked up again through a small registry keyed on
// that parent. The host therefore holds nothing, and the whole feature can be
// added and removed by touching only the call sites below.
//
// The three call sites a host needs, and nothing else:
//
//   RefreshTranslatePreviewBar()  push the composer's current state; creates
//                                 the bar the first time it is actually needed
//   TranslatePreviewBarHeight()   the ONE height term, 0 whenever the bar is
//                                 not on screen
//   MoveTranslatePreviewBar()     place it; a no-op when there is no bar
//
// Behaviour neutrality: while Lumina::TranslateBeforeSendActive() is false -
// and it is false at every default, because
// Lumina::TranslationFeatureEnabled() and Lumina::TranslateBeforeSend() both
// default false - no widget is ever constructed, every height term is 0 and
// no provider request is made, so the composer lays out and paints exactly as
// stock does. The one thing that exists in that state is the host's rpl
// subscription to TranslatePreviewRefreshRequests(), which is what lets the
// bar appear when the user turns the feature on with a chat already open.
//
// Cost control: the preview is debounced (the provider is asked only after
// typing pauses) and cached against the exact source text and target language,
// so a metered API key is never charged per keystroke. Every request that is
// superseded - newer text, a different target language, a chat switch, the bar
// going away - is dropped by a generation counter before its result is used.
//
// When it is shown, and why it asks the send path instead of deciding: the bar
// is up exactly while Lumina::ResolveSendLanguage()
// (lumina/lumina_translate_send.h) answers a language, which is exactly when
// the next Send in this chat would be translated, and into what. That function
// is the send pipeline's own resolver - master opt-in, the send-menu quick
// toggle, translate-before-send, the scope and the per-dialog lock, in the
// pipeline's own order - so the bar can never promise a translation the send
// would not make, nor a language it would not use. A second spelling of that
// predicate here is the exact bug Android shipped, and it is also why this
// file does not re-decide the scope question: whatever the send pipeline does
// with it, the preview follows.
//
// It is NOT gated on TranslateBeforeSendConfirm(). That preference means "ask
// in a dialog before sending", and the pipeline already skips its dialog for a
// translation this bar produced: every result goes into
// Lumina::NoteSendTranslationPreview(), and the pipeline's reuse branch both
// avoids a second paid request and sends straight away - "the panel IS the
// confirmation", as on Android.

struct TranslatePreviewBarState {
	not_null<Ui::RpWidget*> parent;
	not_null<Ui::InputField*> field;
	History *history = nullptr;

	// The composer is editing an existing message, so nothing is going to be
	// translated on send. Voice / video recording needs no flag of its own:
	// the record button only replaces send while the field is empty, and an
	// empty field hides the bar anyway.
	bool editing = false;

	// Invoked when the bar appeared or went away on its own - the user typed
	// the first character, emptied the field, or the field was shown or
	// hidden. Always from a later main thread turn, never from inside this
	// call and never from inside a provider callback, so the host may
	// relayout, and therefore call back into RefreshTranslatePreviewBar(),
	// freely.
	Fn<void()> layoutChanged;
};

void RefreshTranslatePreviewBar(TranslatePreviewBarState state);

// The single height term to add at each of the host's layout sites. Zero when
// no bar was ever created for this parent, and zero while it is hidden. Takes
// a const parent because one of those sites (HistoryWidget's
// computeMaxFieldHeight) is a const member function.
//
// !! A non-zero answer must be followed by MoveTranslatePreviewBar() in the
// same layout pass. The bar reports its height before it is placed - the host
// needs the height to work out where it goes - and stays invisible until it is
// placed, so that it can never flash at whatever geometry it last had. A host
// that reserved the space and then did not place it would leave an empty gap.
[[nodiscard]] int TranslatePreviewBarHeight(
	not_null<const Ui::RpWidget*> parent);

// Places the bar and, if it should be on screen, reveals it. A no-op when
// there is no bar for this parent.
void MoveTranslatePreviewBar(
	not_null<Ui::RpWidget*> parent,
	int left,
	int top,
	int width);

// Fires when anything the bar's visibility depends on changes: the master
// opt-in, the translate-before-send toggle, the configured send language, the
// provider, or the per-dialog send-language lock. A host subscribes to this
// once and answers by re-running its geometry pass, which is what pushes the
// new state through RefreshTranslatePreviewBar(). This is the only part of the
// feature that costs anything while it is switched off.
[[nodiscard]] rpl::producer<> TranslatePreviewRefreshRequests();

// Read-only observers of the bar's state.
//
// The send path needs neither of these to reuse a translation: the bar pushes
// every result into Lumina::NoteSendTranslationPreview() as it lands, and the
// pipeline matches it on (chat, source text, target language). They exist for
// a caller that wants to know whether the user can already see what will be
// sent, which is Android's livePreviewVisible.
//
// TranslatePreviewTranslation() answers the translation the bar is showing for
// exactly this text, or an empty string. Empty covers "still translating",
// "the request failed" and "the text changed since", so an empty answer always
// means "do the translation yourself"; it never means "send it untranslated".
[[nodiscard]] bool TranslatePreviewShown(not_null<History*> history);
[[nodiscard]] QString TranslatePreviewTranslation(
	not_null<History*> history,
	const QString &original);

} // namespace Lumina
