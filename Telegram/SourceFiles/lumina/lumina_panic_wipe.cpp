/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_panic_wipe.h"

#include "base/call_delayed.h"
#include "base/weak_ptr.h"
#include "core/application.h"
#include "data/data_session.h"
#include "lumina/lumina_settings.h"
#include "main/main_account.h"
#include "main/main_domain.h"
#include "main/main_session.h"
#include "storage/cache/storage_cache_database.h"
#include "ui/emoji_config.h"

#include <QtCore/QJsonObject>

#include <vector>

namespace Lumina {
namespace {

// How long a server-side auth.logOut is given before the local authorization
// keys are dropped anyway. Under duress what matters is that the data is gone,
// not that the server agreed to it, so a logout that never gets an answer must
// not leave a usable account on this device.
constexpr auto kForcedLogOutTimeout = crl::time(5000);

// Every LuminaGram preference, not only the ones in Store::Private.
//
// This is a deliberate departure from Android, whose panic wipe leaves
// LuminaConfig alone. On desktop the fork's own files hold the translation API
// keys, the saved pre-translation text of outgoing messages, the bookmarks and
// the decoy/duress configuration, all in plaintext JSON next to tdata - so a
// wipe that spares them leaves behind exactly the material the wipe exists to
// remove. Wiping the ordinary preferences too is the honest reading of "this
// device now looks like a fresh install", and it costs the user nothing that
// is not already gone with their accounts.
//
// exportAll() is used rather than a hand-written key list so that a key added
// by a later wave is covered without anyone having to remember this file. The
// one reserved entry it carries is not a stored key, and remove() on a key the
// store does not know is a no-op, so it needs no special case here.
void ClearLuminaPreferences() {
	auto &settings = Settings::Instance();
	const auto all = settings.exportAll();
	for (auto i = all.constBegin(); i != all.constEnd(); ++i) {
		settings.remove(i.key());
	}
	settings.saveNow();
}

} // namespace

void PerformPanicWipe() {
	const auto &accounts = Core::App().domain().accounts();

	// Snapshot before anything is torn down. Main::Domain's account vector is
	// mutated while accounts log out - Domain::watchSession() reacts to a
	// session disappearing by scheduling removeRedundantAccounts(), which
	// erases entries - so neither loop below may iterate the live vector.
	// Weak pointers rather than raw ones because the timeout fallback runs
	// seconds later, by which time most of these are expected to be gone.
	auto snapshot = std::vector<base::weak_ptr<Main::Account>>();
	snapshot.reserve(accounts.size());
	for (const auto &[index, account] : accounts) {
		snapshot.push_back(base::make_weak(account.get()));
	}

	// THE ORDER BELOW IS THE REVERSE OF ANDROID'S, ON PURPOSE. DO NOT "FIX" IT
	// BACK.
	//
	// Android logs every account out first and empties its cache directories
	// afterwards, because there the cache lives in process-global directories
	// that outlive the logout, and doing the slow part last keeps the UI
	// thread free. On desktop the cache is reached only through the account's
	// Data::Session, and Main::Account::logOut() destroys that session. Clear
	// after the logout and there is no handle left to clear through: every
	// cached photo, video and document would silently stay on disk, which is
	// the one outcome this feature exists to prevent. Caches first, logout
	// second. Nothing is lost by the inversion, because the clears are
	// asynchronous anyway - Storage::Cache::Database::clear() hands the work
	// to the database's own thread and returns immediately.
	for (const auto &weak : snapshot) {
		const auto account = weak.get();
		const auto session = account ? account->maybeSession() : nullptr;
		if (session) {
			session->data().cache().clear();
			session->data().cacheBigFile().clear();
		}
	}
	Ui::Emoji::ClearIrrelevantCache();

	ClearLuminaPreferences();

	// Main::Account::logOut(), never Core::App().logoutWithChecks(). The
	// latter is the right call for a user who chose "Log out" from a menu: it
	// stops to ask about an export in progress, about uploads in progress and
	// about downloads in progress. Under duress every one of those boxes is a
	// prompt the user has to read and dismiss before anything is destroyed,
	// which is exactly wrong here.
	for (const auto &weak : snapshot) {
		if (const auto account = weak.get()) {
			account->logOut();
		}
	}

	// The fallback. logOut() waits for the server to answer, so on a blocked
	// or dead network the accounts would stay usable on this device forever.
	// forcedLogOut() resets the authorization keys locally and finishes the
	// logout without the server; it checks sessionExists() itself, so every
	// account that logged out normally is left alone.
	//
	// The preferences are cleared a second time here, after the logouts have
	// had their turn. Settings::set() coalesces its writes over ~500ms, so a
	// preference written by anything that reacts to a session going away -
	// directly, or from the crl::on_main turn such teardown code tends to
	// defer its own save to - lands on disk after the saveNow() above and
	// would otherwise survive the wipe in the file it was supposed to be
	// removed from. Clearing again is idempotent and costs one rewrite of
	// three small files.
	auto force = [snapshot = std::move(snapshot)] {
		for (const auto &weak : snapshot) {
			if (const auto account = weak.get()) {
				account->forcedLogOut();
			}
		}
		ClearLuminaPreferences();
	};
	base::call_delayed(kForcedLogOutTimeout, std::move(force));
}

} // namespace Lumina
