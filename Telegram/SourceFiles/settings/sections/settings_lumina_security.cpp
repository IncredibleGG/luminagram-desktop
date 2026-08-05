/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "settings/sections/settings_lumina_security.h"

#include "ui/wrap/vertical_layout.h"
#include "ui/rp_widget.h"
#include "ui/vertical_list.h"

namespace Settings {

Type LuminaSecurityId() {
	return LuminaSecurity::Id();
}

LuminaSecurity::LuminaSecurity(
	QWidget *parent,
	not_null<Window::SessionController*> controller)
: Section(parent, controller) {
	const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);
	setupContent(content);
	Ui::ResizeFitChild(this, content);
}

LuminaSecurity::~LuminaSecurity() = default;

rpl::producer<QString> LuminaSecurity::title() {
	return rpl::single(u"Security"_q);
}

// F-02 sub-page contract: this body stays an ordered list of one call per
// feature, each declared in that feature's own `lumina/*.h`.
// Planned owners, in the order their rows should appear:
//   W3-A disguise vault (gate, password door, calculator and notepad decoys);
//   W3-B panic wipe;
//   W3-C fake-crash duress unlock.
void LuminaSecurity::setupContent(not_null<Ui::VerticalLayout*> container) {
	Ui::AddSkip(container);
	Ui::AddDividerText(
		container,
		rpl::single(u"Disguise vault, panic wipe and duress unlock options "
			"will appear here."_q));
}

} // namespace Settings
