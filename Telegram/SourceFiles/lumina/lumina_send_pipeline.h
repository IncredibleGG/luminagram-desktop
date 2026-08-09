/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "api/api_common.h" // Api::SendOptions.

class History;

namespace Lumina {

// The single seam LuminaGram has into the text composers. tdesktop has no
// shared text-send entry point: HistoryWidget::sendTextWithTags(),
// HistoryView::ChatWidget::sendTextWithTags() and
// HistoryView::ScheduledWidget::send() each build their own
// Api::MessageToSend, so each of those three holds exactly one call into this
// header and nothing else.
//
// Where the call sits is load-bearing, and moving it breaks the features built
// on top of it:
//
//  * after the slowmode / Stars / ephemeral checks and after the
//    payment-approved re-entry, so an interceptor never sees a message the
//    composer is about to reject anyway, and never sees the same message twice
//    while the Stars confirmation is up;
//  * before the composer clears its field, saves the draft or mints a local
//    message id, so a send can still be held or dropped with nothing to undo
//    and nothing to restore;
//  * and therefore *not* in ApiWrap::sendMessage(), which is past all of it.
//
// Composer state at the call, in all three: the message - send action, reply
// to, web page draft, options - is fully built, and reply returns have been
// cleared by the two non-scheduled composers. Nothing else has happened. The
// composer field still holds the text, the local and cloud drafts are
// untouched, and no message exists yet.

// Returns false if an interceptor took ownership; it will invoke `proceed`
// later (or not at all). Returns true to send now, unchanged.
//
// `text` is the text the composer is about to send. An interceptor may rewrite
// it in place and the rewritten value is what goes on the wire. The referenced
// object is owned by `proceed`, so it also outlives the call for an
// interceptor that took ownership: holding a `TextWithTags*` next to the
// `proceed` callback and rewriting it just before invoking that callback is
// the supported way to change the text asynchronously. It dangles the moment
// the last copy of `proceed` is dropped.
//
// `proceed` finishes exactly this send - same reply, same web page draft, same
// options, same starsApproved, same text object - without re-reading the
// composer and without re-running any of the checks above. Invoke it once,
// from the main thread. Never invoking it drops the message silently, which is
// the intended way to cancel.
//
// What a late `proceed` does depends on the composer, because they have
// different lifetimes. The two section widgets are destroyed when the user
// navigates away, so `proceed` is then a no-op and the message is lost - hold
// a send there only for as long as the section is on screen. HistoryWidget is
// reused for the next chat instead of being destroyed, so `proceed` there
// still sends the message to its own destination, but skips clearing the
// field, saving the draft and resetting the keyboard once the composer has
// moved to another chat, since by then those belong to someone else.
[[nodiscard]] bool InterceptSend(
	not_null<History*> history,
	TextWithTags &text,
	Api::SendOptions options,
	Fn<void()> proceed);

// An interceptor returns true to pass the message on to the next interceptor
// (and finally to the composer), false to take ownership of it exactly as
// InterceptSend() describes above. The `proceed` an interceptor is given
// resumes the rest of the chain, so an interceptor that holds a message never
// silently disables the ones registered after it.
//
// The two are exclusive: an interceptor that returns true must NOT keep or
// invoke its `proceed`, because the chain is already continuing without it and
// invoking it would send the message a second time. Return false first, then
// invoke `proceed` exactly once, or never.
//
// Note that `options.scheduled != 0` for a scheduled send: an interceptor that
// only makes sense for an immediate send has to check that itself.
//
// `history` is not kept alive by anything here. An interceptor that holds a
// message across an async step and needs the history afterwards has to guard
// it itself rather than assume the raw pointer is still good - History derives
// from Dialogs::Entry, which is a base::has_weak_ptr, so
// `base::make_weak(history)` is enough.
using SendInterceptor = Fn<bool(
	not_null<History*> history,
	TextWithTags &text,
	Api::SendOptions options,
	Fn<void()> proceed)>;

// Interceptors run in registration order, AFTER the OTP guard, which is a
// fixed first stage inside InterceptSend() and is deliberately not in this
// registry - lumina/lumina_otp_guard.h and the comment in InterceptSend()
// explain why nothing may get in front of it. Whatever is registered here
// therefore sees a message the user has already been asked about, if asking
// was warranted.
//
// The registry is append-only and lives for the application lifetime, so
// register from something that lives at least as long, and resolve per-session
// state from `history` inside the interceptor rather than capturing it here.
//
// Not synchronised: register and run on the main thread only, which is where
// every composer calls InterceptSend() from anyway.
void RegisterSendInterceptor(SendInterceptor interceptor);

} // namespace Lumina
