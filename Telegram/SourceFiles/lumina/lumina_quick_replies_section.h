/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "settings/settings_common_session.h"

namespace Settings {

// The reply-template list, as a settings sub-page.
//
// A section rather than a box because the composer chooser links into it
// ("Manage templates"): a box opened from a popup menu over a chat would sit
// on top of the chat the user was writing in, while a section is pushed onto
// the same Info stack the settings hub uses and the back button returns to
// where it came from.
//
// Nothing central has to know this id exists. Settings::Section<T>::Id() is a
// pointer to a function-local static factory, so declaring the class is the
// whole registration - which is why this sub-page can live in lumina/ and be
// opened with controller->showSettings(LuminaReplyTemplatesId()) without
// touching the F-02 hub.
[[nodiscard]] Type LuminaReplyTemplatesId();

class LuminaReplyTemplates : public Section<LuminaReplyTemplates> {
public:
	LuminaReplyTemplates(
		QWidget *parent,
		not_null<Window::SessionController*> controller);
	~LuminaReplyTemplates();

	[[nodiscard]] rpl::producer<QString> title() override;

private:
	void setupContent(not_null<Ui::VerticalLayout*> container);

};

} // namespace Settings
