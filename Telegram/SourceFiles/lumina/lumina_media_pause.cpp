/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_media_pause.h"

#include "core/application.h"
#include "lumina/lumina_settings.h"

#include <QtCore/QJsonValue>

namespace Lumina {
namespace {

const auto kKeyEnabled = u"autoPauseBgVideo"_q;

} // namespace

bool AutoPauseBackgroundVideo() {
	return Settings::Instance().getBool(kKeyEnabled, false);
}

void SetAutoPauseBackgroundVideo(bool value) {
	Settings::Instance().set(kKeyEnabled, value, Store::Prefs);
}

rpl::producer<bool> AutoPauseBackgroundVideoValue() {
	return rpl::single(
		rpl::empty
	) | rpl::then(
		Settings::Instance().changesFor(kKeyEnabled)
	) | rpl::map([] {
		return AutoPauseBackgroundVideo();
	});
}

bool ShouldPauseBackgroundVideo(ViewerState state) {
	if (state.pictureInPicture || state.stories) {
		return false;
	} else if (!AutoPauseBackgroundVideo()) {
		return false;
	} else if (state.minimized) {
		return true;
	}
	return state.applicationBackgrounded && !state.windowed;
}

rpl::producer<bool> ApplicationBackgroundedValue() {
	return Core::App().appDeactivatedValue();
}

} // namespace Lumina
