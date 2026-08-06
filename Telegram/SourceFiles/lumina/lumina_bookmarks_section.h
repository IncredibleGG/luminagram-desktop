/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "settings/settings_common_session.h"

namespace Settings {

// The bookmark list, as a settings sub-page.
//
// It is a Settings::Section rather than a box because it is a browsable list
// that navigates INTO chats: a box would have to close itself to open the
// message, losing the list, while a section is pushed onto the same Info stack
// the hub uses and the back button returns to it.
//
// Nothing central has to know this id exists. Settings::Section<T>::Id() is a
// pointer to a function-local static factory, so declaring the class is the
// whole registration - which is why this sub-page can live in lumina/ and be
// opened with controller->showSettings(LuminaBookmarksId()) without touching
// the F-02 hub, which is written once and never edited again.
[[nodiscard]] Type LuminaBookmarksId();

class LuminaBookmarks : public Section<LuminaBookmarks> {
public:
	LuminaBookmarks(
		QWidget *parent,
		not_null<Window::SessionController*> controller);
	~LuminaBookmarks();

	[[nodiscard]] rpl::producer<QString> title() override;

private:
	void setupContent(not_null<Ui::VerticalLayout*> container);

};

} // namespace Settings
