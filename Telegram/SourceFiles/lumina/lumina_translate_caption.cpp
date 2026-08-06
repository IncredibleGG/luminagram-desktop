/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_translate_caption.h"

#include "base/weak_ptr.h"
#include "data/data_premium_limits.h"
#include "history/history.h"
#include "lumina/lumina_send_pipeline.h"
#include "lumina/lumina_translate_gating.h"
#include "main/main_session.h"
#include "ui/chat/attach/attach_prepare.h"

#include <crl/crl_on_main.h>

namespace Lumina {
namespace {

// One held media send.
//
// `caption` points into `bundle`, which this object keeps alive for as long
// as the text pipeline holds the send. That is exactly what
// lumina_send_pipeline.h asks of whoever hands it a TextWithTags reference,
// and it is why the bundle is held here rather than only in the caller's
// continuation: the pipeline rewrites the caption after it has dropped its own
// Request and just before it invokes `proceed`.
struct CaptionSend {
	std::shared_ptr<Ui::PreparedBundle> bundle;
	base::weak_ptr<History> history;
	TextWithTags *caption = nullptr;
	TextWithTags original;
	Fn<void()> proceed;
	bool finished = false;

	~CaptionSend();
};

// Exactly one caption, or nothing at all.
//
// An album can carry a caption per file. Translating one of them and sending
// the rest as typed reads worse than translating none, and translating all of
// them is not on offer: the pipeline holds one send per chat at a time, so N
// captions would mean N confirms and N round-trips for a single album. Every
// ordinary case - a photo with a caption, an album with the composer's text
// attached to one of its files - has exactly one.
[[nodiscard]] TextWithTags *SingleCaption(
		not_null<Ui::PreparedBundle*> bundle) {
	auto result = (TextWithTags*)nullptr;
	for (auto &group : bundle->groups) {
		for (auto &file : group.list.files) {
			if (file.caption.text.trimmed().isEmpty()) {
				continue;
			} else if (result) {
				return nullptr;
			}
			result = &file.caption;
		}
	}
	return result;
}

// A translation that outgrew the caption limit is put back. The caption the
// user typed already passed SendFilesBox's own length check, and an over-long
// one is not recoverable the way an over-long text message is: ApiWrap splits
// text into several messages, but a caption travels with the upload and the
// server simply rejects it. The pipeline's own check is against the message
// limit, which is the larger of the two, so this is not a check it can make on
// our behalf.
void RestoreOverlongCaption(not_null<CaptionSend*> state) {
	const auto caption = state->caption;
	const auto history = state->history.get();
	if (!caption) {
		return;
	} else if (!history) {
		*caption = state->original;
		return;
	}
	const auto limits = Data::PremiumLimits(&history->session());
	if (int(caption->text.size()) > limits.captionLengthCurrent()) {
		*caption = state->original;
	}
}

void FinishCaptionSend(not_null<CaptionSend*> state) {
	if (state->finished) {
		return;
	}
	state->finished = true;
	RestoreOverlongCaption(state);
	if (const auto proceed = base::take(state->proceed)) {
		proceed();
	}
}

CaptionSend::~CaptionSend() {
	if (finished) {
		return;
	}
	finished = true;
	if (caption) {
		*caption = original;
	}
	auto callback = base::take(proceed);
	if (!callback) {
		return;
	}

	// The pipeline dropped the send without answering it: a cancelled confirm,
	// a closed language picker, a chat that already had a send in flight. For a
	// text message that is how the user cancels and nothing is lost, because
	// the composer still holds what they typed. Here the box is closed and the
	// files exist nowhere else, so the send is finished with the caption as
	// typed instead. It goes to the next main-thread turn because this can run
	// from inside the InterceptSend() call itself, and it carries a copy of the
	// bundle so that a caller which did not capture one cannot turn a cancelled
	// translation into a send from a destroyed bundle.
	crl::on_main([callback = std::move(callback), kept = bundle] {
		callback();
	});
}

} // namespace

bool InterceptSendFiles(
		not_null<History*> history,
		std::shared_ptr<Ui::PreparedBundle> bundle,
		Api::SendOptions options,
		Fn<void()> proceed) {
	if (!TranslationFeatureEnabled() || !bundle || !proceed) {
		return true;
	}
	const auto caption = SingleCaption(bundle.get());
	if (!caption) {
		return true;
	}

	// Everything that decides whether this caption is translated at all - the
	// stored preference and the send-menu quick toggle, the private / group
	// scope, the per-dialog language lock with its one-time confirm, the rule
	// that sends a message carrying formatting as typed because its offsets
	// index the original characters, the in-flight lock and the watchdog behind
	// it - belongs to the text pipeline and is deliberately not repeated here.
	const auto state = std::make_shared<CaptionSend>();
	state->bundle = std::move(bundle);
	state->history = base::make_weak(history);
	state->caption = caption;
	state->original = *caption;
	state->proceed = std::move(proceed);
	const auto carryOn = InterceptSend(history, *caption, options, [state] {
		FinishCaptionSend(state.get());
	});

	// `finished` here would mean an interceptor both answered and returned
	// true, which lumina_send_pipeline.h forbids - but if one ever does, this
	// send has already gone out and the caller must not send it a second time.
	// Marking the state finished on the way out is what stops the destructor
	// above from re-sending a bundle the caller is about to send itself.
	if (!carryOn || state->finished) {
		return false;
	}
	state->finished = true;
	return true;
}

} // namespace Lumina
