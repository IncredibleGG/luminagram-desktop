/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_profile_card.h"

#include "lumina/lumina_locale.h"
#include "lumina/lumina_settings.h"

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>

namespace Lumina {
namespace {

// Android's LuminaProfileCardActivity.KEY_PROFILE_CARD.
const auto kKey = u"profileCard"_q;

constexpr auto kStore = Store::Private;

// Android imposes no limit at all, which is fine for a dialog on a phone and
// not fine for a value that is read on every settings repaint and written into
// a JSON file. These are generous - a tagline four times the length of a
// Telegram bio - and exist so that a corrupt or hostile backup cannot put a
// megabyte of text behind a settings row.
constexpr auto kTaglineMaxLength = 128;
constexpr auto kLanguagesMaxLength = 128;
constexpr auto kInterestsMaxLength = 256;
constexpr auto kBioMaxLength = 1024;

[[nodiscard]] QJsonObject ReadCardObject() {
	auto &settings = Settings::Instance();
	const auto object = settings.getObject(kKey);
	if (!object.isEmpty()) {
		return object;
	}

	// Android stores the whole card as a *string* holding a JSON object
	// (LuminaProfileCardActivity.setField() -> LuminaConfig.putString()). A
	// backup taken there and imported here therefore arrives as a string, and
	// dropping it would look exactly like "my card did not come across".
	const auto raw = settings.getString(kKey);
	if (raw.isEmpty()) {
		return QJsonObject();
	}
	const auto document = QJsonDocument::fromJson(raw.toUtf8());
	return document.isObject() ? document.object() : QJsonObject();
}

} // namespace

const std::vector<ProfileCardFieldInfo> &ProfileCardFields() {
	static const auto result = std::vector<ProfileCardFieldInfo>{
		{
			.value = &ProfileCard::tagline,
			.jsonKey = u"tagline"_q,
			.labelKey = u"LuminaProfileCardTagline"_q,
			.hintKey = u"LuminaProfileCardTaglineHint"_q,
			.maxLength = kTaglineMaxLength,
		},
		{
			.value = &ProfileCard::languages,
			.jsonKey = u"languages"_q,
			.labelKey = u"LuminaProfileCardLanguages"_q,
			.hintKey = u"LuminaProfileCardLanguagesHint"_q,
			.maxLength = kLanguagesMaxLength,
		},
		{
			.value = &ProfileCard::interests,
			.jsonKey = u"interests"_q,
			.labelKey = u"LuminaProfileCardInterests"_q,
			.hintKey = u"LuminaProfileCardInterestsHint"_q,
			.maxLength = kInterestsMaxLength,
		},
		{
			.value = &ProfileCard::bio,
			.jsonKey = u"bio"_q,
			.labelKey = u"LuminaProfileCardBio"_q,
			.hintKey = u"LuminaProfileCardBioHint"_q,
			.maxLength = kBioMaxLength,
			.multiline = true,
		},
	};
	return result;
}

ProfileCard CurrentProfileCard() {
	const auto object = ReadCardObject();
	auto result = ProfileCard();
	for (const auto &info : ProfileCardFields()) {
		result.*(info.value) = object.value(info.jsonKey)
			.toString()
			.trimmed()
			.left(info.maxLength);
	}
	return result;
}

void SetProfileCard(ProfileCard card) {
	auto object = QJsonObject();
	for (const auto &info : ProfileCardFields()) {
		const auto value = (card.*(info.value)).trimmed().left(info.maxLength);
		if (!value.isEmpty()) {
			object.insert(info.jsonKey, value);
		}
	}
	if (object.isEmpty()) {
		Settings::Instance().remove(kKey);
	} else {
		Settings::Instance().set(kKey, object, kStore);
	}
}

rpl::producer<> ProfileCardChanges() {
	return Settings::Instance().changesFor(kKey);
}

bool ProfileCardEmpty(const ProfileCard &card) {
	for (const auto &info : ProfileCardFields()) {
		if (!(card.*(info.value)).trimmed().isEmpty()) {
			return false;
		}
	}
	return true;
}

QString ProfileCardShareText(const ProfileCard &card) {
	auto result = QString();
	for (const auto &info : ProfileCardFields()) {
		const auto value = (card.*(info.value)).trimmed();
		if (value.isEmpty()) {
			continue;
		}
		if (!result.isEmpty()) {
			result += '\n';
		}
		result += Tr(info.labelKey) + u": "_q + value;
	}
	return result;
}

} // namespace Lumina
