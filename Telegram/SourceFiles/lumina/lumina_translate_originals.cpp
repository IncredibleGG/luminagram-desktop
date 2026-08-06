/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_translate_originals.h"

#include "base/flat_map.h"
#include "base/flat_set.h"
#include "base/unixtime.h"
#include "data/data_peer.h"
#include "data/data_session.h"
#include "history/history.h"
#include "history/history_item.h"
#include "lumina/lumina_dual_language_line.h"
#include "lumina/lumina_settings.h"
#include "lumina/lumina_translate_gating.h"
#include "lumina/lumina_translate_send.h"
#include "main/main_session.h"

#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <crl/crl_on_main.h>
#include <crl/crl_time.h>
#include <rpl/producer.h>

#include <algorithm>
#include <optional>
#include <utility>
#include <vector>

namespace Lumina {
namespace {

constexpr auto kMaxEntries = 500;
constexpr auto kMaxAge = TimeId(90 * 24 * 60 * 60);
constexpr auto kArmedLifetime = crl::time(60 * 1000);

[[nodiscard]] QString StoreKey() {
	return u"tbsOriginals"_q;
}

struct Key {
	uint64 session = 0;
	FullMsgId id;

	friend inline auto operator<=>(Key, Key) = default;
	friend inline bool operator==(Key, Key) = default;
};

struct Entry {
	QString original;
	TimeId time = 0;
	uint64 order = 0;
};

// One original waiting for the message id that ApiWrap::sendMessage() is about
// to mint for it. There is only ever one, because W2-A's hook fires once per
// translated send and the composer's `proceed` runs to the mint without
// returning to the event loop.
struct Armed {
	uint64 session = 0;
	PeerId peer;
	QString original;
	crl::time when = 0;
};

struct State {
	base::flat_map<Key, Entry> map;
	base::flat_set<uint64> hooked;
	std::optional<Armed> armed;
	uint64 order = 0;
	bool loaded = false;
};

[[nodiscard]] State &Current() {
	static auto result = State();
	return result;
}

// A client-side id is reassigned from StartClientMsgId on every launch, so an
// entry under one is meaningful only for as long as this process lives.
[[nodiscard]] bool Persistable(const Key &key) {
	return key.id.msg && !IsClientMsgId(key.id.msg);
}

void Save() {
	auto &state = Current();
	auto ordered = std::vector<std::pair<uint64, QJsonObject>>();
	ordered.reserve(state.map.size());
	for (const auto &[key, entry] : state.map) {
		if (!Persistable(key)) {
			continue;
		}
		auto object = QJsonObject();
		object.insert(u"s"_q, QString::number(key.session));
		object.insert(u"p"_q, QString::number(key.id.peer.value));
		object.insert(u"m"_q, QString::number(key.id.msg.bare));
		object.insert(u"o"_q, entry.original);
		object.insert(u"t"_q, entry.time);
		ordered.emplace_back(entry.order, std::move(object));
	}
	if (ordered.empty()) {
		Settings::Instance().remove(StoreKey());
		return;
	}
	std::sort(
		ordered.begin(),
		ordered.end(),
		[](const auto &a, const auto &b) { return a.first < b.first; });

	auto array = QJsonArray();
	for (const auto &pair : ordered) {
		array.append(pair.second);
	}
	Settings::Instance().set(StoreKey(), array, Store::Private);
}

// True when something that was on disk is now gone, i.e. the caller has to
// rewrite the file even if it did not add anything itself.
[[nodiscard]] bool Prune() {
	auto &state = Current();
	const auto now = base::unixtime::now();
	auto persisted = false;
	for (auto i = state.map.begin(); i != state.map.end();) {
		if (now - i->second.time > kMaxAge) {
			persisted = persisted || Persistable(i->first);
			i = state.map.erase(i);
		} else {
			++i;
		}
	}
	while (int(state.map.size()) > kMaxEntries) {
		auto oldest = state.map.begin();
		for (auto i = state.map.begin(); i != state.map.end(); ++i) {
			if (i->second.order < oldest->second.order) {
				oldest = i;
			}
		}
		persisted = persisted || Persistable(oldest->first);
		state.map.erase(oldest);
	}
	return persisted;
}

void EnsureLoaded() {
	auto &state = Current();
	if (state.loaded) {
		return;
	}
	state.loaded = true;

	const auto array = Settings::Instance().getArray(StoreKey());
	auto dropped = false;
	for (auto i = 0, count = int(array.size()); i != count; ++i) {
		const auto object = array.at(i).toObject();
		auto sessionOk = false;
		auto peerOk = false;
		auto msgOk = false;
		const auto session = uint64(
			object.value(u"s"_q).toString().toULongLong(&sessionOk));
		const auto peer = PeerId(
			object.value(u"p"_q).toString().toULongLong(&peerOk));
		const auto msg = MsgId(
			object.value(u"m"_q).toString().toLongLong(&msgOk));
		const auto original = object.value(u"o"_q).toString();
		const auto time = TimeId(object.value(u"t"_q).toInt());
		const auto key = Key{ session, FullMsgId(peer, msg) };
		if (!sessionOk
			|| !peerOk
			|| !msgOk
			|| !session
			|| !peer
			|| (time <= 0)
			|| original.isEmpty()
			|| !Persistable(key)) {
			dropped = true;
			continue;
		}
		const auto added = state.map.emplace_or_assign(
			key,
			Entry{ original, time, ++state.order }).second;
		if (!added) {
			dropped = true;
		}
	}
	if (Prune() || dropped) {
		// SentOriginalText() calls this from Element::validateText(), i.e.
		// from inside a layout pass, and Settings::set() fires changes()
		// synchronously. Rewriting the file is never urgent - the entries it
		// drops are already gone from the map that answers every lookup - so
		// it goes to the next main-thread turn rather than re-entering
		// whatever is being laid out.
		crl::on_main([] { Save(); });
	}
}

void Unhook(uint64 session) {
	auto &state = Current();
	state.hooked.remove(session);
	if (state.armed && state.armed->session == session) {
		state.armed = std::nullopt;
	}
	for (auto i = state.map.begin(); i != state.map.end();) {
		if (i->first.session == session && !Persistable(i->first)) {
			i = state.map.erase(i);
		} else {
			++i;
		}
	}
}

// Drops the stored original for one message.
//
// NOT wired to Data::Session::itemRemoved, and this is load-bearing: that
// signal comes from Session::unregisterMessage(), which runs from
// ~HistoryItem() - i.e. every time a message object is freed, including the
// ordinary memory churn of leaving a chat or reloading history. Hooking it
// deleted the user's typed original while the message was still on screen,
// which is exactly the "the original disappeared again" bug they reported.
// MessageUpdate::Flag::Destroyed is fired from the same place and is no better.
//
// Real deletion would have to be caught in the delete flow itself. Until then
// the plaintext is bounded by the retention policy above, which is what keeps
// it from growing without limit.
[[maybe_unused]] void Forget(uint64 session, FullMsgId id) {
	auto &state = Current();
	const auto key = Key{ session, id };
	if (!state.map.remove(key)) {
		return;
	}
	if (Persistable(key)) {
		Save();
	}
}

[[nodiscard]] bool Rekey(uint64 session, FullMsgId newId, MsgId oldId) {
	auto &state = Current();
	if (!oldId || !newId.msg || oldId == newId.msg) {
		return false;
	}
	const auto from = Key{ session, FullMsgId(newId.peer, oldId) };
	auto entry = state.map.take(from);
	if (!entry) {
		return false;
	}
	const auto to = Key{ session, newId };
	state.map.emplace_or_assign(to, std::move(*entry));
	if (Prune() || Persistable(to) || Persistable(from)) {
		Save();
	}
	return true;
}

// Data::Session::IdChange carries no account marker and local ids repeat
// across accounts, so `unique` is captured by value here rather than read back
// off the session - the teardown callback also runs while the session is being
// torn down. The session pointer itself is safe inside the id-change
// subscription: that subscription is owned by session->lifetime().
void EnsureHooked(not_null<Main::Session*> session) {
	const auto unique = session->uniqueId();
	if (!Current().hooked.emplace(unique).second) {
		return;
	}
	session->data().itemIdChanged(
	) | rpl::on_next([=](const Data::Session::IdChange &change) {
		if (!Rekey(unique, change.newId, change.oldId)) {
			return;
		}
		// The bubble was laid out under the local id, found no original and
		// will not look again on its own. This is the exact point Android
		// lost the original for good.
		if (const auto item = session->data().message(change.newId)) {
			RefreshDualLanguage(item);
		}
	}, session->lifetime());
	session->lifetime().add([=] {
		Unhook(unique);
	});
}

void Remember(
		not_null<Main::Session*> session,
		FullMsgId id,
		const QString &original) {
	EnsureLoaded();
	EnsureHooked(session);

	auto &state = Current();
	const auto key = Key{ session->uniqueId(), id };
	state.map.emplace_or_assign(
		key,
		Entry{ original, base::unixtime::now(), ++state.order });
	if (Prune() || Persistable(key)) {
		Save();
	}
}

// The send that is being armed here reaches ApiWrap::sendMessage() without
// returning to the event loop - all three composers call it straight out of
// the `proceed` W2-A invokes immediately after this hook. So the arm only has
// to survive the current turn, and dropping it at the end of that turn is what
// keeps a send that never reached apiwrap (a rejected slowmode send, a dice
// emoji taking the media path, a destroyed section widget) from binding its
// original to whatever the user sends next. The wall-clock check below is the
// same guarantee written a second way, for a queue that never drains.
void Arm(not_null<History*> history, const QString &original) {
	Current().armed = Armed{
		history->session().uniqueId(),
		history->peer->id,
		original,
		crl::now(),
	};
	crl::on_main([] {
		Current().armed = std::nullopt;
	});
}

} // namespace

void SetupSentOriginals() {
	static auto registered = false;
	if (registered) {
		return;
	}
	registered = true;
	SetSendOriginalHook([](
			not_null<History*> history,
			const QString &, // sentText, re-read from apiwrap instead.
			const QString &originalText) {
		if (!originalText.trimmed().isEmpty()) {
			Arm(history, originalText);
		}
	});

	// The renderer deliberately does not include this header, so the store
	// pushes the lookup at it rather than being pulled - see
	// lumina/lumina_dual_language_line.h. Without this the outgoing half of
	// the dual-language bubble stays inert no matter what is stored here.
	SetOutgoingOriginalLookup([](not_null<const HistoryItem*> item) {
		return SentOriginalText(item);
	});
}

void NoteOutgoingText(
		not_null<Main::Session*> session,
		FullMsgId id,
		const QString &text) {
	auto &state = Current();
	if (!state.armed || !id.msg) {
		return;
	} else if (state.armed->session != session->uniqueId()
		|| state.armed->peer != id.peer
		|| (crl::now() - state.armed->when > kArmedLifetime)) {
		return;
	}
	auto original = std::move(state.armed->original);
	state.armed = std::nullopt;
	if (original.isEmpty() || original == text) {
		return;
	}
	Remember(session, id, original);
}

QString SentOriginalText(not_null<const HistoryItem*> item) {
	return item->out()
		? SentOriginalText(&item->history()->session(), item->fullId())
		: QString();
}

QString SentOriginalText(not_null<Main::Session*> session, FullMsgId id) {
	if (!id.msg || !TranslationFeatureEnabled()) {
		return QString();
	}
	EnsureLoaded();

	auto &state = Current();
	const auto i = state.map.find(Key{ session->uniqueId(), id });
	return (i != state.map.end()) ? i->second.original : QString();
}

namespace {

// Both ends of the hook are function-local statics, so installing it from
// dynamic initialization is order-independent, and nothing it touches is read
// until a message is actually sent. This mirrors the Registrar in
// lumina_translate_send.cpp; if an explicit Lumina init point is ever added,
// both should move into it.
struct Registrar {
	Registrar() {
		SetupSentOriginals();
	}
};

[[maybe_unused]] const auto kRegistrar = Registrar();

} // namespace

} // namespace Lumina
