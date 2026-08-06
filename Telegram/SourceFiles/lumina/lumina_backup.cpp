/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_backup.h"

#include "base/openssl_help.h"
#include "base/random.h"
#include "lumina/lumina_locale.h"
#include "lumina/lumina_settings.h"

#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonParseError>
#include <QtCore/QSaveFile>

namespace Lumina {
namespace {

constexpr auto kFormatVersion = 2;
constexpr auto kAndroidFormatVersion = 1;

// Raising this stays compatible in both directions: a new file records its own
// count in `iter`, and every reader takes the count from the file.
constexpr auto kIterations = 200000;
constexpr auto kMinIterations = 1000;

// A hostile file could otherwise name an iteration count that blocks the main
// thread for hours. A file outside this range is refused rather than clamped -
// clamping would derive a different key than the writer used and then report a
// wrong passphrase, which is a worse answer than "this is not a backup". The
// bound is ~10x the count we write, which is enough headroom for any plausible
// future increase and still bounded by a second or two of work.
constexpr auto kMaxIterations = 2000000;

constexpr auto kSaltSize = size_type(16);
constexpr auto kIvSize = size_type(16);
constexpr auto kBlockSize = size_type(16);
constexpr auto kKeySize = size_type(32);
constexpr auto kMacSize = openssl::kSha256Size;
constexpr auto kKeyMaterialSize = kKeySize + kMacSize;

constexpr auto kMinPassphraseLength = 4;

// Backups are a few kilobytes of JSON. The cap only exists so that pointing the
// importer at a video file costs nothing.
constexpr auto kMaxFileSize = 8 * 1024 * 1024;

[[nodiscard]] QString Magic() {
	return u"LuminaGramBackup"_q;
}

[[nodiscard]] QString KdfName() {
	return u"PBKDF2-HMAC-SHA256"_q;
}

[[nodiscard]] QString CipherName() {
	return u"AES-256-CBC+HMAC-SHA256"_q;
}

[[nodiscard]] bytes::const_span Bytes(const QByteArray &data) {
	return bytes::make_span(
		data.constData(),
		std::size_t(data.size()));
}

[[nodiscard]] QByteArray ToByteArray(bytes::const_span data) {
	return QByteArray(
		reinterpret_cast<const char*>(data.data()),
		int(data.size()));
}

void Wipe(QByteArray &data) {
	if (!data.isEmpty()) {
		OPENSSL_cleanse(data.data(), std::size_t(data.size()));
	}
}

void Wipe(bytes::vector &data) {
	if (!data.empty()) {
		OPENSSL_cleanse(data.data(), std::size_t(data.size()));
	}
}

// Never short-circuits, so the time it takes says nothing about how many
// leading bytes of a forged tag were correct.
[[nodiscard]] bool EqualConstantTime(
		const QByteArray &a,
		const QByteArray &b) {
	if (a.size() != b.size()) {
		return false;
	}
	// Indexed through constData() rather than operator[]: Qt 5.15's overload
	// set takes int and uint, and a qsizetype argument converts equally well
	// to both, so a[i] does not compile here. It also avoids the bounds check
	// and the non-const detach, which is what a constant-time compare wants.
	const auto first = a.constData();
	const auto second = b.constData();
	auto difference = 0;
	for (auto i = qsizetype(); i != a.size(); ++i) {
		difference |= (uchar(first[i]) ^ uchar(second[i]));
	}
	return (difference == 0);
}

[[nodiscard]] QByteArray Be32(int value) {
	const auto number = uint32(value);
	auto result = QByteArray(4, char(0));
	result[0] = char((number >> 24) & 0xFFU);
	result[1] = char((number >> 16) & 0xFFU);
	result[2] = char((number >> 8) & 0xFFU);
	result[3] = char(number & 0xFFU);
	return result;
}

// Everything the tag has to cover. The parameters are in here as well as in the
// key derivation, so a file whose version or iteration count was edited fails
// the tag check rather than quietly deriving some other key.
[[nodiscard]] QByteArray MacInput(
		int version,
		int iterations,
		const QByteArray &salt,
		const QByteArray &iv,
		const QByteArray &cipher) {
	auto result = QByteArray();
	result += Magic().toLatin1();
	result += Be32(version);
	result += Be32(iterations);
	result += salt;
	result += iv;
	result += cipher;
	return result;
}

[[nodiscard]] QByteArray DecodeBase64(const QString &value) {
	if (value.isEmpty()) {
		return QByteArray();
	}
	const auto decoded = QByteArray::fromBase64Encoding(
		value.toLatin1(),
		QByteArray::Base64Encoding
			| QByteArray::AbortOnBase64DecodingErrors);
	return decoded ? *decoded : QByteArray();
}

[[nodiscard]] QByteArray AddPadding(const QByteArray &data) {
	const auto padding = int(kBlockSize - (data.size() % kBlockSize));
	return data + QByteArray(padding, char(padding));
}

[[nodiscard]] bool RemovePadding(QByteArray &data) {
	if (data.isEmpty() || (data.size() % kBlockSize) != 0) {
		return false;
	}
	const auto padding = int(uchar(data[data.size() - 1]));
	if (padding < 1
		|| padding > int(kBlockSize)
		|| padding > int(data.size())) {
		return false;
	}
	for (auto i = 0; i != padding; ++i) {
		if (int(uchar(data[data.size() - 1 - i])) != padding) {
			return false;
		}
	}
	data.chop(padding);
	return true;
}

// Same shape as passport/passport_encryption.cpp EncryptOrDecrypt(), with the
// iv copied because AES_cbc_encrypt() advances the one it is given.
[[nodiscard]] QByteArray AesCbc(
		const QByteArray &data,
		bytes::const_span key,
		const QByteArray &iv,
		int encryptOrDecrypt) {
	Expects(key.size() == kKeySize);
	Expects(iv.size() == kIvSize);
	Expects((data.size() % kBlockSize) == 0);

	auto aesKey = AES_KEY();
	const auto error = (encryptOrDecrypt == AES_ENCRYPT)
		? AES_set_encrypt_key(
			reinterpret_cast<const uchar*>(key.data()),
			key.size() * CHAR_BIT,
			&aesKey)
		: AES_set_decrypt_key(
			reinterpret_cast<const uchar*>(key.data()),
			key.size() * CHAR_BIT,
			&aesKey);
	if (error != 0) {
		LOG(("Lumina Error: Could not set the backup AES key."));
		return QByteArray();
	}
	auto counter = bytes::make_vector(Bytes(iv));
	auto result = QByteArray(data.size(), char(0));
	AES_cbc_encrypt(
		reinterpret_cast<const uchar*>(data.constData()),
		reinterpret_cast<uchar*>(result.data()),
		data.size(),
		&aesKey,
		reinterpret_cast<uchar*>(counter.data()),
		encryptOrDecrypt);
	OPENSSL_cleanse(&aesKey, sizeof(aesKey));
	return result;
}

[[nodiscard]] bytes::vector DeriveKeyMaterial(
		const QString &passphrase,
		const QByteArray &salt,
		int iterations) {
	auto utf8 = passphrase.toUtf8();
	auto result = openssl::details::Pbkdf2<kKeyMaterialSize>(
		Bytes(utf8),
		Bytes(salt),
		iterations,
		EVP_sha256());
	Wipe(utf8);
	return result;
}

[[nodiscard]] QByteArray ComputeMac(
		bytes::const_span macKey,
		const BackupEnvelope &envelope) {
	const auto input = MacInput(
		envelope.version,
		envelope.iterations,
		envelope.salt,
		envelope.iv,
		envelope.data);
	return ToByteArray(bytes::make_span(
		openssl::HmacSha256(macKey, Bytes(input))));
}

// The ToS-grey cluster is parked project-wide. The three keys still exist and
// two of them still have live readers - `stealthOnline` at api_updates.cpp:999
// and `stealthTyping` at api_send_progress.cpp:116 - but F-02 deleted every row
// that could set them, so on this client they can only ever be false.
//
// A backup file is the one remaining way to write them. Without this, importing
// a file that carries `stealthOnline: true` (a hand-edited one, or one from a
// future Android build that grows the rows Android does not have today) would
// silently put this client into permanent always-offline / never-typing mode,
// and there is no row left anywhere to turn it back off. So neither direction
// of this feature carries the cluster: export drops it, and import drops it
// again rather than trusting the file.
void RemoveParkedKeys(QJsonObject &data) {
	data.remove(u"stealthOnline"_q);
	data.remove(u"stealthTyping"_q);
	data.remove(u"stealthReadReceipts"_q);
}

[[nodiscard]] bool EnvelopeShapeIsPossible(const BackupEnvelope &envelope) {
	return (envelope.version == kFormatVersion)
		&& (envelope.iterations >= kMinIterations)
		&& (envelope.iterations <= kMaxIterations)
		&& (envelope.salt.size() == kSaltSize)
		&& (envelope.iv.size() == kIvSize)
		&& (envelope.mac.size() == kMacSize)
		&& !envelope.data.isEmpty()
		&& ((envelope.data.size() % kBlockSize) == 0);
}

} // namespace

int MinimumPassphraseLength() {
	return kMinPassphraseLength;
}

QString BackupFileExtension() {
	return u".lgbak"_q;
}

BackupError WriteBackup(const QString &path, const QString &passphrase) {
	if (passphrase.size() < kMinPassphraseLength) {
		return BackupError::PassphraseTooShort;
	}

	auto salt = QByteArray(int(kSaltSize), char(0));
	auto iv = QByteArray(int(kIvSize), char(0));
	bytes::set_random(bytes::make_detached_span(salt));
	bytes::set_random(bytes::make_detached_span(iv));

	auto material = DeriveKeyMaterial(passphrase, salt, kIterations);
	if (material.size() != kKeyMaterialSize) {
		Wipe(material);
		return BackupError::CryptoFailed;
	}
	const auto key = bytes::make_span(material).subspan(0, kKeySize);
	const auto macKey = bytes::make_span(material).subspan(
		kKeySize,
		kMacSize);

	auto payload = Settings::Instance().exportAll();
	RemoveParkedKeys(payload);
	auto plain = QJsonDocument(payload).toJson(QJsonDocument::Compact);
	auto padded = AddPadding(plain);
	Wipe(plain);

	auto envelope = BackupEnvelope();
	envelope.version = kFormatVersion;
	envelope.iterations = kIterations;
	envelope.salt = salt;
	envelope.iv = iv;
	envelope.data = AesCbc(padded, key, iv, AES_ENCRYPT);
	Wipe(padded);
	if (envelope.data.isEmpty()) {
		Wipe(material);
		return BackupError::CryptoFailed;
	}
	envelope.mac = ComputeMac(macKey, envelope);
	Wipe(material);

	auto object = QJsonObject();
	object.insert(u"magic"_q, Magic());
	object.insert(u"version"_q, envelope.version);
	object.insert(u"kdf"_q, KdfName());
	object.insert(u"iter"_q, envelope.iterations);
	object.insert(u"cipher"_q, CipherName());
	object.insert(u"salt"_q, QString::fromLatin1(envelope.salt.toBase64()));
	object.insert(u"iv"_q, QString::fromLatin1(envelope.iv.toBase64()));
	object.insert(u"data"_q, QString::fromLatin1(envelope.data.toBase64()));
	object.insert(u"mac"_q, QString::fromLatin1(envelope.mac.toBase64()));

	const auto content = QJsonDocument(object).toJson(
		QJsonDocument::Indented);
	auto file = QSaveFile(path);
	if (!file.open(QIODevice::WriteOnly)) {
		LOG(("Lumina Error: Could not open a backup at '%1'.").arg(path));
		return BackupError::WriteFailed;
	} else if (file.write(content) != content.size()) {
		// Returning without commit() leaves whatever was at `path` untouched:
		// the QSaveFile destructor discards the temporary.
		LOG(("Lumina Error: Could not write a backup at '%1'.").arg(path));
		return BackupError::WriteFailed;
	} else if (!file.commit()) {
		LOG(("Lumina Error: Could not commit a backup at '%1'.").arg(path));
		return BackupError::WriteFailed;
	}
	return BackupError::None;
}

BackupRead ReadBackupFile(const QString &path) {
	auto result = BackupRead();
	auto file = QFile(path);
	if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
		result.error = BackupError::FileUnreadable;
		return result;
	} else if (file.size() > kMaxFileSize) {
		result.error = BackupError::FileUnreadable;
		return result;
	}
	const auto content = file.readAll();
	file.close();
	return ReadBackupBytes(content);
}

BackupRead ReadBackupBytes(const QByteArray &content) {
	auto result = BackupRead();
	if (content.isEmpty() || content.size() > kMaxFileSize) {
		result.error = BackupError::NotABackup;
		return result;
	}

	auto error = QJsonParseError{ 0, QJsonParseError::NoError };
	const auto document = QJsonDocument::fromJson(content, &error);
	if (error.error != QJsonParseError::NoError || !document.isObject()) {
		result.error = BackupError::NotABackup;
		return result;
	}
	const auto object = document.object();
	if (object.value(u"magic"_q).toString() != Magic()) {
		result.error = BackupError::NotABackup;
		return result;
	}

	const auto version = object.value(u"version"_q).toInt();
	if (version == kAndroidFormatVersion) {
		// The container Android writes carries no tag, so "the passphrase was
		// wrong" and "somebody edited this file" are both indistinguishable
		// from "it decrypted fine". Refusing it is the only honest answer; the
		// message tells the user to make a new backup instead.
		result.error = BackupError::Unauthenticated;
		return result;
	} else if (version > kFormatVersion) {
		result.error = BackupError::NewerFormat;
		return result;
	} else if (version != kFormatVersion
		|| object.value(u"kdf"_q).toString() != KdfName()
		|| object.value(u"cipher"_q).toString() != CipherName()) {
		result.error = BackupError::NotABackup;
		return result;
	}

	auto envelope = BackupEnvelope();
	envelope.version = version;
	envelope.iterations = object.value(u"iter"_q).toInt();
	envelope.salt = DecodeBase64(object.value(u"salt"_q).toString());
	envelope.iv = DecodeBase64(object.value(u"iv"_q).toString());
	envelope.data = DecodeBase64(object.value(u"data"_q).toString());
	envelope.mac = DecodeBase64(object.value(u"mac"_q).toString());
	if (!EnvelopeShapeIsPossible(envelope)) {
		result.error = BackupError::NotABackup;
		return result;
	}
	result.envelope = envelope;
	return result;
}

BackupError ApplyBackup(
		const BackupEnvelope &envelope,
		const QString &passphrase) {
	if (passphrase.size() < kMinPassphraseLength) {
		return BackupError::PassphraseTooShort;
	} else if (!EnvelopeShapeIsPossible(envelope)) {
		return BackupError::NotABackup;
	}

	auto material = DeriveKeyMaterial(
		passphrase,
		envelope.salt,
		envelope.iterations);
	if (material.size() != kKeyMaterialSize) {
		Wipe(material);
		return BackupError::CryptoFailed;
	}
	const auto key = bytes::make_span(material).subspan(0, kKeySize);
	const auto macKey = bytes::make_span(material).subspan(
		kKeySize,
		kMacSize);

	if (!EqualConstantTime(ComputeMac(macKey, envelope), envelope.mac)) {
		Wipe(material);
		return BackupError::BadPassphrase;
	}

	auto padded = AesCbc(envelope.data, key, envelope.iv, AES_DECRYPT);
	Wipe(material);
	if (padded.isEmpty() || !RemovePadding(padded)) {
		Wipe(padded);
		return BackupError::Damaged;
	}

	// `padded` is wiped only once nothing can still be reading it. Qt's parser
	// copies, so the document does not alias this buffer - but if that were
	// ever to stop being true, wiping it here rather than at the end of every
	// path would mean writing zeroed strings into the store, which is exactly
	// the failure this feature must not have.
	auto error = QJsonParseError{ 0, QJsonParseError::NoError };
	const auto document = QJsonDocument::fromJson(padded, &error);
	if (error.error != QJsonParseError::NoError || !document.isObject()) {
		Wipe(padded);
		return BackupError::Damaged;
	}

	auto restored = document.object();
	RemoveParkedKeys(restored);

	// Nothing above this line touched the store, which is what makes a wrong
	// passphrase incapable of half-restoring anything. saveNow() then replaces
	// each of the three store files atomically instead of leaving the restored
	// values sitting in the debounce window.
	//
	// saveNow() returns void and swallows a store it could not write, so this
	// function cannot tell a flushed import from one that only reached memory
	// and reports success either way. When Settings::saveNow() is changed to
	// report whether every dirty store was written, this becomes:
	//
	//   Settings::Instance().importAll(restored);
	//   const auto saved = Settings::Instance().saveNow();
	//   Wipe(padded);
	//   return saved ? BackupError::None : BackupError::WriteFailed;
	Settings::Instance().importAll(restored);
	Settings::Instance().saveNow();
	Wipe(padded);
	return BackupError::None;
}

QString BackupErrorText(BackupError error) {
	switch (error) {
	case BackupError::None:
		return QString();
	case BackupError::PassphraseTooShort:
		return Tr(u"LuminaBackupPassphraseTooShort"_q);
	case BackupError::FileUnreadable:
		return Tr(u"LuminaBackupImportFailed"_q);
	case BackupError::NotABackup:
		return Tr(u"LuminaBackupInvalidFile"_q);
	case BackupError::Unauthenticated:
		return Tr(u"LuminaBackupUnauthenticated"_q);
	case BackupError::NewerFormat:
		return Tr(u"LuminaBackupNewerFormat"_q);
	case BackupError::BadPassphrase:
		return Tr(u"LuminaBackupWrongPassphrase"_q);
	case BackupError::Damaged:
		return Tr(u"LuminaBackupDamaged"_q);
	case BackupError::WriteFailed:
		return Tr(u"LuminaBackupExportFailed"_q);
	case BackupError::CryptoFailed:
		return Tr(u"LuminaBackupCryptoFailed"_q);
	}
	Unexpected("BackupError value in Lumina::BackupErrorText.");
}

} // namespace Lumina
