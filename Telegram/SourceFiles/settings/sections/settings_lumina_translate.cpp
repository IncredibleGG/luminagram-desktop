/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "settings/sections/settings_lumina_translate.h"

#include "ui/wrap/vertical_layout.h"
#include "ui/rp_widget.h"
#include "ui/vertical_list.h"

namespace Settings {

Type LuminaTranslateId() {
	return LuminaTranslate::Id();
}

LuminaTranslate::LuminaTranslate(
	QWidget *parent,
	not_null<Window::SessionController*> controller)
: Section(parent, controller) {
	const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);
	setupContent(content);
	Ui::ResizeFitChild(this, content);
}

LuminaTranslate::~LuminaTranslate() = default;

rpl::producer<QString> LuminaTranslate::title() {
	return rpl::single(u"Translation"_q);
}

// F-02 sub-page contract: this body stays an ordered list of one call per
// feature, each declared in that feature's own `lumina/*.h`. Nothing else
// belongs here, so that no two feature owners ever edit the same lines.
// Planned owners, in the order their rows should appear:
//   W1-B provider config, API keys, languages, mode, read scope;
//   W1-C tracker / entry-point gating and the per-chat translate toggle;
//   W1-D read language override;
//   W2-A translate-before-send pipeline and send-side scope;
//   W2-C dual-language display;
//   W2-D live inline send-translation preview.
void LuminaTranslate::setupContent(not_null<Ui::VerticalLayout*> container) {
	Ui::AddSkip(container);
	Ui::AddDividerText(
		container,
		rpl::single(u"Translation provider, languages and send-side "
			"translation options will appear here."_q));
}

} // namespace Settings
