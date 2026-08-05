/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "settings/sections/settings_lumina_privacy.h"

#include "ui/wrap/vertical_layout.h"
#include "ui/rp_widget.h"
#include "ui/vertical_list.h"

namespace Settings {

Type LuminaPrivacyId() {
	return LuminaPrivacy::Id();
}

LuminaPrivacy::LuminaPrivacy(
	QWidget *parent,
	not_null<Window::SessionController*> controller)
: Section(parent, controller) {
	const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);
	setupContent(content);
	Ui::ResizeFitChild(this, content);
}

LuminaPrivacy::~LuminaPrivacy() = default;

rpl::producer<QString> LuminaPrivacy::title() {
	return rpl::single(u"Privacy"_q);
}

// The stealth cluster - "Appear offline" (`stealthOnline`), "Don't send
// typing / recording status" (`stealthTyping`) and "Don't send read receipts"
// (`stealthReadReceipts`) - is parked project-wide as ToS-risky and has no UI
// on purpose. The preferences are deliberately kept, as is the code that
// reads two of them: `stealthOnline` in `api/api_updates.cpp` and
// `stealthTyping` in `api/api_send_progress.cpp`. `stealthReadReceipts` has
// no reader at all - the pref exists but nothing acts on it, so it is not
// merely hidden, it is unimplemented. Only the rows are gone, pending a
// product decision. Do not re-add them without one.
//
// F-02 sub-page contract: this body stays an ordered list of one call per
// feature, each declared in that feature's own `lumina/*.h`.
// Planned owners, in the order their rows should appear:
//   W3-E link safety inspector;
//   W3-D crypto-address clipboard paste guard;
//   W3-F scam keyword warning;
//   W4-H strip photo GPS / EXIF before upload;
//   W4-G disable link preview by default;
//   W6-B account registration date row.
void LuminaPrivacy::setupContent(not_null<Ui::VerticalLayout*> container) {
	Ui::AddSkip(container);
	Ui::AddDividerText(
		container,
		rpl::single(u"Link, clipboard and outgoing-media privacy options "
			"will appear here."_q));
}

} // namespace Settings
