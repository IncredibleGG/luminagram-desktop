/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_undo_send.h"

#include "base/timer.h"
#include "base/weak_ptr.h"
#include "data/data_drafts.h"
#include "dialogs/dialogs_key.h"
#include "history/history.h"
#include "lumina/lumina_locale.h"
#include "lumina/lumina_send_pipeline.h"
#include "lumina/lumina_settings.h"
#include "main/main_session.h"
#include "mainwidget.h"
#include "mainwindow.h"
#include "storage/storage_account.h"
#include "ui/qt_object_factory.h"
#include "ui/toast/toast.h"
#include "ui/widgets/buttons.h"
#include "window/window_session_controller.h"

#include "styles/style_chat.h"

#include <QtCore/QJsonValue>
#include <crl/crl_on_main.h>

namespace Lumina {
namespace {

const auto kEnabledKey = u"undoSendWindow"_q;

// Android holds for 5000ms and so do we. Long enough to notice the mistake,
// short enough that the recipient does not see the message arrive late in a
// conversation that is actually moving.
constexpr auto kWindow = crl::time(5000);

// One held send.
//
// `proceed` is the composer's continuation, which owns the Api::MessageToSend
// the text reference in InterceptSend() points into. Dropping it is how
// lumina_send_pipeline.h spells "cancel"; invoking it exactly once is how it
// spells "send". `text` is only a snapshot for the collision rule below - the
// live object is never touched, because undo-send does not rewrite messages.
struct Hold {
	base::weak_ptr<History> history;
	base::weak_ptr<Window::SessionController> controller;
	QString text;
	Fn<void()> proceed;
	base::weak_ptr<Ui::Toast::Instance> toast;
	base::Timer timer;
	rpl::lifetime lifetime;
	uint64 generation = 0;
};

// Deliberately leaked. A Hold owns a base::Timer, which is a QObject, and not
// every exit runs readyToQuit() - Sandbox::isSavingSession() quits without it -
// so a plain function-local static could be destroyed at static-destruction
// time, after QApplication is already gone. Leaking one pointer costs nothing
// and removes that whole class of shutdown crash.
[[nodiscard]] std::unique_ptr<Hold> &Current() {
	static const auto result = new std::unique_ptr<Hold>();
	return *result;
}

[[nodiscard]] uint64 NextGeneration() {
	static auto result = uint64(0);
	return ++result;
}

// Every terminal is addressed by generation rather than by "whatever is held
// right now", because the Undo button lives on a toast that outlives the hold
// it was shown for: the toast is still on screen, and still clickable, while
// it animates away after the send has already gone out. Without the
// generation that late click would cancel a *different* message.
[[nodiscard]] std::unique_ptr<Hold> TakeHold(uint64 generation) {
	auto &current = Current();
	if (!current || current->generation != generation) {
		return nullptr;
	}
	return std::move(current);
}

void ReleaseHold(const std::unique_ptr<Hold> &hold) {
	Expects(hold != nullptr);

	hold->timer.cancel();
	hold->lifetime.destroy();
	if (const auto toast = hold->toast.get()) {
		toast->hideAnimated();
	}
}

// Removes the local draft this send left behind in the chat it came from, and
// ONLY when that draft is character-for-character the text that just went out.
//
// THE ORDERING THIS EXISTS FOR, BECAUSE IT IS NOT THE OBVIOUS ONE.
// HistoryWidget writes the composer's text into the chat's local draft on its
// way out of that chat - showHistory() calls saveCurrentDraftToCloud(), which
// runs Core::App().materializeLocalDrafts() -> saveFieldToHistoryLocalDraft()
// (history_widget.cpp:3022), and MainWidget::showNewSection() gets there
// through _history->showHistory(PeerId(), MsgId()) (mainwidget.cpp:1939).
// Both of those happen before anything announces the move, so by the time a
// held send is dispatched the residue is already on disk. Meanwhile `proceed`
// itself skips clearFieldText() / saveDraftWithTextNow() once the composer has
// moved, because by then those belong to another chat.
//
// The cloud draft is not a problem: ApiWrap::sendMessage() clears it and sets
// f_clear_draft on the wire. The local one is - without this the text would be
// sitting in the message box again the next time the user opened this chat,
// directly under the message it had already sent, which invites sending it
// twice.
//
// The exact-text guard is what makes this safe to do at all: a draft the user
// typed instead of, or on top of, the held message does not match and is left
// completely alone. It also means the residue survives when an interceptor
// registered ahead of this one rewrote the text - translate-before-send does -
// because then the draft holds what the user typed and this holds what went
// out. Leaving a draft behind is the right way to be wrong here.
void ClearSentLocalDraft(History *history, const QString &text) {
	if (!history) {
		return;
	}

	// The same key HistoryWidget::saveFieldToHistoryLocalDraft() writes: this
	// send provably came from HistoryWidget, which has no topic and no
	// monoforum sublist of its own.
	const auto topicRootId = MsgId();
	const auto monoforumPeerId = PeerId();
	const auto draft = history->localDraft(topicRootId, monoforumPeerId);
	if (!draft || draft->textWithTags.text != text) {
		return;
	}
	history->clearLocalDraft(topicRootId, monoforumPeerId);
	history->session().local().writeDrafts(history);
}

// Sends the held message now. Everything is torn down before `proceed` runs,
// so a terminal reached from inside `proceed` - the quit flush re-entering,
// another interceptor further down the chain - finds nothing left to act on.
void DispatchHold(std::unique_ptr<Hold> hold) {
	if (!hold) {
		return;
	}
	ReleaseHold(hold);
	auto proceed = std::move(hold->proceed);
	if (!proceed) {
		return;
	}

	// The Api::MessageToSend that `proceed` owns names this History by raw
	// pointer, so finishing a send whose History has been destroyed would
	// dereference it. lumina_translate_send.h makes exactly the same call for
	// exactly the same reason: a message whose chat no longer exists cannot be
	// sent anywhere, so it is dropped rather than crashed on. In practice a
	// History outlives its session's composer, so this is a guard and not a
	// path the five second window is expected to take.
	if (!hold->history) {
		return;
	}
	const auto history = hold->history.get();

	// Whether the composer has moved off this chat decides who owns the text
	// left in the chat's local draft, and it has to be read before `proceed`
	// runs, because `proceed` is what moves the composer's own state. The
	// nullable sessionContent() rather than SessionController::content(): the
	// latter wraps it in not_null and would assert while a window is being
	// torn down. A window or a MainWidget that is already gone resolves to
	// "not moved", which is the conservative answer - `proceed` is guarded on
	// the composer, so a dead one means the message never went out, and a
	// draft removed then would take the user's text with it.
	const auto controller = hold->controller.get();
	const auto content = controller
		? controller->widget()->sessionContent()
		: nullptr;
	const auto moved = content && (content->peer() != history->peer.get());
	proceed();
	if (moved) {
		ClearSentLocalDraft(history, hold->text);
	}
}

// Cancels the held message. The composer never cleared its field, so the text
// the user asked for back is already sitting in it, and there is deliberately
// nothing to restore here.
void DropHold(std::unique_ptr<Hold> hold) {
	if (!hold) {
		return;
	}
	ReleaseHold(hold);
}

[[nodiscard]] QString ToastText() {
	return Tr(u"LuminaUndoSendBulletin"_q);
}

// An empty result means no toast could be shown, and then no send may be held:
// a hold the user can neither see nor undo is only an invisible delay.
[[nodiscard]] base::weak_ptr<Ui::Toast::Instance> ShowUndoToast(
		uint64 generation,
		not_null<QWidget*> parent) {
	const auto buttonText = Tr(u"LuminaUndoSendUndo"_q);

	// The toast has to reserve room on its right for a button it does not
	// know about. The formula is the one HistoryView::TranslateBar uses for
	// its own undo toast: historyPremiumViewSet has a negative style width,
	// which a RoundButton reads as "text width plus this much", so
	// subtracting it is how the reserved space ends up matching the button.
	const auto st = std::make_shared<style::Toast>(st::historyPremiumToast);
	st->padding.setRight(
		st::historyPremiumViewSet.style.font->width(buttonText)
			- st::historyPremiumViewSet.width);

	const auto weak = Ui::Toast::Show(parent, Ui::Toast::Config{
		.text = { .text = ToastText() },
		.st = st.get(),
		.attach = RectPart::Bottom,
		.acceptinput = true,
		.duration = kWindow,
	});
	const auto strong = weak.get();
	if (!strong) {
		return {};
	}

	const auto widget = strong->widget();
	widget->lifetime().add([st] {});

	const auto button = Ui::CreateChild<Ui::RoundButton>(
		widget.get(),
		rpl::single(buttonText),
		st::historyPremiumViewSet);
	button->show();
	rpl::combine(
		widget->sizeValue(),
		button->sizeValue()
	) | rpl::on_next([=](QSize outer, QSize inner) {
		button->moveToRight(
			0,
			(outer.height() - inner.height()) / 2,
			outer.width());
	}, widget->lifetime());

	button->setClickedCallback([=] {
		DropHold(TakeHold(generation));
	});
	return weak;
}

// Leaving the chat sends the held message rather than keeping it waiting in a
// chat the user is no longer looking at.
//
// It is not a correctness fix for the message itself - Intercept() has already
// made sure the composer survives, so the five second timer would send it
// anyway - but a hold must not outlive the chat it belongs to on screen.
// Dispatch stays synchronous so that the send and the draft cleanup that
// DispatchHold() performs happen before anything else can write a new draft
// for the chat being left.
//
// Dialogs::Key::history() is null for a forum topic or a saved sublist, so any
// move at all - to another chat, into a topic, to the chat list - ends the
// hold.
//
// This is a shortcut, not the guarantee. activeChatChanges() does NOT fire on
// every move away from the chat: MainWidget::showNewSection() only announces
// the new entry when the incoming section has one (mainwidget.cpp:1941), so
// opening Settings or a profile page leaves the hold running with the composer
// already off this chat. That case is handled by the timer instead, and the
// draft residue by DispatchHold() rather than here.
//
// DispatchHold() consumes the Hold and destroys the lifetime that owns this
// very subscription. rpl copies a handler before invoking it
// (rpl/consumer.h put_next), so the call is safe - but nothing may follow it.
void WatchActiveChat(
		not_null<Hold*> hold,
		not_null<Window::SessionController*> controller,
		not_null<History*> history) {
	const auto generation = hold->generation;
	const auto watched = history.get();
	controller->activeChatChanges(
	) | rpl::on_next([=](Dialogs::Key key) {
		if (key.history() != watched) {
			DispatchHold(TakeHold(generation));
		}
	}, hold->lifetime);
}

// The window whose composer this send came from, or null if none can be shown
// to have produced it.
//
// THE OBVIOUS CALL IS THE WRONG ONE. Session::tryResolveWindow(peer) prefers a
// window DEDICATED to that peer over the window that is asking
// (main_session.cpp:590-601, the `windowId().thread->peer() == forPeer` early
// return). With chat X open both in the main window and in a window of its
// own, a send typed in the main window resolves to the other window, and then
// three things are wrong at once: the Undo toast is shown on a window the user
// may not even be looking at - which is exactly the "invisible five second
// delay" ShowUndoToast()'s contract forbids - the activeChatChanges() shortcut
// watches a window that will never move, and DispatchHold() asks the wrong
// composer whether it has left the chat and so skips the draft cleanup.
//
// The window the user pressed Enter in is the active one, so that is what is
// looked for first. Requiring its HistoryWidget to be showing this chat is the
// same test the fallback makes, and it is what keeps section composers out:
// MainWidget::peer() is HistoryWidget's peer and is null whenever a forum
// topic, a discussion thread, a saved sublist or the scheduled view owns the
// composer instead. See "WHERE IT IS AVAILABLE" in lumina_undo_send.h for why
// that restriction exists at all.
//
// When no window is active - which should not happen for a keystroke-driven
// send, but is not worth crashing the feature over - this falls back to
// exactly the pair of checks this file made before, so it can never accept a
// send the previous version rejected.
[[nodiscard]] Window::SessionController *ResolveComposerWindow(
		not_null<History*> history) {
	const auto peer = history->peer.get();
	const auto shows = [&](Window::SessionController *window) {
		if (!window) {
			return false;
		}
		// The nullable sessionContent() rather than the not_null content():
		// the latter asserts while a window is being torn down.
		const auto content = window->widget()->sessionContent();
		return content && (content->peer() == peer);
	};
	for (const auto &window : history->session().windows()) {
		if (window->widget()->isActiveWindow() && shows(window)) {
			return window.get();
		}
	}
	const auto resolved = history->session().tryResolveWindow(peer);
	return shows(resolved) ? resolved : nullptr;
}

void ArmHold(
		not_null<History*> history,
		const QString &text,
		Fn<void()> proceed,
		not_null<Window::SessionController*> controller,
		uint64 generation,
		base::weak_ptr<Ui::Toast::Instance> toast) {
	auto hold = std::make_unique<Hold>();
	hold->history = base::make_weak(history);
	hold->controller = base::make_weak(controller);
	hold->text = text;
	hold->proceed = std::move(proceed);
	hold->toast = toast;
	hold->generation = generation;
	hold->timer.setCallback([=] {
		// Never dispatch from inside the timer's own callback: DispatchHold()
		// destroys the Hold, and with it the base::Timer that is running.
		crl::on_main([=] { DispatchHold(TakeHold(generation)); });
	});
	hold->timer.callOnce(kWindow);

	WatchActiveChat(hold.get(), controller, history);

	Current() = std::move(hold);
}

bool Intercept(
		not_null<History*> history,
		TextWithTags &text,
		const Api::SendOptions &options,
		Fn<void()> proceed) {
	if (!UndoSendWindow()) {
		return true;
	}

	// Scheduled sends, business quick-reply shortcuts and suggested posts all
	// carry their own confirmation step and none of them is "the message just
	// left" - there is nothing for a five second window to protect. Android
	// excludes schedule mode for the same reason. Edits, forwards, media and
	// voice never reach this seam at all: they are not text sends.
	if (options.scheduled || options.shortcutId || options.suggest) {
		return true;
	}

	// An empty text is the composer marking the chat as read, not a message.
	if (text.text.trimmed().isEmpty()) {
		return true;
	}

	// THE CONSTRAINT THAT DECIDES WHERE THIS FEATURE IS AVAILABLE AT ALL.
	//
	// `proceed` is crl::guard()ed on the composer that produced it, so a hold
	// outliving its composer does not send late - it does not send at all, and
	// the message is gone without a trace. lumina_send_pipeline.h says as much
	// and tells interceptors to hold a section send "only for as long as the
	// section is on screen", which is not something a five second timer can
	// promise: MainWidget::showNewSection() destroys the old section *before*
	// it announces the new active chat, so there is no signal to flush on.
	//
	// HistoryWidget is the one composer that is reused across chats instead of
	// being destroyed, and MainWidget::peer() is exactly "the chat
	// HistoryWidget is currently showing" - it is null whenever a section
	// widget (forum topic, discussion thread, saved sublist, scheduled) owns
	// the composer instead. So a hold is armed only when this send provably
	// came from a HistoryWidget showing this chat, and every other composer
	// sends unchanged. That is a deliberate reduction against Android, which
	// holds everywhere: losing a message is far worse than not offering to
	// undo one.
	//
	// A null answer also covers "no window at all", and then there is nowhere
	// to put a toast - a hold the user can neither see nor undo is only an
	// invisible five second delay. Either way: fail open and send now.
	const auto controller = ResolveComposerWindow(history);
	if (!controller) {
		return true;
	}

	// The toast goes up before anything else is touched: everything below this
	// point changes state, and failing after that would leave a previous hold
	// already dispatched behind a message that went out ahead of it.
	const auto generation = NextGeneration();
	auto toast = ShowUndoToast(
		generation,
		controller->uiShow()->toastParent());
	if (!toast) {
		return true;
	}

	// One hold at a time, as on Android. What to do with the previous one is
	// decided by the text, because the composer field was never cleared:
	//
	//  * Identical text in the same chat is the same message sent twice from a
	//    field that still shows it - the "did that even send?" second press.
	//    Dispatching the first would post two copies, so the first is dropped
	//    and this one replaces it. Nothing is lost: the message this hold
	//    carries is the one the user is looking at.
	//  * Anything else is a genuinely different message - another chat, or a
	//    composer the user edited before pressing send again - and the
	//    previous one is dispatched rather than dropped, so that it cannot be
	//    lost and so that the two keep their order. The cost is that editing a
	//    held message and sending again posts both; the alternative costs a
	//    message, which is worse.
	if (auto previous = base::take(Current())) {
		if (previous->history.get() == history.get()
			&& previous->text == text.text) {
			DropHold(std::move(previous));
		} else {
			// Deferred: dispatching here would re-enter a composer that is
			// still inside its own send call, and clear its field from under
			// it. The new hold is armed first either way, so the order the two
			// messages go out in is unchanged.
			const auto shared = std::make_shared<std::unique_ptr<Hold>>(
				std::move(previous));
			crl::on_main([=] { DispatchHold(std::move(*shared)); });
		}
	}
	ArmHold(
		history,
		text.text,
		std::move(proceed),
		controller,
		generation,
		toast);
	return false;
}

} // namespace

bool UndoSendWindow() {
	return Settings::Instance().getBool(kEnabledKey);
}

void SetUndoSendWindow(bool value) {
	Settings::Instance().set(kEnabledKey, value, Store::Prefs);
}

rpl::producer<> UndoSendWindowChanges() {
	return Settings::Instance().changesFor(kEnabledKey);
}

int UndoSendWindowSeconds() {
	return int(kWindow / 1000);
}

void SetupUndoSendPipeline() {
	static auto registered = false;
	if (registered) {
		return;
	}
	registered = true;
	RegisterSendInterceptor([](
			not_null<History*> history,
			TextWithTags &text,
			Api::SendOptions options,
			Fn<void()> proceed) {
		return Intercept(history, text, options, std::move(proceed));
	});
}

void FlushUndoSend() {
	DispatchHold(base::take(Current()));
}

} // namespace Lumina
