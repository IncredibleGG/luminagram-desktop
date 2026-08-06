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
#include <QtCore/QSaveFile>

namespace Lumina {
namespace {

constexpr auto kSaveDelay = crl::time(500);

[[nodiscard]] int StoreIndex(Store store) {
	return static_cast<int>(store);
}

[[nodiscard]] QString StoreName(Store store) {
	switch (store) {
	case Store::Prefs: return u"prefs"_q;
	case Store::Private: return u"private"_q;
	case Store::Bookmarks: return u"bookmarks"_q;
	}
	Unexpected("Store value in Lumina::StoreName.");
}

[[nodiscard]] std::optional<Store> StoreByName(const QString &name) {
	if (name == u"prefs"_q) {
		return Store::Prefs;
	} else if (name == u"private"_q) {
		return Store::Private;
	} else if (name == u"bookmarks"_q) {
		return Store::Bookmarks;
	}
	return std::nullopt;
}

[[nodiscard]] QString FilePath(Store store) {
	const auto name = [&] {
		switch (store) {
		case Store::Prefs: return u"luminagram.json"_q;
		case Store::Private: return u"luminagram_private.json"_q;
		case Store::Bookmarks: return u"luminagram_bookmarks.json"_q;
		}
		Unexpected("Store value in Lumina::FilePath.");
	}();
	return cWorkingDir() + u"tdata/"_q + name;
}

// Reserved key in exportAll() / importAll() payloads. It carries the Store of
// every key that does not live in Store::Prefs, so that a backup restored into
// a fresh profile still puts private values into the private file instead of
// dumping everything into luminagram.json. Absent means "everything is Prefs".
[[nodiscard]] QString StoresKey() {
	return u"_luminaStores"_q;
}

} // namespace

Settings &Settings::Instance() {
	static auto result = Settings();
	return result;
}

Settings::Settings()
: _saveTimer([=] { saveNow(); }) {
	load();
}

Settings::~Settings() {
	saveNow();
}

auto Settings::Defaults()
-> const base::flat_map<QString, QJsonValue> & {
	static const auto result = base::flat_map<QString, QJsonValue>{
		{ u"chatListOnlineDot"_q, QJsonValue(true) },
		{ u"showBookmarks"_q, QJsonValue(true) },
		{ u"stripPhotoMetadata"_q, QJsonValue(true) },
		{ u"stickerScale"_q, QJsonValue(100) },
	};
	return result;
}

void Settings::load() {
	for (auto i = 0; i != kStoreCount; ++i) {
		loadStore(static_cast<Store>(i));
	}
	migrateLegacyKeys();
}

void Settings::loadStore(Store store) {
	const auto index = StoreIndex(store);
	const auto path = FilePath(store);
	auto file = QFile(path);
	if (!file.exists()) {
		return;
	} else if (!file.open(QIODevice::ReadOnly)) {
		// It is there and we could not read it. Remember that: writing this
		// store now would replace the user's settings with an empty object.
		LOG(("Lumina Error: Could not read '%1', refusing to overwrite it."
			).arg(path));
		_loadFailed[index] = true;
		return;
	}
	const auto bytes = file.readAll();
	file.close();
	if (bytes.isEmpty()) {
		return;
	}

	auto error = QJsonParseError{ 0, QJsonParseError::NoError };
	const auto document = QJsonDocument::fromJson(bytes, &error);
	if (error.error != QJsonParseError::NoError || !document.isObject()) {
		const auto corrupt = path + u".corrupt"_q;
		QFile::remove(corrupt);
		if (QFile::rename(path, corrupt)) {
			LOG(("Lumina Error: Bad JSON in '%1' (%2), kept as '%3'."
				).arg(path, error.errorString(), corrupt));
		} else {
			LOG(("Lumina Error: Bad JSON in '%1' (%2), rename failed."
				).arg(path, error.errorString()));
		}
		return;
	}

	// Copy key by key instead of assigning the parsed object wholesale: an
	// empty key, the reserved key, or a key another store already claimed is
	// dropped here and the file is rewritten without it.
	const auto object = document.object();
	auto &data = _data[index];
	data = QJsonObject();
	for (auto i = object.constBegin(); i != object.constEnd(); ++i) {
		const auto key = i.key();
		if (key.isEmpty() || key == StoresKey() || _where.contains(key)) {
			_dirty[index] = true;
			continue;
		}
		data.insert(key, i.value());
		_where.emplace(key, store);
	}
	if (_dirty[index]) {
		scheduleSave();
	}
}

void Settings::migrateLegacyKeys() {
	const auto legacy = u"forwardWithoutAuthor"_q;
	const auto renamed = u"forwardNoAuthor"_q;
	const auto i = _where.find(legacy);
	if (i == _where.end()) {
		return;
	}
	const auto store = i->second;
	const auto index = StoreIndex(store);
	const auto value = _data[index].value(legacy);
	_data[index].remove(legacy);
	_where.remove(legacy);
	if (!_where.contains(renamed)) {
		_data[index].insert(renamed, value);
		_where.emplace(renamed, store);
	}
	_dirty[index] = true;
	scheduleSave();
}

QJsonValue Settings::lookup(const QString &key) const {
	const auto i = _where.find(key);
	if (i != _where.end()) {
		const auto result = _data[StoreIndex(i->second)].value(key);
		if (!result.isUndefined() && !result.isNull()) {
			return result;
		}
	}
	const auto &defaults = Defaults();
	const auto j = defaults.find(key);
	return (j != defaults.end()) ? j->second : QJsonValue();
}

bool Settings::getBool(const QString &key, bool def) const {
	const auto value = lookup(key);
	return value.isBool() ? value.toBool() : def;
}

int Settings::getInt(const QString &key, int def) const {
	const auto value = lookup(key);
	return value.isDouble() ? value.toInt() : def;
}

QString Settings::getString(const QString &key, const QString &def) const {
	const auto value = lookup(key);
	return value.isString() ? value.toString() : def;
}

QJsonArray Settings::getArray(const QString &key) const {
	return lookup(key).toArray();
}

QJsonObject Settings::getObject(const QString &key) const {
	return lookup(key).toObject();
}

void Settings::set(const QString &key, QJsonValue value, Store store) {
	if (value.isUndefined()) {
		remove(key);
	} else {
		apply(key, value, store);
	}
}

bool Settings::apply(
		const QString &key,
		const QJsonValue &value,
		Store store) {
	if (key.isEmpty() || key == StoresKey()) {
		return false;
	}
	const auto index = StoreIndex(store);
	const auto i = _where.find(key);
	const auto known = (i != _where.end());
	const auto was = known ? StoreIndex(i->second) : index;
	if (known && was == index && _data[index].value(key) == value) {
		return false;
	}
	const auto changed = !known || (_data[was].value(key) != value);
	if (known && was != index) {
		_data[was].remove(key);
		_dirty[was] = true;
		_where.remove(key);
	}
	if (!known || was != index) {
		_where.emplace(key, store);
	}
	_data[index].insert(key, value);
	_dirty[index] = true;
	scheduleSave();
	if (changed) {
		_changes.fire_copy(key);
	}
	return changed;
}

void Settings::remove(const QString &key) {
	const auto i = _where.find(key);
	if (i == _where.end()) {
		return;
	}
	const auto index = StoreIndex(i->second);
	_where.remove(key);
	_data[index].remove(key);
	_dirty[index] = true;
	scheduleSave();
	_changes.fire_copy(key);
}

rpl::producer<QString> Settings::changes() const {
	return _changes.events();
}

rpl::producer<> Settings::changesFor(QString key) const {
	return _changes.events(
	) | rpl::filter([key = std::move(key)](const QString &changed) {
		return (changed == key);
	}) | rpl::to_empty;
}

QJsonObject Settings::exportAll() const {
	auto result = QJsonObject();
	for (auto i = 0; i != kStoreCount; ++i) {
		const auto &data = _data[i];
		for (auto j = data.constBegin(); j != data.constEnd(); ++j) {
			result.insert(j.key(), j.value());
		}
	}
	auto stores = QJsonObject();
	for (const auto &[key, store] : _where) {
		if (store != Store::Prefs) {
			stores.insert(key, StoreName(store));
		}
	}
	if (!stores.isEmpty()) {
		result.insert(StoresKey(), stores);
	}
	return result;
}

void Settings::importAll(const QJsonObject &data) {
	auto declared = base::flat_map<QString, Store>();
	const auto stores = data.value(StoresKey()).toObject();
	for (auto i = stores.constBegin(); i != stores.constEnd(); ++i) {
		if (const auto store = StoreByName(i.value().toString())) {
			declared.emplace(i.key(), *store);
		}
	}
	for (auto i = data.constBegin(); i != data.constEnd(); ++i) {
		const auto key = i.key();
		if (key.isEmpty() || key == StoresKey()) {
			continue;
		}
		const auto j = declared.find(key);
		const auto k = _where.find(key);
		const auto store = (j != declared.end())
			? j->second
			: (k != _where.end())
			? k->second
			: Store::Prefs;
		if (!apply(key, i.value(), store)) {
			_changes.fire_copy(key);
		}
	}
}

void Settings::scheduleSave() {
	if (!_saveTimer.isActive()) {
		_saveTimer.callOnce(kSaveDelay);
	}
}

void Settings::saveNow() {
	_saveTimer.cancel();
	for (auto i = 0; i != kStoreCount; ++i) {
		// Keep the store dirty if the write failed, so that the next
		// saveNow() retries instead of silently dropping the change.
		if (_dirty[i] && writeStore(static_cast<Store>(i))) {
			_dirty[i] = false;
		}
	}
}

bool Settings::writeStore(Store store) {
	if (_loadFailed[StoreIndex(store)]) {
		// See _loadFailed: this store never loaded, so what is in memory is not
		// the user's data. Staying dirty loses nothing from this session's point
		// of view, and destroys nothing on disk.
		return false;
	}
	const auto path = FilePath(store);
	QDir().mkpath(cWorkingDir() + u"tdata"_q);

	const auto bytes = QJsonDocument(
		_data[StoreIndex(store)]
	).toJson(QJsonDocument::Indented);

	auto file = QSaveFile(path);
	if (!file.open(QIODevice::WriteOnly)) {
		LOG(("Lumina Error: Could not open '%1' for writing.").arg(path));
		return false;
	} else if (file.write(bytes) != bytes.size()) {
		// Returning without commit() leaves the old file untouched: the
		// QSaveFile destructor discards the temporary.
		LOG(("Lumina Error: Could not write '%1'.").arg(path));
		return false;
	} else if (!file.commit()) {
		LOG(("Lumina Error: Could not commit '%1'.").arg(path));
		return false;
	}
	return true;
}

bool Settings::hideChatFolders() const {
	return getBool(u"hideChatFolders"_q);
}

void Settings::setHideChatFolders(bool value) {
	set(u"hideChatFolders"_q, value);
}

bool Settings::stealthOnline() const {
	return getBool(u"stealthOnline"_q);
}

void Settings::setStealthOnline(bool value) {
	set(u"stealthOnline"_q, value);
}

bool Settings::stealthTyping() const {
	return getBool(u"stealthTyping"_q);
}

void Settings::setStealthTyping(bool value) {
	set(u"stealthTyping"_q, value);
}

bool Settings::stealthReadReceipts() const {
	return getBool(u"stealthReadReceipts"_q);
}

void Settings::setStealthReadReceipts(bool value) {
	set(u"stealthReadReceipts"_q, value);
}

bool Settings::forwardNoAuthor() const {
	return getBool(u"forwardNoAuthor"_q);
}

void Settings::setForwardNoAuthor(bool value) {
	set(u"forwardNoAuthor"_q, value);
}

bool Settings::allowSaveRestricted() const {
	return getBool(u"allowSaveRestricted"_q);
}

void Settings::setAllowSaveRestricted(bool value) {
	set(u"allowSaveRestricted"_q, value);
}

bool Settings::autoTranslate() const {
	return getBool(u"autoTranslate"_q);
}

void Settings::setAutoTranslate(bool value) {
	set(u"autoTranslate"_q, value);
}

} // namespace Lumina
