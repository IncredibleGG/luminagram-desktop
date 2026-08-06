/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_link_preview.h"

#include "lumina/lumina_settings.h"

#include <QtCore/QJsonValue>

namespace Lumina {
namespace {

const auto kKeyDisabled = u"disableLinkPreview"_q;

} // namespace

bool LinkPreviewOffByDefault() {
	return Settings::Instance().getBool(kKeyDisabled, false);
}

void SetLinkPreviewOffByDefault(bool value) {
	Settings::Instance().set(kKeyDisabled, value, Store::Prefs);
}

rpl::producer<bool> LinkPreviewOffByDefaultValue() {
	return rpl::single(
		rpl::empty
	) | rpl::then(
		Settings::Instance().changesFor(kKeyDisabled)
	) | rpl::map([] {
		return LinkPreviewOffByDefault();
	});
}

} // namespace Lumina
