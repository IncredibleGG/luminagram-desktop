/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "settings/settings_common_session.h"

namespace Settings {

[[nodiscard]] Type LuminaChatId();

class LuminaChat : public Section<LuminaChat> {
public:
	LuminaChat(
		QWidget *parent,
		not_null<Window::SessionController*> controller);
	~LuminaChat();

	[[nodiscard]] rpl::producer<QString> title() override;

private:
	void setupContent(not_null<Ui::VerticalLayout*> container);

};

} // namespace Settings
