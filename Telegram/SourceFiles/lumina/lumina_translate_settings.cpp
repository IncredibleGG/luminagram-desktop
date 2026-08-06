/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_translate_settings.h"

#include "lang/lang_keys.h"
#include "lumina/lumina_locale.h"
#include "lumina/lumina_settings.h"
#include "lumina/lumina_translate_gating.h"
#include "lumina/lumina_translate_providers.h"
#include "settings/settings_common.h"
#include "ui/boxes/single_choice_box.h"
#include "ui/layers/generic_box.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/fields/input_field.h"
#include "ui/wrap/slide_wrap.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"

#include "styles/style_layers.h"
#include "styles/style_settings.h"
#include "styles/style_widgets.h"

#include <QtCore/QJsonValue>

namespace Lumina {
namespace {

const auto kKeyFeatureEnabled = u"translateEnabled"_q;
const auto kKeyBeforeSend = u"translateBeforeSend"_q;
const auto kKeyBeforeSendConfirm = u"translateBeforeSendConfirm"_q;
const auto kKeySendLang = u"trSendLang"_q;
const auto kKeyReadLang = u"trReadLang"_q;
const auto kKeyDualLanguage = u"dualLanguageDisplay"_q;
const auto kKeyScopePrivate = u"trScopePrivate"_q;
const auto kKeyScopeGroup = u"trScopeGroup"_q;
const auto kKeyMode = u"trMode"_q;

const auto kSendLangAuto = u"auto"_q;
const auto kModeAll = u"all"_q;
const auto kModeManual = u"manual"_q;

constexpr auto kApiKeyMaxLength = 512;
constexpr auto kBaseUrlMaxLength = 512;
constexpr auto kModelMaxLength = 128;
constexpr auto kPromptMaxLength = 4096;

// How much of an API key a row shows. Enough to tell two keys apart, not
// enough to be worth a screenshot.
constexpr auto kApiKeyTailShown = 4;
constexpr auto kApiKeyDotsShown = 6;

// The keys this file owns. The provider keys are deliberately absent: their
// change stream comes from TranslateProviderChanges(), so the key names behind
// it stay lumina_translate_providers' business.
[[nodiscard]] bool IsOwnedKey(const QString &key) {
	return (key == kKeyFeatureEnabled)
		|| (key == kKeyBeforeSend)
		|| (key == kKeyBeforeSendConfirm)
		|| (key == kKeySendLang)
		|| (key == kKeyReadLang)
		|| (key == kKeyDualLanguage)
		|| (key == kKeyScopePrivate)
		|| (key == kKeyScopeGroup)
		|| (key == kKeyMode);
}

struct LanguageEntry {
	QString code;
	QString key;
};

// The languages LuminaGram can translate into, in the order the pickers show
// them. Codes are ISO tags, and the provider layer maps them to whatever
// spelling each service wants. The dialect entries are the reason this table
// exists instead of tdesktop's own LanguageId list, which is a bare
// QLocale::Language and cannot tell zh-TW from zh-CN - which is also why the
// names are our own strings rather than QLocale::languageToString().
[[nodiscard]] const std::vector<LanguageEntry> &LanguageEntries() {
	static const auto result = std::vector<LanguageEntry>{
		{ u"en"_q, u"LuminaLangEn"_q },
		{ u"zh-TW"_q, u"LuminaLangZhTw"_q },
		{ u"zh-CN"_q, u"LuminaLangZhCn"_q },
		{ u"ja"_q, u"LuminaLangJa"_q },
		{ u"ko"_q, u"LuminaLangKo"_q },
		{ u"es"_q, u"LuminaLangEs"_q },
		{ u"fr"_q, u"LuminaLangFr"_q },
		{ u"de"_q, u"LuminaLangDe"_q },
		{ u"ru"_q, u"LuminaLangRu"_q },
		{ u"pt-BR"_q, u"LuminaLangPtBr"_q },
		{ u"pt-PT"_q, u"LuminaLangPtPt"_q },
		{ u"it"_q, u"LuminaLangIt"_q },
		{ u"ar"_q, u"LuminaLangAr"_q },
		{ u"hi"_q, u"LuminaLangHi"_q },
		{ u"id"_q, u"LuminaLangId"_q },
		{ u"th"_q, u"LuminaLangTh"_q },
		{ u"vi"_q, u"LuminaLangVi"_q },
		{ u"tr"_q, u"LuminaLangTr"_q },
		{ u"pl"_q, u"LuminaLangPl"_q },
		{ u"uk"_q, u"LuminaLangUk"_q },
		{ u"nl"_q, u"LuminaLangNl"_q },

		{ u"af"_q, u"LuminaLangAf"_q },
		{ u"sq"_q, u"LuminaLangSq"_q },
		{ u"am"_q, u"LuminaLangAm"_q },
		{ u"hy"_q, u"LuminaLangHy"_q },
		{ u"az"_q, u"LuminaLangAz"_q },
		{ u"eu"_q, u"LuminaLangEu"_q },
		{ u"be"_q, u"LuminaLangBe"_q },
		{ u"bn"_q, u"LuminaLangBn"_q },
		{ u"bs"_q, u"LuminaLangBs"_q },
		{ u"bg"_q, u"LuminaLangBg"_q },
		{ u"my"_q, u"LuminaLangMy"_q },
		{ u"ca"_q, u"LuminaLangCa"_q },
		{ u"hr"_q, u"LuminaLangHr"_q },
		{ u"cs"_q, u"LuminaLangCs"_q },
		{ u"da"_q, u"LuminaLangDa"_q },
		{ u"et"_q, u"LuminaLangEt"_q },
		{ u"tl"_q, u"LuminaLangTl"_q },
		{ u"fi"_q, u"LuminaLangFi"_q },
		{ u"gl"_q, u"LuminaLangGl"_q },
		{ u"ka"_q, u"LuminaLangKa"_q },
		{ u"el"_q, u"LuminaLangEl"_q },
		{ u"gu"_q, u"LuminaLangGu"_q },
		{ u"he"_q, u"LuminaLangHe"_q },
		{ u"hu"_q, u"LuminaLangHu"_q },
		{ u"is"_q, u"LuminaLangIs"_q },
		{ u"ga"_q, u"LuminaLangGa"_q },
		{ u"jv"_q, u"LuminaLangJv"_q },
		{ u"kn"_q, u"LuminaLangKn"_q },
		{ u"kk"_q, u"LuminaLangKk"_q },
		{ u"km"_q, u"LuminaLangKm"_q },
		{ u"ku"_q, u"LuminaLangKu"_q },
		{ u"ky"_q, u"LuminaLangKy"_q },
		{ u"lo"_q, u"LuminaLangLo"_q },
		{ u"lv"_q, u"LuminaLangLv"_q },
		{ u"lt"_q, u"LuminaLangLt"_q },
		{ u"mk"_q, u"LuminaLangMk"_q },
		{ u"ms"_q, u"LuminaLangMs"_q },
		{ u"ml"_q, u"LuminaLangMl"_q },
		{ u"mr"_q, u"LuminaLangMr"_q },
		{ u"mn"_q, u"LuminaLangMn"_q },
		{ u"ne"_q, u"LuminaLangNe"_q },
		{ u"no"_q, u"LuminaLangNo"_q },
		{ u"ps"_q, u"LuminaLangPs"_q },
		{ u"fa"_q, u"LuminaLangFa"_q },
		{ u"pa"_q, u"LuminaLangPa"_q },
		{ u"ro"_q, u"LuminaLangRo"_q },
		{ u"sr"_q, u"LuminaLangSr"_q },
		{ u"si"_q, u"LuminaLangSi"_q },
		{ u"sk"_q, u"LuminaLangSk"_q },
		{ u"sl"_q, u"LuminaLangSl"_q },
		{ u"so"_q, u"LuminaLangSo"_q },
		{ u"sw"_q, u"LuminaLangSw"_q },
		{ u"sv"_q, u"LuminaLangSv"_q },
		{ u"tg"_q, u"LuminaLangTg"_q },
		{ u"ta"_q, u"LuminaLangTa"_q },
		{ u"te"_q, u"LuminaLangTe"_q },
		{ u"ur"_q, u"LuminaLangUr"_q },
		{ u"uz"_q, u"LuminaLangUz"_q },
		{ u"cy"_q, u"LuminaLangCy"_q },
		{ u"yi"_q, u"LuminaLangYi"_q },
		{ u"zu"_q, u"LuminaLangZu"_q },
	};
	return result;
}

[[nodiscard]] const TranslateProviderInfo &CurrentProvider() {
	const auto found = FindTranslateProvider(CurrentProviderId());
	return found ? *found : TranslateProviders().front();
}

// Deliberately not Lumina::UsingOwnProvider(): that predicate is declared by
// both lumina_translate_providers.h and lumina_translate_gating.h, and this
// row does not need to care which of them ends up owning it.
[[nodiscard]] bool ProviderIsTelegram() {
	return (CurrentProvider().id == TelegramProviderId());
}

// Every label and every toggle on the page is recomputed from the store on any
// change, so a value edited in a box, restored from a backup or written by
// another part of the app is reflected without the page having to know which
// row owns which key. A computed label also names a language or a state in the
// user's own language, which is why this listens for the in-app language as
// well - a row that is spelled out by Lumina::Tr() directly gets that from
// Lumina::TrValue() instead.
[[nodiscard]] rpl::producer<QString> LabelValue(Fn<QString()> compute) {
	return rpl::single(
		rpl::empty
	) | rpl::then(rpl::merge(
		TranslateSettingsChanges(),
		LangChanges())
	) | rpl::map([compute = std::move(compute)] {
		return compute();
	});
}

[[nodiscard]] rpl::producer<bool> FlagValue(Fn<bool()> compute) {
	return rpl::single(
		rpl::empty
	) | rpl::then(
		TranslateSettingsChanges()
	) | rpl::map([compute = std::move(compute)] {
		return compute();
	});
}

[[nodiscard]] QString MaskedApiKey(const QString &key) {
	if (key.isEmpty()) {
		return Tr(u"LuminaTranslateApiKeyNotSet"_q);
	}
	const auto length = int(key.size());
	const auto tail = std::min(kApiKeyTailShown, length);
	const auto dots = std::min(kApiKeyDotsShown, length - tail);
	return QString(dots, QChar(0x2022)) + key.right(tail);
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
	))->toggleOn(FlagValue(std::move(checked)));
	button->toggledChanges(
	) | rpl::on_next([save = std::move(save)](bool value) {
		save(value);
	}, button->lifetime());
}

// `Settings` names Lumina::Settings inside this namespace, so the settings
// section helpers have to be reached through the global namespace.
void AddValueRow(
		not_null<Ui::VerticalLayout*> container,
		rpl::producer<QString> label,
		Fn<QString()> value,
		Fn<void()> activate) {
	::Settings::AddButtonWithLabel(
		container,
		std::move(label),
		LabelValue(std::move(value)),
		st::settingsButtonNoIcon
	)->setClickedCallback(std::move(activate));
}

// A block of rows that only exists for providers that need them. It is built
// once and toggled reactively, so switching provider never rebuilds the page
// and never invalidates a pointer another row is holding.
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
		bool multiline,
		Fn<void(QString)> save) {
	box->setTitle(rpl::single(title));

	const auto field = box->addRow(object_ptr<Ui::InputField>(
		box,
		st::defaultInputField,
		(multiline
			? Ui::InputField::Mode::MultiLine
			: Ui::InputField::Mode::SingleLine),
		rpl::single(placeholder),
		value));
	field->setMaxLength(maxLength);
	box->setFocusCallback([=] {
		field->setFocusFast();
	});

	const auto submit = [=] {
		save(field->getLastText().trimmed());
		box->closeBox();
	};
	if (!multiline) {
		field->submits() | rpl::on_next(submit, field->lifetime());
	}
	box->addButton(tr::lng_settings_save(), submit);
	box->addButton(tr::lng_cancel(), [=] {
		box->closeBox();
	});
}

void ShowTextEditor(
		not_null<Window::SessionController*> controller,
		const QString &title,
		const QString &placeholder,
		const QString &value,
		int maxLength,
		bool multiline,
		Fn<void(QString)> save) {
	controller->show(Box(
		EditTextBox,
		title,
		placeholder,
		value,
		maxLength,
		multiline,
		std::move(save)));
}

void ShowProviderPicker(not_null<Window::SessionController*> controller) {
	const auto &providers = TranslateProviders();
	auto options = std::vector<QString>();
	options.reserve(providers.size());
	auto selected = 0;
	const auto current = CurrentProviderId();
	for (auto i = 0, count = int(providers.size()); i != count; ++i) {
		options.push_back(providers[i].name);
		if (providers[i].id == current) {
			selected = i;
		}
	}
	controller->show(Box([=](not_null<Ui::GenericBox*> box) {
		SingleChoiceBox(box, {
			.title = TrValue(u"LuminaTranslateProvider"_q),
			.options = options,
			.initialSelection = selected,
			.callback = [=](int index) {
				const auto &list = TranslateProviders();
				if (index >= 0 && index < int(list.size())) {
					SetCurrentProviderId(list[index].id);
				}
			},
		});
	}));
}

void ShowModePicker(not_null<Window::SessionController*> controller) {
	const auto options = std::vector<QString>{
		Tr(u"LuminaTranslateModeAll"_q),
		Tr(u"LuminaTranslateModeManual"_q),
	};
	const auto selected = AutoTranslateEverything() ? 0 : 1;
	controller->show(Box([=](not_null<Ui::GenericBox*> box) {
		SingleChoiceBox(box, {
			.title = TrValue(u"LuminaTranslateModeHeader"_q),
			.options = options,
			.initialSelection = selected,
			.callback = [=](int index) {
				SetAutoTranslateEverything(index == 0);
			},
		});
	}));
}

// `firstOption` is the special entry at the top of the list - "Recipient's
// language" on the send side, "Interface language" on the read side - and
// `firstValue` the code it stores ("auto" and "").
void ShowLanguagePicker(
		not_null<Window::SessionController*> controller,
		const QString &title,
		const QString &firstOption,
		const QString &firstValue,
		const QString &current,
		Fn<void(QString)> save) {
	const auto &languages = TranslateLanguages();
	auto options = std::vector<QString>();
	auto codes = std::vector<QString>();
	options.reserve(languages.size() + 2);
	codes.reserve(languages.size() + 2);
	options.push_back(firstOption);
	codes.push_back(firstValue);
	for (const auto &language : languages) {
		options.push_back(language.name);
		codes.push_back(language.code);
	}

	// A stored code this build's table does not carry - written by another
	// client, or restored from a backup - gets an entry of its own. Without it
	// the picker falls back to highlighting the first row, so it would show a
	// selection the settings row above disagrees with, and the next tap would
	// silently discard the stored dialect.
	if (!current.isEmpty()
		&& (current != firstValue)
		&& (ranges::find(codes, current) == end(codes))) {
		options.push_back(TranslateLanguageName(current));
		codes.push_back(current);
	}

	auto selected = 0;
	for (auto i = 0, count = int(codes.size()); i != count; ++i) {
		if (codes[i] == current) {
			selected = i;
			break;
		}
	}
	controller->show(Box([=](not_null<Ui::GenericBox*> box) {
		SingleChoiceBox(box, {
			.title = rpl::single(title),
			.options = options,
			.initialSelection = selected,
			.callback = [=](int index) {
				if (index >= 0 && index < int(codes.size())) {
					save(codes[index]);
				}
			},
		});
	}));
}

void AddSendRows(
		not_null<Ui::VerticalLayout*> container,
		not_null<Window::SessionController*> controller) {
	Ui::AddSkip(container);
	Ui::AddSubsectionTitle(
		container,
		TrValue(u"LuminaTranslateSendHeader"_q));
	AddToggleRow(
		container,
		TrValue(u"LuminaTranslateBeforeSend"_q),
		[] { return TranslateBeforeSend(); },
		[](bool value) { SetTranslateBeforeSend(value); });
	AddValueRow(
		container,
		TrValue(u"LuminaTranslateSendLang"_q),
		[] {
			return TranslateSendLanguageIsAuto()
				? Tr(u"LuminaTranslateSendLangAuto"_q)
				: TranslateLanguageName(TranslateSendLanguage());
		},
		[=] {
			ShowLanguagePicker(
				controller,
				Tr(u"LuminaTranslateSendLang"_q),
				Tr(u"LuminaTranslateSendLangAuto"_q),
				kSendLangAuto,
				TranslateSendLanguage(),
				[](QString code) { SetTranslateSendLanguage(code); });
		});
	// Android's LuminaTranslateBeforeSendConfirm: the per-send preview, not the
	// per-chat language lock. The lock is asked once per chat by the send
	// pipeline whatever this is set to, and has no preference of its own.
	AddToggleRow(
		container,
		TrValue(u"LuminaTranslateBeforeSendConfirm"_q),
		[] { return TranslateBeforeSendConfirm(); },
		[](bool value) { SetTranslateBeforeSendConfirm(value); });
	Ui::AddSkip(container);
	Ui::AddDividerText(container, TrValue(u"LuminaTranslateSendInfo"_q));
}

void AddReceiveRows(
		not_null<Ui::VerticalLayout*> container,
		not_null<Window::SessionController*> controller) {
	Ui::AddSkip(container);
	Ui::AddSubsectionTitle(
		container,
		TrValue(u"LuminaTranslateReceiveHeader"_q));
	AddToggleRow(
		container,
		TrValue(u"LuminaDualLanguageDisplay"_q),
		[] { return DualLanguageDisplay(); },
		[](bool value) { SetDualLanguageDisplay(value); });
	AddValueRow(
		container,
		TrValue(u"LuminaTranslateReadLang"_q),
		[] {
			const auto code = TranslateReadLanguage();
			return code.isEmpty()
				? Tr(u"LuminaTranslateReadLangFollow"_q)
				: TranslateLanguageName(code);
		},
		[=] {
			ShowLanguagePicker(
				controller,
				Tr(u"LuminaTranslateReadLang"_q),
				Tr(u"LuminaTranslateReadLangFollow"_q),
				QString(),
				TranslateReadLanguage(),
				[](QString code) { SetTranslateReadLanguage(code); });
		});
	AddValueRow(
		container,
		TrValue(u"LuminaTranslateModeHeader"_q),
		[] {
			return AutoTranslateEverything()
				? Tr(u"LuminaTranslateModeAll"_q)
				: Tr(u"LuminaTranslateModeManual"_q);
		},
		[=] { ShowModePicker(controller); });
	Ui::AddSkip(container);
	Ui::AddDividerText(container, TrValue(u"LuminaTranslateReceiveInfo"_q));
}

void AddScopeRows(not_null<Ui::VerticalLayout*> container) {
	Ui::AddSkip(container);
	Ui::AddSubsectionTitle(
		container,
		TrValue(u"LuminaTranslateScopeHeader"_q));
	AddToggleRow(
		container,
		TrValue(u"LuminaTranslateScopePrivate"_q),
		[] { return TranslateScopePrivate(); },
		[](bool value) { SetTranslateScopePrivate(value); });
	AddToggleRow(
		container,
		TrValue(u"LuminaTranslateScopeGroup"_q),
		[] { return TranslateScopeGroup(); },
		[](bool value) { SetTranslateScopeGroup(value); });
	Ui::AddSkip(container);
	Ui::AddDividerText(container, TrValue(u"LuminaTranslateScopeInfo"_q));
}

void AddProviderRows(
		not_null<Ui::VerticalLayout*> container,
		not_null<Window::SessionController*> controller) {
	Ui::AddSkip(container);
	Ui::AddSubsectionTitle(
		container,
		TrValue(u"LuminaTranslateProviderHeader"_q));
	AddValueRow(
		container,
		TrValue(u"LuminaTranslateProvider"_q),
		[] { return CurrentProvider().name; },
		[=] { ShowProviderPicker(controller); });

	const auto keyBlock = AddConditionalBlock(container, [] {
		return CurrentProvider().needsKey;
	});
	AddValueRow(
		keyBlock,
		TrValue(u"LuminaTranslateApiKey"_q),
		[] { return MaskedApiKey(ProviderApiKey(CurrentProviderId())); },
		[=] {
			const auto id = CurrentProviderId();
			ShowTextEditor(
				controller,
				Tr(u"LuminaTranslateApiKey"_q),
				Tr(u"LuminaTranslateApiKey"_q),
				ProviderApiKey(id),
				kApiKeyMaxLength,
				false,
				[id](QString value) { SetProviderApiKey(id, value); });
		});

	const auto baseUrlBlock = AddConditionalBlock(container, [] {
		return CurrentProvider().needsBaseUrl;
	});
	AddValueRow(
		baseUrlBlock,
		TrValue(u"LuminaTranslateBaseUrl"_q),
		[] { return LlmBaseUrl(); },
		[=] {
			ShowTextEditor(
				controller,
				Tr(u"LuminaTranslateBaseUrl"_q),
				DefaultLlmBaseUrl(),
				LlmBaseUrl(),
				kBaseUrlMaxLength,
				false,
				[](QString value) { SetLlmBaseUrl(value); });
		});

	const auto modelBlock = AddConditionalBlock(container, [] {
		return CurrentProvider().needsModel;
	});
	AddValueRow(
		modelBlock,
		TrValue(u"LuminaTranslateModel"_q),
		[] { return LlmModel(); },
		[=] {
			ShowTextEditor(
				controller,
				Tr(u"LuminaTranslateModel"_q),
				DefaultLlmModel(),
				LlmModel(),
				kModelMaxLength,
				false,
				[](QString value) { SetLlmModel(value); });
		});

	const auto promptBlock = AddConditionalBlock(container, [] {
		return CurrentProvider().needsPrompt;
	});
	AddValueRow(
		promptBlock,
		TrValue(u"LuminaTranslateSystemPrompt"_q),
		[] {
			return (LlmPrompt() == DefaultLlmPrompt())
				? Tr(u"LuminaTranslatePromptDefault"_q)
				: Tr(u"LuminaTranslatePromptCustom"_q);
		},
		[=] {
			ShowTextEditor(
				controller,
				Tr(u"LuminaTranslateSystemPrompt"_q),
				Tr(u"LuminaTranslateSystemPrompt"_q),
				LlmPrompt(),
				kPromptMaxLength,
				true,
				[](QString value) { SetLlmPrompt(value); });
		});

	const auto fallbackBlock = AddConditionalBlock(container, [] {
		return !ProviderIsTelegram();
	});
	AddToggleRow(
		fallbackBlock,
		TrValue(u"LuminaTranslateFallbackTelegram"_q),
		[] { return TranslateFallbackToTelegram(); },
		[](bool value) { SetTranslateFallbackToTelegram(value); });

	Ui::AddSkip(container);
	Ui::AddDividerText(
		container,
		TrValue(u"LuminaTranslateProviderSecurityInfo"_q));
}

} // namespace

bool TranslateBeforeSend() {
	return Settings::Instance().getBool(kKeyBeforeSend, false);
}

void SetTranslateBeforeSend(bool value) {
	Settings::Instance().set(kKeyBeforeSend, value);
}

bool TranslateBeforeSendConfirm() {
	return Settings::Instance().getBool(kKeyBeforeSendConfirm, false);
}

void SetTranslateBeforeSendConfirm(bool value) {
	Settings::Instance().set(kKeyBeforeSendConfirm, value);
}

QString TranslateSendLanguage() {
	const auto stored = Settings::Instance().getString(
		kKeySendLang,
		kSendLangAuto).trimmed();
	return stored.isEmpty() ? kSendLangAuto : stored;
}

void SetTranslateSendLanguage(const QString &code) {
	const auto trimmed = code.trimmed();
	Settings::Instance().set(
		kKeySendLang,
		trimmed.isEmpty() ? kSendLangAuto : trimmed);
}

bool TranslateSendLanguageIsAuto() {
	return (TranslateSendLanguage() == kSendLangAuto);
}

bool DualLanguageDisplay() {
	return Settings::Instance().getBool(kKeyDualLanguage, false);
}

void SetDualLanguageDisplay(bool value) {
	Settings::Instance().set(kKeyDualLanguage, value);
}

QString TranslateReadLanguage() {
	return Settings::Instance().getString(kKeyReadLang).trimmed();
}

void SetTranslateReadLanguage(const QString &code) {
	const auto trimmed = code.trimmed();
	if (trimmed.isEmpty()) {
		Settings::Instance().remove(kKeyReadLang);
	} else {
		Settings::Instance().set(kKeyReadLang, trimmed);
	}
}

bool TranslateScopePrivate() {
	return Settings::Instance().getBool(kKeyScopePrivate, true);
}

void SetTranslateScopePrivate(bool value) {
	Settings::Instance().set(kKeyScopePrivate, value);
}

bool TranslateScopeGroup() {
	return Settings::Instance().getBool(kKeyScopeGroup, true);
}

void SetTranslateScopeGroup(bool value) {
	Settings::Instance().set(kKeyScopeGroup, value);
}

void SetAutoTranslateEverything(bool value) {
	Settings::Instance().set(kKeyMode, value ? kModeAll : kModeManual);
}

const std::vector<TranslateLanguage> &TranslateLanguages() {
	// The names are re-read whenever the in-app language changes, in place: a
	// caller holds a reference to this vector while it copies the names out of
	// it, so the entries themselves must never move.
	static auto result = std::vector<TranslateLanguage>();
	static auto builtFor = QString();
	if (result.empty()) {
		const auto &entries = LanguageEntries();
		result.reserve(entries.size());
		for (const auto &entry : entries) {
			result.push_back({ entry.code, QString() });
		}
	}
	const auto locale = LocaleCode();
	if (builtFor != locale) {
		builtFor = locale;
		const auto &entries = LanguageEntries();
		for (auto i = 0, count = int(result.size()); i != count; ++i) {
			result[i].name = Tr(entries[i].key);
		}
	}
	return result;
}

QString TranslateLanguageName(const QString &code) {
	if (code.isEmpty()) {
		return QString();
	}
	for (const auto &language : TranslateLanguages()) {
		if (language.code == code) {
			return language.name;
		}
	}
	return code;
}

rpl::producer<> TranslateSettingsChanges() {
	return rpl::merge(
		TranslateProviderChanges(),
		Settings::Instance().changes(
		) | rpl::filter([](const QString &key) {
			return IsOwnedKey(key);
		}) | rpl::to_empty);
}

void AddTranslateRows(
		not_null<Ui::VerticalLayout*> container,
		not_null<Window::SessionController*> controller) {
	// The master opt-in, first row on the page. Off by default: the default
	// provider needs no key, so without this a fresh profile would silently
	// gain a translate bar and an unlocked "Translate chats" switch that stock
	// does not show. See Lumina::TranslationFeatureEnabled().
	Ui::AddSkip(container);
	AddToggleRow(
		container,
		TrValue(u"LuminaTranslateEnable"_q),
		[] { return TranslationFeatureEnabled(); },
		[](bool value) {
			Settings::Instance().set(kKeyFeatureEnabled, value);
		});
	Ui::AddSkip(container);
	Ui::AddDividerText(container, TrValue(u"LuminaTranslateEnableInfo"_q));
	AddSendRows(container, controller);
	AddReceiveRows(container, controller);
	AddScopeRows(container);
	AddProviderRows(container, controller);
}

} // namespace Lumina
