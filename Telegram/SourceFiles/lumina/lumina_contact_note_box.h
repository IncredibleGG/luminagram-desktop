/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "base/basic_types.h"

class UserData;

namespace Ui {
class GenericBox;
} // namespace Ui

namespace Window {
class SessionController;
} // namespace Window

namespace Lumina {

// The editor behind the private-note profile row. Two plain-text fields, a
// note and a comma-separated tags line, both stored locally by
// lumina_contact_notes and never sent anywhere.
//
// The note field is present only when ContactNoteFieldAvailable() says so; for
// an ordinary contact the box is tags-only and Telegram's own synced note is
// left to its own editor. See the policy note at the top of
// lumina/lumina_contact_notes.h.
//
// Emptying both fields deletes the entry, which is also what the box's delete
// button does. The box writes through Lumina::SetContactNote() and holds no
// pointer back to whatever opened it - the profile row rebuilds itself from
// ContactNoteChanges().
void ContactNoteBox(
	not_null<Ui::GenericBox*> box,
	not_null<UserData*> user);

void ShowContactNoteBox(
	not_null<Window::SessionController*> controller,
	not_null<UserData*> user);

} // namespace Lumina
