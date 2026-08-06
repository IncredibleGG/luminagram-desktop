/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_sticker_scale_settings.h"

#include "core/application.h"
#include "lang/lang_keys.h"
#include "lumina/lumina_locale.h"
#include "lumina/lumina_settings.h"
#include "lumina/lumina_sticker_scale.h"
#include "ui/boxes/confirm_box.h"
#include "ui/vertical_list.h"
#include "ui/widgets/checkbox.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"

#include "styles/style_settings.h"

namespace Lumina {
namespace {

[[nodiscard]] QString ChoiceText(int percent) {
	const auto number = QString::number(percent);
	return (percent == kStickerScaleDefault)
		? Tr(u"LuminaStickerSizeChoiceDefault"_q, number)
		: Tr(u"LuminaStickerSizeChoice"_q, number);
}

void OfferRestart(not_null<Window::SessionController*> controller) {
	controller->show(Ui::MakeConfirmBox({
		.text = tr::lng_settings_need_restart(),
		.confirmed = [] {
			Settings::Instance().saveNow();
			Core::Restart();
		},
		.confirmText = tr::lng_settings_restart_now(),
		.cancelText = tr::lng_settings_restart_later(),
	}));
}

} // namespace

void AddStickerScaleRows(
		not_null<Ui::VerticalLayout*> container,
		not_null<Window::SessionController*> controller) {
	Ui::AddSkip(container);
	Ui::AddSubsectionTitle(
		container,
		TrValue(u"LuminaAppearanceStickerSizeHeader"_q));
	Ui::AddSkip(container, st::settingsSendTypeSkip);

	const auto group = std::make_shared<Ui::RadiobuttonGroup>(StickerScale());
	for (const auto percent : kStickerScaleChoices) {
		const auto row = container->add(
			object_ptr<Ui::Radiobutton>(
				container,
				group,
				percent,
				ChoiceText(percent),
				st::settingsSendType),
			st::settingsSendTypePadding);
		LangChanges(
		) | rpl::on_next([=] {
			row->setText(ChoiceText(percent));
		}, row->lifetime());
	}

	// The preference can also move under the page - a restored backup writes
	// every key it carries - so the rows follow the preference rather than
	// only the clicks. `syncing` keeps that path from writing the value back
	// and, more to the point, from throwing a restart box at somebody who was
	// importing a backup rather than choosing a sticker size.
	const auto syncing = container->lifetime().make_state<bool>(false);
	group->setChangedCallback([=](int value) {
		if (*syncing) {
			return;
		}
		SetStickerScale(value);
		if (StickerScaleNeedsRestart()) {
			OfferRestart(controller);
		}
	});
	StickerScaleValue(
	) | rpl::on_next([=](int percent) {
		*syncing = true;
		group->setValue(percent);
		*syncing = false;
	}, container->lifetime());

	Ui::AddSkip(container, st::settingsSendTypeSkip);
	Ui::AddDividerText(container, TrValue(u"LuminaStickerSizeInfo"_q));
}

} // namespace Lumina
