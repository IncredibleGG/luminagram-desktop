/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_profile_user_id.h"

#include "data/data_changes.h"
#include "data/data_user.h"
#include "lang/lang_keys.h"
#include "lumina/lumina_locale.h"
#include "main/main_session.h"

namespace Lumina {

QString UserIdText(not_null<UserData*> user) {
	return QString::number(peerToUser(user->id).bare);
}

void AddUserIdRow(
		const ProfileRowsContext &context,
		not_null<UserData*> user) {
	// The id itself never changes, but the row still needs one initial emission
	// to appear, and FullInfo delivers that immediately on subscription the same
	// way it does for the registration-date row; LangChanges keeps the value in
	// step if the label's language is ever switched.
	auto text = rpl::merge(
		LangChanges(),
		user->session().changes().peerFlagsValue(
			user,
			Data::PeerUpdate::Flag::FullInfo) | rpl::to_empty
	) | rpl::map([=] {
		return tr::marked(UserIdText(user));
	});
	context.addInfoOneLine(
		TrValue(u"LuminaProfileUserId"_q),
		std::move(text),
		QString());
}

} // namespace Lumina
