/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_exif_strip_settings.h"

#include "lumina/lumina_exif_strip.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "ui/wrap/vertical_layout.h"

#include "styles/style_settings.h"

namespace Lumina {

void AddExifStripRows(
		not_null<Ui::VerticalLayout*> container,
		not_null<Window::SessionController*>) {
	RefreshStripPhotoLocationCache();

	Ui::AddSkip(container);
	const auto button = container->add(object_ptr<Ui::SettingsButton>(
		container,
		rpl::single(u"Remove photo location before sending"_q),
		st::settingsButtonNoIcon
	))->toggleOn(StripPhotoLocationValue());
	button->toggledChanges(
	) | rpl::on_next([](bool value) {
		SetStripPhotoLocation(value);
	}, button->lifetime());
	Ui::AddSkip(container);
	Ui::AddDividerText(
		container,
		rpl::single(u"Erase the GPS coordinates a camera stored inside a "
			u"JPEG photo before it is uploaded — both when you send the "
			u"photo the ordinary compressed way and when you send it as a "
			u"file. "
			u"Only the location tags go: orientation, camera and date tags "
			u"are left exactly as they were, so a photo sent as a file still "
			u"shows the right way up. Your own copy of the file on disk is "
			u"never modified. This does not cover videos, a location written "
			u"into a photo by XMP or by a camera's own maker notes, or the "
			u"second copy of the picture that some phones tuck into the same "
			u"file."_q));
}

} // namespace Lumina
