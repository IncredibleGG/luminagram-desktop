/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "lumina/lumina_vault.h"

namespace Lumina {

// The notepad decoy, and the default skin: a working single-note notepad.
// Whatever is typed is kept in Lumina::SetDecoyNotepadContent(), so the note
// looks lived-in across launches instead of like a prop that resets.
//
// The unlock is deliberately invisible. Android long-presses the title bar;
// desktop has no long press, so the equivalent here is a double click on the
// title, which does nothing at all unless the note body, trimmed, is exactly
// the secret code. Nothing on screen hints that any of this is a lock, and a
// double click with the wrong body is indistinguishable from a misclick.
// Closing the window returns VaultOutcome::Closed.
//
// Runs a nested QEventLoop and returns only once the user has decided. See
// lumina/lumina_vault.h for why nothing in here may touch st::, tr:: or Ui::.
[[nodiscard]] VaultOutcome RunVaultNotepad();

} // namespace Lumina
