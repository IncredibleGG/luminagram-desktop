/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_onboarding.h"

#include "base/call_delayed.h"
#include "base/weak_ptr.h"
#include "core/application.h"
#include "lang/lang_keys.h"
#include "lumina/lumina_locale.h"
#include "lumina/lumina_settings.h"
#include "settings/sections/settings_lumina_translate.h"
#include "ui/layers/generic_box.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/labels.h"
#include "ui/wrap/vertical_layout.h"
#include "ui/rp_widget.h"
#include "ui/vertical_list.h"
#include "window/window_session_controller.h"

#include "styles/style_layers.h"
#include "styles/style_settings.h"

#include <QtCore/QJsonValue>

namespace Lumina {
namespace {

constexpr auto kShowDelay = crl::time(400);

// Set the moment ANY card is put on the layer stack, automatic or asked for
// from the Tools row. The persisted flag alone is not enough to stop a second
// one: it is written from showFinishes(), so between the show and the end of
// the show animation AlreadyShown() still reads false, and a card opened by
// hand inside the first-run delay would be followed by an identical automatic
// one stacked on top of it.
bool CardShownThisRun = false;

[[nodiscard]] const QString &ShownKey() {
	static const auto result = u"onboardingShown"_q;
	return result;
}

[[nodiscard]] bool AlreadyShown() {
	return Settings::Instance().getBool(ShownKey());
}

void MarkShown() {
	if (!AlreadyShown()) {
		Settings::Instance().set(ShownKey(), true);
	}
}

[[nodiscard]] TextWithEntities Feature(
		const QString &nameKey,
		const QString &textKey) {
	auto result = tr::marked();
	result.append(u"\n\n"_q);
	result.append(tr::bold(Tr(nameKey)));
	result.append(u"\n"_q);
	result.append(Tr(textKey));
	return result;
}

[[nodiscard]] TextWithEntities CardText() {
	auto result = tr::marked(Tr(u"LuminaOnboardingIntro"_q));
	// The translation block is named by the settings page it points at, which
	// every locale table already translates; the other three name a feature,
	// not a page, so they need keys of their own.
	result.append(Feature(
		u"LuminaTranslateTitle"_q,
		u"LuminaOnboardingTranslateText"_q));
	result.append(Feature(
		u"LuminaOnboardingDualName"_q,
		u"LuminaOnboardingDualText"_q));
	result.append(Feature(
		u"LuminaOnboardingVaultName"_q,
		u"LuminaOnboardingVaultText"_q));
	result.append(Feature(
		u"LuminaOnboardingSafetyName"_q,
		u"LuminaOnboardingSafetyText"_q));
	result.append(u"\n\n"_q);
	result.append(Tr(u"LuminaOnboardingFooter"_q));
	return result;
}

[[nodiscard]] rpl::producer<TextWithEntities> CardTextValue() {
	return rpl::single(
		rpl::empty
	) | rpl::then(
		LangChanges()
	) | rpl::map([] {
		return CardText();
	});
}

void OnboardingBox(
		not_null<Ui::GenericBox*> box,
		not_null<Window::SessionController*> controller) {
	box->setWidth(st::boxWideWidth);
	box->setTitle(TrValue(u"LuminaOnboardingTitle"_q));

	box->addRow(object_ptr<Ui::FlatLabel>(
		box,
		CardTextValue(),
		st::boxLabel));

	box->addButton(TrValue(u"LuminaOnboardingGotIt"_q), [=] {
		box->closeBox();
	});
	// Android labels its deep-link button with the destination page's own
	// title (LuminaPrivacyTitle) rather than a sentence. Doing the same here
	// keeps the button short enough for a st::boxWideWidth row next to "Got
	// it", and reuses a key every locale table already carries - a new key
	// would read English in all nine of them.
	box->addLeftButton(TrValue(u"LuminaTranslateTitle"_q), [=] {
		box->closeBox();
		controller->showSettings(::Settings::LuminaTranslateId());
	});

	// The once-per-install flag is written here and nowhere else. This fires
	// when the layer's show animation has completed, so by the time the flag
	// is stored the card has genuinely been on screen - which is the point of
	// Android's `if (showDialog(...) != null)`, and the reason neither the
	// scheduling site nor show() itself may write it. A card the user closes
	// mid-animation therefore leaves the flag unset and appears once more,
	// which is the safe direction: the card is allowed to be shown again, it
	// is not allowed to be silently spent. showFinished() is re-issued every
	// time a layer above this one closes, so the write is idempotent.
	box->showFinishes(
	) | rpl::on_next([] {
		MarkShown();
	}, box->lifetime());
}

void ShowOnboardingCard(not_null<Window::SessionController*> controller) {
	CardShownThisRun = true;
	controller->show(Box(OnboardingBox, controller));
}

// `parent` is the page that asked for the card, and guarding the delayed call
// on it is the desktop equivalent of Android's `getParentActivity() == null`
// check: navigate away inside the delay and the card is not shown, and nothing
// is written, so the next LuminaGram page that opens arms it again.
void ArmFirstRun(
		not_null<Ui::RpWidget*> parent,
		not_null<Window::SessionController*> controller) {
	if (CardShownThisRun || AlreadyShown()) {
		return;
	}
	const auto weak = base::make_weak(controller);
	base::call_delayed(kShowDelay, parent, [=] {
		const auto strong = weak.get();
		if (!strong
			|| CardShownThisRun
			|| AlreadyShown()
			|| Core::App().passcodeLocked()) {
			return;
		}
		ShowOnboardingCard(strong);
	});
}

} // namespace

void AddOnboardingRows(
		not_null<Ui::VerticalLayout*> container,
		not_null<Window::SessionController*> controller) {
	Ui::AddSkip(container);
	Ui::AddSubsectionTitle(container, TrValue(u"LuminaOnboardingHeader"_q));
	container->add(object_ptr<Ui::SettingsButton>(
		container,
		TrValue(u"LuminaOnboardingRow"_q),
		st::settingsButtonNoIcon
	))->setClickedCallback([=] {
		ShowOnboardingCard(controller);
	});
	Ui::AddSkip(container);
	Ui::AddDividerText(container, TrValue(u"LuminaOnboardingRowAbout"_q));

	ArmFirstRun(container, controller);
}

void SetupFirstRunOnboarding(
		not_null<Ui::RpWidget*> parent,
		not_null<Window::SessionController*> controller) {
	ArmFirstRun(parent, controller);
}

} // namespace Lumina
