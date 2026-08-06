/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_number_format.h"

#include "lang/lang_tag.h"
#include "lumina/lumina_settings.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QJsonValue>
#include <QtCore/QThread>

namespace Lumina {
namespace {

const auto kKeyDisableNumberRounding = u"disableNumberRounding"_q;

int &RoundedDepth() {
	static thread_local auto result = 0;
	return result;
}

class RoundedScope final {
public:
	RoundedScope() {
		++RoundedDepth();
	}
	~RoundedScope() {
		--RoundedDepth();
	}

	RoundedScope(const RoundedScope &other) = delete;
	RoundedScope &operator=(const RoundedScope &other) = delete;

};

} // namespace

bool ExactNumbers() {
	return Settings::Instance().getBool(kKeyDisableNumberRounding, false);
}

void SetExactNumbers(bool value) {
	Settings::Instance().set(kKeyDisableNumberRounding, value, Store::Prefs);
}

rpl::producer<bool> ExactNumbersValue() {
	return rpl::single(
		rpl::empty
	) | rpl::then(
		ExactNumbersChanges()
	) | rpl::map([] {
		return ExactNumbers();
	});
}

rpl::producer<> ExactNumbersChanges() {
	return Settings::Instance().changesFor(kKeyDisableNumberRounding);
}

bool ExactNumbersActive() {
	if (RoundedDepth() > 0) {
		return false;
	}

	// Lang::FormatCountToShort() is a general-purpose formatter in lang/, not
	// a widget's paint handler, so this is reachable from more contexts than
	// the rest of this fork's preference reads are. Lumina::Settings is main
	// thread only and its save timer is a QObject, which makes a read from a
	// worker thread, or from anything running before the application object
	// exists, a crash rather than a wrong number. Both answer with today's
	// shortened output - the behaviour-neutral fallback, and the same posture
	// as the try/catch Android wraps the identical read in.
	const auto application = QCoreApplication::instance();
	if (!application || QThread::currentThread() != application->thread()) {
		return false;
	}
	return ExactNumbers();
}

QString FormatCountRounded(int64 number, bool onlyK) {
	const auto guard = RoundedScope();
	return Lang::FormatCountToShort(number, onlyK).string;
}

} // namespace Lumina
