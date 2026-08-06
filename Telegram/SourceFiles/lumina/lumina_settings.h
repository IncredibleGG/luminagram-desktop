/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "base/flat_map.h"
#include "base/timer.h"

#include <rpl/event_stream.h>
#include <rpl/producer.h>

#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>

#include <array>

namespace Lumina {

// Which file a key is persisted to. A value behaves identically in all three;
// the split only limits how much has to be rewritten on a save, and keeps
// sensitive values out of the ordinary pref file.
enum class Store {
	Prefs,     // tdata/luminagram.json
	Private,   // tdata/luminagram_private.json
	Bookmarks, // tdata/luminagram_bookmarks.json
};

// A small, self-contained key/value store for LuminaGram-specific options.
//
// It intentionally does NOT touch Core::Settings' serialization blob (which is
// version-sensitive and easy to corrupt). Instead it persists tiny JSON files
// next to tdesktop's own data, one per Store.
//
// Main thread only. Getters are cheap and safe to call from hot-ish paths
// (context-menu building, status updates, etc). Writes are atomic (QSaveFile)
// and coalesced (~500ms), and a file that fails to parse is renamed to
// `*.corrupt` rather than dropped, so nothing is ever silently destroyed.
class Settings final {
public:
	// !! NOT SAFE BEFORE QApplication EXISTS. The first call constructs the
	// store, and _saveTimer is a base::Timer, which is a QObject. Calling this
	// from a namespace-scope initialiser crashes the app before it can even
	// open its log - which is exactly what a Wave 2 registrar did. Register
	// callbacks from static init if you must, but never read a preference
	// there.
	[[nodiscard]] static Settings &Instance();

	[[nodiscard]] bool getBool(const QString &key, bool def = false) const;
	[[nodiscard]] int getInt(const QString &key, int def = 0) const;
	[[nodiscard]] QString getString(
		const QString &key,
		const QString &def = {}) const;
	[[nodiscard]] QJsonArray getArray(const QString &key) const;
	[[nodiscard]] QJsonObject getObject(const QString &key) const;

	// The store passed here becomes the key's store: passing a different one
	// than before MOVES the value from one file to the other.
	//
	// WARNING: the argument defaults to Store::Prefs, so a bare
	// set(key, value) on a key that lives in Store::Private relocates that
	// value into the plain pref file. Every owner of a Store::Private key
	// must funnel all of its writes through one setter that names the store.
	void set(
		const QString &key,
		QJsonValue value,
		Store store = Store::Prefs);
	void remove(const QString &key);

	// Emits the key that changed.
	[[nodiscard]] rpl::producer<QString> changes() const;
	[[nodiscard]] rpl::producer<> changesFor(QString key) const;

	// For lumina_backup.
	[[nodiscard]] QJsonObject exportAll() const;
	void importAll(const QJsonObject &data); // fires changes() per key

	// Flushes the debounced write. Called on destruction as well.
	void saveNow();

	// Named accessors for the keys that already have callers in the tree.
	// New code should prefer getBool() / set() with a key.
	[[nodiscard]] bool hideChatFolders() const;
	void setHideChatFolders(bool value);

	// Grey / ToS-sensitive. The keys stay, the rows are hidden by F-02.
	[[nodiscard]] bool stealthOnline() const;
	void setStealthOnline(bool value);
	[[nodiscard]] bool stealthTyping() const;
	void setStealthTyping(bool value);
	[[nodiscard]] bool stealthReadReceipts() const;
	void setStealthReadReceipts(bool value);

	[[nodiscard]] bool forwardNoAuthor() const;
	void setForwardNoAuthor(bool value);
	[[nodiscard]] bool allowSaveRestricted() const;
	void setAllowSaveRestricted(bool value);
	[[nodiscard]] bool autoTranslate() const;
	void setAutoTranslate(bool value);

	// Deprecated spelling, kept only so the current settings page keeps
	// compiling. Remove together with its last caller in F-02.
	[[nodiscard]] bool forwardWithoutAuthor() const {
		return forwardNoAuthor();
	}
	void setForwardWithoutAuthor(bool value) {
		setForwardNoAuthor(value);
	}

private:
	static constexpr auto kStoreCount = 3;

	Settings();
	~Settings();

	void load();
	void loadStore(Store store);
	void migrateLegacyKeys();

	[[nodiscard]] QJsonValue lookup(const QString &key) const;
	bool apply(const QString &key, const QJsonValue &value, Store store);
	void scheduleSave();
	bool writeStore(Store store); // false keeps the store dirty for a retry.

	// Defaults live in ONE table, not in scattered call sites.
	[[nodiscard]] static auto Defaults()
	-> const base::flat_map<QString, QJsonValue> &;

	std::array<QJsonObject, kStoreCount> _data;
	std::array<bool, kStoreCount> _dirty = { false, false, false };
	base::flat_map<QString, Store> _where;
	base::Timer _saveTimer;
	rpl::event_stream<QString> _changes;

};

} // namespace Lumina
