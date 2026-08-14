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

// The "User ID" row on a user profile: the peer's numeric Telegram id, the bare
// user id that `peerToUser(user->id).bare` yields - the same value Android
// prints on the profile screen.
//
// UNCONDITIONAL. It shows on your own profile and on everyone else's alike, and
// there is no preference to switch it off, matching Android where the id is
// always visible. A user id is never zero, so the row never resolves to an
// empty value and is therefore always shown. It is copyable the same way its
// neighbours are: the value label goes through the same addInfoOneLine factory,
// which makes it selectable and gives it a "Copy" context-menu entry.
//
// Purely local: the id is already part of the peer, so nothing is requested
// from the server to build this row.

// The bare numeric id as text. Never empty for a real user.
[[nodiscard]] QString UserIdText(not_null<UserData*> user);

// Adds the row through the F-05 seam. Call from Lumina::AddUserInfoRows().
void AddUserIdRow(
	const ProfileRowsContext &context,
	not_null<UserData*> user);

} // namespace Lumina
