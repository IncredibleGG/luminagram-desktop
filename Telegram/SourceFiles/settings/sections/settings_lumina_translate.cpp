/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "settings/sections/settings_lumina_translate.h"

#include "lumina/lumina_translate_settings.h"
#include "ui/wrap/vertical_layout.h"
#include "ui/rp_widget.h"

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
//
// Every preference this page shows - provider config, API keys, both language
// pickers, translate mode and both scopes - is declared, defaulted and
// rendered by lumina/lumina_translate_settings. The waves that act on those
// values (W1-C gating, W1-D read-language override, W2-A send pipeline, W2-C
// dual-language display, W2-D send preview) read them through that header's
// accessors and add no rows of their own here.
void LuminaTranslate::setupContent(not_null<Ui::VerticalLayout*> container) {
	Lumina::AddTranslateRows(container, controller());
}

} // namespace Settings
