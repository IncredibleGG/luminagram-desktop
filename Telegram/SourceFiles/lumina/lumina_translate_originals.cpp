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
#include "data/data_user.h"
#include "history/history.h"
#include "history/history_item.h"
#include "lumina/lumina_dual_language_line.h"
#include "lumina/lumina_settings.h"
#include "lumina/lumina_text_replace.h"
#include "lumina/lumina_translate_gating.h"
#include "lumina/lumina_translate_send.h"
#include "main/main_session.h"
#include "ui/item_text_options.h"
#include "ui/text/text_entity.h"

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

// Far more than the number of sends that can be waiting for an id at once -
// undo-send holds one message at a time - and here only so that an arm nothing
// ever claims cannot accumulate.
constexpr auto kMaxArmed = 8;

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
// to mint for it.
//
// `sent` is what the text W2-A released will look like once apiwrap has
// finished with it (AsSent() below), not the provider's raw answer, and
// `sameTurn` says whether the mint can still be the one this arm was made for
// without any further proof. See Arm() and FindArmed() below - between them
// they are the whole reason a translated send that another interceptor holds
// for a few seconds still keeps its original.
struct Armed {
	uint64 session = 0;
	PeerId peer;
	QString original;
	QString sent;
	crl::time when = 0;
	uint64 id = 0;
	bool sameTurn = false;
};

struct State {
	base::flat_map<Key, Entry> map;
	base::flat_set<uint64> hooked;

	// More than one, because more than one translated send can be waiting: the
	// hook fires when the translate pipeline releases a send, and undo-send
	// then holds it for five seconds, so a second translated send arriving
	// inside that window arms a second original while the first is still on its
	// way to apiwrap. A single slot lost the first one - which is the whole
	// original, and it exists nowhere else.
	std::vector<Armed> armed;

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
	for (auto i = state.armed.begin(); i != state.armed.end();) {
		if (i->session == session) {
			i = state.armed.erase(i);
		} else {
			++i;
		}
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

// What the text W2-A just released will actually look like by the time
// ApiWrap::sendMessage() mints an id for it, which is the string the exact
// match in FindArmed() below has to be made of.
//
// Between the hook and the mint apiwrap rewrites the text twice, in this order:
// Lumina::ApplyOutgoingTextReplacements() and then
// TextUtilities::PrepareForSending() with Ui::ItemTextOptions - which removes
// '\r', turns each tab into two spaces, replaces the characters the server will
// not take, parses markdown markers out of the text (ItemTextOptions carries
// TextParseMarkdown) and trims. Arming the provider's raw answer instead means
// arming a string no outgoing message can ever equal whenever the translation
// carries a CRLF, a tab or a markdown character, or whenever the user has an
// outgoing replacement rule that matches it. Inside its own turn the arm is
// still accepted without any such proof, so this only ever mattered for a send
// another interceptor holds past that turn - which is exactly the undo-send
// case the whole mechanism below exists for, i.e. every case where the exact
// match is the only thing left.
//
// Both steps are pure functions of the text, so running them here cannot change
// what is sent; they only say what to expect.
[[nodiscard]] QString AsSent(
		not_null<History*> history,
		const QString &text) {
	auto replaced = TextWithTags{ text };
	ApplyOutgoingTextReplacements(replaced);
	auto prepared = TextWithEntities{ std::move(replaced.text) };
	TextUtilities::PrepareForSending(
		prepared,
		Ui::ItemTextOptions(history, history->session().user()).flags);
	return prepared.text;
}

// THE ARM IS NOT DROPPED AT THE END OF THE TURN, AND THAT IS THE FIX FOR A
// FEATURE THAT WAS OTHERWISE DEAD FOR ANYONE WITH UNDO-SEND SWITCHED ON.
//
// W2-A invokes this hook immediately before its `proceed`, and `proceed` is not
// the composer: it is the rest of the interceptor chain
// (lumina/lumina_send_pipeline.h). Undo-send is registered on that chain after
// the translate pipeline, deliberately and from Core::Application::run(), and
// it holds a text send for five seconds. ApiWrap::sendMessage() - the one place
// an id is minted, and therefore the only place NoteOutgoingText() can run - is
// then reached several turns later. Dropping the arm at the end of the turn it
// was made in meant every translated send made with undo-send on recorded no
// original at all, and the bubble degraded to translation-only with no way
// back.
//
// So the turn boundary is a demotion rather than a deletion. Inside the turn
// the arm is accepted as it always was, which keeps every send that does reach
// apiwrap synchronously recorded exactly as before. After the turn it is
// accepted only for a message whose outgoing text is character-for-character
// what AsSent() above says this translation becomes on the way to the wire,
// which is the exact-sent-text match Android needed for the same reason. A send
// that never reached apiwrap therefore cannot bind its original to whatever the
// user sends next: the text would have to be identical, and then the original
// is right. kArmedLifetime remains the outer bound in both cases.
void Arm(
		not_null<History*> history,
		const QString &sent,
		const QString &original) {
	static auto counter = uint64(0);
	const auto id = ++counter;
	const auto now = crl::now();
	auto &armed = Current().armed;
	for (auto i = armed.begin(); i != armed.end();) {
		if (now - i->when > kArmedLifetime) {
			i = armed.erase(i);
		} else {
			++i;
		}
	}
	while (int(armed.size()) >= kMaxArmed) {
		armed.erase(armed.begin());
	}
	armed.push_back(Armed{
		history->session().uniqueId(),
		history->peer->id,
		original,
		AsSent(history, sent),
		now,
		id,
		true,
	});
	crl::on_main([id] {
		for (auto &entry : Current().armed) {
			if (entry.id == id) {
				entry.sameTurn = false;
				return;
			}
		}
	});
}

// The armed original this outgoing chunk belongs to, or -1.
//
// An exact sent-text match wins wherever it is found, because it is proof. The
// arm made in this very turn is the fallback, and it is what covers a message
// whose text apiwrap rewrote in a way AsSent() above cannot reproduce - that is
// the case the exact match cannot see, and it is only safe to guess at inside
// the turn.
//
// Scanned oldest first, both for the match and for the fallback, because ids
// are minted in the order the sends were released: with two arms alive in one
// chat - which is what undo-send holding the first send while a second is
// translated produces - the newest-first scan handed the first message the
// second message's original.
[[nodiscard]] int FindArmed(
		uint64 session,
		PeerId peer,
		const QString &text) {
	const auto &armed = Current().armed;
	const auto now = crl::now();
	auto fallback = -1;
	for (auto i = 0, count = int(armed.size()); i != count; ++i) {
		const auto &entry = armed[i];
		if ((entry.session != session)
			|| (entry.peer != peer)
			|| (now - entry.when > kArmedLifetime)) {
			continue;
		} else if (!entry.sent.isEmpty() && (entry.sent == text)) {
			return i;
		} else if (entry.sameTurn && (fallback < 0)) {
			fallback = i;
		}
	}
	return fallback;
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
			const QString &sentText,
			const QString &originalText) {
		if (!originalText.trimmed().isEmpty()) {
			Arm(history, sentText, originalText);
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
	if (state.armed.empty() || !id.msg) {
		return;
	}
	const auto index = FindArmed(session->uniqueId(), id.peer, text);
	if (index < 0) {
		// Nothing armed answers for this message. Whatever is armed is left
		// alone rather than dropped: the send it belongs to may still be
		// waiting behind another interceptor, and past its own turn it can only
		// ever be claimed by a message carrying that exact translation.
		// kArmedLifetime is what ends it otherwise.
		return;
	}
	auto original = std::move(state.armed[index].original);
	state.armed.erase(state.armed.begin() + index);
	if (original.isEmpty() || (original == text)) {
		return;
	}
	Remember(session, id, original);
}

QString SentOriginalText(not_null<const HistoryItem*> item) {
	// out() alone would miss Saved Messages: NewMessageFlags() withholds
	// MessageFlag::Outgoing for a self-chat send, so an own message there is not
	// out(). The store is keyed on fullId(), which a self-chat item resolves the
	// same way it was bound (its history peer is self), so widening the gate to
	// self-chat cannot return anything but this message's own stored original.
	return (item->out() || item->history()->peer->isSelf())
		? SentOriginalText(&item->history()->session(), item->fullId())
		: QString();
}

QString SentOriginalText(not_null<Main::Session*> session, FullMsgId id) {
	if (!id.msg || !ContinuousTranslationAvailable()) {
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
