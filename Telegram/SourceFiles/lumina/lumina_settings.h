/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include <rpl/event_stream.h>
#include <rpl/producer.h>

namespace Lumina {

// A small, self-contained preference store for LuminaGram-specific options.
//
// It intentionally does NOT touch Core::Settings' serialization blob (which is
// version-sensitive and easy to corrupt). Instead it persists a tiny JSON file
// next to tdesktop's own data: `<workingDir>/tdata/luminagram.json`.
//
// Access through Lumina::Settings::Instance(). All getters are cheap and safe
// to call from hot-ish paths (context-menu building, status updates, etc).
class Settings final {
public:
	[[nodiscard]] static Settings &Instance();

	// Fired after any value changes (already persisted).
	[[nodiscard]] rpl::producer<> changes() const {
		return _changes.events();
	}

#define LUMINA_BOOL_PREF(name, Name) \
	[[nodiscard]] bool name() const { return _##name; } \
	void set##Name(bool value) { setBool(_##name, value); }

	// Interface.
	LUMINA_BOOL_PREF(hideChatFolders, HideChatFolders)

	// Privacy / stealth (safe defaults: OFF, i.e. behave like vanilla).
	LUMINA_BOOL_PREF(stealthOnline, StealthOnline)         // appear offline
	LUMINA_BOOL_PREF(stealthTyping, StealthTyping)         // no typing/status
	LUMINA_BOOL_PREF(stealthReadReceipts, StealthReadReceipts) // no read ticks

	// Message actions.
	LUMINA_BOOL_PREF(forwardWithoutAuthor, ForwardWithoutAuthor)
	// ToS-sensitive: allow local save/copy/download from restricted chats.
	// MUST default to OFF.
	LUMINA_BOOL_PREF(allowSaveRestricted, AllowSaveRestricted)

	// Translation.
	LUMINA_BOOL_PREF(autoTranslate, AutoTranslate)

#undef LUMINA_BOOL_PREF

private:
	Settings();

	void load();
	void save() const;
	void setBool(bool &field, bool value);

	bool _hideChatFolders = false;
	bool _stealthOnline = false;
	bool _stealthTyping = false;
	bool _stealthReadReceipts = false;
	bool _forwardWithoutAuthor = false;
	bool _allowSaveRestricted = false;
	bool _autoTranslate = false;

	bool _loaded = false;
	mutable rpl::event_stream<> _changes;

};

} // namespace Lumina
