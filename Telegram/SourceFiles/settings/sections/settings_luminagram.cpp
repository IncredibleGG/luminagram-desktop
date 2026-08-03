/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "settings/sections/settings_luminagram.h"

#include "settings/settings_common.h"
#include "lumina/lumina_settings.h"
#include "ui/rp_widget.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "ui/wrap/vertical_layout.h"
#include "styles/style_settings.h"
#include "styles/style_layers.h"

namespace Settings {
namespace {

using SettingsButton = Ui::SettingsButton;

// Add a labelled on/off row bound to a LuminaGram boolean preference.
void AddToggle(
		not_null<Ui::VerticalLayout*> container,
		const QString &label,
		bool checked,
		Fn<void(bool)> save) {
	const auto button = container->add(object_ptr<SettingsButton>(
		container,
		rpl::single(label),
		st::settingsButtonNoIcon
	))->toggleOn(rpl::single(checked));
	button->toggledChanges(
	) | rpl::start_with_next([=](bool value) {
		save(value);
	}, button->lifetime());
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
	return rpl::single(u"LuminaGram"_q);
}

void LuminaGram::setupContent() {
	const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);
	auto &settings = Lumina::Settings::Instance();

	// --- Interface ------------------------------------------------------
	Ui::AddSkip(content);
	Ui::AddSubsectionTitle(content, rpl::single(u"Interface"_q));
	AddToggle(
		content,
		u"Hide chat folders bar"_q,
		settings.hideChatFolders(),
		[](bool value) {
			Lumina::Settings::Instance().setHideChatFolders(value);
		});
	Ui::AddSkip(content);
	Ui::AddDividerText(
		content,
		rpl::single(u"Hides the folder tabs in the chats list. "
			"Restart may be required to fully apply."_q));

	// --- Privacy / stealth ---------------------------------------------
	Ui::AddSkip(content);
	Ui::AddSubsectionTitle(content, rpl::single(u"Privacy (stealth)"_q));
	AddToggle(
		content,
		u"Appear offline"_q,
		settings.stealthOnline(),
		[](bool value) {
			Lumina::Settings::Instance().setStealthOnline(value);
		});
	AddToggle(
		content,
		u"Don't send typing / recording status"_q,
		settings.stealthTyping(),
		[](bool value) {
			Lumina::Settings::Instance().setStealthTyping(value);
		});
	AddToggle(
		content,
		u"Don't send read receipts"_q,
		settings.stealthReadReceipts(),
		[](bool value) {
			Lumina::Settings::Instance().setStealthReadReceipts(value);
		});
	Ui::AddSkip(content);
	Ui::AddDividerText(
		content,
		rpl::single(u"When enabled, LuminaGram avoids telling the server "
			"you are online, typing or have read a message."_q));

	// --- Message actions -----------------------------------------------
	Ui::AddSkip(content);
	Ui::AddSubsectionTitle(content, rpl::single(u"Messages"_q));
	AddToggle(
		content,
		u"Forward without author (default)"_q,
		settings.forwardWithoutAuthor(),
		[](bool value) {
			Lumina::Settings::Instance().setForwardWithoutAuthor(value);
		});
	AddToggle(
		content,
		u"Allow save / copy from restricted chats"_q,
		settings.allowSaveRestricted(),
		[](bool value) {
			Lumina::Settings::Instance().setAllowSaveRestricted(value);
		});
	Ui::AddSkip(content);
	Ui::AddDividerText(
		content,
		rpl::single(u"\"Allow save / copy from restricted chats\" only "
			"affects local actions on this device. Some chats restrict "
			"saving for a reason - use responsibly."_q));

	// --- Translation ----------------------------------------------------
	Ui::AddSkip(content);
	Ui::AddSubsectionTitle(content, rpl::single(u"Translation"_q));
	AddToggle(
		content,
		u"Auto-translate incoming messages"_q,
		settings.autoTranslate(),
		[](bool value) {
			Lumina::Settings::Instance().setAutoTranslate(value);
		});
	Ui::AddSkip(content);
	Ui::AddDividerText(
		content,
		rpl::single(u"Experimental. Uses Telegram's built-in translation."_q));

	Ui::ResizeFitChild(this, content);
}

} // namespace Settings
