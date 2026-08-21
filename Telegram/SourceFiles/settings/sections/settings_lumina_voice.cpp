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
#include "lumina/lumina_whisper_model.h"
#include "settings/settings_common.h"
#include "ui/basic_click_handlers.h"
#include "ui/boxes/confirm_box.h"
#include "ui/boxes/single_choice_box.h"
#include "ui/layers/generic_box.h"
#include "ui/rp_widget.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/fields/input_field.h"
#include "ui/widgets/labels.h"
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

// PLACEHOLDER destinations for the onboarding funnel below. Both stand in for
// the real LuminaGram bot and its setup guide, which do not exist yet; they are
// grouped and tagged here so one grep for "PLACEHOLDER" finds every URL to swap
// when they do. The "?start=" tail is a bot deep link, so the guide can be a
// flow inside the same bot.
const auto kBotUrlPlaceholder = u"https://t.me/LuminaGramBot"_q; // PLACEHOLDER
const auto kGuideUrlPlaceholder
	= u"https://t.me/LuminaGramBot?start=setup"_q; // PLACEHOLDER

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

// The master switch, as a stream. Every other row on the page is gated on it,
// and so is the readiness note under it.
[[nodiscard]] rpl::producer<bool> MasterEnabledValue() {
	return rpl::single(
		rpl::empty
	) | rpl::then(
		Lumina::VoiceToTextEnabledChanges()
	) | rpl::map([] {
		return Lumina::VoiceToTextEnabled();
	});
}

// True while voice to text is switched on but the engine selected below cannot
// actually run yet - which on desktop means a cloud engine with no API key, and
// (once an offline engine exists) a model that has not been downloaded. Both
// are exactly what TranscriberConfigured() already answers, so readiness never
// grows a second rule here.
//
// The switch defaults ON, which is Android's default for the same preference
// key - and there that default is honest, because Android's default engine is
// Vosk: offline, free, nothing to configure. There is no offline engine here
// yet (lumina/lumina_transcribers.h says why), so "on" does not imply "ready".
// Rather than turn the default off - which would put the desktop preference out
// of step with Android's for a key both platforms store and a backup carries
// between them - the page owns the gap, with the onboarding funnel this drives.
//
// Recomputes on the two streams readiness depends on and no others: the engine
// settings and the master switch. The funnel's own text tracks the in-app
// language on its own through TrValue(), so LangChanges() is left out here.
[[nodiscard]] rpl::producer<bool> NotReadyValue() {
	return rpl::single(
		rpl::empty
	) | rpl::then(rpl::merge(
		Lumina::TranscriberChanges(),
		Lumina::VoiceToTextEnabledChanges())
	) | rpl::map([] {
		return Lumina::VoiceToTextEnabled()
			&& !Lumina::TranscriberConfigured(Lumina::CurrentTranscriberId());
	});
}

// Greys a dependent row out and stops it taking presses while the master
// switch is off.
//
// The rows stay where they are rather than disappearing, because this page is
// also where somebody finds out what the switch would give them - but a row
// that still looks live invites them to pick an engine, save a key and set a
// model that nothing will read.
//
// Qt::WA_TransparentForMouseEvents is what actually blocks the press:
// Ui::AbstractButton::setDisabled() gates the accessibility action and nothing
// else in this lib_ui, so a row "disabled" that way still toggles under the
// mouse. clearState() drops a hover the row may be holding, which would
// otherwise leave it painted as if the cursor were still on it.
void GateRow(not_null<Ui::SettingsButton*> button, Ui::FlatLabel *label) {
	MasterEnabledValue(
	) | rpl::on_next([=](bool enabled) {
		if (!enabled) {
			// Before setDisabled(), which clearState() would undo.
			button->clearState();
		}
		// For screen readers; the attribute below is what stops the mouse.
		button->setDisabled(!enabled);
		button->setAttribute(Qt::WA_TransparentForMouseEvents, !enabled);
		button->setPointerCursor(enabled);
		const auto fg = enabled
			? std::optional<QColor>()
			: std::optional<QColor>(st::windowSubTextFg->c);
		button->setColorOverride(fg);
		if (label) {
			label->setTextColorOverride(fg);
		}
	}, button->lifetime());
}

// AddButtonWithLabel() keeps no handle on the value it draws on the right, and
// a gated row has to grey that value out along with the rest of itself - a
// value left in the accent colour is a control that looks pressable and is
// not. So the label is built here, with the geometry CreateRightLabel() gives
// it, and kept. (The same note is on the identical helper in
// lumina/lumina_translate_settings.cpp.)
not_null<Ui::FlatLabel*> AddRightLabel(
		not_null<Ui::SettingsButton*> button,
		rpl::producer<QString> name,
		rpl::producer<QString> value) {
	const auto &st = st::settingsButtonNoIcon;
	const auto label = Ui::CreateChild<Ui::FlatLabel>(
		button.get(),
		st.rightLabel);
	label->show();
	rpl::combine(
		button->widthValue(),
		std::move(name),
		std::move(value)
	) | rpl::on_next([=, &st](
			int width,
			const QString &rowText,
			const QString &text) {
		const auto available = width
			- st.padding.left()
			- st.padding.right()
			- st.style.font->width(rowText)
			- st::settingsButtonRightSkip;
		label->setText(text);
		label->resizeToNaturalWidth(available);
		label->moveToRight(st::settingsButtonRightSkip, st.padding.top());
	}, label->lifetime());
	// LuminaGram: let presses over the value on the right fall through to the row button, so the
	// whole row (not just the value) opens the chooser. Matches upstream CreateRightLabel().
	label->setAttribute(Qt::WA_TransparentForMouseEvents);
	return label;
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

// Both row builders below make DEPENDENT rows: everything they add is gated on
// the master switch. The master switch itself is built inline in
// setupContent() and must never go through them, or it would grey itself out
// and could not be switched back on.
not_null<Ui::SettingsButton*> AddToggleRow(
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
	GateRow(button, nullptr);
	return button;
}

not_null<Ui::SettingsButton*> AddValueRow(
		not_null<Ui::VerticalLayout*> container,
		rpl::producer<QString> label,
		Fn<QString()> value,
		Fn<void()> activate) {
	auto rowText = rpl::duplicate(label);
	const auto button = AddButtonWithIcon(
		container,
		std::move(label),
		st::settingsButtonNoIcon);
	button->setClickedCallback(std::move(activate));
	GateRow(button, AddRightLabel(
		button,
		std::move(rowText),
		LabelValue(std::move(value))));
	return button;
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

// LuminaGram: the engine names are the services own brands, but the label
// shown on the settings page is a translated sentence, so it comes out of the
// locale table rather than TranscriberInfo::name. One place maps an engine id
// to its locale key, used by both the engine row and the picker.
[[nodiscard]] QString EngineNameKey(const QString &id) {
	if (id == Lumina::AppleTranscriberId()) {
		return u"LuminaSttEngineApple"_q;
	} else if (id == Lumina::WhisperCppTranscriberId()) {
		return u"LuminaSttEngineWhisperCpp"_q;
	} else if (id == Lumina::GoogleTranscriberId()) {
		return u"LuminaSttEngineGoogle"_q;
	}
	return u"LuminaSttEngineWhisper"_q;
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
		options.push_back(Lumina::Tr(EngineNameKey(engines[i].id)));
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

#ifndef Q_OS_MAC
// Right-label text for the offline model row: recomputes on the model state
// (download start / progress / finish / delete) and the in-app language.
[[nodiscard]] rpl::producer<QString> WhisperModelLabelValue() {
	return rpl::single(
		rpl::empty
	) | rpl::then(rpl::merge(
		Lumina::WhisperModelChanges(),
		Lumina::LangChanges())
	) | rpl::map([] {
		const auto state = Lumina::CurrentWhisperModelState();
		using Stage = Lumina::WhisperModelState::Stage;
		switch (state.stage) {
		case Stage::Downloading:
			return Lumina::Tr(
				u"LuminaSttModelDownloading"_q,
				QString::number(state.progress));
		case Stage::Ready:
			return Lumina::Tr(u"LuminaSttModelReady"_q);
		default:
			return Lumina::Tr(u"LuminaSttModelAbsent"_q);
		}
	});
}

// The single offline-model row, shown only for the whisper.cpp engine. It
// folds away for any cloud engine exactly like the key / base-url / model
// blocks. Tap while absent downloads the model; tap while ready confirms and
// deletes it. The label tracks progress reactively, so no separate progress
// bulletin is needed beyond a one-shot toast on the initial tap.
void AddWhisperModelRow(
		not_null<Ui::VerticalLayout*> container,
		not_null<Window::SessionController*> controller) {
	const auto block = AddConditionalBlock(container, [] {
		return Lumina::CurrentTranscriberId()
			== Lumina::WhisperCppTranscriberId();
	});
	const auto button = AddButtonWithIcon(
		block,
		Lumina::TrValue(u"LuminaSttOfflineModel"_q),
		st::settingsButtonNoIcon);
	button->setClickedCallback([=] {
		const auto state = Lumina::CurrentWhisperModelState();
		using Stage = Lumina::WhisperModelState::Stage;
		if (state.stage == Stage::Ready) {
			controller->show(Ui::MakeConfirmBox({
				.text = Lumina::Tr(u"LuminaSttModelDeleteConfirm"_q),
				.confirmed = [](Fn<void()> close) {
					Lumina::DeleteWhisperModel();
					close();
				},
				.confirmText = Lumina::Tr(u"LuminaSttModelDelete"_q),
			}));
		} else if (state.stage == Stage::Absent) {
			Lumina::DownloadWhisperModel();
			controller->showToast(
				Lumina::Tr(u"LuminaSttModelDownloadingStart"_q));
		}
	});
	GateRow(button, AddRightLabel(
		button,
		Lumina::TrValue(u"LuminaSttOfflineModel"_q),
		WhisperModelLabelValue()));
}
#endif // !Q_OS_MAC

void AddEngineRows(
		not_null<Ui::VerticalLayout*> container,
		not_null<Window::SessionController*> controller) {
	Ui::AddSkip(container);
	Ui::AddSubsectionTitle(container, Lumina::TrValue(u"LuminaSttEngine"_q));
	AddValueRow(
		container,
		Lumina::TrValue(u"LuminaSttEngine"_q),
		[] {
			return Lumina::Tr(EngineNameKey(Lumina::CurrentTranscriberId()));
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

#ifndef Q_OS_MAC
	// LuminaGram: the offline model download/status row. Non-mac only (mac uses
	// the Apple Speech engine, which has no model to fetch).
	AddWhisperModelRow(container, controller);
#endif // !Q_OS_MAC

	Ui::AddSkip(container);

	// LuminaGram: desktop transcription is no longer cloud-only - the whisper.cpp
	// engine runs offline, on-device, after a one-time model download. The old
	// "Vosk unsupported" note (LuminaSttVoskUnsupported, kept in the locale table)
	// is retired in favour of this one.
	Ui::AddDividerText(
		container,
		Lumina::TrValue(u"LuminaSttOfflineInfo"_q));
}

// The onboarding funnel that replaces the old one-line "engine has no key" note.
// Built once, slide-wrapped, and shown only while NotReadyValue() is true, so a
// ready engine never sees it, and saving a key or picking a configured engine
// folds it away without rebuilding the page. Every user-visible string is a
// TrValue(), so it re-reads when the in-app language changes too.
//
// It turns a dead end into a path: a short "why", a link to the guide, and a
// call to action that opens our own bot. Both destinations are PLACEHOLDER URLs
// (see the top of this file) opened through the app's own URL handler - the
// same way the link-safety box opens a link - so a t.me/... link resolves to
// the bot in-app rather than bouncing out through a browser first.
void AddOnboardingFunnel(not_null<Ui::VerticalLayout*> container) {
	const auto block = container->add(
		object_ptr<Ui::SlideWrap<Ui::VerticalLayout>>(
			container,
			object_ptr<Ui::VerticalLayout>(container))
	)->toggleOn(NotReadyValue())->finishAnimating()->entity();

	Ui::AddSkip(block);
	Ui::AddSubsectionTitle(block, Lumina::TrValue(u"LuminaSttSetupTitle"_q));

	// Ordinary settings rows, wired exactly like AddValueRow's activate
	// callback. TrValue(key) is evaluated now (the row keeps the producer); the
	// press handler keeps only the url.
	const auto addLinkRow = [&](const QString &key, const QString &url) {
		const auto row = AddButtonWithIcon(
			block,
			Lumina::TrValue(key),
			st::settingsButtonNoIcon);
		row->setClickedCallback([=] {
			UrlClickHandler::Open(url);
		});
		return row;
	};
	addLinkRow(u"LuminaSttGuide"_q, kGuideUrlPlaceholder);
	addLinkRow(u"LuminaSttGetViaBot"_q, kBotUrlPlaceholder);

	Ui::AddSkip(block);
	Ui::AddDividerText(block, Lumina::TrValue(u"LuminaSttSetupInfo"_q));
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
	// The master switch. Built here rather than through AddToggleRow(), which
	// gates every row it makes on this switch: a master row that greyed itself
	// out could never be switched back on.
	Ui::AddSkip(container);
	const auto master = container->add(object_ptr<Ui::SettingsButton>(
		container,
		Lumina::TrValue(u"LuminaSttEnable"_q),
		st::settingsButtonNoIcon
	))->toggleOn(MasterEnabledValue());
	master->toggledChanges(
	) | rpl::on_next([](bool value) {
		Lumina::SetVoiceToTextEnabled(value);
	}, master->lifetime());
	Ui::AddSkip(container);
	Ui::AddDividerText(container, Lumina::TrValue(u"LuminaSttInfoDesktop"_q));

	// The onboarding funnel sits between the master switch and the engine rows:
	// it is the first thing a not-yet-ready profile sees, and it folds away the
	// moment the selected engine can run.
	AddOnboardingFunnel(container);

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
