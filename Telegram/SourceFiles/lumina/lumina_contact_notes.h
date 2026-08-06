/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "lumina/lumina_profile_rows.h"

#include <rpl/producer.h>

#include <QtCore/QString>

class UserData;

namespace Lumina {

// Private contact notes and tags - a free-form note plus a short list of tags
// that you attach to a person, that live only on this device and that are
// never sent anywhere. Android's LuminaContactNotes.java, same storage key,
// same JSON shape, same 60-character summary.
//
//
// WHY THIS EXISTS NEXT TO TELEGRAM'S OWN CONTACT NOTES
//
// tdesktop already ships a contact note: `user->note()`, shown by
// CreateNotes() (info/profile/info_profile_actions.cpp:804) and written by
// EditContactNoteBox() through `contacts.addContact` with `Flag::f_note`
// (boxes/peers/edit_contact_box.cpp:92-104). That note is a SERVER value: it
// syncs to every device on the account, and Telegram holds it.
//
// Two note fields with different sync semantics on one profile is a bad
// feature, so the local note is offered only where the server one genuinely
// is not:
//
//  * bots - CreateNotes() is called from inside `if (!user->isBot())`
//    (info_profile_actions.cpp:1748), so a bot profile has no note row at all,
//    and `addNewContact` (window/window_peer_menu.cpp:1120-1126) refuses bots,
//    so there is no way to reach the editor either;
//  * non-contacts - the only writer is `contacts.addContact`, so saving a
//    server note on a stranger ADDS THEM TO YOUR CONTACTS. The stock row is
//    also bound to a non-empty `user->note()`, which a non-contact cannot
//    have.
//
// For an ordinary contact the server note stays the one and only note, and
// this feature contributes the tags line, which stock has no equivalent of.
//
// One exception keeps that policy from eating data: a user who already HAS a
// local note keeps their note field even after becoming a contact. Without it,
// adding somebody to your contacts would silently hide a note you wrote while
// they were a stranger, with no way to read or delete it. Such a profile can
// show both notes at once - but only because the user deliberately created
// both.
//
//
// WHAT IS PERSISTED
//
// Store::Private, key `contactNotes` (tdata/luminagram_private.json) - the
// same key name Android uses. One object per user, keyed by the decimal user
// id, exactly Android's layout:
//
//   { "<userId>": { "note": "<text>", "tags": "<text>" }, ... }
//
// The id is a JSON KEY, so it is a string and cannot be mangled by the double
// that a JSON number is. An entry whose note and tags are both empty is not
// written - clearing both is how the user deletes one.
//
// There is no session in the key, and that is deliberate: unlike a message id,
// a Telegram user id is global, so both halves of a two-account desktop mean
// the same person by it. A note is about a person, not about one of your
// logins, and Android - one account per process - stores it the same way.
//
// Nothing here is ever transmitted. The note text is the only content in the
// file, which is why it goes to Store::Private rather than the ordinary pref
// file. Settings::exportAll() carries it, so the W6-E backup carries the notes
// too.
//
//
// BEHAVIOUR NEUTRALITY
//
// `showContactNotes` is opt-IN and defaults to FALSE, so by default no profile
// gains a row, no store is read and nothing at all changes. Android has no
// toggle and always shows the row; a desktop default of "on" would add a
// permanently visible row to every user profile in the app, which is not a
// neutral default.
struct ContactNote {
	QString note;
	QString tags;

	[[nodiscard]] bool empty() const {
		return note.isEmpty() && tags.isEmpty();
	}
};

// Field caps. Android has none; an unbounded note in a file that is rewritten
// on every save is not worth the risk of a paste of a whole chat log.
inline constexpr auto kContactNoteMaxLength = 1024;
inline constexpr auto kContactTagsMaxLength = 128;

// How much of the note reaches the one-line profile row before it is cut.
// Android's number (LuminaContactNotes.getSummary).
inline constexpr auto kContactNoteSummaryMaxLength = 60;

// Preference `showContactNotes`, Store::Prefs, default false.
[[nodiscard]] bool ContactNotesEnabled();
void SetContactNotesEnabled(bool value);
[[nodiscard]] rpl::producer<> ContactNotesEnabledChanges();

// Fires whenever any stored note changes, including a W6-E backup import.
[[nodiscard]] rpl::producer<> ContactNoteChanges();

// The stored note for a user. Both fields empty when there is none.
[[nodiscard]] ContactNote ContactNoteFor(not_null<UserData*> user);

// Trims the note, normalises the tags (each one trimmed, empties dropped,
// rejoined with ", ") and stores the result. An entry that ends up completely
// empty is removed instead of stored.
void SetContactNote(not_null<UserData*> user, ContactNote note);

// Across every account, because one file holds them all.
[[nodiscard]] int ContactNotesCount();
void ClearContactNotes();

// Whether this profile gets a row at all: everybody except yourself.
[[nodiscard]] bool ContactNoteRowAvailable(not_null<UserData*> user);

// Whether the local NOTE (as opposed to the tags) is offered for this user -
// the policy described at the top of this file. False means the profile shows
// a tags-only row and the editor shows a tags-only box, leaving the note to
// Telegram's own synced one.
[[nodiscard]] bool ContactNoteFieldAvailable(not_null<UserData*> user);

// The one-line row value: the note whitespace-collapsed and cut to
// kContactNoteSummaryMaxLength, then the tags after a spaced middle dot.
// Empty when the user has neither.
[[nodiscard]] QString ContactNoteSummary(not_null<UserData*> user);

// Adds the row through the F-05 seam. Call from Lumina::AddUserInfoRows() -
// including for bots, which are the main reason the feature exists.
void AddContactNoteRow(
	const ProfileRowsContext &context,
	not_null<UserData*> user);

} // namespace Lumina
