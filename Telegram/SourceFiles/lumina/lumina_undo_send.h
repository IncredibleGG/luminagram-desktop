/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include <rpl/producer.h>

namespace Lumina {

// Undo-send window: hold a plain typed message for a few seconds behind an
// Undo button before it goes on the wire.
//
// Android: LuminaConfig "undoSendWindow", consumed by
// ChatActivityEnterView.luminaUndoSendWindow(). The key name is shared so a
// backup restored from the phone lands on the same preference.
//
// Off by default. With it off nothing below is ever constructed, no
// interceptor state is allocated and the composers send exactly the way
// upstream does.
//
// WHERE THIS DELIBERATELY DIVERGES FROM ANDROID, AND WHY IT MATTERS
//
// Android clears the composer field the moment a send is held, so its window
// has to persist the held text to survive process death and restore it into an
// empty composer afterwards - the bug that cost it messages. The desktop seam
// sits *before* the field is cleared (lumina/lumina_send_pipeline.h, and that
// is the whole reason it sits where it sits), so a held send here leaves the
// user's text exactly where it was: in the composer, in tdesktop's own local
// draft pipeline, on its way to disk like any other unsent typed text.
//
// That single difference answers every durability question:
//
//  * Undo is simply never calling `proceed`. There is nothing to restore, so
//    there is no "restore only into an empty field" guard to get wrong and no
//    window in which a draft the user typed meanwhile could be clobbered.
//    The one place this file touches a draft at all is the opposite problem:
//    leaving a chat while a send is held makes the composer save that text as
//    the chat's local draft on its way out, and the draft has to be removed
//    again once the message really goes. See ClearSentLocalDraft() in
//    lumina_undo_send.cpp - it removes a draft only when it is exactly the
//    text that was just sent, so a draft the user typed instead is untouched.
//  * A graceful quit flushes: FlushUndoSend() below dispatches the held send
//    from Application::readyToQuit(), before that same function asks whether
//    ApiWrap wants to prevent the quit - so the message is queued first and
//    the quit then waits for it. The message is sent, not lost.
//  * A hard kill inside the window loses nothing that upstream would have
//    kept: the text never left the composer, so it lands in the local draft
//    exactly as text typed and not sent always does. There is no LuminaGram
//    copy of the message text on disk anywhere, which is also why there is
//    none to leak.
//
// The cost of that trade is that the composer still shows the text while the
// send is held, which is what the toast explains, and what the collision rule
// in lumina_undo_send.cpp is written around.
//
// WHERE IT IS AVAILABLE
//
// Only in ordinary chats, i.e. only when HistoryWidget is the composer.
// `proceed` is crl::guard()ed on the widget that produced it, and the section
// composers - forum topics, discussion threads, saved sublists, the scheduled
// view - are destroyed when the user navigates away, which would turn a hold
// into a silently dropped message. There is no signal to flush on either:
// MainWidget::showNewSection() destroys the old section before it announces
// the new active chat. So a send from a section composer is never held and
// goes out unchanged. Android holds everywhere; this is the one place the
// port is deliberately narrower, and the reason is that losing a message is
// worse than not offering to undo one.

// The preference. Default false - the whole feature is opt-in.
[[nodiscard]] bool UndoSendWindow();
void SetUndoSendWindow(bool value);

// Fires whenever the value above may have changed, including a whole-file
// restore through Settings::importAll().
[[nodiscard]] rpl::producer<> UndoSendWindowChanges();

// How long a send is held, in whole seconds. Only the settings text uses it,
// so that the wording and the timer cannot drift apart.
[[nodiscard]] int UndoSendWindowSeconds();

// Registers the send interceptor. Idempotent.
//
// !! This must be called from Core::Application::run(), NOT from a file-scope
// initializer the way lumina_translate_send.cpp registers its own. The
// registry runs interceptors in registration order, and undo-send has to be
// the LAST hold before the wire: FlushUndoSend() below has to be able to
// finish a held send synchronously at quit time, and it can only do that if
// invoking `proceed` reaches the composer rather than handing the message to
// another interceptor that would hold it for a network round-trip the quit
// will not wait for. Registering from run() runs after every static-init
// registration, which is what guarantees that.
void SetupUndoSendPipeline();

// Dispatches the held send, if there is one, right now and synchronously.
// Safe to call when nothing is held, and safe to call repeatedly.
//
// The one caller is Core::Application::readyToQuit(); see the note above for
// why it belongs at the top of that function specifically.
void FlushUndoSend();

} // namespace Lumina
