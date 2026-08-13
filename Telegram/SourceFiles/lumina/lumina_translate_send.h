/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "api/api_common.h" // Api::SendOptions.

#include <QtCore/QString>

#include <memory>

class History;
struct TextWithTags;

namespace ChatHelpers {
class Show;
} // namespace ChatHelpers

namespace Ui {
class PopupMenu;
} // namespace Ui

namespace SendMenu {
struct Details;
} // namespace SendMenu

namespace Lumina {

// Translate-before-send: rewrite an outgoing text message into the language
// the recipient reads before it goes on the wire, and keep the original so the
// bubble can show both.
//
// This file owns the send *pipeline*. It plugs into Lumina::InterceptSend()
// (lumina/lumina_send_pipeline.h) and holds exactly one interceptor, so no
// composer knows anything about translation.
//
// The keys it reads all belong to somebody else, and it reads them through
// their owner rather than by name:
//
//   the continuous tier, `translateEnabled`
//   today                                 lumina/lumina_translate_gating.h
//                                 (Lumina::ContinuousTranslationAvailable)
//   translateBeforeSend, trSendLang,
//   translateBeforeSendConfirm            lumina/lumina_translate_settings.h
//
// trMode, trScopePrivate and trScopeGroup were read here through
// lumina/lumina_translate_gating.h as well, and are gone: they decided WHICH
// chats translate, which the user now decides per chat. The scope keys also
// narrowed translate-before-send, so a profile that had turned "Groups and
// channels" off and translate-before-send on now translates outgoing messages
// in groups too - see the note at the top of
// lumina/lumina_translate_settings.h.
//
// The two keys it owns are `trSendLangDialog`, the per-dialog send-language
// lock described at DialogSendLanguage() below, and `trSendChatOn`, the
// per-dialog switch that decides whether a chat translates its outgoing
// messages at all. `translateBeforeSend` above is now only a capability gate:
// it makes the feature available, and translates nothing until a chat is
// switched on. Both per-dialog keys default to "off"/absent, so a fresh
// profile with the global switch on still translates no chat until one is
// turned on from inside the conversation (AddSendMenuTranslateRow below).
//
// While Lumina::ContinuousTranslationAvailable() is false - and it is until
// `translateEnabled` is turned on, which is the default - the interceptor
// returns on its first line and the composers send exactly the way upstream
// does, the send-menu row is not added, and nothing here is ever constructed.
// Translate-before-send keeps running per message against the user's own
// quota for as long as it is on, so it is continuous tier, not free tier;
// the free tier is lumina/lumina_translate_selection.h and is not gated.
//
// WHERE THIS DELIBERATELY DIVERGES FROM ANDROID
//
// Android's ChatActivityEnterView clears the composer field the moment a
// translate-before-send starts, so every abort path there has to restore the
// captured draft, and it has to be careful not to clobber a draft the user
// typed during the round-trip. The desktop seam sits *before* the field is
// cleared (lumina/lumina_send_pipeline.h says so, and it is why it sits where
// it sits), so holding a send here leaves the user's text exactly where it is.
// There is therefore no capture, no restore and no draft-clobber window on
// desktop: cancelling is simply never calling `proceed`, and the message the
// user typed is still in the composer afterwards.
//
// The one place that is NOT true is the watchdog, which fires 20 seconds into
// a stalled provider request and then sends the ORIGINAL text rather than
// dropping the message. If the user typed more into the composer during those
// 20 seconds, the composer's own `proceed` clears the field after sending only
// the text that was captured when Send was pressed. Losing a message is worse
// than losing an addendum typed into a chat that visibly has not sent yet, and
// a provider that answers nothing at all for 20 seconds is already the failure
// case; this is the deliberate trade.
//
// A SECOND SEND IS QUEUED, NOT DROPPED
//
// Only one send per chat is held at a time. A second send arriving while the
// first is still held used to be ignored, which is what Android does - and on
// Android it costs nothing, because its composer field was cleared when the
// first send started, so a second Send tap carries no text at all. Here the
// seam sits before the field is cleared, so a second send arrives with real
// text, and two callers pass text that exists nowhere else once it is dropped:
// HistoryWidget::sendWithTextOverride (the AI editor, whose `done` callback is
// what closes its box) and sendRichDraftWithoutFormatting (the flattened rich
// page, which drops its draft immediately afterwards). Those messages were
// gone for good and their callers were never told.
//
// So a second send waits behind the held one and runs when it finishes, in the
// order it was sent. Three rules keep that from being worse than the drop it
// replaces:
//
//  * a send whose text is already held or already queued for this chat is
//    dropped rather than queued. That is a second tap on Send with the same
//    text still sitting in the field - it is still sitting there precisely
//    because the send is held - and queueing it would put the same message on
//    the wire twice. It is also why a waiting send is never "just sent
//    untranslated" instead: the held request would complete afterwards and
//    send the very same message again.
//  * the queue is bounded per chat, and past the bound a send is passed
//    straight through untranslated. It reaches the chat ahead of the ones
//    still waiting, which is the price of never losing it, and getting there
//    takes five DIFFERENT messages sent inside a single hold.
//  * the queue carries a watchdog of its own. The 20s one above bounds a
//    request that is in flight; nothing bounds a confirm box the user walked
//    away from, and everything queued behind it would wait exactly as long. So
//    a backlog that is still waiting a minute later is flushed as typed - the
//    held send first, then everything behind it, in order. A box still open at
//    that point has nothing left to answer, and pressing its buttons does
//    nothing; that is the same trade the 20s watchdog already makes.
//
// A held or queued send whose History has been destroyed is dropped silently
// rather than finished: `proceed` is guarded by the composer widget that made
// it, but the message it owns still names that History.

// Registers the send interceptor. Idempotent, and already called from a
// file-scope initializer in lumina_translate_send.cpp, so nothing has to call
// it - it is public only so that an explicit init point can be added later
// without changing anything else.
void SetupTranslateSendPipeline();

// !! THE CAPTION SEAM'S ENTRY POINT, AND THE REASON IT IS NOT
// Lumina::InterceptSend().
//
// Same contract as InterceptSend(): returns false if this pipeline took the
// send over, in which case `proceed` is invoked later, on the main thread,
// exactly once, or never (which is how a cancel is spelled). Returns true to
// send now, unchanged, with `proceed` untouched.
//
// The difference is what it does NOT do: it runs this pipeline alone instead of
// the whole interceptor chain. A caption must not travel the chain, because
// undo-send sits on it and lumina_undo_send.h states outright that media never
// reaches that seam - and its behaviour is built on that:
//
//  * it raises an "Undo send" toast over a photo whose Undo cannot stop it.
//    Dropping the hold drops `proceed`, and lumina_translate_caption.cpp then
//    sends the files with the caption as typed, because losing a photo to a
//    dismissed hold is not on the table. So Undo silently becomes "send it
//    untranslated" instead of "do not send it";
//  * its one-hold-at-a-time rule compares text, so a caption whose text matches
//    a typed message still inside its own undo window makes that typed message
//    be dropped rather than sent. That one is a lost message.
//
// Neither is reachable by a caption that never enters the chain. Routing a
// caption through InterceptSend() reaches both whenever the send is translated
// at all, because this pipeline's `proceed` IS the rest of the chain.
//
// If an interceptor is ever added that genuinely should see captions, it has to
// be called from here rather than assumed.
[[nodiscard]] bool InterceptCaptionSend(
	not_null<History*> history,
	TextWithTags &caption,
	Api::SendOptions options,
	Fn<void()> proceed);

// Finishes every send this pipeline is holding, right now and synchronously,
// each one as the user typed it - the same answer the 20s and 60s watchdogs
// give. Safe when nothing is held, and safe to call repeatedly.
//
// !! It has to be called from Core::Application::readyToQuit(), and BEFORE
// Lumina::FlushUndoSend() on the line below it, for two separate reasons:
//
//  * without it, a quit inside a hold drops the send. For a typed message that
//    costs nothing - the seam sits before the composer clears its field, so the
//    text is still there and goes to the local draft like any unsent text. For
//    a caption send it is unrecoverable: SendFilesBox has already closed and
//    the files exist nowhere but in the bundle the hold is carrying, so the
//    photo is gone with no draft and no trace. A confirm box the user has not
//    answered holds a send with no watchdog over it at all, so that window is
//    not small.
//  * before FlushUndoSend(), because finishing a send here hands it to the
//    rest of the interceptor chain, and undo-send is registered last. Flushing
//    undo-send first would leave whatever this releases sitting in a five
//    second hold that the quit is not going to wait for.
//
// FlushTranslateSendsAndCaptions() is the one to call: it flushes this
// pipeline and then the caption seam's own ordering queue
// (lumina/lumina_translate_caption.h), in that order, so media the user sent
// during a hold leaves behind the send it was waiting for rather than ahead of
// it.
void FlushTranslateSends();
void FlushTranslateSendsAndCaptions();

// Whether the next send in this chat would be translated: the continuous-tier
// opt-in, the global capability switch and this chat's own persistent switch,
// all three. The global switch no longer decides this on its own - a chat is
// translated only once it has been switched on from inside the conversation.
// This is the predicate W2-D's live preview panel should show itself on.
[[nodiscard]] bool TranslateBeforeSendActive(not_null<History*> history);

// The per-dialog send-language lock, key `trSendLangDialog`, an object of
// { "<sessionUniqueId>_<peerId>": "<code>" }.
//
// It exists because trSendLang defaults to "auto", and auto-detection is not
// reliable enough to trust silently on every send - Malay versus Indonesian is
// the case that made Android add this. So the pipeline asks ONCE per chat,
// with a confirm, and remembers the answer here; a locked chat never asks
// again.
//
// !! The key lives in Store::Private and SetDialogSendLanguage() is its only
// writer. Lumina::Settings::set() defaults to Store::Prefs, and a bare set()
// on this key would relocate the whole map into the plaintext pref file. It is
// in the private store because it is a list of who you talk to and in what
// language, which is the same class of thing as tbsOriginals.
//
// The session id is part of the key because two logged-in accounts must not
// share a lock silently; the peer id alone is nearly unique, but "nearly" is
// the kind of thing that turns into a cross-account bug report.
//
// An empty code clears the lock. Setting a lock also invalidates the send
// preview cache below - Android shipped that invalidation late, and until it
// did, changing a chat's send language kept sending the previous language's
// cached translation.
[[nodiscard]] QString DialogSendLanguage(not_null<History*> history);
void SetDialogSendLanguage(not_null<History*> history, const QString &code);

// The per-chat send-translate switch, the peer of the language lock above:
// default false, so a chat never switched on sends as typed no matter what the
// global switch is. Public so the chat language menu can drive the same
// per-chat switch the send-button menu does.
[[nodiscard]] bool DialogSendTranslateOn(not_null<History*> history);
void SetDialogSendTranslateOn(not_null<History*> history, bool on);

// The language an outgoing message in this chat would be translated into, or
// an empty string when there is nothing to translate into yet: the feature is
// off, the chat is out of scope, or trSendLang is "auto" and this chat has no
// lock (in which case the language is only decided by the confirm the pipeline
// shows at send time - it is deliberately not guessed here). Android's
// luminaResolveSendLang.
[[nodiscard]] QString ResolveSendLanguage(not_null<History*> history);

// Opens the language picker for this chat's lock and returns immediately.
// Choosing writes the lock and clears the preview cache; it never sends
// anything. Android's luminaChooseDialogSendLang, which hangs off its Send
// long-press menu; on desktop the row that opens it is not placed by this
// item.
void ShowDialogSendLanguagePicker(not_null<History*> history);

// W2-D's seam. The live preview panel translates the composer text as it is
// typed; handing the result here lets a Send that follows reuse it instead of
// paying for a second request, exactly as Android's panel does ("the panel IS
// the confirmation" - a reused translation also skips the confirm box).
//
// The cache is keyed by chat + source text + target language, all three, so a
// language change can never serve a translation made for the previous one.
// One entry is kept, which is all a single focused composer needs, and it is
// dropped by the send that uses it: the key does not cover the provider or its
// API key, so a kept entry would serve a stale engine's translation to a later
// identical message, and it would leave the last composed message sitting in a
// process-lifetime static. Call this again after every panel update.
void NoteSendTranslationPreview(
	not_null<History*> history,
	const QString &source,
	const QString &target,
	const QString &translated);

// W2-B's seam, and the whole of this item's contract with it.
//
// Invoked on the main thread, exactly once per message that actually goes out
// translated, immediately BEFORE the composer's `proceed` - that is, before
// ApiWrap::sendMessage() mints the local message id and the random_id. So by
// the time apiwrap's capture point runs, the pending {chat, sent text,
// original text} is already armed and can be correlated the way Android's
// LuminaTBS.setPending() / onOutgoingText() pair does.
//
// `sentText` is the exact trimmed translation that will be sent, which is what
// makes the correlation possible; `originalText` is the trimmed text the user
// typed. Nothing is reported for a message that goes out untranslated, so a
// profile that never translates never writes an entry.
//
// !! W2-B still owns the hard part, and it is the bug Android shipped first:
// the correlation above binds the original to a *local* identity. The moment
// the server assigns the real message id the local binding stops being
// reachable, and after a reload the bubble would show only the translation. It
// needs a second, stable key of (session, peer, server msg id) written at the
// local -> server id change. This file cannot do that: it is gone by then.
using SendOriginalHook = Fn<void(
	not_null<History*> history,
	const QString &sentText,
	const QString &originalText)>;
void SetSendOriginalHook(SendOriginalHook hook);

// The send-menu rows this item adds to menu/menu_send.cpp's FillSendMenu(),
// and the place a chat's send translation is turned on and off from inside the
// conversation. Call it with the `maybeShow` FillSendMenu() already has:
//
//     Lumina::AddSendMenuTranslateRow(menu, maybeShow, details);
//
// The first row is a checkbox for this chat's own persistent switch: checking
// it makes this chat's outgoing text translate, unchecking it sends as typed
// again. The second, behind it and only while the send language is per chat, is
// the only way to change that language once the one-time confirm has locked it.
//
// The `show` is not optional: the per-chat switch and the language lock are both
// keyed on (session, peer) - see DialogKey() - and SendMenu::Details carries a
// bare peer id with no session. Guessing the session from the peer id is exactly
// the cross-account mistake that key exists to prevent, so with a null `show`
// there is nothing safe to add and the call adds nothing.
//
// Adds nothing at all unless the feature is available, the global capability
// switch is on, and the menu belongs to a plain text composer.
//
// Why the language row matters: trSendLang defaults to "auto", so a chat's send
// language is decided by the one-time confirm the pipeline shows on its first
// translated send, and SetDialogSendLanguage() then locks it. With no row that
// reaches ShowDialogSendLanguagePicker(), a language accepted by mistake would
// be permanent for that chat.
void AddSendMenuTranslateRow(
	not_null<Ui::PopupMenu*> menu,
	const std::shared_ptr<ChatHelpers::Show> &show,
	const SendMenu::Details &details);

} // namespace Lumina
