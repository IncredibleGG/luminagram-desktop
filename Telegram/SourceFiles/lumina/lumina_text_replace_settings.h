/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "base/basic_types.h"

namespace Ui {
class VerticalLayout;
} // namespace Ui

namespace Window {
class SessionController;
} // namespace Window

namespace Lumina {

// The text replacer's settings rows, in the F-02 sub-page shape: the Tools
// sub-page holds this one call and nothing else.
//
// Two rows - the master switch and a "Rules" row that opens the rule list in
// a box. The list is a box rather than a settings sub-page because a new
// sub-page would have to be registered in the hub, and the hub is written
// once in F-02 and never edited again.
//
// Every preference behind these rows is declared and defaulted in
// lumina/lumina_text_replace.h, which is also where the send path reads them.
void AddTextReplaceRows(
	not_null<Ui::VerticalLayout*> container,
	not_null<Window::SessionController*> controller);

} // namespace Lumina
