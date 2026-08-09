/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "settings/sections/settings_lumina_voice.h"

#include "lang/lang_keys.h"
#include "lumina/lumina_locale.h"
#include "lumina/lumina_transcribers.h"
#include "lumina/lumina_voice_to_text.h"
#include "settings/settings_common.h"
#include "ui/boxes/single_choice_box.h"
#include "ui/layers/generic_box.h"
#include "ui/rp_widget.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/fields/input_field.h"
#include "ui/wrap/slide_wrap.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"

#include "styles/style_layers.h"
#include "styles/style_settings.h"
#include "styles/style_widgets.h"

namespace Settings {
namespace {

constexpr auto kApiKeyMaxLength = 512;
constexpr auto kBaseUrlMaxLength = 512;
constexpr auto kModelMaxLength = 128;

// How much of an API key a row shows. Enough to tell two keys apart, not
// enough to be worth a screenshot. Same rule as the translation page.
constexpr auto kApiKeyTailShown = 4;
constexpr auto kApiKeyDotsShown = 6;

// Every row on this page reads a value that some other row can change, so
// they all recompute on the same two streams: the engine settings, and the
// in-app language.
[[nodiscard]] rpl::producer<QString> LabelValue(Fn<QString()> compute) {
	return rpl::single(
		rpl::empty
	) | rpl::then(rpl::merge(
		Lumina::TranscriberChanges(),
		Lumina::LangChanges())
	) | rpl::map([compute = std::move(compute)] {
		return compute();
	});
}

[[nodiscard]] rpl::producer<bool> FlagValue(Fn<bool()> compute) {
	return rpl::single(
		rpl::empty
	) | rpl::then(
		Lumina::TranscriberChanges()
	) | rpl::map([compute = std::move(compute)] {
		return compute();
	});
}

[[nodiscard]] QString MaskedApiKey(const QString &key) {
	if (key.isEmpty()) {
		return Lumina::Tr(u"LuminaTranslateApiKeyNotSet"_q);
	} else if (key.size() <= kApiKeyTailShown) {
		return QString(kApiKeyDotsShown, QChar(0x2022));
	}
	return QString(kApiKeyDotsShown, QChar(0x2022))
		+ key.right(kApiKeyTailShown);
}

void AddToggleRow(
		not_null<Ui::VerticalLayout*> container,
		rpl::producer<QString> label,
		Fn<bool()> checked,
		Fn<void(bool)> save) {
	const auto button = container->add(object_ptr<Ui::SettingsButton>(
		container,
		std::move(label),
		st::settingsButtonNoIcon
	))->toggleOn(rpl::single(checked()));
	button->toggledChanges(
	) | rpl::on_next([save = std::move(save)](bool value) {
		save(value);
	}, button->lifetime());
}

void AddValueRow(
		not_null<Ui::VerticalLayout*> container,
		rpl::producer<QString> label,
		Fn<QString()> value,
		Fn<void()> activate) {
	AddButtonWithLabel(
		container,
		std::move(label),
		LabelValue(std::move(value)),
		st::settingsButtonNoIcon
	)->setClickedCallback(std::move(activate));
}

// A block that only exists for engines that need it. Built once and toggled
// reactively, so switching engine never rebuilds the page and never
// invalidates a pointer another row is holding.
not_null<Ui::VerticalLayout*> AddConditionalBlock(
		not_null<Ui::VerticalLayout*> container,
		Fn<bool()> shown) {
	return container->add(
		object_ptr<Ui::SlideWrap<Ui::VerticalLayout>>(
			container,
			object_ptr<Ui::VerticalLayout>(container))
	)->toggleOn(
		FlagValue(std::move(shown))
	)->finishAnimating()->entity();
}

void EditTextBox(
		not_null<Ui::GenericBox*> box,
		QString title,
		QString placeholder,
		QString value,
		int maxLength,
		Fn<void(QString)> save) {
	box->setTitle(rpl::single(title));

	const auto field = box->addRow(object_ptr<Ui::InputField>(
		box,
		st::defaultInputField,
		Ui::InputField::Mode::SingleLine,
		rpl::single(placeholder),
		value));

	// setMaxLength() chops the text the field is ALREADY holding, so a stored
	// value longer than the cap would be truncated before the user touched
	// anything and the truncation would then be saved back. Cap what they
	// type, never what they already had. (The same note is on the identical
	// row in lumina/lumina_translate_settings.cpp.)
	if (int(value.size()) < maxLength) {
		field->setMaxLength(maxLength);
	}
	box->setFocusCallback([=] {
		field->setFocusFast();
	});

	const auto submit = [=] {
		save(field->getLastText().trimmed());
		box->closeBox();
	};
	field->submits() | rpl::on_next(submit, field->lifetime());
	box->addButton(tr::lng_settings_save(), submit);
	box->addButton(tr::lng_cancel(), [=] {
		box->closeBox();
	});
}

void ShowTextEditor(
		not_null<Window::SessionController*> controller,
		const QString &title,
		const QString &value,
		int maxLength,
		Fn<void(QString)> save) {
	controller->show(Box(
		EditTextBox,
		title,
		title,
		value,
		maxLength,
		std::move(save)));
}

void ShowEnginePicker(not_null<Window::SessionController*> controller) {
	const auto &engines = Lumina::Transcribers();
	auto options = std::vector<QString>();
	auto selected = 0;
	const auto current = Lumina::CurrentTranscriberId();
	for (auto i = 0, count = int(engines.size()); i != count; ++i) {
		// The engine names are the services' own brands, but the "(your own
		// key)" half of the label is a sentence and is translated - which is
		// why these come out of the locale table rather than out of
		// TranscriberInfo::name.
		options.push_back(Lumina::Tr(
			(engines[i].id == Lumina::GoogleTranscriberId())
				? u"LuminaSttEngineGoogle"_q
				: u"LuminaSttEngineWhisper"_q));
		if (engines[i].id == current) {
			selected = i;
		}
	}
	controller->show(Box([=](not_null<Ui::GenericBox*> box) {
		SingleChoiceBox(box, {
			.title = Lumina::TrValue(u"LuminaSttEngine"_q),
			.options = options,
			.initialSelection = selected,
			.callback = [=](int index) {
				const auto &list = Lumina::Transcribers();
				if (index >= 0 && index < int(list.size())) {
					Lumina::SetCurrentTranscriberId(list[index].id);
				}
			},
		});
	}));
}

void AddEngineRows(
		not_null<Ui::VerticalLayout*> container,
		not_null<Window::SessionController*> controller) {
	Ui::AddSkip(container);
	Ui::AddSubsectionTitle(container, Lumina::TrValue(u"LuminaSttEngine"_q));
	AddValueRow(
		container,
		Lumina::TrValue(u"LuminaSttEngine"_q),
		[] {
			return Lumina::Tr(
				(Lumina::CurrentTranscriberId()
					== Lumina::GoogleTranscriberId())
					? u"LuminaSttEngineGoogle"_q
					: u"LuminaSttEngineWhisper"_q);
		},
		[=] { ShowEnginePicker(controller); });

	const auto keyBlock = AddConditionalBlock(container, [] {
		return Lumina::CurrentTranscriber().needsKey;
	});
	AddValueRow(
		keyBlock,
		Lumina::TrValue(u"LuminaSttKey"_q),
		[] {
			return MaskedApiKey(
				Lumina::TranscriberApiKey(Lumina::CurrentTranscriberId()));
		},
		[=] {
			const auto id = Lumina::CurrentTranscriberId();
			ShowTextEditor(
				controller,
				Lumina::Tr(u"LuminaSttKey"_q),
				Lumina::TranscriberApiKey(id),
				kApiKeyMaxLength,
				[=](QString value) {
					Lumina::SetTranscriberApiKey(id, value);
				});
		});

	const auto baseUrlBlock = AddConditionalBlock(container, [] {
		return Lumina::CurrentTranscriber().needsBaseUrl;
	});
	AddValueRow(
		baseUrlBlock,
		Lumina::TrValue(u"LuminaSttBaseUrl"_q),
		[] { return Lumina::SttBaseUrl(); },
		[=] {
			ShowTextEditor(
				controller,
				Lumina::Tr(u"LuminaSttBaseUrl"_q),
				Lumina::SttBaseUrl(),
				kBaseUrlMaxLength,
				[](QString value) { Lumina::SetSttBaseUrl(value); });
		});

	const auto modelBlock = AddConditionalBlock(container, [] {
		return Lumina::CurrentTranscriber().needsModel;
	});
	AddValueRow(
		modelBlock,
		Lumina::TrValue(u"LuminaSttModel"_q),
		[] { return Lumina::SttModel(); },
		[=] {
			ShowTextEditor(
				controller,
				Lumina::Tr(u"LuminaSttModel"_q),
				Lumina::SttModel(),
				kModelMaxLength,
				[](QString value) { Lumina::SetSttModel(value); });
		});

	Ui::AddSkip(container);

	// The promise this page must not break. Android's default engine is Vosk,
	// offline and free; there is no offline engine on desktop, and saying so
	// here is the whole reason this divider exists. See
	// lumina/lumina_transcribers.h for what shipping one would take.
	Ui::AddDividerText(
		container,
		Lumina::TrValue(u"LuminaSttVoskUnsupported"_q));
}

} // namespace

Type LuminaVoiceId() {
	return LuminaVoice::Id();
}

LuminaVoice::LuminaVoice(
	QWidget *parent,
	not_null<Window::SessionController*> controller)
: Section(parent, controller) {
	const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);
	setupContent(content);
	Ui::ResizeFitChild(this, content);
}

LuminaVoice::~LuminaVoice() = default;

rpl::producer<QString> LuminaVoice::title() {
	return Lumina::TrValue(u"LuminaVoiceToTextTitle"_q);
}

void LuminaVoice::setupContent(not_null<Ui::VerticalLayout*> container) {
	Ui::AddSkip(container);
	AddToggleRow(
		container,
		Lumina::TrValue(u"LuminaSttEnable"_q),
		[] { return Lumina::VoiceToTextEnabled(); },
		[](bool value) { Lumina::SetVoiceToTextEnabled(value); });
	Ui::AddSkip(container);
	Ui::AddDividerText(
		container,
		Lumina::TrValue(u"LuminaSttInfoDesktop"_q));

	AddEngineRows(container, controller());

	Ui::AddSkip(container);
	AddToggleRow(
		container,
		Lumina::TrValue(u"LuminaSttAutoTranslate"_q),
		[] { return Lumina::VoiceToTextAutoTranslate(); },
		[](bool value) { Lumina::SetVoiceToTextAutoTranslate(value); });
	Ui::AddSkip(container);
	Ui::AddDividerText(
		container,
		Lumina::TrValue(u"LuminaSttAutoTranslateInfo"_q));
}

} // namespace Settings
