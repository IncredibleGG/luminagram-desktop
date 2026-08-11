/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_session_guard.h"

#include "api/api_authorizations.h"
#include "apiwrap.h"
#include "base/flat_map.h"
#include "base/flat_set.h"
#include "base/timer.h"
#include "base/unixtime.h"
#include "base/weak_ptr.h"
#include "core/application.h"
#include "lang/lang_keys.h"
#include "lumina/lumina_locale.h"
#include "lumina/lumina_settings.h"
#include "main/main_session.h"
#include "settings/cloud_password/settings_cloud_password_start.h"
#include "settings/settings_common.h"
#include "ui/boxes/confirm_box.h"
#include "ui/layers/generic_box.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_controller.h"
#include "window/window_session_controller.h"

#include "styles/style_layers.h"
#include "styles/style_settings.h"

#include <QtCore/QDateTime>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>

namespace Lumina {
namespace {

const auto kKeyEnabled = u"sessionGuardEnabled"_q;
const auto kKeyKnown = u"sessionGuardKnown"_q;
const auto kKeyLastCheck = u"sessionGuardLastCheck"_q;

// Android's LuminaSessionGuard.MIN_INTERVAL_MS. Wall-clock milliseconds, not
// crl::time: it is compared against a value that outlives the process.
constexpr auto kMinIntervalMs = qint64(30 * 60 * 1000);

// Android's FOREGROUND_DELAY_MS - let the window, and any passcode screen,
// settle before a box can appear.
constexpr auto kForegroundDelay = crl::time(4000);

// Api::Authorizations::reload() reports failure by doing nothing at all (it
// only clears its request id), so the only way to notice one is to stop
// waiting. Generous, because this is never on a user-visible critical path.
constexpr auto kRequestTimeout = crl::time(30 * 1000);

// Android's MAX_SHOW_RETRIES / 3s spacing: no window is in front yet, or the
// passcode screen is up. After that we give up quietly - the session is left
// unknown, so the next check asks again.
constexpr auto kShowRetryDelay = crl::time(3000);
constexpr auto kMaxShowRetries = 5;

[[nodiscard]] qint64 NowMs() {
	return QDateTime::currentMSecsSinceEpoch();
}

[[nodiscard]] QString AccountKey(not_null<Main::Session*> session) {
	return QString::number(session->userId().bare);
}

[[nodiscard]] bool HasKnownSessions(const QString &account) {
	return Lumina::Settings::Instance().getObject(kKeyKnown).contains(account);
}

[[nodiscard]] base::flat_set<QString> KnownSessions(const QString &account) {
	auto result = base::flat_set<QString>();
	const auto array = Lumina::Settings::Instance().getObject(
		kKeyKnown
	).value(account).toArray();
	for (const auto &value : array) {
		const auto hash = value.toString();
		if (!hash.isEmpty()) {
			result.emplace(hash);
		}
	}
	return result;
}

// Store::Private is this key's store, and the header of lumina_settings.h is
// explicit that every write of such a key must name it - a bare set() would
// relocate the whole list into the plain pref file.
void SetKnownSessions(
		const QString &account,
		const base::flat_set<QString> &hashes) {
	auto array = QJsonArray();
	for (const auto &hash : hashes) {
		array.append(hash);
	}
	auto all = Lumina::Settings::Instance().getObject(kKeyKnown);
	all.insert(account, array);
	Lumina::Settings::Instance().set(kKeyKnown, all, Lumina::Store::Private);
}

[[nodiscard]] qint64 LastCheck(const QString &account) {
	// Stored as text: Lumina::Settings::getInt() is 32-bit and a millisecond
	// epoch is not.
	const auto text = Lumina::Settings::Instance().getObject(
		kKeyLastCheck
	).value(account).toString();
	auto ok = false;
	const auto value = text.toLongLong(&ok);
	return (ok && value > 0) ? value : qint64(0);
}

void SetLastCheck(const QString &account, qint64 when) {
	auto all = Lumina::Settings::Instance().getObject(kKeyLastCheck);
	all.insert(account, QString::number(when));
	Lumina::Settings::Instance().set(
		kKeyLastCheck,
		all,
		Lumina::Store::Private);
}

[[nodiscard]] bool ThrottleAllows(const QString &account) {
	const auto now = NowMs();
	auto last = LastCheck(account);
	if (last > now) {
		// The clock moved backwards. Do not lock the guard out forever.
		last = 0;
	}
	return (last <= 0) || ((now - last) >= kMinIntervalMs);
}

void AppendRow(QString &to, const QString &label, const QString &value) {
	if (value.trimmed().isEmpty()) {
		return;
	}
	to.append(u"\n"_q).append(label).append(u": "_q).append(value.trimmed());
}

[[nodiscard]] QString DeviceLine(const Api::Authorizations::Entry &entry) {
	auto os = entry.platform.trimmed();
	if (!entry.system.trimmed().isEmpty()) {
		if (!os.isEmpty()) {
			os.append(' ');
		}
		os.append(entry.system.trimmed());
	}
	auto result = entry.name.trimmed();
	if (!os.isEmpty()) {
		if (!result.isEmpty()) {
			result.append(u" · "_q);
		}
		result.append(os);
	}
	return result;
}

[[nodiscard]] QString DateLine(const Api::Authorizations::Entry &entry) {
	if (!entry.activeTime) {
		return QString();
	}
	// Api::Authorizations::Entry does not keep date_created separately - it
	// already collapsed date_active ?: date_created into activeTime - so this
	// is the last-active time where Android prefers the creation time. For a
	// session nobody has used since it appeared the two are the same, which is
	// the case the alert exists for.
	return langDateTime(base::unixtime::parse(entry.activeTime));
}

[[nodiscard]] QString BuildMessage(const Api::Authorizations::Entry &entry) {
	auto result = Tr(u"LuminaSessionAlertIntro"_q);
	result.append(u"\n"_q);
	AppendRow(result, Tr(u"LuminaSessionAlertDevice"_q), DeviceLine(entry));
	AppendRow(result, Tr(u"LuminaSessionAlertApp"_q), entry.info);
	AppendRow(result, Tr(u"LuminaSessionAlertIp"_q), entry.ip);
	AppendRow(result, Tr(u"LuminaSessionAlertLocation"_q), entry.location);
	AppendRow(result, Tr(u"LuminaSessionAlertTime"_q), DateLine(entry));
	return result.trimmed();
}

// One guard per account, not per window: two windows on the same account must
// not each open an alert for the same login.
class Guard final : public base::has_weak_ptr {
public:
	explicit Guard(not_null<Main::Session*> session);

	void check(bool manual, Fn<void(SessionGuardOutcome)> done);

	[[nodiscard]] rpl::producer<bool> runningValue() const {
		return _running.value();
	}

private:
	void onForeground();
	void onTimeout();
	void handle();
	void finish(SessionGuardOutcome outcome);

	void showNext();
	void advance();
	void remember(uint64 hash);
	void forget(uint64 hash);
	void terminate(uint64 hash);
	void terminateFailed();
	void twoStepTip();

	[[nodiscard]] Window::SessionController *presenter() const;

	const not_null<Main::Session*> _session;
	const QString _account;

	base::Timer _foregroundTimer;
	base::Timer _timeoutTimer;
	base::Timer _retryTimer;

	rpl::lifetime _waiting;
	rpl::lifetime _lifetime;

	Fn<void(SessionGuardOutcome)> _done;
	std::vector<Api::Authorizations::Entry> _queue;
	crl::time _receivedBefore = 0;
	int _index = 0;
	int _retriesLeft = kMaxShowRetries;

	// A variable rather than a bool so the settings row can follow it. Every
	// read below is _running.current(); the assignments are unchanged.
	rpl::variable<bool> _running = false;

};

Guard::Guard(not_null<Main::Session*> session)
: _session(session)
, _account(AccountKey(session))
, _foregroundTimer([=] {
	if (SessionGuardEnabled() && ThrottleAllows(_account)) {
		check(false, nullptr);
	}
})
, _timeoutTimer([=] { onTimeout(); })
, _retryTimer([=] { showNext(); }) {
	// appDeactivatedValue() opens with the CURRENT application state, so
	// installing this while the window is in front counts as a foreground
	// event - which is what Android gets from LaunchActivity.onResume firing
	// at start-up as well as on every return to the app.
	Core::App().appDeactivatedValue(
	) | rpl::on_next([=](bool deactivated) {
		if (!deactivated) {
			onForeground();
		}
	}, _lifetime);
}

void Guard::onForeground() {
	try {
		if (!SessionGuardEnabled()
			|| _running.current()
			|| _foregroundTimer.isActive()
			|| !ThrottleAllows(_account)) {
			return;
		}
		_foregroundTimer.callOnce(kForegroundDelay);
	} catch (...) {
	}
}

void Guard::check(bool manual, Fn<void(SessionGuardOutcome)> done) {
	try {
		// A manual run deliberately ignores the preference AND the throttle:
		// the user just asked for it. An automatic one re-tests the
		// preference, which may have been turned off while the delay ran.
		if (_running.current() || (!manual && !SessionGuardEnabled())) {
			// A run that is already in flight is its own answer: reporting
			// "nothing new" for it, as this used to, told the user their
			// devices had been checked when nothing had been checked yet -
			// and the real answer was seconds away and about to contradict
			// it. The preference being off still reports "nothing new",
			// which is what Android does rather than call it an error.
			if (done) {
				done(_running.current()
					? SessionGuardOutcome::Busy
					: SessionGuardOutcome::NoNew);
			}
			return;
		}
		_running = true;
		_done = std::move(done);
		_foregroundTimer.cancel();

		auto &authorizations = _session->api().authorizations();
		_receivedBefore = authorizations.lastReceivedTime();

		// reload() reports success only by refreshing the list, and it also
		// early-returns while a request of its own is already in flight, so
		// waiting on the list is the only correct signal in both cases.
		// _listChanges also fires for a termination and for a device-model
		// change, which is why the arrival timestamp is what is tested.
		_waiting.destroy();
		authorizations.listValue(
		) | rpl::filter([=](const Api::Authorizations::List &) {
			return (_session->api().authorizations().lastReceivedTime()
				!= _receivedBefore);
		}) | rpl::take(1) | rpl::on_next([=](
				const Api::Authorizations::List &) {
			_timeoutTimer.cancel();
			handle();
		}, _waiting);

		_timeoutTimer.callOnce(kRequestTimeout);
		authorizations.reload();
	} catch (...) {
		finish(SessionGuardOutcome::Failed);
	}
}

void Guard::onTimeout() {
	try {
		_waiting.destroy();
		// Remember the attempt even though it failed: a machine that is
		// offline for a week must not retry on every single activation.
		SetLastCheck(_account, NowMs());
	} catch (...) {
	}
	finish(SessionGuardOutcome::Failed);
}

void Guard::handle() {
	auto outcome = SessionGuardOutcome::Failed;
	auto show = false;
	try {
		SetLastCheck(_account, NowMs());

		const auto list = _session->api().authorizations().list();
		const auto firstRun = !HasKnownSessions(_account);
		auto present = base::flat_set<QString>();
		auto fresh = std::vector<Api::Authorizations::Entry>();
		const auto known = KnownSessions(_account);
		for (const auto &entry : list) {
			// hash == 0 is this very client (ParseEntry zeroes it for the
			// current authorization), which is the only entry Android skips
			// too - a login that is still waiting for the 2FA password is
			// reported, because that is an attempt in progress.
			if (!entry.hash) {
				continue;
			}
			const auto hash = QString::number(entry.hash);
			present.emplace(hash);
			if (!known.contains(hash)) {
				fresh.push_back(entry);
			}
		}

		if (firstRun) {
			// Baseline only. Whatever the account already has is known, so a
			// fresh profile never opens a wall of alerts.
			SetKnownSessions(_account, present);
			finish(SessionGuardOutcome::NoNew);
			return;
		}

		// Drop hashes the server no longer reports, so the stored list cannot
		// grow without bound.
		auto retained = base::flat_set<QString>();
		for (const auto &hash : known) {
			if (present.contains(hash)) {
				retained.emplace(hash);
			}
		}
		SetKnownSessions(_account, retained);

		if (fresh.empty()) {
			finish(SessionGuardOutcome::NoNew);
			return;
		}
		_queue = std::move(fresh);
		_index = 0;
		_retriesLeft = kMaxShowRetries;
		outcome = SessionGuardOutcome::NewLogins;
		show = true;
	} catch (...) {
		outcome = SessionGuardOutcome::Failed;
		show = false;
	}
	finish(outcome);
	if (show) {
		showNext();
	}
}

void Guard::finish(SessionGuardOutcome outcome) {
	_running = false;
	_timeoutTimer.cancel();
	if (const auto done = base::take(_done)) {
		try {
			done(outcome);
		} catch (...) {
		}
	}
}

Window::SessionController *Guard::presenter() const {
	// An alert here while the passcode screen is up would print the intruder's
	// IP and device on the lock screen. Android refuses for the same reason.
	if (Core::App().passcodeLocked()) {
		return nullptr;
	}
	const auto window = Core::App().activeWindow();
	if (!window) {
		return nullptr;
	}
	const auto controller = window->sessionController();
	if (!controller || (&controller->session() != _session.get())) {
		return nullptr;
	}
	return controller;
}

void Guard::advance() {
	++_index;
	_retriesLeft = kMaxShowRetries;
	// Never open the next box from inside the closing one.
	crl::on_main(this, [=] { showNext(); });
}

void Guard::showNext() {
	try {
		if (_index < 0 || _index >= int(_queue.size())) {
			_queue.clear();
			_index = 0;
			return;
		}
		const auto controller = presenter();
		if (!controller) {
			if (_retriesLeft > 0) {
				--_retriesLeft;
				_retryTimer.callOnce(kShowRetryDelay);
			} else {
				// Give up quietly. Nothing was remembered, so the next check
				// asks about these sessions again.
				_queue.clear();
				_index = 0;
			}
			return;
		}
		const auto entry = _queue[_index];
		const auto hash = entry.hash;
		const auto answered = std::make_shared<bool>(false);
		const auto weak = base::make_weak(this);
		controller->show(Box([=](not_null<Ui::GenericBox*> box) {
			Ui::ConfirmBox(box, {
				.text = BuildMessage(entry),
				.confirmed = [=](Fn<void()> close) {
					if (*answered) {
						return;
					}
					*answered = true;
					close();
					if (const auto strong = weak.get()) {
						strong->terminate(hash);
					}
				},
				.cancelled = [=](Fn<void()> close) {
					if (*answered) {
						return;
					}
					*answered = true;
					close();
					if (const auto strong = weak.get()) {
						strong->remember(hash);
						strong->advance();
					}
				},
				.confirmText = Tr(u"LuminaSessionAlertNotMe"_q),
				.cancelText = Tr(u"LuminaSessionAlertItsMe"_q),
				.confirmStyle = &st::attentionBoxButton,
				.title = Tr(u"LuminaSessionAlertTitle"_q),
				// Without this, Ui::ConfirmBox routes plain closing through
				// the cancel button, and closing the box is NOT the same
				// answer as pressing "That was me".
				.strictCancel = true,
			});
			box->boxClosing(
			) | rpl::on_next([=] {
				if (*answered) {
					return;
				}
				*answered = true;
				// Dismissed without answering: deliberately not remembered,
				// so an unknown login is asked about again rather than
				// silently trusted.
				if (const auto strong = weak.get()) {
					strong->advance();
				}
			}, box->lifetime());
		}));
	} catch (...) {
		_queue.clear();
		_index = 0;
	}
}

void Guard::remember(uint64 hash) {
	try {
		auto known = KnownSessions(_account);
		known.emplace(QString::number(hash));
		SetKnownSessions(_account, known);
	} catch (...) {
	}
}

void Guard::forget(uint64 hash) {
	try {
		auto known = KnownSessions(_account);
		if (known.remove(QString::number(hash))) {
			SetKnownSessions(_account, known);
		}
	} catch (...) {
	}
}

// The ONLY caller is the "Not me" button above. Nothing in this file ever
// terminates a session on its own.
void Guard::terminate(uint64 hash) {
	try {
		const auto weak = base::make_weak(this);
		_session->api().authorizations().requestTerminate([=](
				const MTPBool &result) {
			const auto strong = weak.get();
			if (!strong) {
				return;
			} else if (mtpIsTrue(result)) {
				strong->forget(hash);
				strong->twoStepTip();
			} else {
				strong->terminateFailed();
			}
		}, [=](const MTP::Error &) {
			if (const auto strong = weak.get()) {
				strong->terminateFailed();
			}
		}, hash);
	} catch (...) {
		terminateFailed();
	}
}

void Guard::terminateFailed() {
	try {
		if (const auto controller = presenter()) {
			controller->show(Ui::MakeInformBox({
				.text = Tr(u"LuminaSessionTerminateFailed"_q),
				.title = Tr(u"LuminaSessionAlertTitle"_q),
			}));
		}
	} catch (...) {
	}
	// Not remembered - the session is still there and still unapproved.
	advance();
}

// Confirms the kill and points at Two-Step Verification, which is what stops
// the same thing happening again.
void Guard::twoStepTip() {
	try {
		const auto controller = presenter();
		if (!controller) {
			advance();
			return;
		}
		const auto answered = std::make_shared<bool>(false);
		const auto weak = base::make_weak(this);
		const auto weakController = base::make_weak(controller);
		controller->show(Box([=](not_null<Ui::GenericBox*> box) {
			Ui::ConfirmBox(box, {
				.text = Tr(u"LuminaSession2FAMessage"_q),
				.confirmed = [=](Fn<void()> close) {
					if (*answered) {
						return;
					}
					*answered = true;
					close();
					if (const auto strong = weakController.get()) {
						strong->showSettings(
							::Settings::CloudPasswordStartId());
					}
					if (const auto strong = weak.get()) {
						strong->advance();
					}
				},
				.confirmText = Tr(u"LuminaSession2FAButton"_q),
				.cancelText = Tr(u"LuminaSessionLater"_q),
				.title = Tr(u"LuminaSessionTerminated"_q),
				.strictCancel = true,
			});
			box->boxClosing(
			) | rpl::on_next([=] {
				if (*answered) {
					return;
				}
				*answered = true;
				if (const auto strong = weak.get()) {
					strong->advance();
				}
			}, box->lifetime());
		}));
	} catch (...) {
		advance();
	}
}

[[nodiscard]] auto Guards()
-> base::flat_map<not_null<Main::Session*>, std::unique_ptr<Guard>> & {
	static auto result
		= base::flat_map<not_null<Main::Session*>, std::unique_ptr<Guard>>();
	return result;
}

[[nodiscard]] Guard *GuardFor(not_null<Main::Session*> session) {
	auto &guards = Guards();
	const auto i = guards.find(session);
	if (i != guards.end()) {
		return i->second.get();
	}
	guards.emplace(session, std::make_unique<Guard>(session));
	session->lifetime().add([=] { Guards().remove(session); });
	const auto j = guards.find(session);
	return (j != guards.end()) ? j->second.get() : nullptr;
}

// What the manual row shows on its right while a check is running. Empty the
// rest of the time, so the row reads as a plain action again once it has
// answered.
[[nodiscard]] rpl::producer<QString> CheckStatusValue(
		not_null<Window::SessionController*> controller) {
	return rpl::combine(
		SessionGuardRunningValue(controller),
		TrValue(u"LuminaSessionGuardChecking"_q)
	) | rpl::map([](bool running, const QString &text) {
		return running ? text : QString();
	});
}

} // namespace

bool SessionGuardEnabled() {
	try {
		// Default true, and the default also lives in Settings::Defaults() so
		// that exportAll()/importAll() round-trip it the same way.
		return Lumina::Settings::Instance().getBool(kKeyEnabled, true);
	} catch (...) {
		return false;
	}
}

void SetSessionGuardEnabled(bool value) {
	try {
		Lumina::Settings::Instance().set(kKeyEnabled, value);
	} catch (...) {
	}
}

rpl::producer<bool> SessionGuardEnabledValue() {
	return rpl::single(
		rpl::empty
	) | rpl::then(
		Lumina::Settings::Instance().changesFor(kKeyEnabled)
	) | rpl::map([] {
		return SessionGuardEnabled();
	});
}

void SetupSessionGuard(not_null<Window::SessionController*> controller) {
	try {
		// GuardFor is [[nodiscard]] - the guard it returns is owned by the
		// per-session map, so setup only needs it constructed, not held.
		[[maybe_unused]] const auto guard = GuardFor(&controller->session());
	} catch (...) {
	}
}

rpl::producer<bool> SessionGuardRunningValue(
		not_null<Window::SessionController*> controller) {
	try {
		if (const auto guard = GuardFor(&controller->session())) {
			return guard->runningValue();
		}
	} catch (...) {
	}
	return rpl::single(false);
}

void SessionGuardCheckNow(
		not_null<Window::SessionController*> controller,
		Fn<void(SessionGuardOutcome)> done) {
	try {
		const auto guard = GuardFor(&controller->session());
		if (!guard) {
			if (done) {
				done(SessionGuardOutcome::Failed);
			}
			return;
		}
		guard->check(true, std::move(done));
	} catch (...) {
		if (done) {
			done(SessionGuardOutcome::Failed);
		}
	}
}

void AddSessionGuardRows(
		not_null<Ui::VerticalLayout*> container,
		not_null<Window::SessionController*> controller) {
	Ui::AddSkip(container);
	Ui::AddSubsectionTitle(
		container,
		TrValue(u"LuminaSessionGuardHeader"_q));

	const auto toggle = container->add(object_ptr<Ui::SettingsButton>(
		container,
		TrValue(u"LuminaSessionGuard"_q),
		st::settingsButtonNoIcon
	))->toggleOn(SessionGuardEnabledValue());
	toggle->toggledChanges(
	) | rpl::on_next([](bool value) {
		SetSessionGuardEnabled(value);
	}, toggle->lifetime());

	// The row says "Checking..." from the moment it is pressed, because the
	// answer is a network round trip away and a request nothing answers takes
	// the full kRequestTimeout to give up. Without it, the press produced no
	// visible change whatsoever for up to 30 seconds, which is the same thing
	// a dead row produces. The text is driven by the guard's own state rather
	// than by this callback, so a check the foreground watcher started is
	// reported here too - and so a press that lands on one of those cannot
	// leave the row stuck saying "Checking..." after it ends.
	const auto weak = base::make_weak(controller);
	const auto check = ::Settings::AddButtonWithLabel(
		container,
		TrValue(u"LuminaSessionGuardCheckNow"_q),
		CheckStatusValue(controller),
		st::settingsButtonNoIcon);
	check->setClickedCallback([=] {
		const auto strong = weak.get();
		if (!strong) {
			return;
		}
		SessionGuardCheckNow(strong, [=](SessionGuardOutcome outcome) {
			const auto again = weak.get();
			if (!again) {
				return;
			}
			switch (outcome) {
			case SessionGuardOutcome::Failed:
				again->showToast(Tr(u"LuminaSessionGuardCheckFailed"_q));
				break;
			case SessionGuardOutcome::NoNew:
				again->showToast(Tr(u"LuminaSessionGuardNoNew"_q));
				break;
			case SessionGuardOutcome::Busy:
				again->showToast(Tr(u"LuminaSessionGuardBusy"_q));
				break;
			case SessionGuardOutcome::NewLogins:
				// Says nothing: the alerts are already on screen.
				break;
			}
		});
	});

	Ui::AddSkip(container);
	Ui::AddDividerText(container, TrValue(u"LuminaSessionGuardInfo"_q));
}

} // namespace Lumina
