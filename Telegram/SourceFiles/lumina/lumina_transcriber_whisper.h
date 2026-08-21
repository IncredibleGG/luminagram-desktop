/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "lumina/lumina_transcribers.h"

#include <memory>

namespace Lumina {

// Offline whisper.cpp on-device engine (id "whispercpp"). Declared
// unconditionally; lumina_transcriber_whisper.cpp provides the real
// implementation when LUMINA_HAVE_WHISPER is defined (Windows + Linux) and a
// nullptr stub otherwise, so this links on any OS - including mac, which never
// builds whisper.cpp and keeps its Apple Speech engine instead.
[[nodiscard]] std::unique_ptr<TranscribeEngine> MakeWhisperCppEngine();

} // namespace Lumina
