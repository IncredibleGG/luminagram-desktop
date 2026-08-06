/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "api/api_common.h" // Api::SendOptions.

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
// is slow are all the text pipeline's answers, reached through the same
// interceptor chain a typed message goes through, so the two cannot drift
// apart or answer differently.

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
// Sends that are passed straight through, with no request, no delay and no
// difference from upstream: the master opt-in is off; there is no caption; the
// caption is empty; or the bundle carries more than one non-empty caption.
[[nodiscard]] bool InterceptSendFiles(
	not_null<History*> history,
	std::shared_ptr<Ui::PreparedBundle> bundle,
	Api::SendOptions options,
	Fn<void()> proceed);

} // namespace Lumina
