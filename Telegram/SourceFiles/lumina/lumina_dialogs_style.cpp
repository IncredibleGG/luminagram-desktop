/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_dialogs_style.h"

#include "lumina/lumina_settings.h"

#include "ui/style/style_core.h"
#include "ui/style/style_core_scale.h"

#include "styles/style_dialogs.h"

#include <rpl/map.h>
#include <rpl/producer.h>
#include <rpl/range.h>
#include <rpl/then.h>

#include <QtCore/QJsonValue>

#include <algorithm>
#include <array>

namespace Lumina {
namespace {

const auto kKeyCompactListRows = u"compactListRows"_q;

enum Index : int {
	kDefaultRow,
	kTaggedRow,
	kForumRow,
	kTaggedForumRow,
	kForumTopicRow,
	kCommunityInfoRow,
	kCommunityInfoTaggedRow,
	kCommunityInfoForumRow,
	kCommunityInfoTaggedForumRow,
	kRowCount,
};

// One compact row family, in UNSCALED px exactly like a .style file - the
// values go through style::ConvertScale() at build time, so they follow the
// interface scale the same way the codegen originals do.
struct Spec {
	int height = 0;
	int paddingTop = 0;
	int paddingBottom = 0;
	int photoSize = 0;
	int nameTop = 0;
	int textTop = 0;
	int tagTop = 0; // 0 keeps the original, which is 0 for untagged families.
};

[[nodiscard]] auto Originals()
-> const std::array<const style::DialogRow*, kRowCount> & {
	static const auto result = std::array<const style::DialogRow*, kRowCount>{
		&st::defaultDialogRow,
		&st::taggedDialogRow,
		&st::forumDialogRow,
		&st::taggedForumDialogRow,
		&st::forumTopicRow,
		&st::communityInfoDialogRow,
		&st::communityInfoTaggedDialogRow,
		&st::communityInfoForumDialogRow,
		&st::communityInfoTaggedForumDialogRow,
	};
	return result;
}

// The communityInfo* families repeat the vertical numbers of the family they
// derive from, and that is load bearing rather than tidy: Dialogs::Row's height
// comes from the base family while Dialogs::CommunityChatsList paints the row
// with the matching communityInfo* one. dialogs.style keeps those two heights
// equal today, and compact has to keep them equal too, or every row in the
// community chats box would be painted taller than the slot it was given.
[[nodiscard]] const std::array<Spec, kRowCount> &Specs() {
	static const auto result = std::array<Spec, kRowCount>{ {
		// height  padT  padB  photo  nameTop  textTop  tagTop
		{     48,    5,    5,    38,       6,      25,       0 },
		{     64,    5,    5,    38,       6,      25,      45 },
		{     68,    5,    5,    38,       6,      25,       0 },
		{     86,    5,    5,    38,       6,      25,      67 },
		{     46,    4,    4,    20,       5,      25,       0 },
		{     48,    5,    5,    38,       6,      25,       0 },
		{     64,    5,    5,    38,       6,      25,      45 },
		{     68,    5,    5,    38,       6,      25,       0 },
		{     86,    5,    5,    38,       6,      25,      67 },
	} };
	return result;
}

struct Cache {
	std::array<style::DialogRow, kRowCount> rows;
	int paletteVersion = -1;
	int scale = 0;
	bool enabled = false;
	bool built = false;
	rpl::lifetime lifetime;
};

// The rows live in a fixed-size array that is only ever overwritten in place,
// never grown or reallocated: Dialogs::InnerWidget keeps a raw
// `const style::DialogRow *_st` into it across a rebuild.
[[nodiscard]] Cache &Data() {
	static auto result = Cache();
	[[maybe_unused]] static const auto init = [] {
		result.enabled = CompactListRows();
		Settings::Instance().changes(
		) | rpl::on_next([](const QString &key) {
			if (key == kKeyCompactListRows) {
				result.enabled = CompactListRows();
				result.built = false;
			}
		}, result.lifetime);
		return true;
	}();
	return result;
}

void Rebuild(Cache &data) {
	const auto &originals = Originals();
	const auto &specs = Specs();
	for (auto i = 0; i != int(kRowCount); ++i) {
		auto row = *originals[i];
		const auto &spec = specs[i];
		const auto top = style::ConvertScale(spec.paddingTop);
		const auto bottom = style::ConvertScale(spec.paddingBottom);
		// The avatar's shrink is rounded down to an even number of physical
		// pixels before it is split between the two sides, so that
		// padding.left() + photoSize + padding.left() stays EXACTLY the stock
		// narrow-column width at every interface scale. Taking half of an odd
		// shrink would lose that pixel, and the scales are 5% apart, so at
		// well over half of them ConvertScale(46) - ConvertScale(38) is odd.
		const auto scaled = style::ConvertScale(spec.photoSize);
		const auto shrink = (row.photoSize - scaled) & ~1;
		const auto photo = row.photoSize - shrink;
		const auto shift = shrink / 2;
		row.padding = style::margins(
			row.padding.left() + shift,
			top,
			row.padding.right(),
			bottom);
		row.photoSize = photo;
		row.height = std::max(
			style::ConvertScale(spec.height),
			top + photo + bottom);
		row.nameTop = style::ConvertScale(spec.nameTop);
		row.textTop = style::ConvertScale(spec.textTop);
		if (spec.tagTop) {
			row.tagTop = style::ConvertScale(spec.tagTop);
		}
		data.rows[i] = row;
	}
	data.paletteVersion = style::PaletteVersion();
	data.scale = style::Scale();
	data.built = true;
}

} // namespace

bool CompactListRows() {
	return Settings::Instance().getBool(kKeyCompactListRows, false);
}

void SetCompactListRows(bool value) {
	Settings::Instance().set(kKeyCompactListRows, value, Store::Prefs);
}

rpl::producer<bool> CompactListRowsValue() {
	// Data() registers the subscriber that invalidates the patched styles, and
	// it has to be registered before any consumer of this producer, because
	// both end up on Settings::changes() and an event_stream fires its
	// consumers in subscription order. A consumer that re-lays out a chat list
	// would otherwise run while DialogRowStyle() still answered with the
	// previous preference.
	[[maybe_unused]] const auto &data = Data();

	return rpl::single(
		rpl::empty
	) | rpl::then(
		Settings::Instance().changesFor(kKeyCompactListRows)
	) | rpl::map([] {
		return CompactListRows();
	});
}

const style::DialogRow &DialogRowStyle(const style::DialogRow &original) {
	auto &data = Data();
	if (!data.enabled) {
		return original;
	}
	const auto &originals = Originals();
	for (auto i = 0; i != int(kRowCount); ++i) {
		if (originals[i] != &original) {
			continue;
		} else if (!data.built
			|| data.paletteVersion != style::PaletteVersion()
			|| data.scale != style::Scale()) {
			Rebuild(data);
		}
		return data.rows[i];
	}
	return original;
}

int DialogRowRightButtonTop(int margin, int buttonHeight) {
	if (!Data().enabled) {
		return margin;
	}
	const auto &row = DialogRowStyle(st::defaultDialogRow);
	return std::min(
		margin,
		row.height - buttonHeight - row.padding.bottom());
}

} // namespace Lumina
