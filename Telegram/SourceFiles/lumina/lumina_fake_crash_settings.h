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

// The fake-crash duress unlock block of the LuminaGram security sub-page, in
// the F-02 sub-page shape: settings/sections/settings_lumina_security.cpp
// holds one call to this and nothing else of ours.
//
// The preferences it writes are declared in lumina/lumina_fake_crash.h; the
// only consumer of them is the passcode lock screen.
void AddFakeCrashRows(
	not_null<Ui::VerticalLayout*> container,
	not_null<Window::SessionController*> controller);

} // namespace Lumina
