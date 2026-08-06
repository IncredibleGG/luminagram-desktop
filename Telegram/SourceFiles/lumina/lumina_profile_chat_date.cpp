/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_profile_chat_date.h"

#include "base/unixtime.h"
#include "data/data_changes.h"
#include "data/data_channel.h"
#include "data/data_chat.h"
#include "data/data_peer.h"
#include "lang/lang_keys.h"
#include "lumina/lumina_locale.h"
#include "lumina/lumina_settings.h"
#include "main/main_session.h"

namespace Lumina {
namespace {

const auto kKey = u"showChatDate"_q;

} // namespace

bool ChatCreationDateRowEnabled() {
	return Settings::Instance().getBool(kKey, false);
}

void SetChatCreationDateRowEnabled(bool value) {
	Settings::Instance().set(kKey, value);
}

rpl::producer<> ChatCreationDateRowChanges() {
	return Settings::Instance().changesFor(kKey);
}

TimeId ChatCreationDate(not_null<PeerData*> peer) {
	if (const auto chat = peer->asChat()) {
		return chat->date;
	} else if (const auto channel = peer->asChannel()) {
		return channel->date;
	}
	return 0;
}

QString ChatCreationDateText(not_null<PeerData*> peer) {
	if (!ChatCreationDateRowEnabled()) {
		return QString();
	}
	const auto date = ChatCreationDate(peer);
	if (date <= 0) {
		return QString();
	}
	// langDayOfMonthFull() drops the year for anything dated inside the
	// current one, which is right for a message header and wrong for a
	// creation date - "12 March" alone says nothing about how old a channel
	// is. This is that function's with-year branch, used unconditionally.
	const auto parsed = base::unixtime::parse(date).date();
	return tr::lng_month_day_year(
		tr::now,
		lt_month,
		Lang::MonthDay(parsed.month())(tr::now),
		lt_day,
		QString::number(parsed.day()),
		lt_year,
		QString::number(parsed.year()));
}

void AddChatCreationDateRow(
		const ProfileRowsContext &context,
		not_null<PeerData*> peer,
		Data::ForumTopic *topic) {
	if (topic) {
		return;
	}
	// `date` has no update flag of its own; it is assigned in processChat() /
	// processChannel() right next to the title, and a "min" channel gets it
	// only once it is loaded for real - which is what FullInfo reports.
	auto text = rpl::merge(
		ChatCreationDateRowChanges(),
		LangChanges(),
		peer->session().changes().peerFlagsValue(
			peer,
			Data::PeerUpdate::Flag::Name
			| Data::PeerUpdate::Flag::FullInfo) | rpl::to_empty
	) | rpl::map([=] {
		return tr::marked(ChatCreationDateText(peer));
	});
	context.addInfoOneLine(
		TrValue(u"LuminaProfileChatCreated"_q),
		std::move(text),
		QString());
}

} // namespace Lumina
