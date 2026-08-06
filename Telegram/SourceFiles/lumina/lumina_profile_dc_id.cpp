/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_profile_dc_id.h"

#include "data/data_changes.h"
#include "data/data_peer.h"
#include "lang/lang_keys.h"
#include "lumina/lumina_locale.h"
#include "lumina/lumina_settings.h"
#include "main/main_session.h"
#include "ui/image/image_location.h"

namespace Lumina {
namespace {

const auto kKey = u"showDcId"_q;

} // namespace

bool ProfileDcIdRowEnabled() {
	return Settings::Instance().getBool(kKey, false);
}

void SetProfileDcIdRowEnabled(bool value) {
	Settings::Instance().set(kKey, value);
}

rpl::producer<> ProfileDcIdRowChanges() {
	return Settings::Instance().changesFor(kKey);
}

int ProfileDcId(not_null<PeerData*> peer) {
	const auto location = peer->userpicLocation();
	const auto storage = std::get_if<StorageFileLocation>(
		&location.file().data);
	return storage ? storage->dcId() : 0;
}

QString ProfileDcIdText(not_null<PeerData*> peer) {
	if (!ProfileDcIdRowEnabled()) {
		return QString();
	}
	const auto dc = ProfileDcId(peer);
	return dc
		? Tr(u"LuminaProfileDcIdValue"_q, QString::number(dc))
		: QString();
}

void AddProfileDcIdRow(
		const ProfileRowsContext &context,
		not_null<PeerData*> peer,
		Data::ForumTopic *topic) {
	if (topic) {
		return;
	}
	auto text = rpl::merge(
		ProfileDcIdRowChanges(),
		LangChanges(),
		peer->session().changes().peerFlagsValue(
			peer,
			Data::PeerUpdate::Flag::Photo) | rpl::to_empty
	) | rpl::map([=] {
		return tr::marked(ProfileDcIdText(peer));
	});
	context.addInfoOneLine(
		TrValue(u"LuminaProfileDcId"_q),
		std::move(text),
		QString());
}

} // namespace Lumina
