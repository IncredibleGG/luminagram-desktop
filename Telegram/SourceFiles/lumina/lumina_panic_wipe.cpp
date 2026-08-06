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
#include "settings.h" // cWorkingDir()
#include "storage/cache/storage_cache_database.h"
#include "ui/emoji_config.h"

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QJsonObject>
#include <QtCore/QStringList>

#include <vector>

namespace Lumina {
namespace {

// How long a server-side auth.logOut is given before the local authorization
// keys are dropped anyway. MTP::Instance::Private::logout() has no timeout of
// its own - the request simply sits in the queue while there is no
// connectivity - so without this the accounts would stay usable on a blocked
// network forever. Under duress what matters is that the data is gone, not
// that the server agreed to it.
constexpr auto kForcedLogOutTimeout = crl::time(5000);

// Passes ClearLuminaPreferences() makes before it gives up on reaching a fixed
// point. Two is the expected cost; see the comment on the function.
constexpr auto kPreferenceClearPasses = 4;

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
//
// The loop is not defensive padding, it is required. Settings::remove() fires
// changesFor(key) synchronously, and the LuminaGram settings page the user
// started the wipe from is still alive behind the confirmation box. Its rows
// are built as `toggleOn(<recomputed from the store>)` plus
// `toggledChanges() -> Set...()`, and Ui::AbstractCheckView::setChecked()
// fires checkedChanges() for a programmatic change exactly as it does for a
// click - so removing a key makes the row that displays it write its new value
// straight back into the store. One further pass settles that: the second
// removal recomputes the same value the toggle already shows, setChecked()
// sees no change, and nothing is written back. Passes stop early as soon as
// exportAll() comes back empty, which is the normal case with no such page
// open.
void ClearLuminaPreferences() {
	auto &settings = Settings::Instance();
	for (auto pass = 0; pass != kPreferenceClearPasses; ++pass) {
		const auto all = settings.exportAll();
		if (all.isEmpty()) {
			break;
		}
		for (auto i = all.constBegin(); i != all.constEnd(); ++i) {
			settings.remove(i.key());
		}
	}
	settings.saveNow();
}

// Lumina::Settings::loadStore() renames a preference file it cannot parse to
// "<name>.corrupt" rather than dropping it, and nothing ever deletes that
// copy. For luminagram_private.json that quarantined file is a verbatim
// plaintext snapshot of the translation API keys, the saved originals of
// outgoing messages, the decoy notepad and the duress passcode - the exact
// material the wipe exists to destroy, sitting next to the file the wipe does
// clear. Emptying the live stores while leaving it is not a wipe.
//
// Matched by name pattern rather than by asking the store, because the store
// does not remember what it quarantined and the point is to catch copies left
// by earlier runs of the application as well as this one.
void RemoveQuarantinedPreferenceFiles() {
	const auto directory = QDir(cWorkingDir() + u"tdata"_q);
	const auto names = directory.entryList(
		QStringList{ u"luminagram*.corrupt"_q },
		QDir::Files);
	for (const auto &name : names) {
		QFile::remove(directory.filePath(name));
	}
}

void ClearLuminaLocalData() {
	ClearLuminaPreferences();
	RemoveQuarantinedPreferenceFiles();
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
	// BACK. Android logs every account out first and empties its cache
	// directories afterwards, because there the cache lives in process-global
	// directories that outlive the logout.
	//
	// On desktop the cache is reached only through the account's Data::Session
	// and Main::Account::logOut() can destroy that session before this
	// function returns: with no MTP instance it calls loggedOut() straight
	// through (main_account.cpp:520-530), and loggedOut() destroys the
	// session. Clearing afterwards would then have no handle to clear
	// through.
	//
	// Note what the inversion is NOT for. A logout that completes does empty
	// both caches on its own - Main::Session::finishLogout() calls
	// Data::Session::clearLocalStorage(), which closes and clears them
	// (data_session.cpp:5806-5811) - and the clear survives the session,
	// because Data::Session holds the databases through a
	// Storage::DatabasePointer and Storage::Databases::destroy() keeps each
	// one alive across waitForCleaner() until the directory removal has
	// finished (lib_storage/storage/storage_databases.cpp:83-95). What the
	// pre-clear buys is time: it starts the erasure now instead of after a
	// server round-trip that may never be answered, so a wipe interrupted by a
	// kill in that window has still destroyed the cached media. Nothing is
	// delayed by it, because Storage::Cache::Database::clear() hands the work
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

	ClearLuminaLocalData();

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
	// The LuminaGram data is cleared a second time here, after the logouts
	// have had their turn. Settings::set() coalesces its writes over ~500ms,
	// so a preference written by anything that reacts to a session going away
	// - directly, or from the crl::on_main turn such teardown code tends to
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
		ClearLuminaLocalData();
	};
	base::call_delayed(kForcedLogOutTimeout, std::move(force));
}

} // namespace Lumina
