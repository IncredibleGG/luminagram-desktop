/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "settings/settings_common_session.h"

namespace Settings {

[[nodiscard]] Type LuminaSecurityId();

class LuminaSecurity : public Section<LuminaSecurity> {
public:
	LuminaSecurity(
		QWidget *parent,
		not_null<Window::SessionController*> controller);
	~LuminaSecurity();

	[[nodiscard]] rpl::producer<QString> title() override;

private:
	void setupContent(not_null<Ui::VerticalLayout*> container);

};

} // namespace Settings
