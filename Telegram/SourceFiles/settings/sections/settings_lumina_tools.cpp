/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "settings/sections/settings_lumina_tools.h"

#include "lumina/lumina_backup_settings.h"
#include "lumina/lumina_profile_card_settings.h"
#include "lumina/lumina_contact_notes_settings.h"
#include "lumina/lumina_bookmarks_settings.h"
#include "lumina/lumina_locale.h"
#include "ui/wrap/vertical_layout.h"
#include "ui/rp_widget.h"
#include "ui/vertical_list.h"

namespace Settings {

Type LuminaToolsId() {
	return LuminaTools::Id();
}

LuminaTools::LuminaTools(
	QWidget *parent,
	not_null<Window::SessionController*> controller)
: Section(parent, controller) {
	const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);
	setupContent(content);
	Ui::ResizeFitChild(this, content);
}

LuminaTools::~LuminaTools() = default;

rpl::producer<QString> LuminaTools::title() {
	return Lumina::TrValue(u"LuminaToolsTitle"_q);
}

// F-02 sub-page contract: this body stays an ordered list of one call per
// feature, each declared in that feature's own `lumina/*.h`.
// Planned owners, in the order their rows should appear:
//   W4-A message bookmarks list;
//   W4-D text replacer / outgoing auto-substitution;
//   W6-D reply templates;
//   W6-F local profile card;
//   W6-E encrypted local backup export / import;
//   W6-G first-run onboarding card.
void LuminaTools::setupContent(not_null<Ui::VerticalLayout*> container) {
	Lumina::AddBookmarksRows(container, controller());
	Lumina::AddContactNotesRows(container, controller());
	Lumina::AddProfileCardRows(container, controller());
	Lumina::AddBackupRows(container, controller());
}

} // namespace Settings
