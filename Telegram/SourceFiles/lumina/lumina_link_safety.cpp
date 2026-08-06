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

#include <QtCore/QUrl>

namespace Lumina {
namespace {

const auto kKeyEnabled = u"linkSafetyCheck"_q;

const auto kWarnUserInfo = u"This link hides its real destination behind "
	"the text placed before an @ sign. Only what follows the @ decides "
	"where it goes."_q;
const auto kWarnShortener = u"This is a link shortener. The real "
	"destination stays hidden until the link opens."_q;

const auto kBoxTitle = u"Open external link?"_q;

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

// Bold the host, so that in an address whose userinfo imitates a domain the
// part that actually decides the destination is the part that stands out.
//
// The search is confined to the authority - everything between "//" and the
// first "/", "?" or "#" - and starts after the LAST "@" inside it. Both
// details matter and stock's BoldDomainInUrl (click_handler_types.cpp:52)
// gets them wrong: a plain indexOf() from the front bolds the decoration in
// https://paypal.com@evil.example, and an "@" in the path or the query
// (https://t.co/x?u=a@b) pushes the search past the host so nothing is bolded
// at all.
//
// Both the encoded and the decoded host are tried, because the address shown
// is the punycode-encoded one whenever it looked deceptive.
[[nodiscard]] TextWithEntities HighlightHost(const QString &url) {
	auto result = TextWithEntities{ .text = url };
	const auto parsed = QUrl(url);
	if (!parsed.isValid()) {
		return result;
	}
	const auto separator = url.indexOf(u"//"_q);
	const auto authorityFrom = (separator >= 0) ? int(separator) + 2 : 0;
	auto authorityTill = int(url.size());
	for (const auto delimiter : { '/', '?', '#' }) {
		const auto found = url.indexOf(delimiter, authorityFrom);
		if (found >= 0 && int(found) < authorityTill) {
			authorityTill = int(found);
		}
	}
	const auto authority = url.mid(
		authorityFrom,
		authorityTill - authorityFrom);
	const auto at = authority.lastIndexOf('@');
	const auto from = authorityFrom + ((at >= 0) ? (int(at) + 1) : 0);
	for (const auto &host : {
			parsed.host(QUrl::FullyEncoded),
			parsed.host(QUrl::FullyDecoded) }) {
		if (host.isEmpty()) {
			continue;
		}
		const auto position = url.indexOf(host, from, Qt::CaseInsensitive);
		if (position < 0 || int(position) + int(host.size()) > authorityTill) {
			continue;
		}
		const auto skip = host.startsWith(u"www."_q) ? 4 : 0;
		result.entities.push_back(EntityInText(
			EntityType::Bold,
			int(position) + skip,
			int(host.size()) - skip));
		break;
	}
	return result;
}

// The password is dropped before anything is displayed. toDisplayString()
// already drops it, but the deceptive-address branch below falls back to
// toEncoded(), which does not - and a box that prints a credential back at
// the user is a worse outcome than the one being warned about.
[[nodiscard]] QString DisplayUrl(const QUrl &parsed) {
	auto shown = parsed;
	shown.setPassword(QString());
	const auto displayed = shown.toDisplayString();
	return !UrlClickHandler::IsSuspicious(displayed)
		? displayed
		: QString::fromUtf8(shown.toEncoded());
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

void FillBox(
		not_null<Ui::GenericBox*> box,
		const QString &displayUrl,
		const QString &warnings,
		bool dark,
		Fn<void()> open) {
	Ui::ConfirmBox(box, {
		.text = warnings,
		.confirmed = [=](Fn<void()> hide) { hide(); open(); },
		.confirmText = tr::lng_open_link(),
		.labelStyle = dark ? &st::groupCallBoxLabel : nullptr,
		.title = kBoxTitle,
	});
	const auto &st = dark ? st::groupCallBoxLabel : st::boxLabel;
	box->addSkip(st.style.lineHeight - st::boxPadding.bottom());
	const auto label = box->addRow(object_ptr<Ui::FlatLabel>(
		box,
		rpl::single(HighlightHost(displayUrl)),
		st));
	label->setSelectable(true);
	label->setContextCopyText(tr::lng_context_copy_link(tr::now));

	// FlatLabel::setMarkedText() parses with _labelMarkedOptions
	// (lib_ui/ui/widgets/labels.cpp:41), which carries TextParseLinks - so
	// the address printed here becomes a live link of its own. In an
	// ordinary confirmation that is merely odd; in the box that exists to
	// warn about this exact address it would be a one-click launcher sitting
	// inside the warning. Swallow the activation; selecting and the copy
	// context menu still work, they do not go through this filter.
	label->setClickHandlerFilter([](const ClickHandlerPtr&, Qt::MouseButton) {
		return false;
	});
}

} // namespace

bool LinkSafetyEnabled() {
	return Settings::Instance().getBool(kKeyEnabled, false);
}

void SetLinkSafetyEnabled(bool value) {
	Settings::Instance().set(kKeyEnabled, value);
}

rpl::producer<> LinkSafetyChanges() {
	return Settings::Instance().changesFor(kKeyEnabled);
}

bool IsKnownUrlShortener(const QString &host) {
	auto bare = host.toLower();
	if (bare.startsWith(u"www."_q)) {
		bare = bare.mid(4);
	}
	return !bare.isEmpty() && Shorteners().contains(bare);
}

std::vector<QString> LinkSafetyWarnings(const QString &url) {
	auto result = std::vector<QString>();
	const auto parsed = ParseExternalUrl(url);
	if (!parsed.isValid()) {
		return result;
	}
	if (!parsed.userInfo().isEmpty()) {
		result.push_back(kWarnUserInfo);
	}
	if (IsKnownUrlShortener(parsed.host())) {
		result.push_back(kWarnShortener);
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
