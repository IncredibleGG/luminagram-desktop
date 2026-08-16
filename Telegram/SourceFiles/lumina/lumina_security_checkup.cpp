/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_security_checkup.h"

#include "api/api_authorizations.h"
#include "api/api_cloud_password.h"
#include "api/api_user_privacy.h"
#include "apiwrap.h"
#include "core/core_cloud_password.h"
#include "lumina/lumina_locale.h"
#include "main/main_session.h"
#include "settings/settings_common.h"
#include "settings/cloud_password/settings_cloud_password_start.h"
#include "settings/cloud_password/settings_cloud_password_input.h"
#include "settings/cloud_password/settings_cloud_password_email_confirm.h"
#include "settings/sections/settings_active_sessions.h"
#include "settings/sections/settings_privacy_security.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"

#include "styles/style_menu_icons.h"
#include "styles/style_settings.h"

namespace Lumina {
namespace {

using CloudState = Core::CloudPasswordState;
using PrivacyKey = Api::UserPrivacy::Key;
using PrivacyOption = Api::UserPrivacy::Option;

// Every status column follows the same shape: start with "Tap to view" so the
// row is never blank and never reads as a value the API has not confirmed,
// then switch to the live value once it arrives. A source that never emits -
// offline - simply leaves the placeholder in place, which is the honest thing
// to show.
//
// The second producer combined in is a TrValue rather than the value's own
// text: TrValue re-emits on a language change, so the label re-runs `format`
// and re-localises then too, and its own text is deliberately ignored. This is
// the shape the session-guard status row uses.
template <typename Value>
[[nodiscard]] rpl::producer<QString> StatusLabel(
		rpl::producer<Value> value,
		Fn<QString(const Value&)> format) {
	return rpl::single(
		Tr(u"LuminaCheckupTapToView"_q)
	) | rpl::then(rpl::combine(
		std::move(value),
		TrValue(u"LuminaCheckupTapToView"_q)
	) | rpl::map([format = std::move(format)](
			const Value &data,
			const QString&) {
		return format(data);
	}));
}

[[nodiscard]] rpl::producer<QString> AccountSummary(
		not_null<Main::Session*> session) {
	return rpl::single(
		Tr(u"LuminaCheckupSummaryChecking"_q)
	) | rpl::then(rpl::combine(
		session->api().cloudPassword().state(),
		TrValue(u"LuminaCheckupSummaryChecking"_q)
	) | rpl::map([](const CloudState &data, const QString&) {
		return Tr(data.hasPassword
			? u"LuminaCheckupSummaryOn"_q
			: u"LuminaCheckupSummaryOff"_q);
	}));
}

[[nodiscard]] rpl::producer<QString> TwoStepStatus(
		not_null<Main::Session*> session) {
	return StatusLabel<CloudState>(
		session->api().cloudPassword().state(),
		[](const CloudState &data) {
			return Tr(!data.unconfirmedPattern.isEmpty()
				? u"LuminaCheckupUnconfirmed"_q
				: data.hasPassword
				? u"LuminaCheckupOn"_q
				: u"LuminaCheckupOff"_q);
		});
}

[[nodiscard]] rpl::producer<QString> RecoveryStatus(
		not_null<Main::Session*> session) {
	return StatusLabel<CloudState>(
		session->api().cloudPassword().state(),
		[](const CloudState &data) {
			return Tr(!data.hasPassword
				? u"LuminaCheckupRecoveryNoPassword"_q
				: data.hasRecovery
				? u"LuminaCheckupOn"_q
				: u"LuminaCheckupOff"_q);
		});
}

[[nodiscard]] rpl::producer<QString> PrivacyStatus(
		not_null<Main::Session*> session,
		PrivacyKey key) {
	return StatusLabel<Api::UserPrivacy::Rule>(
		session->api().userPrivacy().value(key),
		[](const Api::UserPrivacy::Rule &rule) {
			switch (rule.option) {
			case PrivacyOption::Everyone:
				return Tr(u"LuminaCheckupEveryone"_q);
			case PrivacyOption::Contacts:
				return Tr(u"LuminaCheckupContacts"_q);
			case PrivacyOption::CloseFriends:
				return Tr(u"LuminaCheckupCloseFriends"_q);
			case PrivacyOption::Nobody:
				return Tr(u"LuminaCheckupNobody"_q);
			}
			return Tr(u"LuminaCheckupTapToView"_q);
		});
}

[[nodiscard]] rpl::producer<QString> SessionsStatus(
		not_null<Main::Session*> session) {
	return StatusLabel<int>(
		session->api().authorizations().totalValue(),
		[](int count) {
			return (count > 0)
				? QString::number(count)
				: Tr(u"LuminaCheckupTapToView"_q);
		});
}

// `Settings` names Lumina::Settings inside this namespace, so the settings
// section helpers and section ids have to be reached through the global
// namespace.
void AddCheckupRow(
		not_null<Ui::VerticalLayout*> container,
		rpl::producer<QString> title,
		rpl::producer<QString> status,
		const style::icon &icon,
		Fn<void()> activate) {
	::Settings::AddButtonWithLabel(
		container,
		std::move(title),
		std::move(status),
		st::settingsButton,
		{ &icon }
	)->setClickedCallback(std::move(activate));
}

} // namespace

void AddSecurityCheckupRows(
		not_null<Ui::VerticalLayout*> container,
		not_null<Window::SessionController*> controller) {
	const auto session = &controller->session();

	// The cloud-password deep-link needs the current state at the moment of
	// the click, so it is kept here rather than re-derived from a producer in
	// the handler. Until the first state arrives both the 2FA and recovery
	// rows fall back to the stock Privacy & Security page, which carries the
	// two-step button itself - so an early tap still lands somewhere useful
	// and never on the "create a password" wizard by mistake.
	const auto cloudCurrent = container->lifetime().make_state<CloudState>();
	const auto cloudLoaded = container->lifetime().make_state<bool>(false);
	session->api().cloudPassword().state(
	) | rpl::on_next([=](const CloudState &state) {
		*cloudCurrent = state;
		*cloudLoaded = true;
	}, container->lifetime());

	const auto openTwoStep = [=] {
		if (!*cloudLoaded) {
			controller->showSettings(::Settings::PrivacySecurityId());
			return;
		}
		const auto &state = *cloudCurrent;
		if (!state.unconfirmedPattern.isEmpty()) {
			controller->showSettings(
				::Settings::CloudPasswordEmailConfirmId());
		} else if (state.hasPassword) {
			controller->showSettings(::Settings::CloudPasswordInputId());
		} else {
			controller->showSettings(::Settings::CloudPasswordStartId());
		}
	};
	const auto openPrivacy = [=] {
		controller->showSettings(::Settings::PrivacySecurityId());
	};

	Ui::AddSkip(container);
	Ui::AddDividerText(container, AccountSummary(session));

	Ui::AddSkip(container);
	Ui::AddSubsectionTitle(
		container,
		TrValue(u"LuminaCheckupTwoStepHeader"_q));
	AddCheckupRow(
		container,
		TrValue(u"LuminaCheckupTwoStep"_q),
		TwoStepStatus(session),
		st::menuIcon2SV,
		openTwoStep);
	AddCheckupRow(
		container,
		TrValue(u"LuminaCheckupRecoveryEmail"_q),
		RecoveryStatus(session),
		st::menuIconRecoveryEmail,
		openTwoStep);

	Ui::AddSkip(container);
	Ui::AddSubsectionTitle(
		container,
		TrValue(u"LuminaCheckupPrivacyHeader"_q));
	AddCheckupRow(
		container,
		TrValue(u"LuminaCheckupInvites"_q),
		PrivacyStatus(session, PrivacyKey::Invites),
		st::menuIconGroups,
		openPrivacy);
	AddCheckupRow(
		container,
		TrValue(u"LuminaCheckupCalls"_q),
		PrivacyStatus(session, PrivacyKey::Calls),
		st::menuIconCallsReceive,
		openPrivacy);
	AddCheckupRow(
		container,
		TrValue(u"LuminaCheckupPhone"_q),
		PrivacyStatus(session, PrivacyKey::PhoneNumber),
		st::menuIconPhone,
		openPrivacy);

	Ui::AddSkip(container);
	Ui::AddSubsectionTitle(
		container,
		TrValue(u"LuminaCheckupDevicesHeader"_q));
	AddCheckupRow(
		container,
		TrValue(u"LuminaCheckupSessions"_q),
		SessionsStatus(session),
		st::menuIconDevices,
		[=] { controller->showSettings(::Settings::SessionsId()); });

	Ui::AddSkip(container);
	Ui::AddDividerText(container, TrValue(u"LuminaCheckupFooter"_q));

	session->api().cloudPassword().reload();
	session->api().authorizations().reload();
	session->api().userPrivacy().reload(PrivacyKey::Invites);
	session->api().userPrivacy().reload(PrivacyKey::Calls);
	session->api().userPrivacy().reload(PrivacyKey::PhoneNumber);
}

} // namespace Lumina
