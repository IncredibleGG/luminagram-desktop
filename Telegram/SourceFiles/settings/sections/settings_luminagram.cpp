/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "settings/sections/settings_luminagram.h"

#include "lumina/lumina_onboarding.h"
#include "lumina/lumina_update_settings.h"
#include "lumina/lumina_locale.h"
#include "settings/sections/settings_lumina_appearance.h"
#include "settings/sections/settings_lumina_chat.h"
#include "settings/sections/settings_lumina_chat_list.h"
#include "settings/sections/settings_lumina_privacy.h"
#include "settings/sections/settings_lumina_security.h"
#include "settings/sections/settings_lumina_tools.h"
#include "settings/sections/settings_lumina_translate.h"
#include "settings/sections/settings_lumina_voice.h"
#include "settings/settings_common.h"
#include "ui/widgets/buttons.h"
#include "ui/wrap/vertical_layout.h"
#include "ui/rp_widget.h"
#include "ui/vertical_list.h"

#include "styles/style_menu_icons.h"
#include "styles/style_settings.h"

namespace Settings {
namespace {

void AddSectionRow(
		not_null<Ui::VerticalLayout*> container,
		rpl::producer<QString> label,
		const style::icon &icon,
		Fn<void()> callback) {
	AddButtonWithIcon(
		container,
		std::move(label),
		st::settingsButton,
		{ &icon }
	)->setClickedCallback(std::move(callback));
}

} // namespace

Type LuminaGramId() {
	return LuminaGram::Id();
}

LuminaGram::LuminaGram(
	QWidget *parent,
	not_null<Window::SessionController*> controller)
: Section(parent, controller) {
	setupContent();
}

LuminaGram::~LuminaGram() = default;

rpl::producer<QString> LuminaGram::title() {
	return Lumina::TrValue(u"LuminaGramTitle"_q);
}

void LuminaGram::setupContent() {
	const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);
	Lumina::SetupFirstRunOnboarding(content, controller());
	const auto showOther = showOtherMethod();

	Ui::AddSkip(content);
	AddSectionRow(
		content,
		Lumina::TrValue(u"LuminaTranslateTitle"_q),
		st::menuIconTranslate,
		[=] { showOther(LuminaTranslateId()); });
	AddSectionRow(
		content,
		Lumina::TrValue(u"LuminaVoiceToTextTitle"_q),
		st::menuIconSoundOn,
		[=] { showOther(LuminaVoiceId()); });
	AddSectionRow(
		content,
		Lumina::TrValue(u"LuminaPrivacyTitle"_q),
		st::menuIconStealth,
		[=] { showOther(LuminaPrivacyId()); });
	AddSectionRow(
		content,
		Lumina::TrValue(u"LuminaSecurityTitle"_q),
		st::menuIconLock,
		[=] { showOther(LuminaSecurityId()); });
	AddSectionRow(
		content,
		Lumina::TrValue(u"LuminaChatSettings"_q),
		st::menuIconChatBubble,
		[=] { showOther(LuminaChatId()); });
	AddSectionRow(
		content,
		Lumina::TrValue(u"LuminaGramChatList"_q),
		st::menuIconChats,
		[=] { showOther(LuminaChatListId()); });
	AddSectionRow(
		content,
		Lumina::TrValue(u"LuminaAppearanceTitle"_q),
		st::menuIconPalette,
		[=] { showOther(LuminaAppearanceId()); });
	AddSectionRow(
		content,
		Lumina::TrValue(u"LuminaToolsTitle"_q),
		st::menuIconManage,
		[=] { showOther(LuminaToolsId()); });
	Ui::AddSkip(content);
	Ui::AddDividerText(
		content,
		Lumina::TrValue(u"LuminaGramStoredLocallyInfo"_q));

	Lumina::AddUpdateRows(content);

	Ui::ResizeFitChild(this, content);
}

} // namespace Settings
