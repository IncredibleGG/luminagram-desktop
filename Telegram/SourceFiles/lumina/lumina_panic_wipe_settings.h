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

// The confirmation in front of Lumina::PerformPanicWipe(), and the settings
// row that leads to it.
//
// The wipe adds no preference of its own: there is nothing to configure, and a
// key that only ever holds `false` would be one more thing to get wrong. The
// feature is therefore behaviour-neutral by construction - it is a row on a
// LuminaGram-only sub-page and one function that nothing else calls.

// Shows the confirmation box. The wipe runs only if the user ticks the
// acknowledgement and then presses the destructive button, which does not
// exist until the box is ticked. An accidental trigger is not recoverable from
// this device, so a single misclick must not be able to reach it.
void ShowPanicWipeConfirm(not_null<Window::SessionController*> controller);

// The whole feature, in the F-02 sub-page shape: the security section .cpp
// holds this one call and nothing else.
void AddPanicWipeRows(
	not_null<Ui::VerticalLayout*> container,
	not_null<Window::SessionController*> controller);

} // namespace Lumina
