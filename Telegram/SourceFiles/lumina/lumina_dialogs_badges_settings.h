/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

namespace Ui {
class VerticalLayout;
} // namespace Ui

namespace Window {
class SessionController;
} // namespace Window

namespace Lumina {

// The chat-list sub-page rows for the avatar corner dot: the online-dot
// opt-out and the last-seen recency dot. Android keeps both under
// LuminaChatListActivity's "Badges & icons" header, next to the muted-badge and
// mute-icon rows that other items in this wave own; this call brings only its
// own two rows and its own subsection title.
void AddChatListDotRows(
	not_null<Ui::VerticalLayout*> container,
	not_null<Window::SessionController*> controller);

} // namespace Lumina
