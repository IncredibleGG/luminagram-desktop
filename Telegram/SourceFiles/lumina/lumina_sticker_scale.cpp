/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_sticker_scale.h"

#include "lumina/lumina_settings.h"
#include "ui/style/style_core_scale.h"

#include <QtCore/QJsonValue>

#include <algorithm>
#include <optional>

namespace Lumina {
namespace {

const auto kKeyStickerScale = u"stickerScale"_q;

// Empty until the first sticker is measured. "Nothing has been laid out yet"
// and "everything has been laid out with N" are different states: in the first
// one a new percentage still applies to the whole run on its own, so asking for
// a restart would be asking for nothing.
[[nodiscard]] std::optional<int> &AppliedValue() {
	static auto result = std::optional<int>();
	return result;
}

} // namespace

int StickerScale() {
	return std::clamp(
		Settings::Instance().getInt(kKeyStickerScale, kStickerScaleDefault),
		kStickerScaleMin,
		kStickerScaleMax);
}

void SetStickerScale(int percent) {
	Settings::Instance().set(
		kKeyStickerScale,
		std::clamp(percent, kStickerScaleMin, kStickerScaleMax),
		Store::Prefs);
}

rpl::producer<int> StickerScaleValue() {
	return rpl::single(
		rpl::empty
	) | rpl::then(
		StickerScaleChanges()
	) | rpl::map([] {
		return StickerScale();
	});
}

rpl::producer<> StickerScaleChanges() {
	return Settings::Instance().changesFor(kKeyStickerScale);
}

int AppliedStickerScale() {
	auto &applied = AppliedValue();
	if (!applied) {
		applied = StickerScale();
	}
	return *applied;
}

bool StickerScaleNeedsRestart() {
	const auto &applied = AppliedValue();
	return applied.has_value() && (StickerScale() != *applied);
}

int ScaleStickerLength(int length) {
	const auto percent = AppliedStickerScale();
	return (percent == kStickerScaleDefault)
		? length
		: style::ConvertScale(length, percent);
}

} // namespace Lumina
