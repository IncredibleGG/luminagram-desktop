/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "api/api_common.h" // Api::SendOptions.

#include <rpl/producer.h>

#include <QtCore/QString>

#include <memory>

class History;

namespace Ui {
struct PreparedBundle;
} // namespace Ui

namespace Lumina {

// The caption half of the send seam. A caption on a photo or a file is a
// message to the person reading it, so it has to be translated by the same
// rules as a text message - and by the same code, not by a second copy of it.
//
// tdesktop keeps the two completely apart. SendFilesBox hands the composer a
// std::shared_ptr<Ui::PreparedBundle>, the caption lives inside it as the
// Ui::PreparedFile::caption of one file, and it reaches the wire through
// ApiWrap::sendFiles() without ever passing anything the text seam
// (lumina/lumina_send_pipeline.h) can see. This is the one function that
// closes that gap, and each composer's sendingFilesConfirmed() holds exactly
// one call into it and nothing else.
//
// Where the call sits mirrors the text seam: after the sending-files error,
// the ephemeral-reply and the Stars checks and after the payment-approved
// re-entry, so a held send is never one the composer is about to reject and is
// never held twice; and immediately before the ApiWrap::sendFiles() loop,
// which is the first step that cannot be taken back.
//
// Nothing is decided here. Whether this caption is translated at all, into
// what language, with or without a confirm, and what happens while a provider
// is slow are all the text pipeline's answers, reached through the very code a
// typed message reaches, so the two cannot drift apart or answer differently.
//
// Reached, though, through Lumina::InterceptCaptionSend() rather than
// Lumina::InterceptSend(): that is the translate pipeline alone and not the
// whole interceptor chain. See the note on it in lumina/lumina_translate_send.h
// - a caption travelling the chain reaches undo-send, which is written on the
// assumption that media never does, and one of the consequences is a typed
// message being dropped instead of sent.
//
// Two things this file does decide, because they only exist on this path:
// what a cancel says (SendCancelLabel() below), and the order media leaves a
// chat in. Media sent while an earlier caption is still being translated waits
// for it, per chat, so photo B never lands ahead of photo A.

// Returns false if the send was taken over: `proceed` is then invoked later,
// on the main thread, exactly once. Returns true to send now, unchanged -
// `proceed` has NOT been called and the caller invokes it itself, the same
// shape lumina_send_pipeline.h uses.
//
// Unlike the text seam there is no cancel. Once this returns false `proceed`
// is always invoked eventually, because by the time a composer holds the
// bundle SendFilesBox is already closed and these files exist nowhere else:
// dropping the send would lose them for good, and the user cannot get them
// back. Every abort inside the translation pipeline - a cancelled confirm, a
// closed language picker, a stalled provider, a second send held in the same
// chat - therefore finishes this send with the caption exactly as it was
// typed.
//
// `proceed` must own the bundle it sends, that is capture the same
// shared_ptr, because the caption being rewritten lives inside it.
//
// A send returning false may also be one that is simply waiting its turn: a
// caption send is held for this chat and this one leaves right after it. The
// caller cannot tell the two apart and does not need to - `proceed` is invoked
// exactly once either way.
//
// Sends that are passed straight through, with no request, no delay and no
// difference from upstream: the master opt-in is off; there is no caption; the
// caption is empty; or the bundle carries more than one non-empty caption. In
// particular a chat with nothing held costs one lookup in an empty map and
// then takes exactly the path it took before this ordering existed.
[[nodiscard]] bool InterceptSendFiles(
	not_null<History*> history,
	std::shared_ptr<Ui::PreparedBundle> bundle,
	Api::SendOptions options,
	Fn<void()> proceed);

// Releases every media send still waiting its turn in this chat's ordering
// queue, right now and synchronously, exactly as the composer handed it over.
//
// Call Lumina::FlushTranslateSendsAndCaptions() rather than this: the send
// that owns each chat's slot lives in the text pipeline, so that one has to be
// finished first or the waiting sends leave ahead of it. See the note on
// Lumina::FlushTranslateSends() in lumina/lumina_translate_send.h for why the
// quit path needs this at all - a held caption send is the one thing on this
// seam that cannot be typed again.
void FlushTranslateCaptionSends();

// The label for the cancel button on the translate-before-send confirm, which
// is the one place in that pipeline where cancelling means two different
// things.
//
// On the text path Cancel abandons the send and the composer still holds what
// the user typed, so "Cancel" is exactly right. On the caption path there is
// nothing to abandon: SendFilesBox has closed, the files exist only inside the
// bundle this send is carrying, and losing a photo because a box was dismissed
// is not acceptable - so the files go out with the caption as it was typed.
// That behaviour is deliberate; the word "Cancel" on top of it is not, because
// it promises that nothing will happen.
//
// `history` may be null, and the chat with nothing of ours held in it - which
// is every text send - gets tr::lng_cancel() back unchanged.
//
// `original` is the text the box is confirming, that is the held send's own
// text exactly as the pipeline trimmed it. Pass it: a chat can have a text send
// held and a caption send queued behind it at the same time, and then the chat
// alone would label that text send's Cancel as if it sent a caption. Leaving it
// empty falls back to matching on the chat only, which is right for a caller
// that genuinely does not know which send its box belongs to.
//
// Called from lumina_translate_send.cpp when it builds the confirm box, which
// is the only file that knows a cancel button is being shown at all.
[[nodiscard]] rpl::producer<QString> SendCancelLabel(
	History *history,
	const QString &original = QString());

// Whether the send the text pipeline is holding for this chat is a media send
// carrying a caption, matched the same way SendCancelLabel() above matches it.
//
// It is the difference between a hold that costs the user nothing and one that
// can cost them a photo, and lumina_translate_send.cpp needs it for more than
// the wording of a button: a confirm box or a language picker left unanswered
// holds a send with no deadline over it, which is correct for typed text -
// still sitting in the composer - and not survivable for a caption, whose files
// exist nowhere but inside the bundle the hold is carrying.
//
// `history` may be null and `original` may be empty, with the same meanings as
// above.
[[nodiscard]] bool HoldsCaptionSend(
	History *history,
	const QString &original = QString());

} // namespace Lumina
