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

// macOS on-device Apple Speech engine (id "apple"). Declared unconditionally;
// lumina_transcriber_apple.mm provides the real implementation on Apple and a
// nullptr stub everywhere else, so this links on any OS.
[[nodiscard]] std::unique_ptr<TranscribeEngine> MakeAppleSpeechEngine();

} // namespace Lumina
