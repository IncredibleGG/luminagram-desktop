/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_chat_lock.h"

#include "base/flat_set.h"
#include "data/data_peer.h"
#include "history/history.h"
#include "lumina/lumina_settings.h"
#include "lumina/lumina_vault.h"

#include <QtCore/QJsonArray>
#include <QtCore/QJsonValue>

namespace Lumina::ChatLock {
namespace {

// Same key names Android uses, both in Store::Private so the ids and the code
// never land in the plaintext pref file. Settings::set() defaults to
// Store::Prefs, so WriteLocked() and the code writer are the only writers and
// they always name the store.
const auto kKeyLocked = u"lockedChats"_q;
const auto kKeyCode = u"chatLockCode"_q;
constexpr auto kStore = Store::Private;

struct State {
	base::flat_set<PeerId> locked;
	rpl::event_stream<> changes;
	rpl::lifetime lifetime;
	bool loaded = false;
	bool subscribed = false;
	bool revealed = false;
};

[[nodiscard]] State &Current() {
	// Lazy, so nothing here runs during static initialisation: the first call
	// arrives from the dialogs list long after QApplication exists. State holds
	// only rpl primitives (no QObject), so constructing it on first use is
	// safe.
	static auto result = State();
	return result;
}

void Reload() {
	auto &state = Current();
	state.locked.clear();
	const auto array = Settings::Instance().getArray(kKeyLocked);
	for (const auto &value : array) {
		auto ok = false;
		const auto raw = value.toString().toULongLong(&ok);
		if (ok && raw) {
			state.locked.emplace(PeerId(BareId(raw)));
		}
	}
}

void EnsureLoaded() {
	auto &state = Current();
	if (!state.subscribed) {
		state.subscribed = true;

		// Any write to the key - our own Lock()/Unlock(), or a whole-file
		// backup restore through Settings::importAll() - reparses and notifies.
		// The file, not this set, is the single source of truth.
		Settings::Instance().changesFor(
			kKeyLocked
		) | rpl::on_next([] {
			Reload();
			Current().changes.fire({});
		}, state.lifetime);
	}
	if (!state.loaded) {
		state.loaded = true;
		Reload();
	}
}

void WriteLocked() {
	auto array = QJsonArray();
	for (const auto &id : Current().locked) {
		array.append(QString::number(qulonglong(id.value)));
	}
	if (array.isEmpty()) {
		Settings::Instance().remove(kKeyLocked);
	} else {
		Settings::Instance().set(kKeyLocked, array, kStore);
	}
}

} // namespace

bool IsLocked(PeerData *peer) {
	if (!peer) {
		return false;
	}
	EnsureLoaded();
	return Current().locked.contains(peer->id);
}

void Lock(PeerData *peer) {
	if (!peer) {
		return;
	}
	EnsureLoaded();
	auto &state = Current();
	if (state.locked.emplace(peer->id).second) {
		// Fires changesFor(kKeyLocked) -> Reload() + changes.fire().
		WriteLocked();
	}
}

void Unlock(PeerData *peer) {
	if (!peer) {
		return;
	}
	EnsureLoaded();
	auto &state = Current();
	if (state.locked.remove(peer->id)) {
		WriteLocked();
	}
}

bool Revealed() {
	return Current().revealed;
}

void Reveal() {
	auto &state = Current();
	if (!state.revealed) {
		state.revealed = true;
		state.changes.fire({});
	}
}

bool Hidden(PeerData *peer) {
	if (!peer) {
		return false;
	}
	EnsureLoaded();
	auto &state = Current();
	return !state.revealed && state.locked.contains(peer->id);
}

bool Hidden(History *history) {
	return history && Hidden(history->peer);
}

QString EffectiveCode() {
	const auto own = Settings::Instance().getString(kKeyCode).trimmed();
	return own.isEmpty() ? VaultCode() : own;
}

bool HasSecretCode() {
	return !EffectiveCode().isEmpty();
}

bool MaybeRevealFromQuery(const QString &text) {
	EnsureLoaded();
	auto &state = Current();
	if (state.revealed || state.locked.empty()) {
		return false;
	}
	const auto code = EffectiveCode();
	if (code.isEmpty() || (text.trimmed() != code)) {
		return false;
	}
	Reveal();
	return true;
}

rpl::producer<> Changes() {
	EnsureLoaded();
	return Current().changes.events();
}

} // namespace Lumina::ChatLock
