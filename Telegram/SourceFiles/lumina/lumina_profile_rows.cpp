/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_profile_rows.h"

namespace Lumina {

void AddUserInfoRows(
		const ProfileRowsContext &context,
		not_null<UserData*> user) {
	// Nothing yet. Feature owners add their call here, one line each, and keep
	// the row itself in their own lumina/*.{h,cpp} pair:
	//  - registration date (server value first, id estimate as a fallback),
	//  - datacenter id,
	//  - private contact note and tags (bots included).
}

void AddChatInfoRows(
		const ProfileRowsContext &context,
		not_null<PeerData*> peer,
		Data::ForumTopic *topic) {
	// Nothing yet. Feature owners add their call here, one line each, and keep
	// the row itself in their own lumina/*.{h,cpp} pair:
	//  - chat / channel creation date,
	//  - datacenter id.
}

} // namespace Lumina
