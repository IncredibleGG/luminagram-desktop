/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_registration_date.h"

#include "data/data_changes.h"
#include "data/data_user.h"
#include "lang/lang_keys.h"
#include "lumina/lumina_locale.h"
#include "lumina/lumina_settings.h"
#include "main/main_session.h"

namespace Lumina {
namespace {

const auto kKey = u"showRegistrationDate"_q;

struct Anchor {
	uint64 id = 0;
	TimeId date = 0;
};

// Transcribed from ProfileActivity.java:10464-10484 (REG_ANCHOR_IDS and
// REG_ANCHOR_DATES), pairwise rather than as two parallel arrays: two arrays
// can silently drift out of alignment by one entry, and the only symptom of
// that - like the only symptom of a mistyped digit - is an estimate that is
// quietly wrong for everybody.
//
// The ids must stay sorted ascending; EstimatedRegistrationDate() below
// binary-searches them. Every date is a midnight UTC. The tail entries beyond
// the highest observed id are Android's own extrapolation, kept as-is so both
// clients estimate the same date for the same account.
constexpr auto kAnchors = std::array<Anchor, 51>{ {
	{    1000000ULL, 1380326400 },
	{    2768409ULL, 1383264000 },
	{    7679610ULL, 1388448000 },
	{   11538514ULL, 1391212800 },
	{   15835244ULL, 1392940800 },
	{   23646077ULL, 1393459200 },
	{   38015510ULL, 1393632000 },
	{   44634663ULL, 1399334400 },
	{   46145305ULL, 1400198400 },
	{   54845238ULL, 1411257600 },
	{   63263518ULL, 1414454400 },
	{  101260938ULL, 1425600000 },
	{  103151531ULL, 1433376000 },
	{  109393468ULL, 1439683200 },
	{  112594714ULL, 1444176000 },
	{  116812045ULL, 1448323200 },
	{  122600695ULL, 1450483200 },
	{  124872445ULL, 1453248000 },
	{  130029930ULL, 1457481600 },
	{  132670343ULL, 1461283200 },
	{  141733941ULL, 1465344000 },
	{  152253017ULL, 1466121600 },
	{  157242073ULL, 1471046400 },
	{  171295414ULL, 1474156800 },
	{  188758258ULL, 1476835200 },
	{  191317690ULL, 1477267200 },
	{  199570902ULL, 1481932800 },
	{  229882272ULL, 1493856000 },
	{  234462946ULL, 1499472000 },
	{  253685473ULL, 1504137600 },
	{  293169835ULL, 1508025600 },
	{  315690368ULL, 1526342400 },
	{  342781860ULL, 1529625600 },
	{  352940995ULL, 1532563200 },
	{  369669043ULL, 1538006400 },
	{  400169472ULL, 1542326400 },
	{  616816630ULL, 1548720000 },
	{  700000000ULL, 1556668800 },
	{  800000000ULL, 1571184000 },
	{  900000000ULL, 1585699200 },
	{ 1000000000ULL, 1600214400 },
	{ 1200000000ULL, 1614556800 },
	{ 1400000000ULL, 1625097600 },
	{ 1600000000ULL, 1633046400 },
	{ 1800000000ULL, 1643673600 },
	{ 2000000000ULL, 1656633600 },
	{ 3000000000ULL, 1690848000 },
	{ 4000000000ULL, 1719792000 },
	{ 5000000000ULL, 1748736000 },
	{ 6000000000ULL, 1777680000 },
	{ 7000000000ULL, 1806624000 },
} };

[[nodiscard]] QString MonthText(TimeId when) {
	// The anchors are midnight UTC, so the date is read back in UTC too.
	// Reading them in local time would move an exact anchor hit to the
	// previous day west of Greenwich, and for the anchors that sit on the
	// first of a month that is a whole month of error in a value the row
	// already only claims to the month.
	const auto date = QDateTime::fromSecsSinceEpoch(when).toUTC().date();
	return langMonthOfYearFull(date.month(), date.year());
}

} // namespace

bool RegistrationDateRowEnabled() {
	return Settings::Instance().getBool(kKey, false);
}

void SetRegistrationDateRowEnabled(bool value) {
	Settings::Instance().set(kKey, value);
}

rpl::producer<> RegistrationDateRowChanges() {
	return Settings::Instance().changesFor(kKey);
}

TimeId EstimatedRegistrationDate(uint64 userId) {
	if (!userId) {
		return 0;
	}
	const auto count = int(kAnchors.size());
	if (userId <= kAnchors[0].id) {
		return kAnchors[0].date;
	} else if (userId >= kAnchors[count - 1].id) {
		return kAnchors[count - 1].date;
	}
	auto low = 0;
	auto high = count - 1;
	while (high - low > 1) {
		const auto middle = low + (high - low) / 2;
		if (kAnchors[middle].id <= userId) {
			low = middle;
		} else {
			high = middle;
		}
	}
	const auto span = kAnchors[high].id - kAnchors[low].id;
	if (!span) {
		return kAnchors[low].date;
	}
	const auto shift = int64(kAnchors[high].date - kAnchors[low].date)
		* int64(userId - kAnchors[low].id)
		/ int64(span);
	return kAnchors[low].date + TimeId(shift);
}

QString RegistrationDateText(not_null<UserData*> user) {
	if (!RegistrationDateRowEnabled() || user->isSelf()) {
		return QString();
	}
	const auto month = user->registrationMonth();
	const auto year = user->registrationYear();
	if (month && year) {
		return langMonthOfYearFull(month, year);
	}
	const auto estimate = EstimatedRegistrationDate(
		peerToUser(user->id).bare);
	return estimate
		? Tr(u"LuminaProfileRegistrationApprox"_q, MonthText(estimate))
		: QString();
}

void AddRegistrationDateRow(
		const ProfileRowsContext &context,
		not_null<UserData*> user) {
	// barSettingsValue() is what actually carries source (1): the registration
	// month arrives inside the same peerSettings that fills the bar flags, and
	// PeerData has no update flag of its own for it. FullInfo covers the
	// profile being refreshed for any other reason.
	auto text = rpl::merge(
		RegistrationDateRowChanges(),
		LangChanges(),
		user->barSettingsValue() | rpl::to_empty,
		user->session().changes().peerFlagsValue(
			user,
			Data::PeerUpdate::Flag::FullInfo) | rpl::to_empty
	) | rpl::map([=] {
		return tr::marked(RegistrationDateText(user));
	});
	context.addInfoOneLine(
		TrValue(u"LuminaProfileRegistrationDate"_q),
		std::move(text),
		QString());
}

} // namespace Lumina
