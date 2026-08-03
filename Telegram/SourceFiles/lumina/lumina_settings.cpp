/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_settings.h"

#include "settings.h" // cWorkingDir()

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

namespace Lumina {
namespace {

[[nodiscard]] QString FilePath() {
	return cWorkingDir() + u"tdata/luminagram.json"_q;
}

} // namespace

Settings &Settings::Instance() {
	static Settings result;
	return result;
}

Settings::Settings() {
	load();
}

void Settings::load() {
	auto file = QFile(FilePath());
	if (!file.open(QIODevice::ReadOnly)) {
		_loaded = true;
		return;
	}
	const auto bytes = file.readAll();
	file.close();

	auto error = QJsonParseError{ 0, QJsonParseError::NoError };
	const auto document = QJsonDocument::fromJson(bytes, &error);
	if (error.error != QJsonParseError::NoError || !document.isObject()) {
		_loaded = true;
		return;
	}
	const auto object = document.object();
	const auto readBool = [&](const QString &key, bool &field) {
		const auto value = object.value(key);
		if (value.isBool()) {
			field = value.toBool();
		}
	};
	readBool(u"hideChatFolders"_q, _hideChatFolders);
	readBool(u"stealthOnline"_q, _stealthOnline);
	readBool(u"stealthTyping"_q, _stealthTyping);
	readBool(u"stealthReadReceipts"_q, _stealthReadReceipts);
	readBool(u"forwardWithoutAuthor"_q, _forwardWithoutAuthor);
	readBool(u"allowSaveRestricted"_q, _allowSaveRestricted);
	readBool(u"autoTranslate"_q, _autoTranslate);

	_loaded = true;
}

void Settings::save() const {
	auto object = QJsonObject();
	object.insert(u"hideChatFolders"_q, _hideChatFolders);
	object.insert(u"stealthOnline"_q, _stealthOnline);
	object.insert(u"stealthTyping"_q, _stealthTyping);
	object.insert(u"stealthReadReceipts"_q, _stealthReadReceipts);
	object.insert(u"forwardWithoutAuthor"_q, _forwardWithoutAuthor);
	object.insert(u"allowSaveRestricted"_q, _allowSaveRestricted);
	object.insert(u"autoTranslate"_q, _autoTranslate);

	const auto bytes = QJsonDocument(object).toJson(QJsonDocument::Indented);

	// Make sure the tdata directory exists before writing.
	QDir().mkpath(cWorkingDir() + u"tdata"_q);

	auto file = QFile(FilePath());
	if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		file.write(bytes);
		file.close();
	}
}

void Settings::setBool(bool &field, bool value) {
	if (field == value) {
		return;
	}
	field = value;
	if (_loaded) {
		save();
		_changes.fire({});
	}
}

} // namespace Lumina
