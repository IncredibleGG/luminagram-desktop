/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_update_settings.h"

#include "boxes/about_box.h"
#include "core/application.h"
#include "core/update_checker.h"
#include "core/version.h"
#include "lang/lang_keys.h"
#include "settings/settings_common.h"
#include "storage/localstorage.h"
#include "ui/text/format_values.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "ui/wrap/slide_wrap.h"
#include "ui/wrap/vertical_layout.h"

#include "styles/style_lumina.h"
#include "styles/style_settings.h"

namespace Lumina {
namespace {

// The channel a build follows, which is also the set of feed entries it is
// willing to see: ParseCommonMap() in core/update_checker.cpp asks for
// "stable" only, adds "beta" when cInstallBetaVersion() is set, and adds
// "alpha" on top for an alpha build. LuminaGram publishes "stable" and
// nothing else, so the other two resolve back to it - the name is reported
// rather than corrected, because a build that says "Beta" while taking
// stable packages is the honest description of that state.
[[nodiscard]] QString UpdateChannelName() {
	if (cAlphaVersion()) {
		return u"Alpha"_q;
	} else if (AppBetaVersion || cInstallBetaVersion()) {
		return u"Beta"_q;
	}
	return u"Stable"_q;
}

[[nodiscard]] QString DownloadText(int64 already, int64 total, bool percent) {
	if (!percent) {
		return Ui::FormatDownloadText(already, total);
	}
	const auto done = (total > 0) ? ((already * 100) / total) : 0;
	return QString::number(done) + '%';
}

} // namespace

void AddUpdateRows(not_null<Ui::VerticalLayout*> container) {
	Ui::AddSkip(container);

	// A fact, not a control. The row has always been inert - the attribute
	// below predates this - but while it was painted like its neighbours, with
	// the channel in the accent colour every pressable value row on these
	// pages uses, it read as a switch that ignores clicks. Giving it a real
	// action was the alternative and it would have been a lie: as
	// UpdateChannelName() above says, this fork publishes "stable" and nothing
	// else, so every entry in a channel picker would install the same
	// packages. st::luminaSettingsFactRow states the channel in ordinary
	// secondary text instead.
	const auto version = ::Settings::AddButtonWithLabel(
		container,
		tr::lng_settings_current_version(
			lt_version,
			rpl::single(currentVersionText())),
		rpl::single(UpdateChannelName()),
		st::luminaSettingsFactRow);
	version->setAttribute(Qt::WA_TransparentForMouseEvents);

	if (Core::UpdaterDisabled()) {
		Ui::AddSkip(container);
		return;
	}

	const auto toggle = container->add(object_ptr<Ui::SettingsButton>(
		container,
		tr::lng_settings_update_automatically(),
		st::settingsButtonNoIcon
	))->toggleOn(rpl::single(cAutoUpdate()));

	const auto wrap = container->add(
		object_ptr<Ui::SlideWrap<Ui::VerticalLayout>>(
			container,
			object_ptr<Ui::VerticalLayout>(container)));
	const auto inner = wrap->entity();

	const auto texts = Ui::CreateChild<rpl::event_stream<QString>>(
		container.get());
	const auto check = ::Settings::AddButtonWithLabel(
		inner,
		tr::lng_settings_check_now(),
		texts->events(),
		st::settingsButtonNoIcon);
	const auto restart = Ui::CreateChild<Ui::SettingsButton>(
		check.get(),
		tr::lng_settings_restart_now(),
		st::settingsUpdate);
	restart->hide();
	check->widthValue() | rpl::on_next([=](int width) {
		restart->resizeToWidth(width);
		restart->moveToLeft(0, 0);
	}, restart->lifetime());

	const auto setDefaultStatus = [=](const Core::UpdateChecker &checker) {
		using State = Core::UpdateChecker::State;
		switch (checker.state()) {
		case State::Download:
			texts->fire(tr::lng_settings_downloading_update(
				tr::now,
				lt_progress,
				DownloadText(
					checker.already(),
					checker.size(),
					checker.percent())));
			break;
		case State::Ready:
			texts->fire(tr::lng_settings_update_ready(tr::now));
			restart->show();
			break;
		default:
			texts->fire(QString());
			break;
		}
	};

	toggle->toggledChanges(
	) | rpl::filter([](bool toggled) {
		return (toggled != cAutoUpdate());
	}) | rpl::on_next([=](bool toggled) {
		cSetAutoUpdate(toggled);

		Local::writeSettings();
		Core::UpdateChecker checker;
		if (cAutoUpdate()) {
			checker.start();
		} else {
			checker.stop();
			setDefaultStatus(checker);
		}
	}, toggle->lifetime());

	wrap->toggleOn(toggle->toggledValue());
	wrap->finishAnimating();

	Core::UpdateChecker checker;
	checker.checking() | rpl::on_next([=] {
		texts->fire(tr::lng_settings_update_checking(tr::now));
	}, check->lifetime());
	checker.isLatest() | rpl::on_next([=] {
		texts->fire(tr::lng_settings_latest_installed(tr::now));
	}, check->lifetime());
	checker.progress(
	) | rpl::on_next([=](Core::UpdateChecker::Progress progress) {
		texts->fire(tr::lng_settings_downloading_update(
			tr::now,
			lt_progress,
			DownloadText(progress.already, progress.size, progress.percent)));
	}, check->lifetime());
	checker.failed() | rpl::on_next([=] {
		texts->fire(tr::lng_settings_update_fail(tr::now));
	}, check->lifetime());
	checker.ready() | rpl::on_next([=] {
		texts->fire(tr::lng_settings_update_ready(tr::now));
		restart->show();
	}, check->lifetime());

	setDefaultStatus(checker);

	check->setClickedCallback([] {
		Core::UpdateChecker checker;

		cSetLastUpdateCheck(0);
		checker.start();
	});
	restart->setClickedCallback([] {
		if (!Core::UpdaterDisabled()) {
			Core::checkReadyUpdate();
		}
		Core::Restart();
	});

	Ui::AddSkip(container);
}

} // namespace Lumina
