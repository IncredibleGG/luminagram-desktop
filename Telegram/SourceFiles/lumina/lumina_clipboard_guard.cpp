/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_clipboard_guard.h"

#include "base/weak_qptr.h"
#include "core/application.h"
#include "core/mime_type.h"
#include "lang/lang_text_entity.h"
#include "lumina/lumina_locale.h"
#include "lumina/lumina_settings.h"
#include "ui/boxes/confirm_box.h"
#include "ui/text/text_utilities.h"
#include "window/window_controller.h"

#include <QtCore/QMimeData>
#include <QtCore/QRegularExpression>

namespace Lumina {
namespace {

const auto kKeyEnabled = u"cryptoClipboardGuard"_q;

// The longest form the expression below accepts is bc1 plus 90 characters, so
// anything much past that is prose and not worth matching against.
constexpr auto kMaxCandidateLength = 256;

[[nodiscard]] const QRegularExpression &AddressExpression() {
	// Ported unchanged from Android's LUMINA_CRYPTO_ADDRESS_PATTERN so both
	// clients warn about exactly the same strings: EVM (0x plus 40 hex
	// digits), legacy Base58 Bitcoin, Tron, and bech32 Bitcoin. It is
	// deliberately permissive - there is no checksum step - because a false
	// positive costs one extra click while a false negative costs the
	// transfer. What keeps it quiet is not the expression but the caller: it
	// only ever runs against a whole trimmed clipboard.
	static const auto result = QRegularExpression(u"^(0x[a-fA-F0-9]{40}"
		"|[13][a-km-zA-HJ-NP-Z1-9]{25,34}"
		"|T[a-zA-Z0-9]{33}"
		"|bc1[a-z0-9]{25,90})$"_q);
	return result;
}

} // namespace

bool CryptoClipboardGuard() {
	return Settings::Instance().getBool(kKeyEnabled, false);
}

void SetCryptoClipboardGuard(bool value) {
	Settings::Instance().set(kKeyEnabled, value, Store::Prefs);
}

rpl::producer<bool> CryptoClipboardGuardValue() {
	return rpl::single(
		rpl::empty
	) | rpl::then(
		Settings::Instance().changesFor(kKeyEnabled)
	) | rpl::map([] {
		return CryptoClipboardGuard();
	});
}

bool LooksLikeCryptoAddress(const QString &text) {
	if (text.isEmpty() || text.size() > kMaxCandidateLength) {
		return false;
	}
	const auto candidate = text.trimmed();
	if (candidate.isEmpty()) {
		return false;
	}
	const auto match = AddressExpression().match(candidate);
	return match.hasMatch()
		&& (match.capturedStart() == 0)
		&& (match.capturedLength() == candidate.size());
}

bool InterceptCryptoAddressPaste(
		not_null<Ui::InputField*> field,
		not_null<const QMimeData*> data,
		Ui::InputField::MimeAction action) {
	if (action != Ui::InputField::MimeAction::Insert
		|| !CryptoClipboardGuard()) {
		return false;
	}
	const auto address = Core::ReadMimeText(data).trimmed();
	if (!LooksLikeCryptoAddress(address)) {
		return false;
	}
	const auto window = [&] {
		const auto found = Core::App().findWindow(field);
		return found ? found : Core::App().activeWindow();
	}();
	if (!window) {
		return false;
	}
	auto text = tr::marked(Tr(u"LuminaClipCryptoBody"_q));
	text.append(u"\n\n"_q).append(
		Ui::Text::Wrapped(tr::marked(address), EntityType::Code));
	const auto weak = base::weak_qptr<Ui::InputField>(field);
	window->show(Ui::MakeConfirmBox({
		.text = std::move(text),
		.confirmed = [=](Fn<void()> close) {
			if (const auto strong = weak.get()) {
				strong->textCursor().insertText(address);
			}
			close();
		},
		.confirmText = Tr(u"LuminaClipCryptoPaste"_q),
		.title = Tr(u"LuminaClipCryptoTitle"_q),
	}));
	return true;
}

} // namespace Lumina
