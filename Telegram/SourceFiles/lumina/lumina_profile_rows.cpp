/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_profile_rows.h"

// UserData has to be complete here, not just forward declared: passing a
// not_null<UserData*> where a not_null<PeerData*> is expected goes through
// gsl::not_null's converting constructor, and its std::is_convertible check
// needs the derived-to-base relationship to be visible.
#include "data/data_user.h"
#include "lumina/lumina_contact_notes.h"
#include "lumina/lumina_profile_chat_date.h"
#include "lumina/lumina_profile_dc_id.h"
#include "lumina/lumina_profile_name_warning.h"
#include "lumina/lumina_profile_user_id.h"
#include "lumina/lumina_registration_date.h"

namespace Lumina {

void AddUserInfoRows(
		const ProfileRowsContext &context,
		not_null<UserData*> user) {
	AddNameWarningRow(context, user);
	AddRegistrationDateRow(context, user);
	AddProfileDcIdRow(context, user);
	AddUserIdRow(context, user);
	AddContactNoteRow(context, user);
}

void AddChatInfoRows(
		const ProfileRowsContext &context,
		not_null<PeerData*> peer,
		Data::ForumTopic *topic) {
	AddChatCreationDateRow(context, peer, topic);
	AddProfileDcIdRow(context, peer, topic);
}

} // namespace Lumina
