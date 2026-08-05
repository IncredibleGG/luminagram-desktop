/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "settings/sections/settings_lumina_chat.h"

#include "lumina/lumina_settings.h"
#include "ui/widgets/buttons.h"
#include "ui/wrap/vertical_layout.h"
#include "ui/rp_widget.h"
#include "ui/vertical_list.h"

#include "styles/style_settings.h"

#include <QtCore/QJsonValue>

namespace Settings {
namespace {

void AddToggle(
		not_null<Ui::VerticalLayout*> container,
		const QString &label,
		const QString &key) {
	const auto button = container->add(object_ptr<Ui::SettingsButton>(
		container,
		rpl::single(label),
		st::settingsButtonNoIcon
	))->toggleOn(rpl::single(Lumina::Settings::Instance().getBool(key)));
	button->toggledChanges(
	) | rpl::on_next([=](bool value) {
		Lumina::Settings::Instance().set(key, value);
	}, button->lifetime());
}

} // namespace

Type LuminaChatId() {
	return LuminaChat::Id();
}

LuminaChat::LuminaChat(
	QWidget *parent,
	not_null<Window::SessionController*> controller)
: Section(parent, controller) {
	const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);
	setupContent(content);
	Ui::ResizeFitChild(this, content);
}

LuminaChat::~LuminaChat() = default;

rpl::producer<QString> LuminaChat::title() {
	return rpl::single(u"Chats"_q);
}

// F-02 sub-page contract: apart from the one row below, which already ships
// and is already wired (`HistoryItem::forbidsSaving`), this body stays an
// ordered list of one call per feature, each declared in that feature's own
// `lumina/*.h`.
// Planned owners, in the order their rows should appear:
//   W4-B forward without author / without captions, save to Saved Messages,
//        message details;
//   W4-C select all messages from this author;
//   W4-E undo-send window;
//   W4-F confirm before sending voice / video messages.
void LuminaChat::setupContent(not_null<Ui::VerticalLayout*> container) {
	Ui::AddSkip(container);
	Ui::AddSubsectionTitle(container, rpl::single(u"Message actions"_q));
	AddToggle(
		container,
		u"Allow save / copy from restricted chats"_q,
		u"allowSaveRestricted"_q);
	Ui::AddSkip(container);
	Ui::AddDividerText(
		container,
		rpl::single(u"\"Allow save / copy from restricted chats\" only "
			"affects local actions on this device. Some chats restrict "
			"saving for a reason - use responsibly."_q));
}

} // namespace Settings
