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

// The "always show unread count" row of the LuminaGram Chat list sub-page, in
// the F-02 sub-page shape: the section .cpp holds this one call and nothing
// else. The preference itself is declared by lumina/lumina_muted_badge.h,
// which is also what the chat list paint path reads.
//
// This block deliberately emits no subsection title. On Android the row sits
// under the shared "Badges & icons" header (LuminaChatListBadgesHeader)
// together with the online dot and the recency dot, and on desktop those two
// dots are a different feature owning a different file, which already emits a
// header of its own. A second header here would show that heading twice, so
// this block is a plain divider-separated one, appended after theirs.
void AddMutedBadgeRows(
	not_null<Ui::VerticalLayout*> container,
	not_null<Window::SessionController*> controller);

} // namespace Lumina
