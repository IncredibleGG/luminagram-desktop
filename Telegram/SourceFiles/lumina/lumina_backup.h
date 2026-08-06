/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include <QtCore/QByteArray>
#include <QtCore/QString>

namespace Lumina {

// Passphrase-encrypted export and import of everything Lumina::Settings holds.
//
// This is the engine only: no widgets, no dialogs, no file pickers. The rows,
// the passphrase boxes and the two file dialogs live in
// lumina/lumina_backup_settings.h.
//
// WHAT TRAVELS. The plaintext is exactly Settings::Instance().exportAll() -
// every key of all three stores plus the reserved `_luminaStores` map that says
// which of them are not Store::Prefs. That deliberately includes the private
// ones: translation API keys, the vault and fake-crash codes, the decoy note,
// saved originals of outgoing messages. Nothing else is touched: tdesktop's own
// settings, the accounts, the message database and the caches are not part of a
// backup and are not restored from one.
//
// ONE EXCEPTION, in both directions: the parked ToS-grey keys (stealthOnline,
// stealthTyping, stealthReadReceipts). F-02 removed every row that could set
// them but two of them still have live readers, so a file that carried them
// would be able to switch this client into permanent always-offline mode with
// no UI left to switch it back. Export drops them and import drops them again;
// see RemoveParkedKeys() in the .cpp.
//
// THE CONTAINER, and why it is not Android's. LuminaBackupActivity writes
// AES/CBC/PKCS5Padding with no authentication tag at all, so a wrong passphrase
// is only noticed when the padding of the garbage it produced happens to be
// invalid - which it is not, roughly once in every few hundred tries, and the
// result is then handed to the JSON parser and, if that survives, written into
// the preference store. Nothing in that container detects a file that was
// edited on purpose either. So this is a different container, deliberately:
//
//   magic   "LuminaGramBackup"      (as in Android, so the file is recognised)
//   version 2                        (Android's unauthenticated container is 1)
//   kdf     "PBKDF2-HMAC-SHA256"
//   iter    the iteration count THIS file was written with - always read back
//           from the file, never assumed, so raising the constant later leaves
//           every older backup importable
//   cipher  "AES-256-CBC+HMAC-SHA256"
//   salt    16 random bytes          iv   16 random bytes
//   data    base64 ciphertext        mac  base64 HMAC-SHA256 tag
//
// 64 bytes of key material come out of PBKDF2-HMAC-SHA256(passphrase, salt,
// iter); the first 32 encrypt, the last 32 authenticate. The tag is taken
// encrypt-then-MAC over magic || version || iter || salt || iv || ciphertext,
// and it is checked - in constant time - BEFORE anything is decrypted. A wrong
// passphrase, a truncated file and a doctored file are therefore all rejected
// with certainty rather than with probability.
//
// Version 1 files are recognised and refused with their own message, not
// silently accepted: there is no honest way to authenticate one.
//
// ATOMICITY. Import decrypts, authenticates and fully parses the payload before
// a single key is written, so a bad passphrase can never leave the store half
// restored. The write itself then goes through Settings::importAll() followed
// by saveNow(). Keys the backup does not carry are left alone; keys it does
// carry are overwritten - the same rule Android states in its own info text.
//
// Each of the three store files is replaced atomically by QSaveFile, but the
// three of them are not replaced atomically as a SET: if the second write fails
// the first has already landed, and the on-disk profile is then part restored
// and part original until a later saveNow() retries the rest. Memory stays
// consistent throughout, so this is only visible after a crash or a kill
// between the failed write and the retry.
//
// THREADING AND COST. Main thread, synchronous, and the key derivation is meant
// to be expensive: expect a few hundred milliseconds of blocked UI per call.
// That is acceptable for a one-shot action the user just confirmed in a dialog,
// and it is the entire defence a short passphrase has. An imported file whose
// iteration count falls outside a sane range is refused before the count is
// ever used, so a hostile file cannot turn that pause into a hang.
enum class BackupError {
	None,
	PassphraseTooShort,
	FileUnreadable,     // missing, unopenable, or absurdly large
	NotABackup,         // not JSON, wrong magic, or structurally impossible
	Unauthenticated,    // Android's version 1 container, refused on purpose
	NewerFormat,        // written by a version of LuminaGram we do not know
	BadPassphrase,      // the tag did not match
	Damaged,            // tag matched, payload still unusable
	WriteFailed,
	CryptoFailed,
};

// The parsed envelope of a backup file. Structurally validated, NOT yet
// authenticated and NOT yet decrypted - holding one of these says only that the
// file looks like a LuminaGram backup, which is what lets the UI report "this
// is not a backup" before it asks for a passphrase.
struct BackupEnvelope {
	int version = 0;
	int iterations = 0;
	QByteArray salt;
	QByteArray iv;
	QByteArray data;
	QByteArray mac;
};

struct BackupRead {
	BackupError error = BackupError::None;
	BackupEnvelope envelope;
};

// The shortest passphrase that will be accepted, matching Android's minimum.
[[nodiscard]] int MinimumPassphraseLength();

// ".lgbak", as on Android, so one account's backups are interchangeable
// between the two clients as soon as Android also writes version 2.
[[nodiscard]] QString BackupFileExtension();

// Encrypts the whole preference store under `passphrase` and writes it to
// `path`, atomically. Never logs, and never writes anything on failure.
[[nodiscard]] BackupError WriteBackup(
	const QString &path,
	const QString &passphrase);

[[nodiscard]] BackupRead ReadBackupFile(const QString &path);
[[nodiscard]] BackupRead ReadBackupBytes(const QByteArray &content);

// Authenticates, decrypts and applies. BackupError::None means the in-memory
// store has been updated and a flush was requested.
//
// !! IT DOES NOT MEAN THE FLUSH SUCCEEDED. Settings::saveNow() returns void and
// only logs a store it could not write, so a read-only or full tdata directory
// produces "Backup restored" while the three JSON files still hold the old
// values - and Settings::saveNow() cancels the debounce timer without
// rescheduling it, so nothing retries until the next unrelated write or the
// destructor. Fixing this needs one change in lumina_settings: make saveNow()
// return whether every dirty store was written, and return WriteFailed here
// when it says no. See the comment at the saveNow() call in ApplyBackup().
[[nodiscard]] BackupError ApplyBackup(
	const BackupEnvelope &envelope,
	const QString &passphrase);

// The message to show for a failure, in the current in-app language. Empty for
// BackupError::None.
[[nodiscard]] QString BackupErrorText(BackupError error);

} // namespace Lumina
