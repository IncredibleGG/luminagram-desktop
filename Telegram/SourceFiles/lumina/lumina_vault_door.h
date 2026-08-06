/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "lumina/lumina_vault.h"

namespace Lumina {

// The password door: the front of the vault in VaultMode::PasswordDoor.
//
// A deliberately anonymous password prompt with nothing on it that names
// Telegram or LuminaGram. The right code returns VaultOutcome::Unlock; any
// other entry returns VaultOutcome::Decoy and the caller presents the decoy
// skin, so a wrong code never reveals that there is a real app behind this.
// Closing the window returns VaultOutcome::Closed.
//
// Runs a nested QEventLoop and returns only once the user has decided. See
// lumina/lumina_vault.h for why nothing in here may touch st::, tr:: or Ui::.
[[nodiscard]] VaultOutcome RunVaultDoor();

} // namespace Lumina
