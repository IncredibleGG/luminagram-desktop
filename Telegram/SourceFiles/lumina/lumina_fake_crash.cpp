/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_fake_crash.h"

#include "lumina/lumina_settings.h"

#include <cstdlib>

namespace Lumina {
namespace {

// Same key names as Android's LuminaSecurityActivity, so the two platforms'
// stores can be read against each other.
const auto kKeyEnabled = u"fakeCrashEnabled"_q;
const auto kKeyCode = u"fakeCrashCode"_q;

} // namespace

bool FakeCrashEnabled() {
	return Settings::Instance().getBool(kKeyEnabled, false);
}

void SetFakeCrashEnabled(bool value) {
	Settings::Instance().set(kKeyEnabled, value, Store::Private);
}

QString FakeCrashCode() {
	return Settings::Instance().getString(kKeyCode).trimmed();
}

void SetFakeCrashCode(const QString &code) {
	const auto trimmed = code.trimmed();
	if (trimmed.isEmpty()) {
		// Not set(key, {}, Store::Private): an empty string would keep the key
		// in the private file, and "the duress code is the empty string" is a
		// state FakeCrashCodeMatches() would have to special-case forever.
		Settings::Instance().remove(kKeyCode);
	} else {
		Settings::Instance().set(kKeyCode, trimmed, Store::Private);
	}
}

bool FakeCrashArmed() {
	return FakeCrashEnabled() && !FakeCrashCode().isEmpty();
}

bool FakeCrashCodeMatches(const QString &entered) {
	// Both sides trimmed, because the stored code is trimmed when it is saved
	// and the lock screen field is masked. A code that silently fails to fire
	// because a space rode along with it fails in the one moment the feature
	// exists for, in front of the person who is watching; and trimming cannot
	// make it fire in place of a real unlock, because by the time this runs
	// the passcode has already been checked and rejected.
	const auto trimmed = entered.trimmed();
	if (trimmed.isEmpty() || !FakeCrashEnabled()) {
		return false;
	}
	const auto code = FakeCrashCode();
	return !code.isEmpty() && (trimmed == code);
}

[[noreturn]] void TriggerFakeCrash() {
	// std::_Exit, and nothing before it. Three things this deliberately does
	// NOT do, each of which was considered and rejected:
	//
	//  * Core::Quit(). It is vetoable. Core::Application::preventsQuit() pops
	//    a confirmation box when an export, an upload or a download is in
	//    flight (core/application.cpp:1068-1121) and then returns without
	//    quitting, and readyToQuit() can defer the exit by three seconds. A
	//    duress control that answers "are you sure you want to stop this
	//    download?" in front of the person holding the phone is worse than no
	//    duress control at all.
	//
	//  * std::abort() or any other real fault. It would be caught by the crash
	//    handler, which writes a genuine dump; the NEXT launch would then open
	//    LastCrashedWindow (core/sandbox.cpp:451-476) offering to send a crash
	//    report, and on macOS the system would show its own "quit
	//    unexpectedly" dialog naming the app. Loud, and it would transmit a
	//    report about something the user did on purpose.
	//
	//  * a graceful shutdown. Saving state takes visible time and leaves the
	//    window on screen while it happens, which is exactly what a crash does
	//    not look like.
	//
	// _Exit() leaves the empty tdata/working marker behind, so the next launch
	// logs "the previous launch was not finished properly" and starts
	// normally - the sandbox handles an empty dump explicitly. That is the
	// same trace a real crash of this kind leaves, and no window.
	//
	// The cost is the same as any crash: anything the app had not flushed is
	// lost. tdesktop writes through temp-file-and-rename and survives this by
	// design; under duress, an exit that cannot be talked out of is worth far
	// more than a tidy one. Nothing is written here on purpose - not the
	// pref store, not a log line. A log line saying a duress code was entered
	// would hand the whole feature to anyone who later reads the log.
	std::_Exit(0);
}

rpl::producer<> FakeCrashChanges() {
	return Settings::Instance().changes(
	) | rpl::filter([](const QString &key) {
		return (key == kKeyEnabled) || (key == kKeyCode);
	}) | rpl::to_empty;
}

} // namespace Lumina
