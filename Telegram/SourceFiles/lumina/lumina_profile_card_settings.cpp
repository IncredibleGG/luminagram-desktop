/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_profile_card_settings.h"

#include "lang/lang_keys.h"
#include "lumina/lumina_locale.h"
#include "lumina/lumina_profile_card.h"
#include "settings/settings_common.h"
#include "ui/layers/generic_box.h"
#include "ui/layers/show.h"
#include "ui/text/text_entity.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/fields/input_field.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"

#include "styles/style_layers.h"
#include "styles/style_menu_icons.h"
#include "styles/style_settings.h"
#include "styles/style_widgets.h"

namespace Lumina {
namespace {

// How much of a field fits on one row before it is elided. A character count,
// not a dimension - nothing here is measured in pixels.
constexpr auto kPreviewMaxLength = 40;

// Every label on these rows is recomputed from the store rather than kept in
// the widget, so a row also follows a card written by a backup restore through
// Settings::importAll() and by the panic wipe, which removes the key outright.
// LangChanges() is merged in because the "Not set" placeholder is itself a
// translated string.
[[nodiscard]] rpl::producer<QString> CardValue(Fn<QString()> compute) {
	return rpl::single(
		rpl::empty
	) | rpl::then(
		rpl::merge(ProfileCardChanges(), LangChanges())
	) | rpl::map([compute = std::move(compute)] {
		return compute();
	});
}

[[nodiscard]] QString Preview(const QString &text) {
	auto result = text;
	result.replace(QChar(u'\n'), QChar(u' '));
	result.replace(QChar(u'\r'), QChar(u' '));
	result = result.trimmed();
	if (result.isEmpty()) {
		return Tr(u"LuminaProfileCardNotSet"_q);
	} else if (result.size() > kPreviewMaxLength) {
		return result.left(kPreviewMaxLength) + QChar(0x2026);
	}
	return result;
}

[[nodiscard]] QString FirstFilled(const ProfileCard &card) {
	for (const auto &info : ProfileCardFields()) {
		const auto value = (card.*(info.value)).trimmed();
		if (!value.isEmpty()) {
			return value;
		}
	}
	return QString();
}

void CopyCard(std::shared_ptr<Ui::Show> show) {
	const auto card = CurrentProfileCard();
	if (ProfileCardEmpty(card)) {
		// Deliberately not an empty clipboard: a card that has nothing in it
		// must not silently destroy whatever the user had copied before.
		show->showToast(Tr(u"LuminaProfileCardEmptyShare"_q));
		return;
	}
	TextUtilities::SetClipboardText(
		TextForMimeData::Simple(ProfileCardShareText(card)));
	show->showToast(Tr(u"LuminaProfileCardCopied"_q));
}

// Writes through Lumina::SetProfileCard() and nothing else: the rows behind it
// rebuild from ProfileCardChanges(), so this box never needs to know who
// opened it or hold a pointer back into it.
void EditFieldBox(
		not_null<Ui::GenericBox*> box,
		ProfileCardFieldInfo info) {
	box->setTitle(TrValue(info.labelKey));

	const auto field = box->addRow(object_ptr<Ui::InputField>(
		box,
		st::defaultInputField,
		(info.multiline
			? Ui::InputField::Mode::MultiLine
			: Ui::InputField::Mode::SingleLine),
		TrValue(info.hintKey),
		CurrentProfileCard().*(info.value)));
	field->setMaxLength(info.maxLength);
	box->setFocusCallback([=] {
		field->setFocusFast();
	});

	// A read-modify-write of the whole card, like Android's setField(). The
	// card is four short strings, and re-reading it here means two edit boxes
	// open at once cannot overwrite each other's field with a stale copy.
	const auto save = [=] {
		auto card = CurrentProfileCard();
		card.*(info.value) = field->getLastText();
		SetProfileCard(std::move(card));
		box->closeBox();
	};
	if (!info.multiline) {
		field->submits() | rpl::on_next(save, field->lifetime());
	}

	box->addButton(tr::lng_settings_save(), save);
	box->addButton(tr::lng_cancel(), [=] {
		box->closeBox();
	});
}

void CardBox(not_null<Ui::GenericBox*> box) {
	box->setStyle(st::layerBox);
	box->setWidth(st::boxWideWidth);
	box->setTitle(TrValue(u"LuminaProfileCardTitle"_q));

	const auto container = box->verticalLayout();

	Ui::AddSkip(container);
	Ui::AddSubsectionTitle(container, TrValue(u"LuminaProfileCardHeader"_q));
	for (const auto &info : ProfileCardFields()) {
		::Settings::AddButtonWithLabel(
			container,
			TrValue(info.labelKey),
			CardValue([info] {
				return Preview(CurrentProfileCard().*(info.value));
			}),
			st::settingsButtonNoIcon
		)->setClickedCallback([=] {
			box->uiShow()->show(Box(EditFieldBox, info));
		});
	}
	Ui::AddSkip(container);
	Ui::AddDividerText(container, TrValue(u"LuminaProfileCardInfo"_q));

	Ui::AddSkip(container);
	::Settings::AddButtonWithIcon(
		container,
		TrValue(u"LuminaProfileCardCopy"_q),
		st::settingsButtonActive,
		{ &st::menuIconCopy }
	)->setClickedCallback([=] {
		CopyCard(box->uiShow());
	});
	Ui::AddSkip(container);
	Ui::AddDividerText(container, TrValue(u"LuminaProfileCardCopyInfo"_q));

	box->addButton(tr::lng_close(), [=] {
		box->closeBox();
	});
}

} // namespace

void AddProfileCardRows(
		not_null<Ui::VerticalLayout*> container,
		not_null<Window::SessionController*> controller) {
	Ui::AddSkip(container);
	Ui::AddSubsectionTitle(container, TrValue(u"LuminaProfileCardTitle"_q));

	::Settings::AddButtonWithLabel(
		container,
		TrValue(u"LuminaProfileCardEdit"_q),
		CardValue([] {
			return Preview(FirstFilled(CurrentProfileCard()));
		}),
		st::settingsButtonNoIcon
	)->setClickedCallback([=] {
		controller->show(Box(CardBox));
	});

	Ui::AddSkip(container);
	Ui::AddDividerText(container, TrValue(u"LuminaProfileCardAbout"_q));
}

} // namespace Lumina
