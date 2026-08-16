/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "settings/sections/settings_lumina_security_checkup.h"

#include "lumina/lumina_security_checkup.h"
#include "lumina/lumina_locale.h"
#include "ui/wrap/vertical_layout.h"
#include "ui/rp_widget.h"
#include "ui/vertical_list.h"

namespace Settings {

Type LuminaSecurityCheckupId() {
	return LuminaSecurityCheckup::Id();
}

LuminaSecurityCheckup::LuminaSecurityCheckup(
	QWidget *parent,
	not_null<Window::SessionController*> controller)
: Section(parent, controller) {
	const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);
	setupContent(content);
	Ui::ResizeFitChild(this, content);
}

LuminaSecurityCheckup::~LuminaSecurityCheckup() = default;

rpl::producer<QString> LuminaSecurityCheckup::title() {
	return Lumina::TrValue(u"LuminaCheckupTitle"_q);
}

void LuminaSecurityCheckup::setupContent(
		not_null<Ui::VerticalLayout*> container) {
	Lumina::AddSecurityCheckupRows(container, controller());
}

} // namespace Settings
