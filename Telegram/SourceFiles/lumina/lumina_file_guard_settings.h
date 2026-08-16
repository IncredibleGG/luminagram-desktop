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

// The disguised-file warning's row on the LuminaGram privacy sub-page. One
// toggle over the `fileMasqueradeGuard` key, which - like the homoglyph guard
// above it - defaults to TRUE, because it is a protection and not a preference.
// The detector and every accessor for that key live in lumina/lumina_file_guard.h.
//
// Called once from Settings::LuminaPrivacy::setupContent().
void AddFileGuardRows(
	not_null<Ui::VerticalLayout*> container,
	not_null<Window::SessionController*> controller);

} // namespace Lumina
