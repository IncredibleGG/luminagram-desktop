/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

class HistoryItem;

namespace Window {
class SessionController;
} // namespace Window

namespace Lumina {

// LuminaGram's translation-settings roaming - the desktop half of the SHARED
// carrier already shipping on iOS/Android.
//
// The user's translation preferences (reading / sending language, the dual
// display and folding toggles, the custom LLM provider endpoint, the glossary,
// and the per-chat send toggles) are mirrored through ONE hidden message in the
// account's own Saved Messages. Every platform reads and rewrites that single
// message, so a change made on the phone reaches the desktop and back with no
// server of ours in the loop.
//
// TOS / privacy boundaries (deliberate):
//
//  * the carrier is the user talking to themselves - Saved Messages, out,
//    self peer - and its body is AES-256-GCM encrypted under a key derived
//    from the user's own id, so nothing readable is ever put on the wire;
//  * API keys and lock codes (translateKey_*, the passcodes) are NEVER read
//    into the blob;
//  * keys this platform does not own are preserved verbatim, so roaming from
//    one platform never wipes another's settings (last-writer-wins per field
//    set, not per whole blob).
//
// Wire format, byte-for-byte with iOS: marker = U+2064 U+2064 "LG-SYNC1:",
// text = marker + base64(nonce(12) || ciphertext || tag(16)); plaintext is
// compact UTF-8 JSON {"v":1,"platform":..,"ts":..,"dev":..,"settings":{..}}.

// True iff `item` is the roaming carrier: in the user's own Saved Messages,
// outgoing, and its text starts with the exact marker. Self-peer + exact
// marker gated and fail-closed, so an ordinary Saved Message is never hidden.
// Used by the two UI seams that keep the carrier invisible.
[[nodiscard]] bool IsTranslateSyncCarrier(not_null<const HistoryItem*> item);

// Installs the per-account roaming controller. Idempotent per Main::Session -
// a second window on the same account reuses the first window's controller.
// The controller dies with the session. Never throws.
void SetupTranslateSync(not_null<Window::SessionController*> controller);

} // namespace Lumina
