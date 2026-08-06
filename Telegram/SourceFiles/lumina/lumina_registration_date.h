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

// The "Registration date" row on a user profile - roughly when the account
// was created.
//
// TWO SOURCES, IN THIS ORDER, AND THEY ARE NOT THE SAME KIND OF FACT.
//
// 1. The server. tdesktop already receives a registration month from
//    messages.peerSettings and keeps it in PeerBarDetails::registrationDate
//    (data/data_peer.h:196, YYYYMM), reachable through
//    PeerData::registrationMonth() / registrationYear(). Today the only thing
//    that renders it is the new-contact bar (history/view/
//    history_view_about_view.cpp:424-431). It is authoritative, so it wins
//    whenever it is there, and it is shown plain.
//
//    It is not always there: the server sends it with the "add contact" bar
//    settings, i.e. mainly for people you are not in contacts with. On most
//    profiles registrationMonth() is 0 and the row falls through to (2).
//
// 2. The client-side estimate, which is what Android has and all it has
//    (ProfileActivity.java:10464-10516). Telegram user ids are handed out in
//    roughly increasing order, so an id can be interpolated against a table of
//    (id, date) anchors into an approximate creation date. This is a
//    heuristic, not a fact: it is off by weeks near the anchors and by more
//    between them, id allocation is not strictly monotonic, and the last few
//    anchors are extrapolation rather than observation. It is therefore always
//    rendered with a leading "~".
//
// Nothing here asks the server anything. (1) is a value that already arrived
// for other reasons and (2) is pure arithmetic on the peer id.

// Preference `showRegistrationDate`, Store::Prefs, default FALSE.
//
// Android defaults this one to true (LuminaPrivacyActivity.java:86). Desktop
// deliberately does not: a new preference here must leave the app looking
// exactly as it did, and this preference adds a visible row to every user
// profile.
[[nodiscard]] bool RegistrationDateRowEnabled();
void SetRegistrationDateRowEnabled(bool value);

// Fires when the preference above changes, including through a whole-file
// restore by Settings::importAll().
[[nodiscard]] rpl::producer<> RegistrationDateRowChanges();

// Source (2) above: the interpolated estimate for a bare user id, as a unix
// timestamp. 0 for id 0. Clamped to the first / last anchor outside the
// table's range, exactly as Android clamps.
[[nodiscard]] TimeId EstimatedRegistrationDate(uint64 userId);

// What the row shows. Empty - which makes the row slide itself away - when the
// preference is off, when the profile is the user's own, or when neither
// source has anything to say.
[[nodiscard]] QString RegistrationDateText(not_null<UserData*> user);

// Adds the row through the F-05 seam. Call from Lumina::AddUserInfoRows().
void AddRegistrationDateRow(
	const ProfileRowsContext &context,
	not_null<UserData*> user);

} // namespace Lumina
