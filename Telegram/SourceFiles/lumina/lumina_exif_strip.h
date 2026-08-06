/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include <rpl/producer.h>

#include <QtCore/QByteArray>

namespace Lumina {

// Erase the GPS coordinates a camera wrote into a photo, before that photo
// leaves this device.
//
// WHY DESKTOP NEEDS THIS MORE THAN ANDROID DOES. Android's string for the same
// toggle promises that "compressed photos never include this data". On
// tdesktop that is false, and it was verified in this tree rather than
// assumed: ComputePhotoJpegBytes() (storage/localimageloader.cpp) hands the
// ORIGINAL file bytes to the uploader whenever the photo is small enough not
// to be downscaled, either verbatim (already-progressive JPEG) or through
// Images::MakeProgressiveJpeg(), which re-emits every one of the 16 APP
// markers it saved - APP1/EXIF included (Telegram/lib_ui/ui/image/
// image_prepare.cpp). So a sub-1280px camera photo sent the ordinary,
// compressed way carries its GPS tags all the way to the server. The
// send-as-file path is leakier still: it uploads the file byte for byte.
//
// WHAT IS REMOVED, AND WHAT IS DELIBERATELY NOT. Only the GPSInfo sub-IFD
// (IFD0 tag 0x8825) is neutralised. Dropping the whole APP1 segment would have
// been three lines instead of a parser, and it would also drop Orientation -
// after which every camera photo sent as a file renders sideways for the
// recipient. Camera model, timestamps and the rest of EXIF are left alone;
// this feature is about location, and its row says so.
//
// There is no EXIF library anywhere in this tree, so the parser below is
// hand-rolled. It is bounded on every read, it only ever overwrites bytes with
// zeroes, and it NEVER inserts or removes any - the JPEG keeps its exact byte
// length, which is what makes it safe to run on bytes whose internal offsets
// are already fixed, and means an upload's size never has to be recomputed.
//
// Behaviour neutrality. The preference defaults to false and both call sites
// in the file loader are no-ops while it is false.
//
// KNOWN GAPS, so nobody reads more into the row than it says: JPEG only (a PNG
// or WebP photo is re-encoded from pixels by the loader anyway, which loses
// metadata for free); location carried in XMP or in a vendor MakerNote instead
// of the GPS sub-IFD is not touched; the scan stops at the start of scan data,
// so a second full-size image embedded after it - Multi-Picture Format, which
// several phones write - keeps its own EXIF and its own GPS; videos are not
// touched; and a file too large for Images::Read (64 MB) never reaches this
// code, so it is sent as-is.

enum class StripResult {
	Unchanged, // Parsed fine, nothing to remove (or the feature is off).
	Stripped,  // Location found and zeroed in place.
	Failed,    // Not a JPEG, or malformed - the caller must not trust it.
};

// Failed is a hard contract, not a hint: it also covers "we saw GPS and could
// only remove part of it", and by then the buffer has already been written
// into. A caller that gets Failed must throw the buffer away - re-encode from
// pixels, or fall back to the untouched original - and must never upload it.

// Preference `stripPhotoLocation`, Store::Prefs, default false.
//
// NOT `stripPhotoMetadata`: that key is already pinned to a hard-coded `true`
// in Lumina::Settings::Defaults(), and a default-on preference would change
// what leaves the device before the user has touched anything, which this
// batch is not allowed to do. Main thread only.
[[nodiscard]] bool StripPhotoLocation();
void SetStripPhotoLocation(bool value);
[[nodiscard]] rpl::producer<bool> StripPhotoLocationValue();

// FileLoadTask::process() runs on the TaskQueue worker thread, and
// Lumina::Settings is main-thread only, so the worker reads this mirror
// instead of the store.
//
// RefreshStripPhotoLocationCache() must be called from the main thread. The
// file loader calls it in the FileLoadTask constructor, which always runs on
// the main thread and always before that task is processed, so the mirror
// carries the preference as it stood when the send was started - including a
// value that arrived through Settings::importAll(). Deliberately NOT an rpl
// subscription: a static rpl::lifetime here would be destroyed after the
// Settings singleton it is subscribed to, and would unsubscribe from freed
// memory at shutdown.
void RefreshStripPhotoLocationCache();
[[nodiscard]] bool StripPhotoLocationCached();

// Neutralises the GPS sub-IFD of `jpeg` in place. Ignores the preference:
// this is the parser, and it is the thing worth testing on its own.
[[nodiscard]] StripResult StripJpegLocation(QByteArray &jpeg);

// What the file loader calls. Returns Unchanged without touching `jpeg` when
// the preference is off, so with the default the bytes are not even read.
[[nodiscard]] StripResult StripLocationForUpload(QByteArray &jpeg);

} // namespace Lumina
