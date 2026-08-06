/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_exif_strip.h"

#include "base/flat_set.h"
#include "lumina/lumina_settings.h"

#include <QtCore/QJsonValue>

#include <algorithm>
#include <atomic>
#include <cstring>
#include <optional>

namespace Lumina {
namespace {

const auto kKeyEnabled = u"stripPhotoLocation"_q;

// Images::Read() refuses anything bigger, so nothing larger ever reaches this
// code with its bytes in memory. Repeated here so the parser is self-contained
// and every offset below is known to fit in an int.
constexpr auto kMaxBytes = 64 * 1024 * 1024;

constexpr auto kGpsIfdPointer = 0x8825;
constexpr auto kExifIfdPointer = 0x8769;
constexpr auto kTiffMagic = 0x002A;
constexpr auto kIfdEntrySize = 12;

// An EXIF block has IFD0 and, at most, IFD1 for the embedded thumbnail. The
// caps below are only there so a hostile or corrupt file cannot make the walk
// run long; a real photo never comes close to either.
constexpr auto kMaxIfdChain = 8;
constexpr auto kMaxIfdDepth = 3;

std::atomic<bool> Enabled/* = false*/;

// A TIFF block: every offset stored inside EXIF is relative to the start of
// the TIFF header, which is what `data` points at.
struct TiffBlock {
	char *data = nullptr;
	int size = 0;
	bool bigEndian = false;
};

// `malformed` means one specific thing: we saw GPS, and either we could not
// remove all of it or we might have written over something we do not
// understand. It is NOT set for the ordinary quirks of a walk that only reads
// - a short IFD, an offset that leads nowhere, a chain deeper than we follow -
// because none of those can leave coordinates behind or damage the block.
//
// It turns the whole call into StripResult::Failed, and both call sites in the
// file loader throw the buffer away on Failed: the compressed path re-encodes
// from the decoded QImage, and the send-as-file path leaves _content empty so
// the uploader streams the untouched original off disk. So a buffer this
// function has already written into is never the buffer that gets uploaded.
struct StripState {
	bool stripped = false;
	bool malformed = false;
};

[[nodiscard]] std::optional<int> ReadU16(const TiffBlock &tiff, int at) {
	if (at < 0 || at + 2 > tiff.size) {
		return std::nullopt;
	}
	const auto p = reinterpret_cast<const uchar*>(tiff.data) + at;
	return tiff.bigEndian
		? ((int(p[0]) << 8) | int(p[1]))
		: ((int(p[1]) << 8) | int(p[0]));
}

[[nodiscard]] std::optional<int64> ReadU32(const TiffBlock &tiff, int at) {
	if (at < 0 || at + 4 > tiff.size) {
		return std::nullopt;
	}
	const auto p = reinterpret_cast<const uchar*>(tiff.data) + at;
	return tiff.bigEndian
		? ((int64(p[0]) << 24)
			| (int64(p[1]) << 16)
			| (int64(p[2]) << 8)
			| int64(p[3]))
		: ((int64(p[3]) << 24)
			| (int64(p[2]) << 16)
			| (int64(p[1]) << 8)
			| int64(p[0]));
}

[[nodiscard]] int ValueUnitSize(int type) {
	switch (type) {
	case 1: // BYTE
	case 2: // ASCII
	case 6: // SBYTE
	case 7: // UNDEFINED
		return 1;
	case 3: // SHORT
	case 8: // SSHORT
		return 2;
	case 4: // LONG
	case 9: // SLONG
	case 11: // FLOAT
	case 13: // IFD
		return 4;
	case 5: // RATIONAL
	case 10: // SRATIONAL
	case 12: // DOUBLE
		return 8;
	}
	return 0;
}

void Zero(const TiffBlock &tiff, int at, int64 size) {
	if (size <= 0 || at < 0 || at >= tiff.size) {
		return;
	}
	const auto count = std::min(size, int64(tiff.size - at));
	memset(tiff.data + at, 0, size_t(count));
}

// Empties the sub-IFD at `at`. For every entry: a value too big to sit inside
// the entry lives elsewhere in the TIFF block and is zeroed there first - that
// is where the actual coordinates are, three RATIONALs per axis - then the
// 12-byte entry itself. Finally the entry count and the link to the next IFD.
//
// What is left is a structurally valid IFD holding zero tags, so the pointer
// in the parent IFD can keep pointing straight at it and every reader simply
// finds no GPS. Nothing is inserted or removed, so every other offset in this
// TIFF block - Orientation's included - stays exactly as valid as it was.
//
// EVERY WRITE IS BOUNDED BY A STRUCTURE THAT FITS. A count or a value length
// is a 16- or 32-bit field of the file, so a corrupt one can claim an IFD or a
// value region far larger than the block; clamping such a claim to the end of
// the block and zeroing anyway is how this used to silently wipe IFD0, taking
// Orientation and the camera tags with it while still reporting success. So
// nothing is zeroed unless the whole structure it belongs to fits inside the
// block, and a claim that does not fit marks the strip malformed instead.
void EmptySubIfd(const TiffBlock &tiff, int at, StripState &state) {
	const auto count = ReadU16(tiff, at);
	if (!count) {
		state.malformed = true;
		return;
	} else if (!*count) {
		return;
	}
	const auto entries = int64(*count) * kIfdEntrySize;
	if (int64(at) + 2 + entries + 4 > int64(tiff.size)) {
		state.malformed = true;
		return;
	}
	for (auto i = 0; i != *count; ++i) {
		const auto entry = at + 2 + (i * kIfdEntrySize);
		const auto type = ReadU16(tiff, entry + 2);
		const auto number = ReadU32(tiff, entry + 4);
		const auto unit = type ? ValueUnitSize(*type) : 0;
		const auto bytes = (unit && number) ? (int64(unit) * *number) : 0;
		if (bytes > 4) {
			const auto where = ReadU32(tiff, entry + 8);
			if (!where || !*where || (*where + bytes) > int64(tiff.size)) {
				state.malformed = true;
			} else {
				Zero(tiff, int(*where), bytes);
			}
		}
		Zero(tiff, entry, kIfdEntrySize);
	}
	Zero(tiff, at, 2);
	Zero(tiff, at + 2 + int(entries), 4);
	state.stripped = true;
}

void StripGpsInIfd(
		const TiffBlock &tiff,
		int at,
		int depth,
		base::flat_set<int> &visited,
		StripState &state) {
	if (depth > kMaxIfdDepth || at <= 0 || visited.contains(at)) {
		return;
	}
	visited.emplace(at);
	const auto count = ReadU16(tiff, at);
	if (!count) {
		return;
	}
	for (auto i = 0; i != *count; ++i) {
		const auto entry = at + 2 + (i * kIfdEntrySize);
		if (entry + kIfdEntrySize > tiff.size) {
			break;
		}
		const auto tag = ReadU16(tiff, entry);
		if (!tag || (*tag != kGpsIfdPointer && *tag != kExifIfdPointer)) {
			continue;
		}
		const auto where = ReadU32(tiff, entry + 8);
		const auto inside = where && *where && (*where < int64(tiff.size));
		const auto sub = inside ? int(*where) : 0;
		if (*tag != kGpsIfdPointer) {
			StripGpsInIfd(tiff, sub, depth + 1, visited, state);
		} else if (!sub || visited.contains(sub)) {
			// A GPS pointer we cannot follow, or one aliasing an IFD we
			// have already walked - emptying that would erase all of IFD0.
			state.malformed = true;
		} else {
			visited.emplace(sub);
			EmptySubIfd(tiff, sub, state);
		}
	}
}

void StripGpsInTiff(char *data, int size, StripState &state) {
	if (size < 8) {
		return;
	}
	auto tiff = TiffBlock{ data, size, false };
	const auto p = reinterpret_cast<const uchar*>(data);
	if (p[0] == 'I' && p[1] == 'I') {
		tiff.bigEndian = false;
	} else if (p[0] == 'M' && p[1] == 'M') {
		tiff.bigEndian = true;
	} else {
		return;
	}
	const auto magic = ReadU16(tiff, 2);
	if (!magic || *magic != kTiffMagic) {
		return;
	}
	auto visited = base::flat_set<int>();
	auto next = ReadU32(tiff, 4);
	for (auto i = 0; i != kMaxIfdChain; ++i) {
		if (!next || !*next || *next >= int64(tiff.size)) {
			break;
		}
		const auto at = int(*next);
		if (visited.contains(at)) {
			break;
		}
		StripGpsInIfd(tiff, at, 0, visited, state);
		const auto count = ReadU16(tiff, at);
		if (!count) {
			break;
		}
		next = ReadU32(tiff, at + 2 + (*count * kIfdEntrySize));
	}
}

} // namespace

bool StripPhotoLocation() {
	return Settings::Instance().getBool(kKeyEnabled, false);
}

void SetStripPhotoLocation(bool value) {
	Settings::Instance().set(kKeyEnabled, value, Store::Prefs);
	RefreshStripPhotoLocationCache();
}

rpl::producer<bool> StripPhotoLocationValue() {
	return rpl::single(
		rpl::empty
	) | rpl::then(
		Settings::Instance().changesFor(kKeyEnabled)
	) | rpl::map([] {
		return StripPhotoLocation();
	});
}

void RefreshStripPhotoLocationCache() {
	Enabled.store(StripPhotoLocation(), std::memory_order_relaxed);
}

bool StripPhotoLocationCached() {
	return Enabled.load(std::memory_order_relaxed);
}

StripResult StripJpegLocation(QByteArray &jpeg) {
	if (jpeg.size() < 4 || jpeg.size() > kMaxBytes) {
		return StripResult::Failed;
	}
	const auto size = int(jpeg.size());
	const auto bytes = jpeg.data();
	const auto raw = reinterpret_cast<const uchar*>(bytes);
	if (raw[0] != 0xFF || raw[1] != 0xD8) {
		return StripResult::Failed;
	}
	auto state = StripState();
	const auto finished = [&] {
		return state.malformed
			? StripResult::Failed
			: state.stripped
			? StripResult::Stripped
			: StripResult::Unchanged;
	};
	auto position = 2;
	while (true) {
		if (position + 2 > size || raw[position] != 0xFF) {
			return StripResult::Failed;
		}
		auto marker = int(raw[position + 1]);
		while (marker == 0xFF) {
			++position;
			if (position + 2 > size) {
				return StripResult::Failed;
			}
			marker = int(raw[position + 1]);
		}
		if (marker == 0xD9 || marker == 0xDA) {
			return finished();
		} else if (marker == 0x01
			|| marker == 0xD8
			|| (marker >= 0xD0 && marker <= 0xD7)) {
			position += 2;
			continue;
		} else if (position + 4 > size) {
			return StripResult::Failed;
		}
		const auto length = (int(raw[position + 2]) << 8)
			| int(raw[position + 3]);
		if (length < 2 || position + 2 + length > size) {
			return StripResult::Failed;
		} else if (marker == 0xE1
			&& length >= 16
			&& !memcmp(bytes + position + 4, "Exif\0\0", 6)) {
			StripGpsInTiff(bytes + position + 10, length - 8, state);
		}
		position += 2 + length;
	}
}

StripResult StripLocationForUpload(QByteArray &jpeg) {
	if (jpeg.isEmpty() || !StripPhotoLocationCached()) {
		return StripResult::Unchanged;
	}
	return StripJpegLocation(jpeg);
}

} // namespace Lumina
