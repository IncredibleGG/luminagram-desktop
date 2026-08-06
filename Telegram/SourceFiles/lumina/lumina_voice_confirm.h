/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include <rpl/producer.h>

#include <memory>

namespace ChatHelpers {
class Show;
} // namespace ChatHelpers

namespace Lumina {

// Ask before a recorded voice or round video message goes out - the misfired
// recording case. Off by default, in which case nothing below does anything
// and the recorder behaves exactly as stock Telegram Desktop's does.
//
// Android: LuminaConfig "confirmSendVoiceVideo", consumed at the four record
// send-sites in ChatActivityEnterView; the key name is shared so a backup
// restored from the phone lands on the same preference.
[[nodiscard]] bool ConfirmVoiceSend();
void SetConfirmVoiceSend(bool value);

// Fires whenever the value above may have changed, including a whole-file
// restore through Settings::importAll(). The key lives in exactly one place,
// so nothing else has to spell it.
[[nodiscard]] rpl::producer<> ConfirmVoiceSendChanges();

// Everything a pending confirmation needs, so that VoiceRecordBar does not
// have to grow a member for it - the record bar's header is not ours to edit,
// and the state has to survive the box being open, so it lives here keyed by
// the bar.
struct VoiceSendConfirmRequest {
	// The record bar. Only used as a key and to close a stale box; never
	// dereferenced as anything but a QObject.
	not_null<QObject*> owner;

	std::shared_ptr<ChatHelpers::Show> show;

	// Round video rather than voice - wording only.
	bool round = false;

	// True when backing out destroys the recording, which is the case
	// everywhere upstream of stopRecording(): the box then says so. False in
	// the listen state, where backing out just leaves the preview alone.
	bool discardOnCancel = false;

	// Exactly one of these runs, exactly once, and never before this call has
	// returned. `send` re-runs the operation that was intercepted: while it
	// runs, RequestVoiceSendConfirm() returns false for this owner, so the
	// second pass falls through to the real send instead of asking again.
	//
	// Both may be null. A null `cancel` means "backing out changes nothing",
	// which is what the listen state wants.
	//
	// Guard them: the box outlives the record bar, so a chat closed while the
	// box is up would otherwise call into a destroyed widget.
	Fn<void()> send;
	Fn<void()> cancel;
};

// Returns true when the send was taken over - the caller must return
// immediately and do nothing else. Returns false to send now, unchanged,
// which is what happens when the preference is off, when a confirmation for
// this owner has just been accepted, and when no box could be put up at all
// (no `show`, a dead one, or one that dropped the box). That last case fails
// open on purpose: a question that cannot be asked must not eat a recording.
//
// Asking again while a box for the same owner is already up also returns true
// without showing a second box; the answer to the first one decides.
[[nodiscard]] bool RequestVoiceSendConfirm(VoiceSendConfirmRequest &&request);

// True between RequestVoiceSendConfirm() showing a box for `owner` and that
// box being answered. The record bar's two key filters consult this so that
// Enter reaches the box instead of being swallowed by a send that is already
// waiting for an answer.
[[nodiscard]] bool VoiceSendConfirmPending(not_null<QObject*> owner);

// Forgets any confirmation pending for `owner` and closes its box. Neither
// callback runs. Call it from anywhere the recording ends by another route -
// the recorder being torn down, the bar being hidden - so a stale box can
// never answer for a recording that is already gone.
void CancelVoiceSendConfirm(not_null<QObject*> owner);

} // namespace Lumina
