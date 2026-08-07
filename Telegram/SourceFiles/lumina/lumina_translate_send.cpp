/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_translate_send.h"

#include "base/flat_map.h"
#include "base/timer.h"
#include "base/weak_ptr.h"
#include "chat_helpers/compose/compose_show.h"
#include "data/data_peer.h"
#include "data/data_premium_limits.h"
#include "data/data_session.h"
#include "history/history.h"
#include "lang/lang_keys.h"
#include "lumina/lumina_locale.h"
#include "lumina/lumina_send_pipeline.h"
#include "lumina/lumina_settings.h"
#include "lumina/lumina_translate_caption.h"
#include "lumina/lumina_translate_gating.h"
#include "lumina/lumina_translate_providers.h"
#include "lumina/lumina_translate_settings.h"
#include "main/main_session.h"
#include "menu/menu_checked_action.h"
#include "menu/menu_send_details.h"
#include "ui/boxes/single_choice_box.h"
#include "ui/layers/generic_box.h"
#include "ui/widgets/labels.h"
#include "ui/widgets/popup_menu.h"
#include "window/window_session_controller.h"

#include "styles/style_layers.h"
#include "styles/style_menu_icons.h"

#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
#include <crl/crl_on_main.h>

namespace Lumina {
namespace {

// The stall watchdog. Every HTTP request the provider layer makes already
// carries a 15s timeout of its own (lumina_translate_providers.cpp), so this
// only has to cover what that timeout cannot see: an engine that resolves
// nothing, and a callback lost between the request and its reply. Android
// shipped the in-flight lock with no timeout at all, and one stalled request
// then swallowed every later send in that chat for the rest of the session.
constexpr auto kWatchdogTimeout = crl::time(20000);

// How long a send-menu quick toggle stays armed. It means "this next message",
// so a menu the user opened and walked away from must not quietly change how a
// message typed much later is sent.
constexpr auto kQuickToggleLifetime = crl::time(5 * 60 * 1000);

// How many sends may wait behind a held one in the same chat. The queue is only
// ever this long when the user sent several DIFFERENT messages inside a single
// hold - a repeat of a message already held or queued is dropped rather than
// queued - and past it a send goes out untranslated instead of waiting, because
// losing it is not on the table and an unbounded queue is how one stalled chat
// turns into a leak.
constexpr auto kQueueLimit = 4;

// The queue's own bound, and the counterpart of kWatchdogTimeout above. That
// watchdog ends a request that is in flight; nothing ends a confirm box the
// user opened and walked away from, and everything queued behind it would wait
// exactly as long. Three watchdogs is longer than any legitimate chain of a
// stalled request plus the one behind it, so this only ever fires on a hold
// nothing else was going to end.
constexpr auto kQueueGuardTimeout = kWatchdogTimeout * 3;

// The bound on a confirm box or a language picker the user never answers, and
// it is armed for a held CAPTION send only.
//
// Nothing else ends that wait. kWatchdogTimeout above covers a provider in
// flight and is cancelled before the confirm box opens; the language boxes open
// before any watchdog is armed at all; kQueueGuardTimeout only exists once a
// second send has queued behind this one, so a lone hold waits forever. For a
// typed message that is the right answer - the composer still shows the text,
// nothing is lost, and cutting the wait short would send an untranslated
// message out from under a box the user is still reading. For a caption it is
// the whole photo: SendFilesBox has closed and the files exist only inside the
// bundle this send carries.
//
// Quitting does not rescue it either. Application::readyToQuit() holds the quit
// open for pending draft saves and nothing else (ApiWrap::isQuitPrevent()), so
// a media upload started at that point never finishes. The wait has to be
// bounded here, in the session, or not at all.
//
// Minutes rather than seconds, matching the ordering timeout in
// lumina_translate_caption.cpp: long enough that it never cuts short a box
// someone is actually reading, short enough that the photo still leaves.
constexpr auto kCaptionBoxTimeout = crl::time(3 * 60 * 1000);

[[nodiscard]] QString DialogLanguagesKey() {
	return u"trSendLangDialog"_q;
}

// The session id is in the key so that two logged-in accounts cannot share a
// per-chat lock. Peer ids are very nearly unique on their own, which is
// exactly the kind of "nearly" that turns into a cross-account bug later.
[[nodiscard]] QString DialogKey(not_null<History*> history) {
	return QString::number(history->session().uniqueId())
		+ QChar('_')
		+ QString::number(history->peer->id.value);
}

struct QuickToggle {
	uint64 peerId = 0;
	crl::time when = 0;
	bool value = false;
};

[[nodiscard]] QuickToggle &CurrentQuickToggle() {
	static auto result = QuickToggle();
	return result;
}

[[nodiscard]] std::optional<bool> PeekQuickToggle(uint64 peerId) {
	auto &toggle = CurrentQuickToggle();
	if (!toggle.peerId) {
		return std::nullopt;
	} else if (crl::now() - toggle.when > kQuickToggleLifetime) {
		toggle = QuickToggle();
		return std::nullopt;
	} else if (toggle.peerId != peerId) {
		return std::nullopt;
	}
	return toggle.value;
}

// Only the send that actually acted on the override clears it. A send this
// pipeline could not have translated anyway - one that arrived while another
// send in the same chat was still held - leaves it armed for the next one.
void ConsumeQuickToggle(uint64 peerId) {
	auto &toggle = CurrentQuickToggle();
	if (toggle.peerId == peerId) {
		toggle = QuickToggle();
	}
}

void SetQuickToggle(uint64 peerId, bool value) {
	CurrentQuickToggle() = QuickToggle{
		.peerId = peerId,
		.when = crl::now(),
		.value = value,
	};
}

struct PreviewCache {
	QString dialog;
	QString source;
	QString target;
	QString translated;
};

[[nodiscard]] PreviewCache &Preview() {
	static auto result = PreviewCache();
	return result;
}

[[nodiscard]] SendOriginalHook &OriginalHook() {
	static auto result = SendOriginalHook();
	return result;
}

// One held send. `text` points into the Api::MessageToSend that `proceed`
// owns, which is what lumina_send_pipeline.h promises: rewriting it just
// before invoking `proceed` is the supported way to change an outgoing message
// asynchronously, and it dangles the moment the last copy of `proceed` goes.
// So every terminal below moves `proceed` out into a local first and only then
// touches `text`.
struct Request {
	base::weak_ptr<History> history;
	QString original;
	QString target;
	TextWithTags *text = nullptr;
	Fn<void()> proceed;
	base::Timer watchdog;
	uint64 generation = 0;
};

// Deliberately leaked, exactly as the hold in lumina_undo_send.cpp is and for
// the same reason: a Request owns a base::Timer, which is a QObject, and not
// every exit runs Application::readyToQuit() - Sandbox::isSavingSession()
// quits without it. A plain function-local static would then be destroyed at
// static-destruction time, killing a running timer after QApplication is
// already gone. Leaking one pointer removes that whole class of shutdown
// crash, and a send still held at that point has nothing left to do anyway.
[[nodiscard]] base::flat_map<QString, std::unique_ptr<Request>> &Requests() {
	using Map = base::flat_map<QString, std::unique_ptr<Request>>;
	static const auto result = new Map();
	return *result;
}

[[nodiscard]] uint64 NextGeneration() {
	static auto result = uint64(0);
	return ++result;
}

// Every terminal takes the generation as well as the chat key, because a box
// can outlive the send it was opened for: cancel one, send again in the same
// chat, and the first box's buttons would otherwise finish the second send
// with the first one's translation.
[[nodiscard]] Request *FindRequest(const QString &key, uint64 generation) {
	const auto i = Requests().find(key);
	return (i != Requests().end() && i->second->generation == generation)
		? i->second.get()
		: nullptr;
}

// One send waiting behind another in the same chat. `text` points into the
// Api::MessageToSend that `proceed` owns, exactly as Request::text does, so an
// entry keeps `proceed` for as long as it keeps the pointer and dropping the
// entry drops both together.
//
// `original` is kept because it is what a repeat send is recognised by, and
// because the text object it points at belongs to that send alone: comparing
// against the pointer would compare two different messages.
struct Queued {
	base::weak_ptr<History> history;
	QString original;
	TextWithTags *text = nullptr;
	Fn<void()> proceed;
};

// `id` names this queue the way Request::generation names a request, and for
// the same reason: the guard below fires as a main-thread event and then posts
// its work, and a post cannot be cancelled. The queue it was armed for can
// drain and be dropped in that gap, and a send arriving right after would build
// a NEW queue under the same key - which the stale post would then empty, and
// cut short a request that had only just started. A queue only answers to the
// timer that was armed for it.
struct Queue {
	std::vector<Queued> entries;
	base::Timer guard;
	uint64 id = 0;
};

// Leaked for the reason Requests() above is: a Queue owns a base::Timer too.
[[nodiscard]] base::flat_map<QString, std::unique_ptr<Queue>> &Queues() {
	using Map = base::flat_map<QString, std::unique_ptr<Queue>>;
	static const auto result = new Map();
	return *result;
}

void DrainQueue(const QString &key);

// Draining is posted rather than run inline, because every terminal below is
// reached with a message either just sent or just dropped, and the next send
// must not start from inside that. Intercept() therefore queues behind a
// non-empty queue as well as behind a held request, so nothing can slip into
// the gap between a terminal and the drain it posted and arrive out of order.
void ScheduleDrain(const QString &key) {
	if (!Queues().contains(key)) {
		return;
	}
	crl::on_main([key] { DrainQueue(key); });
}

[[nodiscard]] Window::SessionController *ResolveController(
		not_null<History*> history) {
	return history->session().tryResolveWindow(history->peer);
}

// A translation longer than the message limit would be split into several
// messages by ApiWrap::sendMessage(), which both looks wrong and breaks the
// single sent-text correlation W2-B needs. The original already passed the
// composer's own length check, so sending that is always the safe answer.
// This has to cover the W2-D preview reuse path as well, which never goes
// near the provider and would otherwise skip the check entirely.
[[nodiscard]] bool TranslationFits(
		History *history,
		const QString &translated) {
	if (!history) {
		return true;
	}
	const auto limits = Data::PremiumLimits(&history->session());
	return (int(translated.size()) <= limits.messageLengthCurrent());
}

void FinishRequest(
		const QString &key,
		uint64 generation,
		const QString &translated) {
	const auto request = FindRequest(key, generation);
	if (!request) {
		return;
	}
	auto proceed = std::move(request->proceed);
	const auto text = request->text;
	const auto original = request->original;
	const auto history = request->history.get();
	Requests().remove(key);
	ScheduleDrain(key);
	if (!proceed) {
		return;
	} else if (!history) {
		// The chat was destroyed while its send was held. `proceed` is guarded
		// by the composer widget that made it, but the message it owns still
		// names this History, so finishing the send would hand a destroyed chat
		// to ApiWrap. Dropping it is the quiet failure, and there is nothing
		// else to do with a message whose destination is gone.
		return;
	}
	if (text && !translated.isEmpty() && (translated != original)) {
		text->text = translated;

		// Not for a caption. The store behind this hook is keyed on the message
		// id ApiWrap::sendMessage() mints, and a media send never goes near
		// that function - it leaves through ApiWrap::sendFiles(). So arming for
		// a caption arms something nothing can ever claim, and it does not just
		// sit there harmlessly: a text message sent in the same chat inside the
		// arm's lifetime and carrying that exact translation WOULD claim it,
		// and would then show the photo's caption as its own original.
		const auto hook = OriginalHook();
		if (hook && !HoldsCaptionSend(history, original)) {
			hook(history, translated, original);
		}
	}
	proceed();
}

// Dropping the request drops `proceed` with it, which is how
// lumina_send_pipeline.h spells "cancel". The composer never cleared its
// field, so the user's text is still sitting in it afterwards.
void CancelRequest(const QString &key, uint64 generation) {
	if (FindRequest(key, generation)) {
		Requests().remove(key);
		ScheduleDrain(key);
	}
}

// Armed just before a box that has no deadline of its own is opened, and only
// when the send behind it is a caption send - see kCaptionBoxTimeout above.
// Everything that answers the box calls FinishRequest() or CancelRequest(),
// both of which drop the Request and the timer with it, and StartTranslation()
// re-arms the same timer with its own watchdog, so this never outlives the wait
// it was armed for. A box still open when it fires has nothing left to answer,
// which is the trade FlushQueue() already makes.
void ArmCaptionBoxGuard(const QString &key, uint64 generation) {
	const auto request = FindRequest(key, generation);
	if (!request
		|| !HoldsCaptionSend(request->history.get(), request->original)) {
		return;
	}
	request->watchdog.setCallback([=] {
		crl::on_main([=] { FinishRequest(key, generation, QString()); });
	});
	request->watchdog.callOnce(kCaptionBoxTimeout);
}

[[nodiscard]] uint64 CreateRequest(
		const QString &key,
		not_null<History*> history,
		TextWithTags &text,
		const QString &original,
		Fn<void()> proceed) {
	const auto generation = NextGeneration();
	auto created = std::make_unique<Request>();
	created->history = base::make_weak(history);
	created->original = original;
	created->text = &text;
	created->proceed = std::move(proceed);
	created->generation = generation;
	Requests().emplace(key, std::move(created));
	return generation;
}

void ApplyTranslation(
		const QString &key,
		uint64 generation,
		const QString &translated);

void ShowTranslationConfirm(
		const QString &key,
		uint64 generation,
		const QString &translated);

void StartTranslation(
		const QString &key,
		uint64 generation,
		const QString &target) {
	const auto request = FindRequest(key, generation);
	if (!request) {
		return;
	}
	request->target = target;
	request->watchdog.setCallback([=] {
		crl::on_main([=] { FinishRequest(key, generation, QString()); });
	});
	request->watchdog.callOnce(kWatchdogTimeout);

	const auto history = request->history.get();
	const auto session = history ? &history->session() : nullptr;
	const auto original = request->original;
	TranslateText(session, original, target, [=](TranslateResult result) {
		const auto text = result.failed()
			? QString()
			: result.text.trimmed();

		// The provider callback runs inside its own network reply, and
		// TranslateText() may even answer synchronously when no engine can be
		// built at all. Everything below sends a message and can open a box,
		// so it belongs on a clean main-thread turn either way.
		crl::on_main([=] { ApplyTranslation(key, generation, text); });
	});
}

void ApplyTranslation(
		const QString &key,
		uint64 generation,
		const QString &translated) {
	const auto request = FindRequest(key, generation);
	if (!request) {
		return;
	}
	request->watchdog.cancel();
	if (translated.isEmpty() || (translated == request->original)) {
		FinishRequest(key, generation, QString());
		return;
	}
	if (!TranslationFits(request->history.get(), translated)) {
		FinishRequest(key, generation, QString());
		return;
	}
	if (TranslateBeforeSendConfirm()) {
		ShowTranslationConfirm(key, generation, translated);
	} else {
		FinishRequest(key, generation, translated);
	}
}

void ShowTranslationConfirm(
		const QString &key,
		uint64 generation,
		const QString &translated) {
	const auto request = FindRequest(key, generation);
	const auto history = request ? request->history.get() : nullptr;
	const auto controller = history ? ResolveController(history) : nullptr;
	if (!controller) {
		FinishRequest(key, generation, translated);
		return;
	}
	const auto original = request->original;
	const auto name = TranslateLanguageName(request->target);
	ArmCaptionBoxGuard(key, generation);
	const auto routed = std::make_shared<bool>(false);
	controller->show(Box([=](not_null<Ui::GenericBox*> box) {
		box->setTitle(TrValue(u"LuminaTranslateBeforeSend"_q));
		box->addRow(object_ptr<Ui::FlatLabel>(
			box.get(),
			Tr(u"LuminaTranslateOriginalLabel"_q)
				+ QChar('\n')
				+ original
				+ u"\n\n"_q
				+ name
				+ QChar('\n')
				+ translated,
			st::boxLabel));
		// Nothing runs after closeBox() anywhere below: closing a box may
		// destroy it, and with it the very lambda that is executing. So each
		// outcome is posted to the next main-thread turn and the close is the
		// last statement, which also keeps the send itself out of a layer that
		// is in the middle of going away.
		box->addButton(TrValue(u"LuminaSendTranslation"_q), [=] {
			*routed = true;
			crl::on_main([=] {
				FinishRequest(key, generation, translated);
			});
			box->closeBox();
		});
		box->addButton(TrValue(u"LuminaSendOriginal"_q), [=] {
			*routed = true;
			crl::on_main([=] {
				FinishRequest(key, generation, QString());
			});
			box->closeBox();
		});
		// NOT tr::lng_cancel(). Cancelling a text send abandons it and the
		// composer still holds what the user typed, so "Cancel" is exact. The
		// same button on a send that carries a caption abandons nothing: the
		// files exist only inside the bundle this send is carrying and
		// lumina_translate_caption.h finishes them with the caption as typed
		// rather than lose a photo to a dismissed box. SendCancelLabel() is the
		// one place that knows which of the two this box belongs to, and it is
		// matched on the held text as well as on the chat because a chat can
		// have a text send held and a caption send queued behind it.
		box->addLeftButton(SendCancelLabel(history, original), [=] {
			box->closeBox();
		});
		box->lifetime().add([=] {
			if (!*routed) {
				CancelRequest(key, generation);
			}
		});
	}));
}

// `pending` is the send this picker was opened for, or an empty key when it
// was opened from the settings-style entry point that only rewrites the lock.
void ShowLanguagePicker(
		not_null<Window::SessionController*> controller,
		base::weak_ptr<History> weak,
		const QString &current,
		const QString &pending,
		uint64 generation) {
	const auto &languages = TranslateLanguages();
	auto options = std::vector<QString>();
	auto codes = std::vector<QString>();
	options.reserve(languages.size() + 1);
	codes.reserve(languages.size() + 1);
	for (const auto &language : languages) {
		options.push_back(language.name);
		codes.push_back(language.code);
	}

	// A code this build's table does not carry - written by another client, or
	// restored from a backup - gets an entry of its own, so the picker cannot
	// show a selection that disagrees with the lock it is editing.
	if (!current.isEmpty()
		&& (ranges::find(codes, current) == end(codes))) {
		options.push_back(TranslateLanguageName(current));
		codes.push_back(current);
	}
	// SingleChoiceBox only fires its callback when the radio group's value
	// CHANGES, and its own button just closes the box. A pre-selected row is
	// therefore a row that cannot be chosen: picking it does nothing, the box
	// closes, and for a picker opened for a send in flight that means the held
	// message is dropped even though the user believes they answered. A
	// send-time picker starts with nothing selected, and -1 is also the right
	// start when there is no lock yet, so that the first language in the table
	// is not silently presented as the current answer.
	auto selected = -1;
	for (auto i = 0, count = int(codes.size()); i != count; ++i) {
		if (codes[i] == current) {
			selected = i;
			break;
		}
	}
	if (!pending.isEmpty()) {
		selected = -1;
	}
	const auto routed = std::make_shared<bool>(false);
	controller->show(Box([=](not_null<Ui::GenericBox*> box) {
		SingleChoiceBox(box, {
			.title = TrValue(u"LuminaTrSendPickerTitle"_q),
			.options = options,
			.initialSelection = selected,
			.callback = [=](int index) {
				if (index < 0 || index >= int(codes.size())) {
					return;
				}
				*routed = true;
				const auto code = codes[index];
				if (const auto history = weak.get()) {
					SetDialogSendLanguage(history, code);
				}
				if (!pending.isEmpty()) {
					crl::on_main([=] {
						StartTranslation(pending, generation, code);
					});
				}
			},
		});
		box->lifetime().add([=] {
			if (!*routed && !pending.isEmpty()) {
				CancelRequest(pending, generation);
			}
		});
	}));
}

// The one-time per-chat confirm. Detection is only a suggestion here - it is
// wrong often enough between close neighbours (Malay and Indonesian being the
// pair that made Android add this box in the first place) that sending on it
// silently is not acceptable - so the answer is locked for the chat and never
// asked again.
void ShowSendLanguageConfirm(
		not_null<Window::SessionController*> controller,
		base::weak_ptr<History> weak,
		const QString &key,
		uint64 generation,
		const QString &code) {
	const auto name = TranslateLanguageName(code);
	const auto routed = std::make_shared<bool>(false);
	const auto weakController = base::make_weak(controller);
	controller->show(Box([=](not_null<Ui::GenericBox*> box) {
		box->setTitle(TrValue(u"LuminaTranslateBeforeSend"_q));
		box->addRow(object_ptr<Ui::FlatLabel>(
			box.get(),
			Tr(u"LuminaTrSendConfirmMessage"_q, name),
			st::boxLabel));
		box->addButton(TrValue(u"LuminaTrSendConfirmTranslate"_q), [=] {
			*routed = true;
			if (const auto history = weak.get()) {
				SetDialogSendLanguage(history, code);
			}
			crl::on_main([=] {
				StartTranslation(key, generation, code);
			});
			box->closeBox();
		});
		box->addButton(TrValue(u"LuminaTrSendAsTyped"_q), [=] {
			*routed = true;
			crl::on_main([=] {
				FinishRequest(key, generation, QString());
			});
			box->closeBox();
		});
		box->addLeftButton(TrValue(u"LuminaTrSendChooseOther"_q), [=] {
			*routed = true;
			crl::on_main([=] {
				if (const auto strong = weakController.get()) {
					ShowLanguagePicker(strong, weak, code, key, generation);
				} else {
					FinishRequest(key, generation, QString());
				}
			});
			box->closeBox();
		});
		box->lifetime().add([=] {
			if (!*routed) {
				CancelRequest(key, generation);
			}
		});
	}));
}

void AskSendLanguage(
		not_null<Window::SessionController*> controller,
		not_null<History*> history,
		const QString &key,
		uint64 generation) {
	const auto weak = base::make_weak(history);
	const auto offered = history->translateOfferedFrom();
	const auto detected = offered
		? NormalizeLanguageCode(offered.twoLetterCode())
		: QString();
	if (detected.isEmpty()) {
		ShowLanguagePicker(controller, weak, QString(), key, generation);
	} else {
		ShowSendLanguageConfirm(controller, weak, key, generation, detected);
	}
}

// A queued send that is not going to be translated after all. The chat can have
// gone while it waited, and then the send is dropped instead: see the note in
// FinishRequest() above, which is the same situation.
void SendAsTyped(Queued &entry) {
	auto proceed = base::take(entry.proceed);
	if (proceed && entry.history) {
		proceed();
	}
}

// The queue's watchdog. It fires on a hold that nothing else ended - in
// practice a confirm box or a language picker the user walked away from - and
// empties the chat: the held send first, so the order the user typed in
// survives, then everything waiting behind it, all as typed.
//
// A box that was still open when this ran keeps its buttons, and they now find
// no request to answer, so pressing one does nothing at all. That is the same
// trade kWatchdogTimeout already makes and the file header describes: a message
// the user pressed Send on a minute ago has to go out.
void FlushQueue(const QString &key, uint64 id) {
	// The queue goes first, and no queue of this id ends it: a chat that
	// drained while the timer's work was already posted has a request in
	// flight that started seconds ago, and cutting that one short would be the
	// timer firing at a backlog that is no longer there - whether the queue is
	// gone entirely or a later send has already built another one.
	const auto i = Queues().find(key);
	if ((i == Queues().end()) || (i->second->id != id)) {
		return;
	}
	const auto queue = std::move(i->second);
	Queues().erase(i);
	const auto j = Requests().find(key);
	if (j != Requests().end()) {
		FinishRequest(key, j->second->generation, QString());
	}
	for (auto &entry : queue->entries) {
		SendAsTyped(entry);
	}
}

// Everything after the decision that this send is ours to translate: the W2-D
// preview reuse, the target language, and the one-time per-chat confirm behind
// it. Returns false when it did NOT take the send over - and only then is
// `proceed` still untouched, which is what lets the caller either pass the
// message on to the rest of the chain or, for a queued send that is long past
// that point, simply send it as typed.
[[nodiscard]] bool BeginRequest(
		const QString &key,
		not_null<History*> history,
		TextWithTags &text,
		const QString &original,
		Fn<void()> &proceed) {
	const auto target = TranslateSendLanguageIsAuto()
		? DialogSendLanguage(history)
		: TranslateSendLanguage();
	if (!target.isEmpty()) {
		const auto &preview = Preview();
		const auto reuse = (preview.dialog == key)
			&& (preview.source == original)
			&& (preview.target == target)
			&& !preview.translated.isEmpty()
			&& TranslationFits(history, preview.translated);
		const auto translated = reuse ? preview.translated : QString();
		if (reuse) {
			// One entry, consumed once. Keeping it would serve the
			// stored translation to a later identical message even
			// after the provider or its key changed, and would leave
			// the last message the user composed sitting in a
			// process-lifetime static.
			Preview() = PreviewCache();
		}
		const auto generation = CreateRequest(
			key,
			history,
			text,
			original,
			std::move(proceed));
		if (reuse) {
			FinishRequest(key, generation, translated);
		} else {
			StartTranslation(key, generation, target);
		}
		return true;
	}
	const auto controller = ResolveController(history);
	if (!controller) {
		return false;
	}
	const auto generation = CreateRequest(
		key,
		history,
		text,
		original,
		std::move(proceed));
	ArmCaptionBoxGuard(key, generation);
	AskSendLanguage(controller, history, key, generation);
	return true;
}

// Recognises the same message arriving twice, and it is the whole reason a
// second send can be queued at all without sending anything twice.
//
// The seam sits before the composer clears its field, so a chat with a send
// held still shows that text and a second tap on Send offers it again. Queueing
// it would put the same message on the wire twice; so would passing it through,
// and passing it through is the shape this arrives in when the pipeline decides
// the second tap is not even its business - a quick toggle consumed by the
// first tap is enough to get there. Both are answered the same way: drop it.
// Nothing is lost, because the identical message is already on its way out.
[[nodiscard]] bool AlreadyHeld(const QString &key, const QString &original) {
	const auto i = Requests().find(key);
	if (i != Requests().end() && i->second->original == original) {
		return true;
	}
	const auto j = Queues().find(key);
	if (j == Queues().end()) {
		return false;
	}
	for (const auto &entry : j->second->entries) {
		if (entry.original == original) {
			return true;
		}
	}
	return false;
}

// Returns false when the queue is full, and only then is `proceed` untouched.
[[nodiscard]] bool Enqueue(
		const QString &key,
		not_null<History*> history,
		TextWithTags &text,
		const QString &original,
		Fn<void()> &proceed) {
	auto i = Queues().find(key);
	if (i == Queues().end()) {
		// The guard is armed once, when the queue appears, so its deadline
		// belongs to the entry that has waited longest, and it dies with the
		// queue - which means it can only ever fire while something is still
		// waiting. An entry queued just before it fires goes out as typed
		// rather than translated; it is still in order and still sent.
		i = Queues().emplace(key, std::make_unique<Queue>()).first;
		const auto queue = i->second.get();

		// The same counter the requests use. All either needs is a value no
		// other live queue or request can carry.
		const auto id = NextGeneration();
		queue->id = id;
		queue->guard.setCallback([=] {
			// Firing destroys the queue and this timer with it, so the work
			// cannot run inside the timer's own callback - and because the
			// post that carries it cannot be cancelled, it names the queue it
			// was armed for rather than only the chat.
			crl::on_main([=] { FlushQueue(key, id); });
		});
		queue->guard.callOnce(kQueueGuardTimeout);
	} else if (int(i->second->entries.size()) >= kQueueLimit) {
		return false;
	}
	i->second->entries.push_back(Queued{
		.history = base::make_weak(history),
		.original = original,
		.text = &text,
		.proceed = std::move(proceed),
	});
	return true;
}

void StartQueued(const QString &key, Queued &entry) {
	const auto history = entry.history.get();
	if (history
		&& entry.text
		&& entry.proceed
		&& BeginRequest(
			key,
			history,
			*entry.text,
			entry.original,
			entry.proceed)) {
		return;
	}
	SendAsTyped(entry);
}

void DrainQueue(const QString &key) {
	while (!Requests().contains(key)) {
		const auto i = Queues().find(key);
		if (i == Queues().end()) {
			return;
		}
		auto &entries = i->second->entries;
		if (entries.empty()) {
			Queues().erase(i);
			return;
		}
		auto entry = std::move(entries.front());
		entries.erase(entries.begin());
		if (entries.empty()) {
			Queues().erase(i);
		}

		// A send that starts a request of its own ends the loop - the next one
		// waits for that request's terminal to post the next drain. A send that
		// turns out not to be translatable after all went out inside
		// StartQueued(), and the one behind it can go now.
		StartQueued(key, entry);
	}
}

bool Intercept(
		not_null<History*> history,
		TextWithTags &text,
		Fn<void()> proceed) {
	if (!ContinuousTranslationAvailable()) {
		return true;
	}
	const auto peer = history->peer;
	const auto original = text.text.trimmed();

	// A tagged message carries bold / mention / custom-emoji ranges as
	// character offsets into this exact string. Replacing the string would
	// leave every one of them pointing at the wrong characters, or past the
	// end, and it fails silently rather than loudly. Dropping the user's
	// formatting instead is no better, so a formatted message is sent as
	// typed. This is checked before the quick toggle is consumed, so a
	// "translate just this one" armed on a message that cannot be translated
	// at all is still there for the next one.
	if (original.isEmpty() || !text.tags.isEmpty()) {
		return true;
	}
	const auto key = DialogKey(history);

	// Before anything decides whether this send is ours, because a repeat is a
	// repeat either way and passing one through is a double send just as much
	// as queueing it would be.
	if (AlreadyHeld(key, original)) {
		return false;
	}
	const auto quick = PeekQuickToggle(peer->id.value);
	if (!quick.value_or(TranslateBeforeSend())) {
		ConsumeQuickToggle(peer->id.value);
		return true;
	}

	// This chat already has a send held, or one waiting behind it. Android
	// ignores the second send here, and on Android that costs nothing: its
	// composer field was cleared when the first send started, so a second Send
	// tap carries nothing. The desktop seam sits before the field is cleared,
	// so a second send arrives with real text - and the AI editor and the
	// flattened rich page pass text that exists nowhere else at all. Queue it.
	//
	// Not "send this one untranslated instead": the held request would complete
	// afterwards and send the very same message a second time.
	if (Requests().contains(key) || Queues().contains(key)) {
		// A full queue is the one case where the message is passed straight
		// through. It reaches the chat ahead of the ones still waiting, which
		// is the price of never losing it, and it takes kQueueLimit different
		// messages sent inside a single hold to get there.
		if (!Enqueue(key, history, text, original, proceed)) {
			return true;
		}
	} else if (!BeginRequest(key, history, text, original, proceed)) {
		return true;
	}

	// The override is consumed by the send that read it, whether that send was
	// started or queued, and never later: a queued send that consumed it when
	// it finally ran would clear an override the user armed for a message they
	// are still typing.
	ConsumeQuickToggle(peer->id.value);
	return false;
}

} // namespace

bool InterceptCaptionSend(
		not_null<History*> history,
		TextWithTags &caption,
		Api::SendOptions options,
		Fn<void()> proceed) {
	Expects(proceed != nullptr);

	// `options` is deliberately not consulted: nothing in this pipeline depends
	// on it, and the parameter is here so the caption seam hands its send over
	// in exactly the shape lumina_send_pipeline.h describes.
	return Intercept(history, caption, std::move(proceed));
}

void FlushTranslateSends() {
	// Every pass either finishes one request or empties one chat's queue, and
	// nothing reachable from here can add either: Intercept() only runs from a
	// composer, and no composer sends while the application is quitting. The
	// bound is a safety net rather than a real limit - one request and one
	// queue per open chat is the whole of what can be waiting.
	constexpr auto kFlushLimit = 1024;
	for (auto pass = 0; pass != kFlushLimit; ++pass) {
		if (!Requests().empty()) {
			const auto i = Requests().begin();
			const auto key = i->first;
			const auto generation = i->second->generation;
			FinishRequest(key, generation, QString());
			continue;
		}
		const auto i = Queues().begin();
		if (i == Queues().end()) {
			return;
		}

		// ScheduleDrain() posts, and a post made during the quit never runs, so
		// the backlog is emptied here rather than left to it.
		auto queue = std::move(i->second);
		Queues().erase(i);
		for (auto &entry : queue->entries) {
			SendAsTyped(entry);
		}
	}
}

void FlushTranslateSendsAndCaptions() {
	// The requests above own the caption sends that reached the chain, so they
	// go first; what is left in the caption seam's own ordering queue never
	// reached it and is released afterwards, which keeps the order the user
	// sent in.
	FlushTranslateSends();
	FlushTranslateCaptionSends();
}

void SetupTranslateSendPipeline() {
	static auto registered = false;
	if (registered) {
		return;
	}
	registered = true;
	RegisterSendInterceptor([](
			not_null<History*> history,
			TextWithTags &text,
			Api::SendOptions,
			Fn<void()> proceed) {
		return Intercept(history, text, std::move(proceed));
	});
}

bool TranslateBeforeSendActive(not_null<History*> history) {
	if (!ContinuousTranslationAvailable()) {
		return false;
	}
	const auto peer = history->peer;
	return PeekQuickToggle(peer->id.value).value_or(TranslateBeforeSend());
}

QString DialogSendLanguage(not_null<History*> history) {
	return Settings::Instance().getObject(
		DialogLanguagesKey()
	).value(DialogKey(history)).toString().trimmed();
}

void SetDialogSendLanguage(not_null<History*> history, const QString &code) {
	const auto key = DialogKey(history);
	const auto trimmed = code.trimmed();
	auto object = Settings::Instance().getObject(DialogLanguagesKey());
	if (trimmed.isEmpty()) {
		object.remove(key);
	} else {
		object.insert(key, trimmed);
	}
	if (object.isEmpty()) {
		Settings::Instance().remove(DialogLanguagesKey());
	} else {
		Settings::Instance().set(
			DialogLanguagesKey(),
			object,
			Store::Private);
	}
	auto &preview = Preview();
	if (preview.dialog == key) {
		preview = PreviewCache();
	}
}

QString ResolveSendLanguage(not_null<History*> history) {
	if (!TranslateBeforeSendActive(history)) {
		return QString();
	} else if (!TranslateSendLanguageIsAuto()) {
		return TranslateSendLanguage();
	}
	return DialogSendLanguage(history);
}

void ShowDialogSendLanguagePicker(not_null<History*> history) {
	if (const auto controller = ResolveController(history)) {
		ShowLanguagePicker(
			controller,
			base::make_weak(history),
			DialogSendLanguage(history),
			QString(),
			0);
	}
}

void NoteSendTranslationPreview(
		not_null<History*> history,
		const QString &source,
		const QString &target,
		const QString &translated) {
	auto &preview = Preview();
	preview.dialog = DialogKey(history);
	preview.source = source.trimmed();
	preview.target = target;
	preview.translated = translated.trimmed();
}

void SetSendOriginalHook(SendOriginalHook hook) {
	OriginalHook() = std::move(hook);
}

void AddSendMenuTranslateRow(
		not_null<Ui::PopupMenu*> menu,
		const SendMenu::Details &details) {
	// Everything below the send-behaviour group of the menu is media state:
	// an album, a caption or a paid post never routes through the text send
	// path this row talks about, so any of it being set rules the row out.
	//
	// The reverse is NOT true, and SendMenu::Details cannot express it. The
	// sticker, GIF, inline-result and field-autocomplete panels all build
	// their menu from the composer's own sendMenuDetails(), which leaves every
	// field here unset, so the row is offered there too. It stays correct -
	// the override is per chat and is consumed by that chat's next text send -
	// but it is offered in more places than it reads well in. Narrowing it
	// needs a flag on SendMenu::Details, which is not this item's file.
	if (!ContinuousTranslationAvailable()
		|| !details.barePeerId
		|| (details.spoiler != SendMenu::SpoilerState::None)
		|| (details.caption != SendMenu::CaptionState::None)
		|| (details.photoQuality != SendMenu::PhotoQualityState::None)
		|| details.price.has_value()) {
		return;
	}
	const auto peerId = details.barePeerId;
	const auto checked = PeekQuickToggle(peerId).value_or(
		TranslateBeforeSend());
	Menu::AddCheckedAction(
		menu,
		Tr(u"LuminaTranslateBeforeSend"_q),
		[=] { SetQuickToggle(peerId, !checked); },
		&st::menuIconTranslate,
		checked);
}

void AddSendMenuTranslateRow(
		not_null<Ui::PopupMenu*> menu,
		const std::shared_ptr<ChatHelpers::Show> &show,
		const SendMenu::Details &details) {
	AddSendMenuTranslateRow(menu, details);
	if (!show
		|| !ContinuousTranslationAvailable()
		|| !details.barePeerId
		|| (details.spoiler != SendMenu::SpoilerState::None)
		|| (details.caption != SendMenu::CaptionState::None)
		|| (details.photoQuality != SendMenu::PhotoQualityState::None)
		|| details.price.has_value()) {
		return;
	}

	// Only while the language is actually per chat. With a non-auto send
	// language set globally, DialogSendLanguage() is never consulted and a row
	// that edits it would be a row that changes nothing.
	if (!TranslateSendLanguageIsAuto()) {
		return;
	}

	// The session comes from the show rather than from the peer id, which is
	// the whole reason this overload exists - see the header.
	const auto history = show->session().data().history(
		PeerId(details.barePeerId));
	if (!TranslateBeforeSendActive(history)) {
		return;
	}
	menu->addAction(
		Tr(u"LuminaTrSendPickerTitle"_q),
		[=] { ShowDialogSendLanguagePicker(history); },
		&st::menuIconTranslate);
}

namespace {

// The registry in lumina_send_pipeline.cpp is a function-local static, so
// registering from dynamic initialization is order-independent, and nothing
// this interceptor reads is touched until a message is actually sent. The
// public SetupTranslateSendPipeline() stays available for an explicit init
// point should one ever be added.
struct Registrar {
	Registrar() {
		SetupTranslateSendPipeline();
	}
};

[[maybe_unused]] const auto kRegistrar = Registrar();

} // namespace

} // namespace Lumina
