/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include <rpl/producer.h>

#include <QtCore/QString>

namespace Lumina {

// The one offline model LuminaGram's desktop whisper.cpp engine uses: the
// multilingual, q5_1-quantized "base" model, which auto-detects the spoken
// language (exactly what a cross-language chat needs) at ~59 MB. It is hosted
// on the project's own R2 bucket and downloaded once, on demand, from the
// Voice-to-Text settings page. The bytes stay on the machine; no audio is ever
// uploaded by this engine.
//
// This is the desktop counterpart of Android's LuminaVoskModelManager, but a
// single file rather than a zip, so there is no unzip step - only the same
// tmp-then-atomic-rename discipline so a half-finished download can never be
// mistaken for a ready model.

// tdata/luminagram-models/ggml-base-q5_1.bin under the working directory.
[[nodiscard]] QString WhisperModelPath();

// True only when the file exists AND is exactly the expected byte size, so a
// truncated or in-progress download never reads as ready.
[[nodiscard]] bool WhisperModelReady();

// UI state for the single model-status row on the settings page.
struct WhisperModelState {
	enum class Stage {
		Absent,      // Not downloaded (or a failed/partial attempt cleaned up).
		Downloading, // In flight; `progress` is 0..100.
		Ready,       // Present and the right size.
	};
	Stage stage = Stage::Absent;
	int progress = 0;
};

[[nodiscard]] WhisperModelState CurrentWhisperModelState();

// Fires whenever CurrentWhisperModelState() changes (download start, progress,
// finish, delete). Mirrors the TranscriberChanges() shape the settings page
// already reacts to.
[[nodiscard]] rpl::producer<> WhisperModelChanges();

// Starts the download on the main thread via QNetworkAccessManager. A second
// call while one is already in flight is a no-op. Writes to PATH + ".tmp",
// verifies the size on finish, and only then atomically renames into place.
void DownloadWhisperModel();

// Removes the model (and any leftover .tmp), moving the state back to Absent.
void DeleteWhisperModel();

} // namespace Lumina
