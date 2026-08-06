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

// The four message-menu switches of W4-B on the LuminaGram > Chats sub-page,
// in the F-02 sub-page shape: settings_lumina_chat.cpp holds one call to this
// and nothing else of ours.
//
// Every switch only decides whether its row is offered in the message context
// menu; none of them changes what an existing action does. All four default
// off, so the page is behaviour-neutral until the user touches it.
void AddMessageActionsRows(
	not_null<Ui::VerticalLayout*> container,
	not_null<Window::SessionController*> controller);

} // namespace Lumina
