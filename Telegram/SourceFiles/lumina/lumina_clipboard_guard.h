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
// KNOWN RESIDUAL - a DROP is not a paste, and MimeAction cannot tell them
// apart. Ui::InputField routes both through MimeAction::Insert
// (input_field.cpp:5964 from dropEventInner() and from the paste path alike),
// and QTextEdit::dropEvent removes the dragged source text BEFORE it calls
// insertFromMimeData(). So dragging a selected wallet address from one place
// in the composer to another, with the guard on, deletes it from where it was
// and then this function swallows the insert: the address is out of the field
// and, unlike a paste, nothing holds a copy of it - a drag never touches the
// clipboard. Confirming the box puts it back at the drop point; cancelling or
// dismissing does not. It is recoverable, because the removal and the missing
// insert sit inside the one edit block QWidgetTextControl::dropEvent opens, so
// a single Ctrl+Z restores the text - but the user has no reason to know that.
//
// It is left as-is deliberately rather than papered over. The obvious in-file
// heuristic - "only guard when this text is what the clipboard holds" - puts a
// clipboard read in FRONT of the guard, and a clipboard read that comes back
// empty (an unfocused window, a Wayland offer that is gone) turns the whole
// feature off silently, which is a strictly worse failure than a Ctrl+Z. The
// real fix is a third Ui::InputField::MimeAction value for a drop, passed
// through WrappedMessageFieldMimeHook; both of those files are lib_ui / core
// and not ours.
//
// NOT GUARDED, because they install their own mime hook instead of going
// through WrappedMessageFieldMimeHook. Verified by reading every
// setMimeDataHook() call in the tree, not by assumption:
//
//   Deliberate - nobody pastes a wallet address there to send it:
//     dialogs/dialogs_widget.cpp:630        chat-list search
//     boxes/create_poll_box.cpp:2280,:2336  poll question / options
//     boxes/peers/edit_peer_reactions.cpp:247
//
//   A GAP, and it is the wrong kind: these are caption fields on the
//   send-a-photo and edit-a-caption paths, which is exactly where an address
//   is pasted next to the QR code of the same wallet:
//     boxes/send_files_box.cpp:1933         send-files caption
//     boxes/edit_caption_box.cpp:610        edit-caption
//     poll/poll_media_upload.cpp:1036       poll media drop target
//   All three hooks return false for a plain-text paste (their Insert branch
//   only handles files), so the field then pastes normally with no guard.
//   Closing it is one line at the top of each Insert branch:
//   `if (Lumina::InterceptCryptoAddressPaste(field, data, action)) return true;`
//   - those files are not ours to edit.
[[nodiscard]] bool InterceptCryptoAddressPaste(
	not_null<Ui::InputField*> field,
	not_null<const QMimeData*> data,
	Ui::InputField::MimeAction action);

} // namespace Lumina
