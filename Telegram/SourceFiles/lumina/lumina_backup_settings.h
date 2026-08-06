/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "base/basic_types.h"

namespace Ui {
class VerticalLayout;
} // namespace Ui

namespace Window {
class SessionController;
} // namespace Window

namespace Lumina {

// The encrypted-backup rows, in the F-02 sub-page shape: the sub-page holds
// this one call and nothing else. Belongs on the Tools page.
//
// Two action rows and nothing persistent - this feature stores no preference of
// its own, so wiring it up cannot change how anything else behaves.
//
// Android puts these in an activity of their own (LuminaBackupActivity) purely
// because Android has no other way to hang two buttons and two paragraphs off a
// settings list. Here they are two rows on the page that already exists; a
// sub-page for two buttons would be a click of ceremony for nothing.
//
// Both rows ask for a passphrase in a box first and only then open a file
// dialog, which is the order Android uses. Export asks for the passphrase
// twice: the file is unrecoverable without it, and a typo the user cannot see
// through a masked field would only surface months later when the backup is
// needed. Import asks once, because a typo there costs one retry.
//
// The engine, the container format and every crypto decision are documented in
// lumina/lumina_backup.h.
void AddBackupRows(
	not_null<Ui::VerticalLayout*> container,
	not_null<Window::SessionController*> controller);

} // namespace Lumina
