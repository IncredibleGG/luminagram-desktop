/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_stories_off_settings.h"

#include "lumina/lumina_locale.h"
#include "lumina/lumina_stories_off.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "ui/wrap/slide_wrap.h"
#include "ui/wrap/vertical_layout.h"

#include "styles/style_settings.h"

namespace Lumina {
namespace {

// The block's explanation, which grows a second paragraph while the
// sub-option is on screen.
//
// The two texts used to be two dividers, with the master switch's sitting
// BETWEEN the two toggles: "Also hide the post button" was therefore drawn
// after the grey paragraph that explains "Turn stories off completely", which
// reads as a row belonging to nothing rather than as that switch's
// sub-option. A block on these pages is title, rows, prose - in that order -
// and this keeps the pair of toggles together with one piece of prose under
// them.
[[nodiscard]] rpl::producer<QString> InfoValue() {
	return rpl::combine(
		TrValue(u"LuminaStoriesFullyOffInfo"_q),
		TrValue(u"LuminaStoriesHidePostEntryInfo"_q),
		StoriesFullyOffValue()
	) | rpl::map([](
			const QString &fully,
			const QString &post,
			bool subOptionShown) {
		return subOptionShown ? (fully + u"\n\n"_q + post) : fully;
	});
}

} // namespace

void AddStoriesOffRows(
		not_null<Ui::VerticalLayout*> container,
		not_null<Window::SessionController*>) {
	Ui::AddSkip(container);
	Ui::AddSubsectionTitle(container, TrValue(u"LuminaStoriesHeader"_q));

	const auto fully = container->add(object_ptr<Ui::SettingsButton>(
		container,
		TrValue(u"LuminaStoriesFullyOff"_q),
		st::settingsButtonNoIcon
	))->toggleOn(StoriesFullyOffValue());
	fully->toggledChanges(
	) | rpl::on_next([](bool value) {
		SetStoriesFullyOff(value);
	}, fully->lifetime());

	// The sub-option means nothing on its own - StoriesPostEntryHidden() is
	// the AND of the two - so it is only shown while the master switch is on,
	// which is what Android's LuminaChatListActivity does with the same pair.
	// It sits directly under its master, with no divider between them, so the
	// two read as one setting.
	const auto wrap = container->add(
		object_ptr<Ui::SlideWrap<Ui::VerticalLayout>>(
			container,
			object_ptr<Ui::VerticalLayout>(container)));
	wrap->toggleOn(StoriesFullyOffValue());
	wrap->finishAnimating();

	const auto inner = wrap->entity();
	const auto post = inner->add(object_ptr<Ui::SettingsButton>(
		inner,
		TrValue(u"LuminaStoriesHidePostEntry"_q),
		st::settingsButtonNoIcon
	))->toggleOn(StoriesHidePostEntryValue());
	post->toggledChanges(
	) | rpl::on_next([](bool value) {
		SetStoriesHidePostEntry(value);
	}, post->lifetime());

	Ui::AddSkip(container);
	Ui::AddDividerText(container, InfoValue());
}

} // namespace Lumina
