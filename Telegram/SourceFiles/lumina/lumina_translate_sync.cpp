/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_translate_sync.h"

#include "api/api_common.h"
#include "api/api_editing.h"
#include "apiwrap.h"
#include "base/bytes.h"
#include "base/flat_map.h"
#include "base/flat_set.h"
#include "base/openssl_help.h"
#include "base/random.h"
#include "base/timer.h"
#include "base/weak_ptr.h"
#include "data/data_drafts.h"
#include "data/data_peer.h"
#include "data/data_session.h"
#include "data/data_user.h"
#include "history/history.h"
#include "history/history_item.h"
#include "history/view/history_view_element.h"
#include "lumina/lumina_settings.h"
#include "main/main_session.h"
#include "ui/text/text_entity.h"
#include "window/window_session_controller.h"

#include <openssl/evp.h>

#include <QtCore/QDateTime>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
#include <QtCore/QStringList>
#include <QtCore/QUuid>

#include <cstring>
#include <memory>
#include <optional>

namespace Lumina {
namespace {

// How long after the last settings change the carrier is (re)written. Coalesces
// a burst of related toggles into one edit, and stays off the UI thread's back.
constexpr auto kPushDelay = crl::time(1500);

// Persistence keys. All Store::Private except the device id, which is a plain
// per-install token with nothing sensitive in it.
const auto kKeyDeviceId = u"trSyncDeviceId"_q;        // Store::Prefs, string
const auto kKeyCarrierMsgId = u"trSyncCarrierMsgId"_q; // Private, obj uid->str
const auto kKeyBase = u"trSyncBase"_q;                 // Private, obj uid->obj

// Desktop-owned store keys (the local spelling of the per-dialog maps).
const auto kStoreSendActive = u"trSendChatOn"_q;
const auto kStoreSendLang = u"trSendLangDialog"_q;
const auto kStoreRegister = u"trRegisterDialog"_q;

// Wire spelling of the send-enabled per-dialog map (NAME REMAP vs desktop).
const auto kWireSendActive = u"trSendEnabledDialog"_q;

[[nodiscard]] const QString &Marker() {
	// U+2064 U+2064 "LG-SYNC1:". The two invisible code points are written as
	// their explicit UTF-8 bytes (E2 81 A4 twice) so the source stays plain
	// ASCII and the on-wire marker is byte-identical to iOS/Android.
	static const auto result = QString::fromUtf8(
		"\xe2\x81\xa4\xe2\x81\xa4LG-SYNC1:");
	return result;
}

// ---- crypto ---------------------------------------------------------------

[[nodiscard]] QByteArray DeriveKey(not_null<Main::Session*> session) {
	// key = HMAC-SHA256(appSecret, ASCII decimal of userId().bare) -> 32 bytes.
	static const unsigned char secret[32] = {
		0x4c, 0x47, 0x52, 0x6f, 0x61, 0x6d, 0x53, 0x79,
		0x6e, 0x63, 0x31, 0xa7, 0x3e, 0x91, 0xd2, 0x58,
		0x0b, 0xf4, 0x6c, 0x22, 0x9d, 0x71, 0xe8, 0x35,
		0x4a, 0xc9, 0x17, 0x60, 0xbe, 0x83, 0x2f, 0xd1 };
	const auto idText = QByteArray::number(qlonglong(session->userId().bare));
	const auto mac = openssl::HmacSha256(
		bytes::make_span(secret, 32),
		bytes::make_span(idText.constData(), idText.size()));
	return QByteArray(
		reinterpret_cast<const char*>(mac.data()),
		int(mac.size()));
}

// AES-256-GCM, no associated data, 12-byte nonce, 16-byte tag. The EVP
// sequence is the one webauthn/cable_core.cpp uses, transcribed onto
// QByteArray. Returns ciphertext||tag, or empty on failure.
[[nodiscard]] QByteArray AesGcmEncrypt(
		const QByteArray &key32,
		const QByteArray &nonce12,
		const QByteArray &plain) {
	auto result = QByteArray(plain.size() + 16, char(0));
	const auto context = EVP_CIPHER_CTX_new();
	if (!context) {
		return QByteArray();
	}
	auto outLength = 0;
	auto fullLength = 0;
	auto ok = (EVP_EncryptInit_ex(
			context,
			EVP_aes_256_gcm(),
			nullptr,
			nullptr,
			nullptr) == 1)
		&& (EVP_CIPHER_CTX_ctrl(
			context,
			EVP_CTRL_GCM_SET_IVLEN,
			int(nonce12.size()),
			nullptr) == 1)
		&& (EVP_EncryptInit_ex(
			context,
			nullptr,
			nullptr,
			reinterpret_cast<const unsigned char*>(key32.constData()),
			reinterpret_cast<const unsigned char*>(nonce12.constData()))
				== 1);
	if (ok && plain.size() > 0) {
		ok = (EVP_EncryptUpdate(
			context,
			reinterpret_cast<unsigned char*>(result.data()),
			&outLength,
			reinterpret_cast<const unsigned char*>(plain.constData()),
			int(plain.size())) == 1);
		fullLength = outLength;
	}
	ok = ok
		&& (EVP_EncryptFinal_ex(
			context,
			reinterpret_cast<unsigned char*>(result.data()) + fullLength,
			&outLength) == 1)
		&& (EVP_CIPHER_CTX_ctrl(
			context,
			EVP_CTRL_GCM_GET_TAG,
			16,
			reinterpret_cast<unsigned char*>(result.data()) + plain.size())
				== 1);
	EVP_CIPHER_CTX_free(context);
	return ok ? result : QByteArray();
}

[[nodiscard]] std::optional<QByteArray> AesGcmDecrypt(
		const QByteArray &key32,
		const QByteArray &nonce12,
		const QByteArray &cipherAndTag) {
	if (cipherAndTag.size() < 16) {
		return std::nullopt;
	}
	const auto plainLength = int(cipherAndTag.size()) - 16;
	auto result = QByteArray(plainLength, char(0));
	unsigned char tag[16];
	memcpy(tag, cipherAndTag.constData() + plainLength, 16);
	const auto context = EVP_CIPHER_CTX_new();
	if (!context) {
		return std::nullopt;
	}
	auto outLength = 0;
	auto fullLength = 0;
	auto ok = (EVP_DecryptInit_ex(
			context,
			EVP_aes_256_gcm(),
			nullptr,
			nullptr,
			nullptr) == 1)
		&& (EVP_CIPHER_CTX_ctrl(
			context,
			EVP_CTRL_GCM_SET_IVLEN,
			int(nonce12.size()),
			nullptr) == 1)
		&& (EVP_DecryptInit_ex(
			context,
			nullptr,
			nullptr,
			reinterpret_cast<const unsigned char*>(key32.constData()),
			reinterpret_cast<const unsigned char*>(nonce12.constData()))
				== 1);
	if (ok && plainLength > 0) {
		ok = (EVP_DecryptUpdate(
			context,
			reinterpret_cast<unsigned char*>(result.data()),
			&outLength,
			reinterpret_cast<const unsigned char*>(cipherAndTag.constData()),
			plainLength) == 1);
		fullLength = outLength;
	}
	ok = ok
		&& (EVP_CIPHER_CTX_ctrl(
			context,
			EVP_CTRL_GCM_SET_TAG,
			16,
			tag) == 1)
		&& (EVP_DecryptFinal_ex(
			context,
			reinterpret_cast<unsigned char*>(result.data()) + fullLength,
			&outLength) == 1);
	EVP_CIPHER_CTX_free(context);
	if (!ok) {
		return std::nullopt;
	}
	return result;
}

[[nodiscard]] QString Encode(
		const QByteArray &key,
		const QByteArray &plain) {
	auto nonce = QByteArray(12, char(0));
	base::RandomFill(nonce.data(), 12);
	const auto cipherAndTag = AesGcmEncrypt(key, nonce, plain);
	if (cipherAndTag.isEmpty()) {
		return QString();
	}
	const auto blob = nonce + cipherAndTag;
	return Marker() + QString::fromLatin1(blob.toBase64());
}

[[nodiscard]] std::optional<QByteArray> Decode(
		const QByteArray &key,
		const QString &text) {
	if (!text.startsWith(Marker())) {
		return std::nullopt;
	}
	const auto b64 = text.mid(Marker().size()).toUtf8();
	const auto decoded = QByteArray::fromBase64Encoding(
		b64,
		QByteArray::Base64Encoding
			| QByteArray::AbortOnBase64DecodingErrors);
	if (!decoded) {
		return std::nullopt;
	}
	const auto blob = *decoded;
	if (blob.size() < 12 + 16) {
		return std::nullopt;
	}
	return AesGcmDecrypt(key, blob.left(12), blob.mid(12));
}

// ---- small persisted values ----------------------------------------------

[[nodiscard]] QString AccountKey(not_null<Main::Session*> session) {
	return QString::number(session->uniqueId());
}

[[nodiscard]] QString DialogPrefix(not_null<Main::Session*> session) {
	return AccountKey(session) + QChar('_');
}

[[nodiscard]] QString DeviceId() {
	auto &s = Lumina::Settings::Instance();
	auto id = s.getString(kKeyDeviceId).trimmed();
	if (id.isEmpty()) {
		id = QUuid::createUuid().toString(QUuid::WithoutBraces);
		s.set(kKeyDeviceId, id);
	}
	return id;
}

[[nodiscard]] MsgId CarrierMsgId(not_null<Main::Session*> session) {
	const auto text = Lumina::Settings::Instance().getObject(
		kKeyCarrierMsgId
	).value(AccountKey(session)).toString();
	auto ok = false;
	const auto value = text.toLongLong(&ok);
	return (ok && value != 0) ? MsgId(value) : MsgId(0);
}

void SetCarrierMsgId(not_null<Main::Session*> session, MsgId id) {
	auto all = Lumina::Settings::Instance().getObject(kKeyCarrierMsgId);
	all.insert(AccountKey(session), QString::number(id.bare));
	Lumina::Settings::Instance().set(
		kKeyCarrierMsgId,
		all,
		Lumina::Store::Private);
}

void ClearCarrierMsgId(not_null<Main::Session*> session) {
	auto all = Lumina::Settings::Instance().getObject(kKeyCarrierMsgId);
	if (all.contains(AccountKey(session))) {
		all.remove(AccountKey(session));
		Lumina::Settings::Instance().set(
			kKeyCarrierMsgId,
			all,
			Lumina::Store::Private);
	}
}

// The last full "settings" object we saw on the wire, kept so a push can start
// from it and overwrite only the keys desktop owns - preserving iOS/Android
// -only fields (trMode, trScopePrivate, trScopeGroup, myLanguages, ...).
[[nodiscard]] QJsonObject LoadBase(not_null<Main::Session*> session) {
	return Lumina::Settings::Instance().getObject(
		kKeyBase
	).value(AccountKey(session)).toObject();
}

void SaveBase(
		not_null<Main::Session*> session,
		const QJsonObject &settings) {
	auto all = Lumina::Settings::Instance().getObject(kKeyBase);
	all.insert(AccountKey(session), settings);
	Lumina::Settings::Instance().set(kKeyBase, all, Lumina::Store::Private);
}

// ---- roamed key set -------------------------------------------------------

[[nodiscard]] bool IsRoamedKey(const QString &key) {
	static const auto keys = base::flat_set<QString>{
		u"trReadLang"_q,
		u"trSendLang"_q,
		u"dualLanguageDisplay"_q,
		u"foldOriginalLongMessages"_q,
		u"groupSkipMyLanguages"_q,
		u"translateBeforeSend"_q,
		u"translateBeforeSendConfirm"_q,
		u"explainMessage"_q,
		u"translateProvider"_q,
		u"translateBaseUrl"_q,
		u"translateModel"_q,
		u"translatePrompt"_q,
		u"glossaryTerms"_q,
		kStoreSendActive,
		kStoreSendLang,
		kStoreRegister,
	};
	return keys.contains(key);
}

// ---- serialize / apply ----------------------------------------------------

// Keeps only the entries of a per-dialog store map that belong to THIS account
// and drops the "<uniqueId>_" prefix, leaving the bare peer id as the wire key.
[[nodiscard]] QJsonObject StripMap(
		const QJsonObject &stored,
		const QString &prefix) {
	auto out = QJsonObject();
	for (auto i = stored.begin(); i != stored.end(); ++i) {
		if (i.key().startsWith(prefix)) {
			out.insert(i.key().mid(prefix.size()), i.value());
		}
	}
	return out;
}

// Replaces this account's slice of a per-dialog store map with the wire map
// (bare peer ids re-prefixed), leaving other accounts' entries untouched.
void MergeMap(
		Lumina::Settings &s,
		const QString &storeKey,
		const QJsonObject &wire,
		const QString &prefix) {
	auto object = s.getObject(storeKey);
	auto remove = QStringList();
	for (auto i = object.begin(); i != object.end(); ++i) {
		if (i.key().startsWith(prefix)) {
			remove.push_back(i.key());
		}
	}
	for (const auto &key : remove) {
		object.remove(key);
	}
	for (auto i = wire.begin(); i != wire.end(); ++i) {
		object.insert(prefix + i.key(), i.value());
	}
	if (object.isEmpty()) {
		s.remove(storeKey);
	} else {
		s.set(storeKey, object, Lumina::Store::Private);
	}
}

// Builds the "settings" object for a push: pass-through base first, then the
// keys desktop owns overwritten from the live store. Secrets are never read.
[[nodiscard]] QJsonObject SerializeSettings(
		not_null<Main::Session*> session) {
	auto &s = Lumina::Settings::Instance();
	auto out = LoadBase(session);

	// Global booleans (defaults mirror the readers so an untouched value
	// roams as its effective value, not as "absent").
	out.insert(u"dualLanguageDisplay"_q, s.getBool(u"dualLanguageDisplay"_q));
	out.insert(
		u"foldOriginalLongMessages"_q,
		s.getBool(u"foldOriginalLongMessages"_q, true));
	out.insert(
		u"groupSkipMyLanguages"_q,
		s.getBool(u"groupSkipMyLanguages"_q, true));
	out.insert(u"translateBeforeSend"_q, s.getBool(u"translateBeforeSend"_q));
	out.insert(
		u"translateBeforeSendConfirm"_q,
		s.getBool(u"translateBeforeSendConfirm"_q));
	out.insert(u"explainMessage"_q, s.getBool(u"explainMessage"_q, true));

	// Global strings.
	out.insert(u"trReadLang"_q, s.getString(u"trReadLang"_q));
	out.insert(u"trSendLang"_q, s.getString(u"trSendLang"_q));
	out.insert(u"translateProvider"_q, s.getString(u"translateProvider"_q));
	out.insert(u"translateBaseUrl"_q, s.getString(u"translateBaseUrl"_q));
	out.insert(u"translateModel"_q, s.getString(u"translateModel"_q));
	out.insert(u"translatePrompt"_q, s.getString(u"translatePrompt"_q));

	// Glossary (Store::Bookmarks on disk, plain JSON array on the wire).
	out.insert(u"glossaryTerms"_q, s.getArray(u"glossaryTerms"_q));

	// Per-dialog maps for this account only (within-platform roaming).
	const auto prefix = DialogPrefix(session);
	out.insert(kWireSendActive, StripMap(s.getObject(kStoreSendActive), prefix));
	out.insert(kStoreSendLang, StripMap(s.getObject(kStoreSendLang), prefix));
	out.insert(kStoreRegister, StripMap(s.getObject(kStoreRegister), prefix));

	return out;
}

void ApplyBool(
		Lumina::Settings &s,
		const QJsonObject &o,
		const QString &key) {
	const auto value = o.value(key);
	if (value.isBool()) {
		s.set(key, value.toBool());
	}
}

void ApplyString(
		Lumina::Settings &s,
		const QJsonObject &o,
		const QString &key) {
	const auto value = o.value(key);
	if (value.isString()) {
		s.set(key, value.toString());
	}
}

void ApplySettings(
		not_null<Main::Session*> session,
		const QJsonObject &settings,
		bool fromDesktop) {
	auto &s = Lumina::Settings::Instance();

	ApplyBool(s, settings, u"dualLanguageDisplay"_q);
	ApplyBool(s, settings, u"foldOriginalLongMessages"_q);
	ApplyBool(s, settings, u"groupSkipMyLanguages"_q);
	ApplyBool(s, settings, u"translateBeforeSend"_q);
	ApplyBool(s, settings, u"translateBeforeSendConfirm"_q);
	ApplyBool(s, settings, u"explainMessage"_q);

	ApplyString(s, settings, u"trReadLang"_q);
	ApplyString(s, settings, u"trSendLang"_q);
	ApplyString(s, settings, u"translateProvider"_q);
	ApplyString(s, settings, u"translateBaseUrl"_q);
	ApplyString(s, settings, u"translateModel"_q);
	ApplyString(s, settings, u"translatePrompt"_q);

	if (settings.value(u"glossaryTerms"_q).isArray()) {
		s.set(
			u"glossaryTerms"_q,
			settings.value(u"glossaryTerms"_q).toArray(),
			Lumina::Store::Bookmarks);
	}

	// Per-dialog maps roam within-platform only.
	if (fromDesktop) {
		const auto prefix = DialogPrefix(session);
		MergeMap(
			s,
			kStoreSendActive,
			settings.value(kWireSendActive).toObject(),
			prefix);
		MergeMap(
			s,
			kStoreSendLang,
			settings.value(kStoreSendLang).toObject(),
			prefix);
		MergeMap(
			s,
			kStoreRegister,
			settings.value(kStoreRegister).toObject(),
			prefix);
	}
}

// ---- the per-account controller -------------------------------------------

class Sync final : public base::has_weak_ptr {
public:
	explicit Sync(not_null<Main::Session*> session);

private:
	void schedulePush();
	void doPush();
	void onCarrierItem(not_null<HistoryItem*> item);
	void tryInitialPull();

	[[nodiscard]] HistoryItem *findCarrier();
	void sendCarrier(const QString &payload);
	void editCarrier(not_null<HistoryItem*> item, const QString &payload);

	const not_null<Main::Session*> _session;
	const QString _devId;
	const QByteArray _key;

	base::Timer _pushTimer;
	qint64 _lastAppliedTs = 0;
	QByteArray _lastSyncedBody;

	rpl::lifetime _lifetime;

};

Sync::Sync(not_null<Main::Session*> session)
: _session(session)
, _devId(DeviceId())
, _key(DeriveKey(session))
, _pushTimer([=] { doPush(); }) {
	// Anti-loop floor: whatever the store currently serializes to is treated
	// as already-synced, so a restart alone never rewrites the carrier.
	_lastSyncedBody = QJsonDocument(
		SerializeSettings(_session)).toJson(QJsonDocument::Compact);

	Lumina::Settings::Instance().changes(
	) | rpl::filter([](const QString &key) {
		return IsRoamedKey(key);
	}) | rpl::on_next([=](const QString &) {
		schedulePush();
	}, _lifetime);

	_session->data().itemDataChanges(
	) | rpl::on_next([=](not_null<HistoryItem*> item) {
		if (IsTranslateSyncCarrier(item)) {
			onCarrierItem(item);
		}
	}, _lifetime);

	_session->data().newItemAdded(
	) | rpl::on_next([=](not_null<HistoryItem*> item) {
		if (IsTranslateSyncCarrier(item)) {
			onCarrierItem(item);
		}
	}, _lifetime);

	// If the carrier is already in memory (e.g. Saved Messages was open), pull
	// it once now. Otherwise the observers above catch it as it loads.
	crl::on_main(this, [=] { tryInitialPull(); });
}

void Sync::schedulePush() {
	if (!_pushTimer.isActive()) {
		_pushTimer.callOnce(kPushDelay);
	}
}

void Sync::doPush() {
	try {
		const auto settings = SerializeSettings(_session);
		const auto body = QJsonDocument(settings).toJson(
			QJsonDocument::Compact);
		if (body == _lastSyncedBody) {
			return; // Nothing this platform owns actually changed.
		}
		const auto ts = QDateTime::currentMSecsSinceEpoch();

		auto env = QJsonObject();
		env.insert(u"v"_q, 1);
		env.insert(u"platform"_q, u"desktop"_q);
		env.insert(u"ts"_q, ts);
		env.insert(u"dev"_q, _devId);
		env.insert(u"settings"_q, settings);
		const auto plain = QJsonDocument(env).toJson(QJsonDocument::Compact);
		const auto payload = Encode(_key, plain);
		if (payload.isEmpty()) {
			return; // Crypto failure - retried on the next change.
		}

		_lastSyncedBody = body;
		_lastAppliedTs = ts; // Our own write is the new LWW floor + echo guard.
		SaveBase(_session, settings);

		if (const auto item = findCarrier()) {
			editCarrier(item, payload);
		} else {
			sendCarrier(payload);
		}
	} catch (...) {
	}
}

void Sync::onCarrierItem(not_null<HistoryItem*> item) {
	try {
		const auto plain = Decode(_key, item->originalText().text);
		if (!plain) {
			return;
		}
		const auto env = QJsonDocument::fromJson(*plain).object();
		const auto dev = env.value(u"dev"_q).toString();
		const auto ts = qint64(env.value(u"ts"_q).toDouble());

		if (dev == _devId) {
			// Our own carrier - just remember where it lives so the next push
			// edits it in place rather than sending a duplicate.
			SetCarrierMsgId(_session, item->id);
			if (ts > _lastAppliedTs) {
				_lastAppliedTs = ts;
			}
			return;
		}
		if (ts <= _lastAppliedTs) {
			return; // Already applied, or older than our last write (LWW).
		}

		SetCarrierMsgId(_session, item->id);
		_lastAppliedTs = ts;

		const auto settings = env.value(u"settings"_q).toObject();
		const auto fromDesktop
			= (env.value(u"platform"_q).toString() == u"desktop"_q);
		ApplySettings(_session, settings, fromDesktop);
		SaveBase(_session, settings);

		// After applying, the store serializes to this; recording it stops the
		// change signals we just fired from bouncing back out as a push.
		_lastSyncedBody = QJsonDocument(
			SerializeSettings(_session)).toJson(QJsonDocument::Compact);
	} catch (...) {
	}
}

void Sync::tryInitialPull() {
	try {
		if (const auto item = findCarrier()) {
			onCarrierItem(item);
		}
	} catch (...) {
	}
}

HistoryItem *Sync::findCarrier() {
	const auto stored = CarrierMsgId(_session);
	if (stored) {
		const auto full = FullMsgId(_session->user()->id, stored);
		if (const auto item = _session->data().message(full)) {
			if (IsTranslateSyncCarrier(item)) {
				return item;
			}
		}
	}
	// Fallback: scan the loaded Saved Messages history for the marker.
	const auto history = _session->data().history(_session->user());
	for (const auto &block : history->blocks) {
		for (const auto &view : block->messages) {
			const auto item = view->data();
			if (IsTranslateSyncCarrier(item)) {
				SetCarrierMsgId(_session, item->id);
				return item;
			}
		}
	}
	return nullptr;
}

void Sync::sendCarrier(const QString &payload) {
	const auto history = _session->data().history(_session->user());
	auto message = Api::MessageToSend(Api::SendAction(history));
	message.textWithTags.text = payload;
	message.webPage = Data::WebPageDraft{ .removed = true };
	_session->api().sendMessage(std::move(message));
	// The new local item surfaces through newItemAdded(); onCarrierItem() then
	// records its id (dev == ours) so later pushes edit rather than re-send.
}

void Sync::editCarrier(
		not_null<HistoryItem*> item,
		const QString &payload) {
	const auto weak = base::make_weak(this);
	Api::EditTextMessage(
		item,
		TextWithEntities{ payload },
		Data::WebPageDraft{ .removed = true },
		Api::SendOptions(),
		[=](mtpRequestId) {
		},
		[=](const QString &error, mtpRequestId) {
			if (error == u"MESSAGE_ID_INVALID"_q) {
				if (const auto strong = weak.get()) {
					ClearCarrierMsgId(strong->_session);
					strong->sendCarrier(payload);
				}
			}
		},
		false);
}

[[nodiscard]] auto Syncs()
-> base::flat_map<not_null<Main::Session*>, std::unique_ptr<Sync>> & {
	static auto result
		= base::flat_map<not_null<Main::Session*>, std::unique_ptr<Sync>>();
	return result;
}

} // namespace

bool IsTranslateSyncCarrier(not_null<const HistoryItem*> item) {
	try {
		return item->history()->peer->isSelf()
			&& item->out()
			&& item->originalText().text.startsWith(Marker());
	} catch (...) {
		return false;
	}
}

void SetupTranslateSync(not_null<Window::SessionController*> controller) {
	try {
		const auto session = &controller->session();
		auto &syncs = Syncs();
		if (syncs.find(session) != syncs.end()) {
			return; // A window on this account already installed the sync.
		}
		syncs.emplace(session, std::make_unique<Sync>(session));
		session->lifetime().add([=] { Syncs().remove(session); });
	} catch (...) {
	}
}

} // namespace Lumina
