/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "base/basic_types.h"

#include <rpl/producer.h>

#include <QtCore/QString>

#include <vector>

class DocumentData;

namespace Window {
class SessionController;
} // namespace Window

namespace Lumina {

// The file masquerade guard: one confirmation, shown offline, before a saved
// document is handed to the operating system to open, when the name the sender
// chose is dressed to hide what the file really is. Ported from Android's
// file-masquerade check.
//
// Hook. The gate sits in Data::ResolveDocument, on the one branch that ends in
// LaunchWithWarning() -> File::Launch() - the path that hands a file to the OS
// shell. Playing media in-app, opening a theme, and rendering markdown in the
// internal viewer never reach a shell and are out of scope. tdesktop's own
// launcher warning (an executable or unknown extension on THIS platform) still
// runs after an approval here; the two ask different questions - stock says
// "this is a program", this says "this program is wearing a photo's name" -
// and fire on different files, so a doubled box only appears for a file that
// earns both.
//
// What it detects, all on the sender-visible name and the declared MIME, with
// no file I/O:
//   * a bidirectional control character in the name (U+202E and the rest),
//     the trick that renders "photo_high_re<RLO>gpj.exe" as a .jpg;
//   * a double extension whose last part is executable and an earlier part is
//     a document, media or archive type - "invoice.pdf.exe";
//   * the final extension and the declared MIME disagreeing across the
//     program/media line - a .mp4 whose type is an executable, or an .exe
//     served as video/mp4.
//
// Cross-platform on purpose. The executable set here is NOT Core::NameType,
// which counts only what is dangerous on the running OS: an .exe mailed to a
// Mac is still a masqueraded file worth a word, not least because the user may
// forward it on.
//
// fail-open. The check answers "not suspicious" for anything it cannot judge,
// and the gate returns false - "not handled, open exactly as before" - on
// every path that is not a shown box: the preference off, a clean name, no
// controller and no way to host a box. The guard never swallows an open.

enum class FileGuardReason : uchar {
	BidiOverride,
	DoubleExtension,
	TypeMismatch,
};

struct FileGuardResult {
	bool suspicious = false;
	std::vector<FileGuardReason> reasons;
};

// Pure, no I/O. Not suspicious for a name and MIME that raise no flag.
[[nodiscard]] FileGuardResult FileGuardCheck(
	const QString &fileName,
	const QString &mime);

// Preference `fileMasqueradeGuard`, Store::Prefs, default true.
[[nodiscard]] bool FileGuardEnabled();
void SetFileGuardEnabled(bool value);

// Fires when the preference above changes, including through a whole-file
// restore by Settings::importAll().
[[nodiscard]] rpl::producer<> FileGuardChanges();

// The gate. Returns true when it has put a confirmation box up and the caller
// must stop; `proceed` is invoked later, from the box, only if the user
// chooses to open anyway. Returns false in every other case, including every
// failure path, and then the caller opens the document as it would have.
[[nodiscard]] bool FileGuardIntercept(
	Window::SessionController *controller,
	not_null<DocumentData*> document,
	Fn<void()> proceed);

} // namespace Lumina
