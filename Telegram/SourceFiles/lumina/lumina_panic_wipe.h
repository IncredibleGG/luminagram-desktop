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
//     the local message database, drafts, stickers, search history, the
//     account's MTP keys;
//   * the server-side authorization, through Main::Account::logOut().
// And once, globally: every LuminaGram preference, which is where the fork
// keeps translation API keys, saved originals of outgoing messages, bookmarks
// and the decoy/duress configuration.
//
// What it deliberately does NOT destroy:
//   * the download folder. Core::App().settings().downloadPath() very often
//     points at the user's real ~/Downloads, and recursively deleting it
//     would take unrelated data with it. Android's clearLocalMediaCache()
//     empties its media directories because on Android those are app-private;
//     on desktop they are not. The confirmation text says so plainly rather
//     than leaving the user to guess.
//   * the accounts themselves. They stay on Telegram's servers, and so does
//     every message; this wipes a device, not an identity.
//
// KNOWN GAP, needs an owner outside this file. Clearing the preference store
// empties the files, but not the in-memory state other Lumina features built
// from them, and that state writes itself back. The concrete case is
// lumina_translate_originals: its process-lifetime map still holds the
// plaintext of every outgoing message it recorded, so signing in again in the
// same run and sending one translated message calls its Save() and restores
// those originals to luminagram_private.json. Closing this needs a
// ForgetAll()-style entry point in lumina/lumina_translate_originals.{h,cpp},
// which this wave does not own, called from PerformPanicWipe() alongside
// ClearLuminaPreferences(). Until then, a wipe is only fully honest if the
// application is restarted afterwards.
//
// Everything runs on the main thread, and nothing here blocks on it: the cache
// clears hand the work to the cache database's own thread, and the logouts are
// server round-trips answered later. A wipe that hangs on a dead network is a
// failed wipe, so the logouts are backed by a timeout that drops the local
// authorization keys of every account the server never answered for, and
// clears the preference store a second time.
void PerformPanicWipe();

} // namespace Lumina
