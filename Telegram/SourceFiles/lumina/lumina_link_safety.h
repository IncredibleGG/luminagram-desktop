/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include <rpl/producer.h>

#include <QtCore/QString>
#include <QtCore/QVariant>

#include <vector>

namespace Lumina {

// The link safety inspector: one confirmation, shown offline, before an
// external web link whose address is trying to look like something it is not.
//
// Hook. The gate sits inside Core::UiIntegration::handleUrlClick, after the
// branches that peel off e-mail, tg://, tonsite:// and internal: links - so
// every Telegram deep link, and every t.me address that
// Core::TryConvertUrlToLocal folds into one, is out of scope for free, which
// is what Android's isInternalUri() check buys there. The alternative the
// security survey proposed - promoting every plain EntityType::Url to a
// HiddenUrlClickHandler - was rejected in the port plan: it also rewrites
// copyToClipboardText(), dragText() and the suspicious-link render path in
// the web page and overview layouts, for the same user-visible result.
//
// What it detects, and just as importantly what it does NOT. Stock tdesktop
// already promotes a look-alike address to HiddenUrlClickHandler through
// UrlClickHandler::IsSuspicious(), which fires on any non-ASCII or otherwise
// unexpected character in the domain - that is the punycode and mixed-script
// coverage, and re-implementing it here would only add a second box saying
// the same thing. So this inspector adds exactly the two lines stock has no
// opinion about:
//
//   * userinfo - https://real-looking.example@evil.example, where everything
//     before the "@" is decoration and the real host is what follows it;
//   * known URL shorteners, where the destination is not in the address at
//     all.
//
// Both checks are pure string work on the parsed URL. Nothing is fetched, no
// list is downloaded, and no URL leaves the device.
//
// Behaviour neutrality. LinkSafetyEnabled() defaults to false, and while it
// is false InterceptExternalUrl() returns on its first line. Every failure
// after that point - an address that does not parse, no warnings, no window
// and no Ui::Show to put a box in - also returns false, which means "not
// handled, open the link exactly as before". The inspector never swallows a
// link.

// Preference `linkSafetyCheck`, Store::Prefs, default false.
[[nodiscard]] bool LinkSafetyEnabled();
void SetLinkSafetyEnabled(bool value);

// Fires when the preference above changes, including through a whole-file
// restore by Settings::importAll().
[[nodiscard]] rpl::producer<> LinkSafetyChanges();

// True for a host that is a known link shortener. Case- and "www."-
// insensitive; matches the registrable host exactly, so a look-alike such as
// bit.ly.evil.example is deliberately NOT reported here - it is a different
// domain and stock's own suspicious-link handling is what covers it.
[[nodiscard]] bool IsKnownUrlShortener(const QString &host);

// Empty when the address is fine, is not an http(s) address, or cannot be
// parsed. The strings are the user-visible warning lines, in display order.
[[nodiscard]] std::vector<QString> LinkSafetyWarnings(const QString &url);

// The gate. Returns true when it has taken the click over - a confirmation
// box is up and the caller must stop; the link is opened later, from the box,
// by re-dispatching through UrlClickHandler::Open() with
// ClickHandlerContext::luminaLinkSafetyApproved set.
//
// Returns false in every other case, including every failure path.
//
// Approval is carried by that context flag rather than by a remembered "last
// approved URL" string as on Android: a static remembers one URL, so a user
// alternating between two links is prompted on every single click, and a
// value that outlives the click is a small piece of browsing history sitting
// in a global.
[[nodiscard]] bool InterceptExternalUrl(
	const QString &url,
	const QVariant &context);

} // namespace Lumina
