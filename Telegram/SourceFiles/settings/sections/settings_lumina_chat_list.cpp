/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "settings/sections/settings_lumina_chat_list.h"

#include "ui/wrap/vertical_layout.h"
#include "ui/rp_widget.h"
#include "ui/vertical_list.h"

namespace Settings {

Type LuminaChatListId() {
	return LuminaChatList::Id();
}

LuminaChatList::LuminaChatList(
	QWidget *parent,
	not_null<Window::SessionController*> controller)
: Section(parent, controller) {
	const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);
	setupContent(content);
	Ui::ResizeFitChild(this, content);
}

LuminaChatList::~LuminaChatList() = default;

rpl::producer<QString> LuminaChatList::title() {
	return rpl::single(u"Chat list"_q);
}

// The old "Hide chat folders bar" row lived on the flat LuminaGram page and
// was never read by anything, so it is not carried over here as a no-op. W5-A
// owns the real implementation and re-adds the row with it.
//
// F-02 sub-page contract: this body stays an ordered list of one call per
// feature, each declared in that feature's own `lumina/*.h`.
// Planned owners, in the order their rows should appear:
//   W5-A hide folder tabs and hide the stories row;
//   W5-B corner badges (online-dot opt-out, last-seen recency dot);
//   W5-C muted chats' unread badge in the accent colour;
//   W6-A compact chat-list rows.
void LuminaChatList::setupContent(not_null<Ui::VerticalLayout*> container) {
	Ui::AddSkip(container);
	Ui::AddDividerText(
		container,
		rpl::single(u"Chat list layout and badge options will appear "
			"here."_q));
}

} // namespace Settings
