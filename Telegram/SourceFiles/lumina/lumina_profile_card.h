/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include <rpl/producer.h>

#include <QtCore/QString>

#include <vector>

namespace Lumina {

// LuminaGram local profile card ("about me").
//
// A small self-description - a tagline, the languages you speak, your
// interests and a short bio - kept ONLY on this device, under the
// `profileCard` preference. Android: LuminaProfileCardActivity.
//
// It is not a Telegram profile and it never becomes one. Nothing here is sent
// anywhere, no API call reads it, and the real profile (name, username, the
// server-side bio) is never touched. The only way the card leaves the device
// is the user copying it and pasting it themselves.
//
// WHY THERE IS NO "SHARE TO CHAT"
// -------------------------------
// Android's share row hands the text to an ACTION_SEND chooser: the user picks
// an app, sees the text again, and confirms there. The desktop analogue would
// be Window::ShowChooseRecipientBox, which SENDS a message the moment a chat
// is picked - no preview, no second confirmation. That turns one misclick in a
// chat list into a personal description broadcast to the wrong person, a far
// worse failure than the one Android can produce. v1 is therefore
// copy-to-clipboard only (DESKTOP-PORT-PLAN section 6 Q5, decided by the
// owner), and the clipboard is a step the user has to complete by hand in the
// chat they actually meant.
//
// STORAGE
// -------
// Store::Private, so that free text the user wrote about themselves does not
// sit in the pref file every unrelated toggle rewrites. SetProfileCard() is
// the only writer in the tree and it always names the store, because
// Settings::set() defaults to Store::Prefs and a bare set() would RELOCATE the
// card into the plaintext pref file.
//
// The card is stored as a JSON object whose field names are Android's
// (`tagline`, `languages`, `interests`, `bio`) under Android's key, so a
// cross-platform backup (W6-E) lands on the card instead of on a dead key.
// Android writes the object as a *string* holding JSON rather than as a JSON
// object; CurrentProfileCard() reads both shapes and the next save normalises
// an imported one to an object.

struct ProfileCard {
	QString tagline;
	QString languages;
	QString interests;
	QString bio;
};

// One field of the card, in the order the card is shown in and shared in.
// `value` is the member the row edits, so both the UI and the serialisation
// are one loop over this table rather than four copies of the same code - and
// adding a field is one entry here.
struct ProfileCardFieldInfo {
	QString ProfileCard::*value = nullptr;
	QString jsonKey;
	QString labelKey;
	QString hintKey;
	int maxLength = 0;
	bool multiline = false;
};

[[nodiscard]] const std::vector<ProfileCardFieldInfo> &ProfileCardFields();

// Trimmed and clamped on the way out as well as on the way in: an oversized or
// untrimmed value can only arrive from an imported backup, and everything that
// reads the card treats the result as already sane.
[[nodiscard]] ProfileCard CurrentProfileCard();

// Writes the whole card. Empty fields are dropped, and a card with nothing
// left in it removes the key rather than storing four empty strings.
void SetProfileCard(ProfileCard card);

[[nodiscard]] rpl::producer<> ProfileCardChanges();

[[nodiscard]] bool ProfileCardEmpty(const ProfileCard &card);

// "Label: value", one line per non-empty field, in ProfileCardFields() order,
// with the labels in the current in-app language. Empty for an empty card,
// which is what the copy action checks before it touches the clipboard - an
// empty card must never silently clear whatever the user had copied.
[[nodiscard]] QString ProfileCardShareText(const ProfileCard &card);

} // namespace Lumina
