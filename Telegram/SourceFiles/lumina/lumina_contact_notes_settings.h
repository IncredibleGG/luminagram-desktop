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

// W6-C's single settings entry point, in the F-02 sub-page shape: the
// sub-page holds this one call and nothing else. Recommended home - the
// LuminaGram "Privacy" sub-page, next to the other profile-row preferences.
//
// Three rows: the `showContactNotes` switch that decides whether user
// profiles carry the row at all, and - only while something is stored - a
// count and a "delete everything" button. Android has no settings for this
// feature at all; the switch exists because a desktop default of "on" would
// not be behaviour-neutral, and the delete row exists because the notes are
// the only Lumina data with no other way to reach them once the switch is
// off.
//
// Every preference behind these rows is declared and defaulted in
// lumina/lumina_contact_notes.h.
void AddContactNotesRows(
	not_null<Ui::VerticalLayout*> container,
	not_null<Window::SessionController*> controller);

} // namespace Lumina
