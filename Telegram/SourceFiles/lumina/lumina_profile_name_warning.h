/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "lumina/lumina_profile_rows.h"

class UserData;

namespace Lumina {

// The impersonation warning row on a user profile. It appears - as a single
// selectable line, the same shape as the User ID and Registration date rows -
// only when Lumina::containsSuspicious() flags the peer's display name and the
// `homoglyphWarn` preference is on; otherwise its value producer emits empty
// and the row slides itself away, exactly like every other optional Lumina
// row. Purely local: it inspects the name already on this device and asks
// Telegram for nothing.
//
// Added from Lumina::AddUserInfoRows(). Users only - a homoglyph in a person's
// name is the impersonation case Android warns about; channels and groups can
// be added later through AddChatInfoRows() the same way.
void AddNameWarningRow(
	const ProfileRowsContext &context,
	not_null<UserData*> user);

} // namespace Lumina
