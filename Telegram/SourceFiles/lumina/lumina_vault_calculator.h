/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "lumina/lumina_vault.h"

namespace Lumina {

// The calculator decoy: a working four-function calculator, with the unlock
// hidden inside it. Pressing "=" (or Enter) while the entry is exactly the
// secret code returns VaultOutcome::Unlock; anything else just computes, so
// there is nothing to see. Closing the window returns VaultOutcome::Closed.
//
// Runs a nested QEventLoop and returns only once the user has decided. See
// lumina/lumina_vault.h for why nothing in here may touch st::, tr:: or Ui::.
[[nodiscard]] VaultOutcome RunVaultCalculator();

// Whether `text` can be produced key by key on the calculator's keypad.
//
// A code is free-form - the password door takes anything - but the calculator
// normalises what it is given: a leading "." becomes "0.", a second "." inside
// one number is dropped, a leading "+", "*" or "/" is ignored and two
// operators in a row replace each other. A code containing any of those shapes
// (or any other character) can never be entered on this skin however hard the
// user tries, so the settings page warns instead of shipping a vault that
// cannot be opened.
[[nodiscard]] bool CalculatorCanType(const QString &text);

} // namespace Lumina
