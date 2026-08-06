/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "data/data_msg_id.h"

class HistoryItem;

namespace Main {
class Session;
} // namespace Main

namespace Lumina {

// Where a translate-before-send message keeps the text the user actually
// typed.
//
// When the send pipeline translates an outgoing message it puts the
// TRANSLATION on the wire and the original never leaves this machine: no field
// of the sent message carries it, and re-opening the chat re-reads the message
// from tdesktop's own cache, which only ever knew the translation. The bubble
// is supposed to show the original large and the translation small, so this
// store is the only copy of the large half, and losing it degrades the bubble
// to translation-only with no way back.
//
// Three participants:
//
//   the send pipeline (W2-A) reports every translated send through the
//                        Lumina::SendOriginalHook this file installs
//   NoteOutgoingText()   apiwrap.cpp, the single capture point: the outgoing
//                        message's local id is minted there and nowhere else
//   SentOriginalText()   the renderer (W2-C)
//
//
// KEY
//
// (session unique id, peer id, message id), and it is re-keyed from the local
// id to the server id the moment the server assigns one.
//
// Every part of that is load-bearing, and Android got two of them wrong:
//
//  * Android keyed by random_id alone. MessagesStorage drops the randoms_v2
//    row once the message has a real id, so after a reload the lookup missed
//    and the bubble showed only the translation. Its fix was a second key of
//    dialogId + server message id, written when the id is assigned. Desktop
//    does not need the two-key dance at all: apiwrap.cpp mints the local
//    FullMsgId and the random id in the same statement, so the id we can look
//    a message up by later is available at capture time, and
//    Data::Session::itemIdChanged() then tells us when it becomes the server
//    id. There is exactly one key here, and it is always the message's current
//    id. The re-key also calls Lumina::RefreshDualLanguage(): the bubble was
//    laid out under the local id, found no original, and does not look again
//    on its own - which is the same lost original Android shipped, arrived at
//    from the rendering side instead of the storage side.
//
//  * itemIdChanged() is an owner-level signal: Data::Session::IdChange carries
//    a FullMsgId and the previous MsgId, and nothing that says which account
//    it belongs to. Local message ids come from a per-account counter that
//    starts at StartClientMsgId in every account, so two logged-in accounts
//    hand out the SAME local ids, and a chat both accounts are in carries the
//    same peer id too. A key without the session therefore does not just risk
//    a collision, it guarantees one on the second account's first send.
//    Hence Main::Session::uniqueId() in the key and captured by value in the
//    per-session subscription.
//
// Only messages sent while a translation was in flight are ever recorded, so
// the store stays empty for a user who does not use the feature.
//
//
// WHAT IS PERSISTED, AND WHAT DELIBERATELY IS NOT
//
// Only entries whose id is no longer a client-side id. Data::Session hands out
// local ids from a counter that is reset to StartClientMsgId on every launch,
// so a local id persisted in one run names a completely different message in
// the next one - the store would answer a brand new message with a stranger's
// text, which is worse than answering nothing. An entry therefore lives in
// memory only until the server acknowledges the message; the re-key is what
// promotes it to disk. Scheduled and business-shortcut ids are derived from a
// server id (Data::ScheduledMessages::localMessageId()) and are stable across
// launches, so they persist too, which is why the test is "not a client id"
// rather than IsServerMsgId().
//
// A message that never reaches the server before the app closes loses its
// original. So does the message itself, so there is nothing to show it next
// to.
//
//
// RETENTION
//
// The store is bounded on two axes and prunes on load, on every record and on
// every re-key: at most 500 entries, and nothing older than 90 days. Oldest
// first, by the order in which the originals were captured rather than by key.
// A prune that drops something rewrites the file.
//
// Pruning is driven by use, not by the clock: nothing here loads the store
// until a message is sent through the pipeline or an outgoing bubble is laid
// out with the feature on. Turning the master toggle off therefore FREEZES
// what is on disk rather than aging it out - the plaintext of everything sent
// through translate-before-send stays in luminagram_private.json until the
// feature is switched back on and used again. Anyone who wants "off means
// gone" has to delete the key explicitly; doing it from the toggle would also
// make the switch destructive and irreversible, which is why it is not done
// here.
//
// It lives in Lumina::Store::Private under the key `tbsOriginals` (the name
// Android uses). That file is plaintext, and this key is the plaintext of
// everything the user has ever sent through translate-before-send, which makes
// it the most sensitive value in it - see the encryption-at-rest question in
// the port plan. It is also included in Settings::exportAll(), so the W6-E
// backup carries it; that backup is encrypted, but whoever adds an "export
// without private data" option should start here.
//
// Lumina::Settings::set() defaults to Store::Prefs, and passing that default
// for a key that lives in Store::Private MOVES the value into the plaintext
// pref file. This file is the only writer of `tbsOriginals` and funnels every
// write through one call that names the store: never call Settings::set() on
// that key anywhere else.
//
//
// BEHAVIOUR NEUTRALITY
//
// With Lumina::TranslationFeatureEnabled() false nothing here can run. The
// hook is only ever invoked by the send pipeline, which is gated;
// NoteOutgoingText() reads one function-local static and returns; and
// SentOriginalText() answers an empty string without even loading the store.
// Turning the master toggle off therefore also hides originals that were
// captured while it was on, which is the intended reading of "behaves exactly
// like upstream" - and W2-C already invalidates on
// Lumina::TranslateSettingsChanges(), which covers that key.

// Installs the two seams this file lives between. Idempotent, and already
// called from a file-scope initializer in lumina_translate_originals.cpp, so
// nothing has to call it - it is public only so that an explicit init point
// can be added later without changing anything else, exactly as
// SetupTranslateSendPipeline() is.
//
//   Lumina::SetSendOriginalHook()      lumina/lumina_translate_send.h
//                                      the write side: W2-A reports every
//                                      translated send through it
//   Lumina::SetOutgoingOriginalLookup() lumina/lumina_dual_language_line.h
//                                      the read side: W2-C's renderer does
//                                      not include this header, so the store
//                                      pushes SentOriginalText() at it. Until
//                                      this runs the outgoing half of the
//                                      dual-language bubble is inert no
//                                      matter what is stored here.
//
// The hook fires once per translated send, immediately before the composer's
// `proceed`, and arms the original for the message id that
// ApiWrap::sendMessage() is about to mint. That is the whole recording path:
// there is no way to record an original for a message that was not sent
// through the pipeline, and nothing is recorded for a message that goes out
// untranslated.
//
// The arm is dropped at the end of the event-loop turn it was made in. All
// three composers reach ApiWrap::sendMessage() synchronously out of `proceed`,
// so a real send is always captured; a send that dies on the way there - a
// destroyed section widget, a slowmode or Stars rejection, a dice emoji taking
// the media path - simply records nothing instead of leaving a pending entry
// that would later attach a stranger's text to an unrelated message. Android
// needed a 60-second TTL and an exact-sent-text match for that, because its
// dispatch is asynchronous; on desktop the turn is the boundary.
//
// A translation long enough to be split into several messages binds the whole
// original to the FIRST part only. The parts of a translation do not line up
// with the parts of its original, so dividing it would attach wrong text to a
// bubble; the later parts show as ordinary messages.
void SetupSentOriginals();

// The capture point, called from ApiWrap::sendMessage() where the outgoing
// message's local id is minted. Does nothing unless a translated send for this
// session and peer is armed.
//
// `text` is the chunk actually going on the wire; it is compared against the
// original only to skip the no-op case where translation changed nothing.
void NoteOutgoingText(
	not_null<Main::Session*> session,
	FullMsgId id,
	const QString &text);

// The stored original for a sent message, or an empty string when there is
// none - which is the answer for every incoming message, every message sent
// without translation, and every message at all while the feature is off.
//
// Cheap enough for Element::validateText(): the item overload rejects incoming
// messages before touching anything, and a lookup is a binary search over a
// map that is capped at 500 entries.
[[nodiscard]] QString SentOriginalText(not_null<const HistoryItem*> item);
[[nodiscard]] QString SentOriginalText(
	not_null<Main::Session*> session,
	FullMsgId id);

} // namespace Lumina
