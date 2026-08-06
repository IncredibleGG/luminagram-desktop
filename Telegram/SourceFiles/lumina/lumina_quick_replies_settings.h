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

// The reply-template settings rows, in the F-02 sub-page shape: the sub-page
// holds this one call and nothing else.
//
// Two rows - the list, which pushes the Settings::LuminaReplyTemplates
// sub-page, and the switch that decides whether the composer offers the
// templates at all. Android splits neither, because it has no switch; see
// lumina/lumina_quick_replies.h for why this one exists and why it is the one
// default-true preference in this item.
void AddReplyTemplatesRows(
	not_null<Ui::VerticalLayout*> container,
	not_null<Window::SessionController*> controller);

} // namespace Lumina
