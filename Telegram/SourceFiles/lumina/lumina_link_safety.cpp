/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_link_safety.h"

#include "base/flat_set.h"
#include "core/application.h"
#include "core/click_handler_types.h"
#include "lang/lang_keys.h"
#include "lumina/lumina_locale.h"
#include "lumina/lumina_settings.h"
#include "ui/boxes/confirm_box.h"
#include "ui/layers/generic_box.h"
#include "ui/layers/show.h"
#include "ui/text/text_entity.h"
#include "ui/widgets/labels.h"
#include "ui/basic_click_handlers.h"
#include "window/window_controller.h"
#include "window/window_session_controller.h"

#include "styles/style_calls.h" // groupCallBoxLabel
#include "styles/style_layers.h"

#include <QtCore/QStringList>
#include <QtCore/QUrl>

namespace Lumina {
namespace {

const auto kKeyEnabled = u"linkSafetyCheck"_q;

// Registrable hosts, lower-case, without "www.". Carried over verbatim from
// Android's LUMINA_URL_SHORTENERS so both clients warn about the same set.
[[nodiscard]] const base::flat_set<QString> &Shorteners() {
	static const auto result = base::flat_set<QString>{
		u"adf.ly"_q,
		u"bit.do"_q,
		u"bit.ly"_q,
		u"buff.ly"_q,
		u"clck.ru"_q,
		u"cutt.ly"_q,
		u"db.tt"_q,
		u"goo.gl"_q,
		u"is.gd"_q,
		u"lnkd.in"_q,
		u"ow.ly"_q,
		u"qr.ae"_q,
		u"rb.gy"_q,
		u"rebrand.ly"_q,
		u"s.id"_q,
		u"shorte.st"_q,
		u"shorturl.at"_q,
		u"shrtco.de"_q,
		u"t.co"_q,
		u"t.ly"_q,
		u"tiny.cc"_q,
		u"tinyurl.com"_q,
		u"trib.al"_q,
		u"u.to"_q,
		u"v.gd"_q,
		u"vk.cc"_q,
		u"x.co"_q,
	};
	return result;
}

// The trailing dot is not decoration: "bit.ly." is a fully qualified name that
// resolves to exactly the same host as "bit.ly", so a list lookup that keeps
// it is a one-character bypass of the whole shortener list.
[[nodiscard]] QString RegistrableHost(const QString &host) {
	auto bare = host.toLower();
	while (bare.endsWith(u"."_q)) {
		bare.chop(1);
	}
	return bare.startsWith(u"www."_q) ? bare.mid(4) : bare;
}

// An invalid QUrl means "not our business": anything that does not parse, and
// anything that is not plain http(s). Every other scheme has already been
// handled by the branches above the gate in UiIntegration::handleUrlClick.
[[nodiscard]] QUrl ParseExternalUrl(const QString &url) {
	if (url.isEmpty()) {
		return QUrl();
	}
	const auto parsed = QUrl::fromUserInput(url);
	if (!parsed.isValid()) {
		return QUrl();
	}
	const auto scheme = parsed.scheme().toLower();
	return (scheme == u"http"_q || scheme == u"https"_q) ? parsed : QUrl();
}

// A host with a punycode label, written as its ASCII punycode form.
//
// QUrl reports the host in ACE whichever way it was written, so the encoded
// host alone cannot tell the two apart - and the two need telling apart,
// because a host written in Unicode is already covered by stock (see the
// header) and would get a second box. Testing whether the clicked string
// contains the ACE host is the whole distinction, and it is exact: the ACE
// form appears in the address only if that is how the address was written.
// Testing the string for non-ASCII characters instead would misfire on a
// Unicode path or query under an ASCII host.
[[nodiscard]] bool HasWrittenPunycodeLabel(
		const QUrl &parsed,
		const QString &url) {
	const auto host = parsed.host(QUrl::FullyEncoded).toLower();
	if (host.isEmpty() || !url.contains(host, Qt::CaseInsensitive)) {
		return false;
	}
	for (const auto &label : host.split(u"."_q)) {
		if (label.startsWith(u"xn--"_q)) {
			return true;
		}
	}
	return false;
}

// The address as it is safe to print.
//
// The password is dropped before anything is displayed. toDisplayString()
// already drops it, but both fallbacks below use toEncoded(), which does not
// - and a box that prints a credential back at the user is a worse outcome
// than the one being warned about.
//
// An address that carries userinfo is always shown in its encoded form, and
// that is the whole point of this function. toDisplayString() decodes percent
// escapes, so the userinfo of
// https://paypal.com%2F%40evil.example@attacker.example/ decodes to
// "paypal.com/@evil.example" and the printed string becomes
// https://paypal.com/@evil.example@attacker.example/ - an address whose
// authority now appears to end at "paypal.com", inside the one box that
// exists to tell the user where the link really goes. Encoded, the escapes
// stay escapes and the authority cannot be forged.
[[nodiscard]] QString DisplayUrl(const QUrl &parsed) {
	const auto hasUserInfo = !parsed.userInfo().isEmpty();
	auto shown = parsed;
	shown.setPassword(QString());
	const auto encoded = QString::fromUtf8(shown.toEncoded());
	if (hasUserInfo) {
		return encoded;
	}
	const auto displayed = shown.toDisplayString();
	return !UrlClickHandler::IsSuspicious(displayed) ? displayed : encoded;
}

// The host, on its own line, in bold.
//
// This replaces bolding the host inside the printed address. Locating a host
// inside a URL string is guesswork that fails in exactly the cases the box
// exists for: stock's BoldDomainInUrl (core/click_handler_types.cpp:52) bolds
// the decoration in https://paypal.com@evil.example because it searches from
// the front, and any search confined to the authority is defeated by an
// escaped "/" in the userinfo, which moves where the authority appears to
// end. A separate line taken straight from QUrl::host() cannot be pointed at
// the wrong span, because it is not a span - it is the parser's own answer to
// "where does this go".
//
// FullyEncoded, so a Unicode homograph host is shown as its punycode.
[[nodiscard]] TextWithEntities DestinationLine(const QUrl &parsed) {
	const auto host = parsed.host(QUrl::FullyEncoded);
	if (host.isEmpty()) {
		return TextWithEntities();
	}
	const auto caption = Tr(u"LuminaLinkSafetyDestination"_q);
	auto result = TextWithEntities{
		.text = caption + u"\n"_q + host,
	};
	result.entities.push_back(EntityInText(
		EntityType::Bold,
		int(caption.size()) + 1,
		int(host.size())));
	return result;
}

[[nodiscard]] QString JoinWarnings(const std::vector<QString> &warnings) {
	auto result = QString();
	for (const auto &warning : warnings) {
		if (!result.isEmpty()) {
			result += u"\n\n"_q;
		}
		result += warning;
	}
	return result;
}

// FlatLabel::setMarkedText() parses with _labelMarkedOptions
// (lib_ui/ui/widgets/labels.cpp:41), which carries TextParseLinks - so a host
// or an address printed as marked text becomes a live link of its own. In an
// ordinary confirmation that is merely odd; in the box that exists to warn
// about this exact address it would be a one-click launcher sitting inside
// the warning. Swallow the activation; selecting and the copy context menu
// still work, they do not go through this filter.
[[nodiscard]] Fn<bool(const ClickHandlerPtr&, Qt::MouseButton)> Inert() {
	return [](const ClickHandlerPtr&, Qt::MouseButton) { return false; };
}

void FillBox(
		not_null<Ui::GenericBox*> box,
		const QString &displayUrl,
		const TextWithEntities &destination,
		const QString &warnings,
		bool dark,
		Fn<void()> open) {
	Ui::ConfirmBox(box, {
		.text = warnings,
		.confirmed = [=](Fn<void()> hide) { hide(); open(); },
		.confirmText = tr::lng_open_link(),
		.labelStyle = dark ? &st::groupCallBoxLabel : nullptr,
		.title = TrValue(u"LuminaLinkSafetyTitle"_q),
	});
	const auto &st = dark ? st::groupCallBoxLabel : st::boxLabel;
	const auto skip = st.style.lineHeight - st::boxPadding.bottom();
	if (!destination.text.isEmpty()) {
		box->addSkip(skip);
		box->addRow(object_ptr<Ui::FlatLabel>(
			box,
			rpl::single(destination),
			st)
		)->setClickHandlerFilter(Inert());
	}
	box->addSkip(skip);

	// The plain-QString overload on purpose: FlatLabel::setText() parses with
	// _labelOptions, which has no TextParseLinks, so the address cannot become
	// a handler in the first place. The filter above is only needed where bold
	// forces marked text.
	const auto label = box->addRow(object_ptr<Ui::FlatLabel>(
		box,
		displayUrl,
		st));
	label->setSelectable(true);
	label->setBreakEverywhere(true);
	label->setContextCopyText(tr::lng_context_copy_link(tr::now));
	label->setClickHandlerFilter(Inert());
}

} // namespace

bool LinkSafetyEnabled() {
	return Settings::Instance().getBool(kKeyEnabled, false);
}

void SetLinkSafetyEnabled(bool value) {
	// The store is named rather than defaulted. Settings::set() defaults to
	// Store::Prefs and silently RELOCATES a key written through that default
	// (lumina_settings.h:62-67), so a bare set() here is a trap waiting for
	// the day this key moves - every other setter in this batch names it.
	Settings::Instance().set(kKeyEnabled, value, Store::Prefs);
}

rpl::producer<> LinkSafetyChanges() {
	return Settings::Instance().changesFor(kKeyEnabled);
}

bool IsKnownUrlShortener(const QString &host) {
	const auto bare = RegistrableHost(host);
	return !bare.isEmpty() && Shorteners().contains(bare);
}

std::vector<QString> LinkSafetyWarnings(const QString &url) {
	auto result = std::vector<QString>();
	const auto parsed = ParseExternalUrl(url);
	if (!parsed.isValid()) {
		return result;
	}
	if (!parsed.userInfo().isEmpty()) {
		result.push_back(Tr(u"LuminaLinkSafetyWarnMismatch"_q));
	}
	if (HasWrittenPunycodeLabel(parsed, url)) {
		result.push_back(Tr(u"LuminaLinkSafetyWarnPunycode"_q));
	}
	if (IsKnownUrlShortener(parsed.host())) {
		result.push_back(Tr(u"LuminaLinkSafetyWarnShortener"_q));
	}
	return result;
}

bool InterceptExternalUrl(const QString &url, const QVariant &context) {
	if (!LinkSafetyEnabled()) {
		return false;
	}
	const auto my = context.value<ClickHandlerContext>();
	if (my.luminaLinkSafetyApproved) {
		return false;
	}
	const auto parsed = ParseExternalUrl(url);
	if (!parsed.isValid()) {
		return false;
	}
	const auto warnings = LinkSafetyWarnings(url);
	if (warnings.empty()) {
		return false;
	}
	const auto show = (my.show && my.show->valid())
		? my.show
		: std::shared_ptr<Ui::Show>();
	const auto controller = my.sessionWindow.get();
	const auto window = controller
		? &controller->window()
		: Core::App().activeWindow();
	if (!show && !window) {
		return false;
	}

	const auto open = [=] {
		auto approved = my;
		approved.luminaLinkSafetyApproved = true;
		UrlClickHandler::Open(url, QVariant::fromValue(approved));
	};
	auto box = Box(
		FillBox,
		DisplayUrl(parsed),
		DestinationLine(parsed),
		JoinWarnings(warnings),
		my.dark,
		Fn<void()>(open));
	if (show) {
		show->showBox(std::move(box));
	} else {
		Core::App().hideMediaView();
		window->show(std::move(box));
		window->activate();
	}
	return true;
}

} // namespace Lumina
