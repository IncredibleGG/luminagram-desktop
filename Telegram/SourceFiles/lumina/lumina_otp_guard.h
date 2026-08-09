/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "lumina/lumina_send_pipeline.h" // Api::SendOptions, TextWithTags.

#include <rpl/producer.h>

#include <QtCore/QString>

class History;

namespace Main {
class Session;
} // namespace Main

namespace Lumina {

// LuminaGram OTP guard - warn before a Telegram login code leaves the
// composer. A port of Android's LuminaOtpGuard.java, rule for rule.
//
// The single most damaging step in a Telegram account takeover is the victim
// typing the 5-6 digit login code into a chat, because a scammer posing as
// "official support" asked for it. The scam-keyword warning in
// lumina/lumina_scam_watch.h only looks at INCOMING messages; this one looks
// at what the user is about to SEND.
//
// Trigger = both of:
//   1. the outgoing text contains a standalone run of 5-6 digits that survives
//      the false-positive filters, and
//   2. Telegram's service account (777000) delivered a message to this account
//      within the last 10 minutes - i.e. a login code really is in flight.
//
// Design constraints, carried over from Android and deliberate:
//   - Pure local string matching. No ML, no model, no network - Telegram's
//     ToS 1.5 forbids deploying ML models on Telegram data.
//   - Read-only advisory. Nothing here mutates or inspects incoming messages,
//     and the outgoing text is never rewritten; the guard only asks a question
//     before the user's own text is dispatched.
//   - Fail-open. Every entry point answers "do not warn" for anything it is
//     not sure about, so a bug here can never stop a normal message.
//
// Key, in Store::Prefs:
//
//   otpGuardEnabled   bool   default TRUE
//
// Default ON, unlike every other LuminaGram row: this is a protection, not a
// preference, so off is opt-out. The default also lives in the one Defaults()
// table in lumina/lumina_settings.cpp.

[[nodiscard]] bool OtpGuardEnabled();
void SetOtpGuardEnabled(bool value);
[[nodiscard]] rpl::producer<> OtpGuardChanges();

// The detector, exposed so it can be reasoned about (and tested) without a
// session. True when `text` holds at least one MAXIMAL run of 5-6 ASCII digits
// that still looks like a login code after the false-positive filters. A run
// of 4 or of 7+ digits is never a Telegram code, so years, card numbers and
// full phone numbers drop out on length alone.
//
// The text is never stored, logged or sent anywhere.
[[nodiscard]] bool ContainsLoginCode(const QString &text);

// True when the service-notifications chat (777000) has a message dated inside
// the recency window. Read from the already-loaded History for that peer - no
// DB query, no request, no blocking. False when that chat has not been loaded
// yet, which is the fail-open answer.
[[nodiscard]] bool ReceivedServiceMessageRecently(
	not_null<Main::Session*> session);

// The guard's stage in the send pipeline. Same contract as
// Lumina::SendInterceptor, except that `text` is const because this guard
// never rewrites a message: returns true to let the send go on to the rest of
// the chain, false when it has taken ownership of the send and will either
// invoke `proceed` once (the user chose "send anyway") or drop it (the user
// cancelled, or the box went away without an answer - the composer still holds
// the text, so nothing is lost).
//
// Called from Lumina::InterceptSend() as a fixed FIRST stage, ahead of every
// registered interceptor; see lumina/lumina_send_pipeline.h for why that
// ordering is not negotiable.
[[nodiscard]] bool OtpGuardIntercept(
	not_null<History*> history,
	const TextWithTags &text,
	Api::SendOptions options,
	Fn<void()> proceed);

} // namespace Lumina
