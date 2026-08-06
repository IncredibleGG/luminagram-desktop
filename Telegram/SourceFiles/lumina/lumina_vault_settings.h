/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

namespace Ui {
class VerticalLayout;
} // namespace Ui

namespace Window {
class SessionController;
} // namespace Window

namespace Lumina {

// The disguise vault's rows on the LuminaGram security sub-page, in the F-02
// sub-page shape: the section .cpp holds this one call and nothing else.
//
// Everything these rows read and write is declared in lumina/lumina_vault.h.
// The rows themselves are ordinary settings widgets and run with the whole app
// up; only the gate they configure is bound by the pre-init restrictions
// documented in that header.
void AddVaultRows(
	not_null<Ui::VerticalLayout*> container,
	not_null<Window::SessionController*> controller);

} // namespace Lumina
