/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "ui/widgets/fields/input_field.h"

#include <rpl/producer.h>

class QMimeData;

namespace Lumina {

// Crypto-address clipboard guard. Android: LuminaConfig
// "cryptoClipboardGuard", consumed in
// ChatActivityEnterView.luminaCheckCryptoClipboardGuard().
//
// Clipboard-hijacking malware watches for a copied wallet address and quietly
// substitutes the attacker's before it is pasted. The two look alike at a
// glance and the money is gone the moment the transfer is made, so the paste
// is the last point where the user can still notice. With the guard on, a
// paste into a message composer whose whole clipboard text is a wallet address
// is held back and a confirm box is shown first.
//
// Everything here is local: no network call, no third-party list, nothing
// logged. The address never leaves the process.
//
// Default OFF. With the preference unset every paste behaves exactly as stock
// Telegram Desktop.

[[nodiscard]] bool CryptoClipboardGuard();
void SetCryptoClipboardGuard(bool value);

// Reactive form of the getter, for the settings row. Fires on a whole-file
// restore through Settings::importAll() as well.
[[nodiscard]] rpl::producer<bool> CryptoClipboardGuardValue();

// True only when the WHOLE trimmed string is one wallet address. Prose that
// merely contains an address is not a match, and must not be: this runs on
// every paste into every composer, and a substring rule would fire on any
// forwarded message or block of quoted text that happens to mention a wallet.
[[nodiscard]] bool LooksLikeCryptoAddress(const QString &text);

// Called from WrappedMessageFieldMimeHook (chat_helpers/message_field.cpp),
// which every message composer field is wrapped in. Returns true when the
// paste was taken over: the caller must then consume the event and insert
// nothing, because the box inserts the address itself if the user confirms.
//
// Two things this deliberately does not do:
//
//  * it ignores MimeAction::Check. Check is also consulted while a drag merely
//    hovers over the field, so acting on it would pop a box for a drag that is
//    never dropped;
//  * it never blocks a paste. Every failure path - guard off, no text, no
//    window to show a box in - returns false and lets the ordinary paste
//    through.
//
// The box is asynchronous here, unlike Android's synchronous dialog callback,
// so the field is captured weakly and a paste confirmed after the composer is
// gone simply does nothing.
//
// Not guarded, on purpose, because they do not go through this wrapper: the
// chat-list search field, poll creation, and reaction editing all install
// their own mime hooks.
[[nodiscard]] bool InterceptCryptoAddressPaste(
	not_null<Ui::InputField*> field,
	not_null<const QMimeData*> data,
	Ui::InputField::MimeAction action);

} // namespace Lumina
