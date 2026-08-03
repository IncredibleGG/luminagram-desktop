/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "settings/settings_common_session.h"

namespace Settings {

[[nodiscard]] Type LuminaGramId();

class LuminaGram : public Section<LuminaGram> {
public:
	LuminaGram(
		QWidget *parent,
		not_null<Window::SessionController*> controller);
	~LuminaGram();

	[[nodiscard]] rpl::producer<QString> title() override;

private:
	void setupContent();

};

} // namespace Settings
