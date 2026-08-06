/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "settings/sections/settings_lumina_appearance.h"

#include "lumina/lumina_media_pause_settings.h"
#include "lumina/lumina_number_format_settings.h"
#include "lumina/lumina_time_format_settings.h"
#include "lumina/lumina_recent_limits_settings.h"
#include "lumina/lumina_sticker_save_settings.h"
#include "lumina/lumina_sticker_scale_settings.h"
#include "lumina/lumina_locale.h"
#include "ui/wrap/vertical_layout.h"
#include "ui/rp_widget.h"
#include "ui/vertical_list.h"

namespace Settings {

Type LuminaAppearanceId() {
	return LuminaAppearance::Id();
}

LuminaAppearance::LuminaAppearance(
	QWidget *parent,
	not_null<Window::SessionController*> controller)
: Section(parent, controller) {
	const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);
	setupContent(content);
	Ui::ResizeFitChild(this, content);
}

LuminaAppearance::~LuminaAppearance() = default;

rpl::producer<QString> LuminaAppearance::title() {
	return Lumina::TrValue(u"LuminaAppearanceTitle"_q);
}

// F-02 sub-page contract: this body stays an ordered list of one call per
// feature, each declared in that feature's own `lumina/*.h`.
// Planned owners, in the order their rows should appear:
//   W5-E adjustable sticker size;
//   W5-F show seconds in message time;
//   W5-D show exact numbers instead of rounded ones;
//   W5-I pause video when the app goes to background;
//   W5-G save a sticker from the sticker panel;
//   W5-H more retained recent stickers / GIFs;
//   W4-A the `showBookmarks` toggle.
void LuminaAppearance::setupContent(not_null<Ui::VerticalLayout*> container) {
	Lumina::AddStickerScaleRows(container, controller());
	Lumina::AddStickerSaveRows(container, controller());
	Lumina::AddRecentLimitsRows(container, controller());
	Lumina::AddTimeFormatRows(container, controller());
	Lumina::AddExactNumbersRows(container, controller());
	Lumina::AddMediaPauseRows(container, controller());
}

} // namespace Settings
