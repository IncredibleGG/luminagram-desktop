/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include <QtCore/QByteArray>
#include <QtCore/QString>

namespace Lumina {

// Decodes an in-memory audio / video file (a Telegram voice note is Opus in
// an .ogg container; a round video is mp4) to signed 16-bit little-endian
// MONO PCM at 16000 Hz - the raw shape both the Apple Speech framework and
// Vosk expect.
//
// This is the desktop counterpart of the Android LuminaAudioDecoder. The
// container is detected from the bytes; fileName is advisory only and is used
// for logging. On any failure it sets *ok to false and returns an empty array.
[[nodiscard]] QByteArray DecodeToPcm16Mono16k(
	const QByteArray &content,
	const QString &fileName,
	bool *ok = nullptr);

} // namespace Lumina
