/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_translate_caption.h"

#include "base/flat_map.h"
#include "base/timer.h"
#include "base/weak_ptr.h"
#include "data/data_peer.h"
#include "data/data_premium_limits.h"
#include "history/history.h"
#include "lang/lang_keys.h"
#include "lumina/lumina_locale.h"
#include "lumina/lumina_translate_gating.h"
#include "lumina/lumina_translate_send.h"
#include "main/main_session.h"
#include "ui/chat/attach/attach_prepare.h"

#include <crl/crl_on_main.h>

namespace Lumina {
namespace {

// The last-resort bound on how long one held caption send may keep the next
// one waiting, and it is deliberately minutes rather than seconds.
//
// Everything that can hold a send is already bounded: the text pipeline runs a
// 20s stall watchdog over the provider, and a box that is dismissed drops its
// request, which finishes this send with the caption as typed. The one wait
// nothing bounds is a confirm box the user has opened and not answered yet -
// and that is a legitimate wait this timeout must never cut short, because
// cutting it short is exactly the out-of-order send the ordering exists to
// remove. So the timeout is here only so that a photo can never be held
// forever by a bug nobody has found yet; in normal use it never fires.
constexpr auto kOrderingTimeout = crl::time(3 * 60 * 1000);

// One held media send.
//
// `caption` points into `bundle`, which this object keeps alive for as long
// as the text pipeline holds the send. That is exactly what
// lumina_send_pipeline.h asks of whoever hands it a TextWithTags reference,
// and it is why the bundle is held here rather than only in the caller's
// continuation: the pipeline rewrites the caption after it has dropped its own
// Request and just before it invokes `proceed`.
//
// `key` and `token` name this chat's ordering slot while `owns` is true. The
// slot goes back only after `proceed` has run, so whatever was waiting behind
// this send cannot leave before it.
struct CaptionSend {
	std::shared_ptr<Ui::PreparedBundle> bundle;
	base::weak_ptr<History> history;
	TextWithTags *caption = nullptr;
	TextWithTags original;
	Fn<void()> proceed;
	QString key;
	uint64 token = 0;
	bool owns = false;
	bool held = false;
	bool finished = false;

	~CaptionSend();
};

// One media send that arrived while another was still held in the same chat,
// kept exactly as the composer handed it over.
struct Waiting {
	base::weak_ptr<History> history;
	std::shared_ptr<Ui::PreparedBundle> bundle;
	Api::SendOptions options;
	Fn<void()> proceed;
};

// One chat's ordering slot. Its presence in Queues() is what "a caption send
// is in flight in this chat" means, and `token` names the send that currently
// owns it - a terminal arriving late, after the watchdog below already handed
// the slot on, finds a token that is no longer its own and does nothing. The
// timer exists only while somebody is actually waiting.
struct ChatQueue {
	std::vector<Waiting> waiting;
	std::unique_ptr<base::Timer> watchdog;
	uint64 token = 0;
};

// Deliberately leaked, as the equivalents in lumina_translate_send.cpp and
// lumina_undo_send.cpp are: a ChatQueue owns a base::Timer, which is a
// QObject, and not every exit runs Application::readyToQuit() -
// Sandbox::isSavingSession() quits without it. A plain function-local static
// would then be destroyed at static-destruction time, killing a running timer
// after QApplication is already gone.
[[nodiscard]] base::flat_map<QString, std::unique_ptr<ChatQueue>> &Queues() {
	using Map = base::flat_map<QString, std::unique_ptr<ChatQueue>>;
	static const auto result = new Map();
	return *result;
}

// The caption currently held in each chat, which is what decides the wording
// of the cancel button. The value is the caption text as the text pipeline sees
// it - trimmed, and a COPY taken when the send was handed over, so that the
// rewrite the pipeline performs just before it invokes `proceed` cannot change
// what we are matching against.
//
// The chat key alone is not enough to answer "is this box's cancel a caption
// cancel". A caption send that arrives while a TEXT send is already held in the
// same chat is queued by the pipeline rather than started, and the confirm box
// that opens next belongs to that text send, not to us - keyed on the chat
// only, its Cancel would be labelled as if it sent a caption. So the caption
// text is stored as well and the caller matches on it.
//
// This is also NOT the same question as "is there an entry in Queues()". The
// slot outlives the hold by one main-thread turn on the path where the pipeline
// dropped the request rather than answering it. Only a send the pipeline
// actually took is in here, and it is out again before its `proceed` runs.
[[nodiscard]] base::flat_map<QString, QString> &HeldCaptions() {
	static auto result = base::flat_map<QString, QString>();
	return result;
}

// The one moment a media send is in no chat's ordering queue and in no request.
// ReleaseChatSlot() below takes the next send out of its chat's queue and posts
// the main-thread turn that starts it, and for that one turn the send lives
// nowhere but inside the posted lambda: a quit landing in the gap would find it
// in no queue and in no request, the post would never run, and the photo would
// be gone. It is registered here for exactly that turn and taken back out by
// StartHeldCaptionSend(), so FlushTranslateCaptionSends() can always see it.
[[nodiscard]] std::vector<std::shared_ptr<Waiting>> &InTransit() {
	static auto result = std::vector<std::shared_ptr<Waiting>>();
	return result;
}

void DropInTransit(const std::shared_ptr<Waiting> &entry) {
	auto &list = InTransit();
	const auto i = ranges::find(list, entry);
	if (i != list.end()) {
		list.erase(i);
	}
}

[[nodiscard]] uint64 NextToken() {
	static auto result = uint64(0);
	return ++result;
}

// Mirrors DialogKey() in lumina_translate_send.cpp, for the same reason: the
// session id is in the key so that two logged-in accounts cannot share one
// chat's ordering.
[[nodiscard]] QString ChatKey(not_null<History*> history) {
	return QString::number(history->session().uniqueId())
		+ QChar('_')
		+ QString::number(history->peer->id.value);
}

void ReleaseChatSlot(QString key, uint64 token);
void StartHeldCaptionSend(
	const QString &key,
	uint64 token,
	std::shared_ptr<Waiting> waiting);

void UpdateOrderingWatchdog(not_null<ChatQueue*> queue, const QString &key) {
	if (queue->waiting.empty()) {
		queue->watchdog = nullptr;
		return;
	} else if (!queue->watchdog) {
		queue->watchdog = std::make_unique<base::Timer>();
	}
	const auto token = queue->token;

	// Posted rather than run from inside timerEvent(): handing the slot on can
	// destroy this very ChatQueue, and with it the timer that is executing.
	queue->watchdog->setCallback([=] {
		crl::on_main([=] { ReleaseChatSlot(key, token); });
	});
	queue->watchdog->callOnce(kOrderingTimeout);
}

// Hands this chat's slot to whatever is waiting, or gives it up when nothing
// is. The slot is never left free with a queue behind it - it goes straight
// from one send to the next - so a media send arriving during the hop below
// queues behind that next one instead of overtaking it.
//
// `key` is taken by value because dropping the entry destroys everything the
// entry owns, and a caller that had handed us a reference into it would be
// left holding a dangling argument for the rest of the call.
void ReleaseChatSlot(QString key, uint64 token) {
	const auto i = Queues().find(key);
	if (i == Queues().end() || i->second->token != token) {
		return;
	}
	const auto queue = i->second.get();
	if (queue->waiting.empty()) {
		Queues().remove(key);
		return;
	}
	const auto next = std::make_shared<Waiting>(
		std::move(queue->waiting.front()));
	queue->waiting.erase(queue->waiting.begin());
	InTransit().push_back(next);
	const auto nextToken = NextToken();
	queue->token = nextToken;
	UpdateOrderingWatchdog(queue, key);
	crl::on_main([=] {
		StartHeldCaptionSend(key, nextToken, next);
	});
}

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

// Whether handing this caption over can accomplish anything, which is the only
// reason to take a chat's ordering slot and hold a photo at all.
//
// Both terms are the pipeline's own answers, not second spellings of them:
// TranslateBeforeSendActive() is the predicate the live preview bar is shown on
// and the one Intercept() reads, and Intercept() refuses a caption carrying
// tags for its own reason - a tag is an absolute offset into these exact
// characters, and replacing them moves every one of them out from under it. So
// this cannot answer differently from the send that follows; asking here only
// means a send that was always going to come straight back does not take a slot
// and does not delay the next photo behind it.
[[nodiscard]] bool PipelineWouldTake(
		not_null<History*> history,
		not_null<const TextWithTags*> caption) {
	return caption->tags.isEmpty() && TranslateBeforeSendActive(history);
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

void SetCaptionHeld(not_null<CaptionSend*> state, bool held) {
	if (state->held == held) {
		return;
	}
	state->held = held;
	if (held) {
		HeldCaptions().emplace_or_assign(
			state->key,
			state->original.text.trimmed());
	} else {
		HeldCaptions().remove(state->key);
	}
}

void ReleaseCaptionSlot(not_null<CaptionSend*> state) {
	if (base::take(state->owns)) {
		ReleaseChatSlot(state->key, state->token);
	}
}

void FinishCaptionSend(not_null<CaptionSend*> state) {
	if (state->finished) {
		return;
	}
	state->finished = true;
	SetCaptionHeld(state, false);
	RestoreOverlongCaption(state);
	if (const auto proceed = base::take(state->proceed)) {
		proceed();
	}
	ReleaseCaptionSlot(state);
}

CaptionSend::~CaptionSend() {
	if (finished) {
		return;
	}
	finished = true;
	SetCaptionHeld(this, false);
	if (caption) {
		*caption = original;
	}
	auto callback = base::take(proceed);
	auto release = Fn<void()>();
	if (base::take(owns)) {
		const auto slotKey = key;
		const auto slotToken = token;
		release = [slotKey, slotToken] {
			ReleaseChatSlot(slotKey, slotToken);
		};
	}
	if (!callback) {
		if (release) {
			release();
		}
		return;
	}

	// The pipeline dropped the send without answering it: a cancelled confirm,
	// a closed language picker, a chat that already had a send in flight. For a
	// text message that is how the user cancels and nothing is lost, because
	// the composer still holds what they typed. Here the box is closed and the
	// files exist nowhere else, so the send is finished with the caption as
	// typed instead. It goes to the next main-thread turn because this can run
	// from inside the InterceptCaptionSend() call itself, and it carries a copy
	// of the bundle so that a caller which did not capture one cannot turn a
	// cancelled translation into a send from a destroyed bundle. The slot goes on
	// that same turn, after the send, so the next media send in this chat
	// leaves after this one rather than ahead of it.
	crl::on_main([callback = std::move(callback), release, kept = bundle] {
		callback();
		if (release) {
			release();
		}
	});
}

// A send that waited its turn. The composer was told this one was taken over
// when it was queued, so unlike InterceptSendFiles() there is no "send it
// yourself" answer left to give: every outcome below ends with `proceed`
// invoked and the slot handed on.
void StartHeldCaptionSend(
		const QString &key,
		uint64 token,
		std::shared_ptr<Waiting> waiting) {
	DropInTransit(waiting);
	if (!waiting) {
		ReleaseChatSlot(key, token);
		return;
	}
	const auto history = waiting->history.get();
	const auto bundle = waiting->bundle;
	const auto caption = bundle ? SingleCaption(bundle.get()) : nullptr;
	auto proceed = base::take(waiting->proceed);
	if (!proceed) {
		ReleaseChatSlot(key, token);
		return;
	} else if (!history
		|| !caption
		|| !PipelineWouldTake(history, caption)) {
		// The preference can have been turned off, or the slot handed on to a
		// caption the pipeline was never going to take, while this one waited.
		// It still waited its turn, so it still leaves after the send in front
		// of it - it just leaves without entering the chain.
		proceed();
		ReleaseChatSlot(key, token);
		return;
	}
	const auto state = std::make_shared<CaptionSend>();
	state->bundle = bundle;
	state->history = waiting->history;
	state->caption = caption;
	state->original = *caption;
	state->proceed = std::move(proceed);
	state->key = key;
	state->token = token;
	state->owns = true;
	SetCaptionHeld(state.get(), true);
	const auto carryOn = InterceptCaptionSend(
		history,
		*caption,
		waiting->options,
		[state] { FinishCaptionSend(state.get()); });
	if (carryOn && !state->finished) {
		FinishCaptionSend(state.get());
	}
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
	const auto key = ChatKey(history);

	// The seam is shared, and entering it costs far more than a wasted lookup
	// when the pipeline was never going to take this caption - see
	// PipelineWouldTake() above, which is where the reasoning lives.
	//
	// The ordering queue is still honoured whenever this chat already has media
	// held, because a preference changed mid-hold must not let a later photo
	// overtake it; a queued send that turns out not to be ours by the time its
	// turn comes is released without entering the chain either.
	if (!Queues().contains(key) && !PipelineWouldTake(history, caption)) {
		return true;
	}

	// Photo B, sent while photo A's caption is still being translated. Before
	// this queue existed B went out at once and A whenever its translation came
	// back, so B landed first. The text pipeline's own answer to a second send
	// in the same chat - drop it, the composer still holds the text - is not
	// available here, because these files exist nowhere but in this bundle. So
	// B waits, and leaves as soon as A has left.
	const auto i = Queues().find(key);
	if (i != Queues().end()) {
		const auto queue = i->second.get();
		queue->waiting.push_back({
			.history = base::make_weak(history),
			.bundle = std::move(bundle),
			.options = options,
			.proceed = std::move(proceed),
		});
		if (queue->waiting.size() == 1) {
			UpdateOrderingWatchdog(queue, key);
		}
		return false;
	}

	// Everything that decides whether this caption is translated at all - the
	// stored preference and the send-menu quick toggle, the private / group
	// scope, the per-dialog language lock with its one-time confirm, the rule
	// that sends a message carrying formatting as typed because its offsets
	// index the original characters, the in-flight lock and the watchdog behind
	// it - belongs to the text pipeline and is deliberately not repeated here.
	const auto queue = Queues().emplace(
		key,
		std::make_unique<ChatQueue>()
	).first->second.get();
	const auto token = NextToken();
	queue->token = token;
	const auto state = std::make_shared<CaptionSend>();
	state->bundle = std::move(bundle);
	state->history = base::make_weak(history);
	state->caption = caption;
	state->original = *caption;
	state->proceed = std::move(proceed);
	state->key = key;
	state->token = token;
	state->owns = true;

	// Marked held before the call rather than after it, because the language
	// confirm and the language picker are opened from inside it, synchronously.
	SetCaptionHeld(state.get(), true);
	const auto carryOn = InterceptCaptionSend(
		history,
		*caption,
		options,
		[state] { FinishCaptionSend(state.get()); });

	// `finished` here would mean an interceptor both answered and returned
	// true, which lumina_send_pipeline.h forbids - but if one ever does, this
	// send has already gone out and the caller must not send it a second time.
	// Marking the state finished on the way out is what stops the destructor
	// above from re-sending a bundle the caller is about to send itself.
	if (!carryOn || state->finished) {
		return false;
	}
	state->finished = true;

	// Nothing was held, so this chat is free again before the caller sends.
	// Anything that queued behind it is dispatched on a later main-thread turn,
	// which is after the caller's own synchronous send.
	SetCaptionHeld(state.get(), false);
	ReleaseCaptionSlot(state.get());
	return true;
}

void FlushTranslateCaptionSends() {
	// Only the sends that are still waiting their turn. The one that owns each
	// chat's slot is held by the text pipeline, and Lumina::FlushTranslateSends()
	// has already finished it by the time this runs; a waiting send never
	// entered the chain at all, and ReleaseChatSlot() would have started it from
	// a posted main-thread turn that the quit is not going to reach.
	//
	// The in-transit list first, because an entry is in it precisely because it
	// was the next one out of its chat's queue.
	const auto release = [](Waiting &entry) {
		auto proceed = base::take(entry.proceed);
		if (proceed && entry.history) {
			proceed();
		}
	};
	auto transit = base::take(InTransit());
	for (const auto &entry : transit) {
		if (entry) {
			release(*entry);
		}
	}
	auto queues = base::take(Queues());
	for (auto &pair : queues) {
		for (auto &entry : pair.second->waiting) {
			release(entry);
		}
	}
}

bool HoldsCaptionSend(History *history, const QString &original) {
	if (!history) {
		return false;
	}
	const auto i = HeldCaptions().find(ChatKey(history));
	if (i == HeldCaptions().end()) {
		return false;
	}

	// An empty `original` is the caller saying it cannot tell us which send the
	// box is for, and then the chat is all we have to go on.
	return original.isEmpty() || (i->second == original);
}

rpl::producer<QString> SendCancelLabel(
		History *history,
		const QString &original) {
	return HoldsCaptionSend(history, original)
		? TrValue(u"LuminaSendOriginalCaption"_q)
		: tr::lng_cancel();
}

} // namespace Lumina
