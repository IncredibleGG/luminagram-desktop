/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_update_settings.h"

#include "base/unique_qptr.h"
#include "boxes/about_box.h"
#include "core/application.h"
#include "core/launcher.h"
#include "core/update_checker.h"
#include "core/version.h"
#include "lang/lang_keys.h"
#include "settings/settings_common.h"
#include "storage/localstorage.h"
#include "ui/text/format_values.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/popup_menu.h"
#include "ui/wrap/slide_wrap.h"
#include "ui/wrap/vertical_layout.h"

#include "styles/style_lumina.h"
#include "styles/style_settings.h"

#include <QtGui/QCursor>

namespace Lumina {
namespace {

// The channel a build follows, which is also the set of feed entries it is
// willing to see: ParseCommonMap() in core/update_checker.cpp asks for
// "stable" only, adds "beta" when cInstallBetaVersion() is set, and adds
// "alpha" on top for an alpha build. LuminaGram now publishes a real beta
// channel alongside stable, so a build with cInstallBetaVersion() set sees
// and installs the feed's beta entries; alpha stays a build-time channel
// with no published feed of its own.
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

	// The channel row. When the updater is compiled out, or this is an alpha
	// build - a build-time channel no in-app switch can leave - the channel
	// is an inert fact, spelled out in ordinary secondary text so it does not
	// read as a control that ignores clicks. Otherwise it is a real picker:
	// LuminaGram now publishes a beta channel alongside stable, the desktop
	// updater already asks the feed for beta entries when cInstallBetaVersion()
	// is set, so switching here changes which build the next check offers.
	if (Core::UpdaterDisabled() || cAlphaVersion()) {
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
	} else {
		// The label is driven by a stream primed just below and fired again
		// after a switch - the same idiom the "check now" row uses for its
		// status - so the shown channel refreshes in place, no page rebuild.
		const auto channel = Ui::CreateChild<rpl::event_stream<QString>>(
			container.get());
		const auto version = ::Settings::AddButtonWithLabel(
			container,
			tr::lng_settings_current_version(
				lt_version,
				rpl::single(currentVersionText())),
			channel->events(),
			st::settingsButtonNoIcon);
		const auto menu = version->lifetime().make_state<
			base::unique_qptr<Ui::PopupMenu>>();
		const auto choose = [=](bool beta) {
			if (beta == cInstallBetaVersion()) {
				return;
			}
			cSetInstallBetaVersion(beta);
			Core::Launcher::Instance().writeInstallBetaVersionsSetting();
			channel->fire(UpdateChannelName());

			// Kick a check off the same shared updater the rest of this block
			// drives, exactly as the "check now" row below does, so the other
			// channel's build is offered right away.
			Core::UpdateChecker checker;
			cSetLastUpdateCheck(0);
			checker.start();
		};
		version->setClickedCallback([=] {
			*menu = base::make_unique_q<Ui::PopupMenu>(version.get());
			(*menu)->addAction(u"Stable"_q, [=] { choose(false); });
			(*menu)->addAction(u"Beta"_q, [=] { choose(true); });
			(*menu)->popup(QCursor::pos());
		});
		channel->fire(UpdateChannelName());
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
