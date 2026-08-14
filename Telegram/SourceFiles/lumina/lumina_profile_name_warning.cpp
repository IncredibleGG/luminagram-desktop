/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_profile_name_warning.h"

#include "data/data_changes.h"
#include "data/data_user.h"
#include "lang/lang_keys.h"
#include "lumina/lumina_homoglyph.h"
#include "lumina/lumina_locale.h"
#include "main/main_session.h"

namespace Lumina {
namespace {

// The prominent value line is the alarm; the caption below it says why. Both
// are one short phrase, so the alarm fits the single-line value style the row
// factory uses (st::infoLabeledOneLine) while the reason wraps in the caption,
// which is not single-line. The leading warning sign (U+26A0) matches the
// desktop scam-watch hint.
[[nodiscard]] QString NameWarningText(not_null<UserData*> user) {
	if (!HomoglyphWarnEnabled()) {
		return QString();
	}
	return containsSuspicious(user->name())
		? Tr(u"LuminaProfileNameWarning"_q)
		: QString();
}

} // namespace

void AddNameWarningRow(
		const ProfileRowsContext &context,
		not_null<UserData*> user) {
	// The name arrives and later changes under PeerUpdate::Flag::Name, and a
	// full refresh under FullInfo; peerFlagsValue() also emits once on
	// subscription, which is what makes the row evaluate the moment the profile
	// opens. The preference and the in-app language complete what the text
	// depends on.
	auto text = rpl::merge(
		HomoglyphWarnChanges(),
		LangChanges(),
		user->session().changes().peerFlagsValue(
			user,
			Data::PeerUpdate::Flag::Name
			| Data::PeerUpdate::Flag::FullInfo) | rpl::to_empty
	) | rpl::map([=]() -> TextWithEntities {
		const auto warning = NameWarningText(user);
		return warning.isEmpty()
			? TextWithEntities()
			: tr::marked(u"⚠ "_q + warning);
	});
	context.addInfoOneLine(
		TrValue(u"LuminaProfileNameWarningLabel"_q),
		std::move(text),
		QString());
}

} // namespace Lumina
