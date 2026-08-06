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

// The scam keyword warning's row on the LuminaGram privacy sub-page. One
// toggle over the `scamKeywordWarning` key, which defaults to false; the
// detector itself, and every accessor for that key, live in
// lumina/lumina_scam_watch.h.
//
// Called once from Settings::LuminaPrivacy::setupContent().
void AddScamWarningRows(
	not_null<Ui::VerticalLayout*> container,
	not_null<Window::SessionController*> controller);

} // namespace Lumina
