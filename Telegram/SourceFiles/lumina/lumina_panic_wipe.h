/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

namespace Lumina {

// Panic wipe ("Kaboom"): one deliberate action that logs every account on this
// device out and erases what LuminaGram left behind locally.
//
// This is the engine only. It performs the wipe unconditionally and asks
// nothing - the confirmation, and the settings row that leads to it, live in
// lumina/lumina_panic_wipe_settings.h. Never call this from a path the user
// did not explicitly confirm: the result is not recoverable from this device.
//
// What it destroys, per account:
//   * the media cache and the big-file cache (Data::Session::cache() and
//     cacheBigFile());
//   * everything Storage::Account::reset() drops when the account logs out -
//     drafts, stickers, saved GIFs, search history and suggestions, the
//     account's settings, locations and trusted-peer files, and the temporary
//     and webview directories;
//   * the server-side authorization, through Main::Account::logOut().
// And once, globally: every LuminaGram preference, which is where the fork
// keeps translation API keys, saved originals of outgoing messages, bookmarks
// and the decoy/duress configuration, plus any `*.corrupt` copy of a
// preference file that Lumina::Settings quarantined at some earlier startup.
//
// The local authorization keys are NOT in that list, and the distinction is
// worth spelling out because it is what makes the fallback below necessary.
// Storage::Account::reset() does not drop them: it keeps map0/map1/maps/
// configs and then calls writeMtpData(), which re-serialises whatever
// Main::Account::_mtp still holds (storage_account.cpp:816-818,
// main_account.cpp:319-324) - and _mtp outlives destroySession(). What makes
// this device stop being an authorized one is the server answering
// auth.logOut, or forcedLogOut() calling resetAuthorizationKeys() before
// loggedOut(), which is exactly why it does them in that order. See THE
// RESIDUAL WINDOW.
//
// What it deliberately does NOT destroy:
//   * the download folder. Core::App().settings().downloadPath() very often
//     points at the user's real ~/Downloads, and recursively deleting it
//     would take unrelated data with it. Android's clearLocalMediaCache()
//     empties its media directories because on Android those are app-private;
//     on desktop they are not. The confirmation text says so plainly rather
//     than leaving the user to guess.
//   * tdesktop's own application settings (tdata/settings*): the interface
//     language, the proxy list, the download path, the local passcode. A
//     normal logout leaves them too, and none of them is LuminaGram's to
//     throw away.
//   * the accounts themselves. They stay on Telegram's servers, and so does
//     every message; this wipes a device, not an identity.
//
// KNOWN GAP, needs an owner outside this file. Clearing the preference store
// empties the files, but not the in-memory state other Lumina features built
// from them, and that state writes itself back. The concrete case is
// lumina_translate_originals: its process-lifetime `State::map` still holds
// the plaintext of every outgoing message it recorded, and its Save() rebuilds
// `tbsOriginals` from that map. Its Unhook() does not save, so the wipe's own
// logouts do not resurrect it - but signing in again in the same run and
// sending one translated message does, and so does any message-id change that
// reaches its Rekey(). Closing this needs a ForgetAll()-style entry point in
// lumina/lumina_translate_originals.{h,cpp}, which this wave does not own,
// called from PerformPanicWipe() alongside ClearLuminaPreferences(). Until
// then, a wipe is only fully honest if the application is restarted
// afterwards.
//
// THE RESIDUAL WINDOW, stated plainly because it cannot be closed from here.
// The local erasure is immediate; the server-side logout is a request that has
// to be answered. Between the two, this device is wiped but still authorized.
// The forced fallback below closes that window after a few seconds even on a
// dead network, but nothing closes it if the application is killed or quits
// first - the account then survives on this device with its caches and its
// LuminaGram data already gone. Erasing first is still the right order under
// duress: the alternative loses the data to an interruption instead of the
// authorization, and the data is what cannot be re-obtained.
//
// Everything runs on the main thread, and nothing here blocks on it: the cache
// clears hand the work to the cache database's own thread, and the logouts are
// server round-trips answered later.
void PerformPanicWipe();

} // namespace Lumina
