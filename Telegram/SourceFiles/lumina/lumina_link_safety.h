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
// What it detects, and just as importantly what it does NOT. Android warns
// about four things; this warns about three of them, and the fourth is left
// to stock because stock genuinely covers it.
//
//   * userinfo - https://real-looking.example@evil.example, where everything
//     before the "@" is decoration and the real host is what follows it;
//   * known URL shorteners, where the destination is not in the address at
//     all;
//   * a punycode label ("xn--") in the host, but ONLY when the address was
//     written in that ASCII form. See below.
//   * mixed-script look-alikes are NOT checked here.
//
// The punycode/mixed-script split is the one place where "stock already does
// it" needed checking rather than assuming, so here is what stock actually
// does. UrlClickHandler::IsSuspicious() (lib_ui/ui/basic_click_handlers.cpp)
// tests the domain for a character outside [a-zA-Z0-9.-], and
// Core::UiIntegration::createLinkHandler() (core/ui_integration.cpp:322-327)
// promotes a plain EntityType::Url that trips it to a HiddenUrlClickHandler,
// which confirms before opening. That covers a host written in Unicode -
// Cyrillic "раypal.com" and every other mixed-script homograph - and it is
// why there is no mixed-script line here: a second box would ask about the
// same address twice.
//
// It does NOT cover a host written as its ASCII punycode form. "xn--" is all
// letters, digits and hyphens, so IsSuspicious() returns false and
// https://xn--pypal-4ve.com opens with no warning of any kind, while Android
// warns about both forms. That is the gap this file closes, and the condition
// is written to close exactly that gap: the punycode line is added only when
// the clicked address itself contains the "xn--" host, so the ASCII form
// warns once and the Unicode form keeps stock's single box.
//
// Residual, and accepted: a Unicode-host address that reaches the gate
// without having gone through createLinkHandler() gets no punycode line. It
// is not silent even then - the box's destination line prints the host in
// punycode whenever any other warning has opened the box.
//
// Both checks are pure string work on the parsed URL. Nothing is fetched, no
// list is downloaded, and no URL leaves the device.
//
// Where this diverges from Android, deliberately. Android shows its sheet for
// EVERY external http(s) link once the preference is on, with the warnings as
// extra lines under the address. Desktop shows the box only when there is at
// least one warning, because tdesktop already confirms on its own for hidden
// URLs and for suspicious ones - a box on every link would stack a second
// modal on top of stock's for a large share of clicks.
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

// True for a host that is a known link shortener. Case-insensitive, and
// insensitive to a leading "www." and to the trailing dot of a fully
// qualified name; matches the registrable host exactly, so a look-alike such
// as bit.ly.evil.example is deliberately NOT reported here - it is a
// different domain and stock's own suspicious-link handling is what covers
// it.
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
// in a global. The cost of the flag is that clicking the same warned link
// twice asks twice, which is the safe direction to err in.
[[nodiscard]] bool InterceptExternalUrl(
	const QString &url,
	const QVariant &context);

} // namespace Lumina
