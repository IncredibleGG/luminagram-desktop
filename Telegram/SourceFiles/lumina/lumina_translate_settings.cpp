/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_translate_settings.h"

#include "base/timer.h"
#include "lang/lang_keys.h"
#include "lumina/lumina_explain.h"
#include "lumina/lumina_glossary.h"
#include "lumina/lumina_locale.h"
#include "lumina/lumina_settings.h"
#include "lumina/lumina_translate_gating.h"
#include "lumina/lumina_translate_providers.h"
#include "main/main_session.h"
#include "settings/settings_common.h"
#include "ui/boxes/single_choice_box.h"
#include "ui/layers/generic_box.h"
#include "ui/layers/show.h"
#include "ui/toast/toast.h"
#include "ui/ui_utility.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/fields/input_field.h"
#include "ui/widgets/labels.h"
#include "ui/wrap/slide_wrap.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"

#include "styles/style_layers.h"
#include "styles/style_menu_icons.h"
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
const auto kKeyFoldOriginal = u"foldOriginalLongMessages"_q;

// LuminaGram: these three keys are owned by other headers - the group-skip
// preference by lumina_translate_gating (its accessor is private there, so the
// key name is spelled out here as the header's own comment documents it),
// "explain this message" by lumina_explain, and the do-not-translate list by
// lumina_glossary. They are listed in IsOwnedKey() below so that a row this page
// adds for each of them refreshes the moment the value changes, exactly like
// every key this file owns outright.
const auto kKeyGroupSkip = u"groupSkipMyLanguages"_q;
const auto kKeyExplain = u"explainMessage"_q;
const auto kKeyGlossary = u"glossaryTerms"_q;

const auto kSendLangAuto = u"auto"_q;

constexpr auto kApiKeyMaxLength = 512;
constexpr auto kBaseUrlMaxLength = 512;
constexpr auto kModelMaxLength = 128;
constexpr auto kPromptMaxLength = 4096;

// The do-not-translate list caps each term at this length in lumina_glossary;
// the editor field matches it so a term cannot be typed longer than it stores.
constexpr auto kGlossaryTermMaxLength = 128;

// How much of an API key a row shows. Enough to tell two keys apart, not
// enough to be worth a screenshot.
constexpr auto kApiKeyTailShown = 4;
constexpr auto kApiKeyDotsShown = 6;

// The test result is the one toast on this page a user has to read rather than
// glance at, and a failure names what to go and change, so it stays up much
// longer than the 1.5s default.
constexpr auto kTestToastDuration = crl::time(7000);

// A service that answers a two-word sample with a paragraph - an LLM that
// explained itself instead of translating, most often - is broken in a way the
// toast should report, not inherit.
constexpr auto kTestResultMaxLength = 200;

// How long the test row waits for an engine that may never answer. It has to
// be comfortably longer than the 15s QNetworkRequest::setTransferTimeout()
// that lumina_translate_providers.cpp puts on every HTTP request, so that an
// engine which does report its own failure always gets to name it and this
// only ever catches the case where nothing came back at all.
constexpr auto kTestTimeoutMs = crl::time(25000);

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
		|| (key == kKeyFoldOriginal)
		|| (key == kKeyGroupSkip)
		|| (key == kKeyExplain)
		|| (key == kKeyGlossary);
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

// The master switch at the top of the page, as a stream. Every row below it
// depends on it, and none of them is worth setting while it is off.
[[nodiscard]] rpl::producer<bool> FeatureEnabledValue() {
	return TranslationFeatureEnabledValue();
}

// Greys a dependent row out and stops it taking presses while the master
// switch is off.
//
// The rows stay where they are rather than disappearing, because this page is
// also where somebody finds out what the switch would give them - but a row
// that still looks live invites them to pick a provider, save a key and
// choose two languages that nothing will read.
//
// Qt::WA_TransparentForMouseEvents is what actually blocks the press:
// Ui::AbstractButton::setDisabled() gates the accessibility action and
// nothing else in this lib_ui, so a row "disabled" that way still toggles
// under the mouse. clearState() drops a hover the row may be holding, which
// would otherwise leave it painted as if the cursor were still on it.
void GateRow(not_null<Ui::SettingsButton*> button, Ui::FlatLabel *label) {
	FeatureEnabledValue(
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

// ::Settings::AddButtonWithLabel() keeps no handle on the value it draws on
// the right, and a gated row has to grey that value out along with the rest of
// itself - a value left in the accent colour is exactly the "looks pressable,
// is not" state this page is removing. So the label is built here, with the
// geometry ::Settings::CreateRightLabel() gives it, and kept.
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
	return label;
}

[[nodiscard]] QString MaskedApiKey(const QString &key) {
	if (key.isEmpty()) {
		return Tr(u"LuminaTranslateApiKeyNotSet"_q);
	}
	// The tail is dropped entirely for a key too short to have one safely.
	// Clamping it to the key's own length instead printed the whole secret
	// into a settings row: a four character key came back as itself, and a
	// five character one as a single dot followed by four real characters.
	// The dot run is a fixed width for the same reason - a run that shrank
	// with the key was reporting its length.
	const auto length = int(key.size());
	const auto tail = (length > kApiKeyTailShown + kApiKeyDotsShown)
		? kApiKeyTailShown
		: 0;
	return QString(kApiKeyDotsShown, QChar(0x2022)) + key.right(tail);
}

// The exact string Android's LuminaTranslateActivity.runProviderTest() sends,
// so that the same key on the same service can be compared across the two
// clients without reading either implementation.
[[nodiscard]] QString TestSampleText() {
	return u"Hello, world!"_q;
}

// The language that sample is written in. The test must never ask for it.
[[nodiscard]] QString TestSampleLanguage() {
	return u"en"_q;
}

// The target used when the language the read side would ask for is the
// sample's own. Spanish because every service in TranslateProviders() can
// reach it, including the narrowest of them, and because the answer is
// unmistakably not English to anyone reading the toast.
[[nodiscard]] QString TestFallbackLanguage() {
	return u"es"_q;
}

// The language the test asks for is the one the read side would ask for, so a
// test that comes back sane is evidence about the setup the user actually has
// - including whether the service honours their dialect.
//
// With one exception, and it is the whole reason this is not one line: the
// sample is English, so an English target makes this test pass for a working
// service and for a dead one alike - "Hello, world!" comes back as "Hello,
// world!" either way. That was the run a default profile got, since the read
// language starts unset and the interface language is usually English, and a
// test that cannot fail is not a test. An English target is therefore replaced
// by one the sample has to visibly change under.
[[nodiscard]] QString TestTargetLanguage() {
	const auto stored = NormalizeLanguageCode(TranslateReadLanguage());
	const auto wanted = stored.isEmpty() ? InterfaceLanguageCode() : stored;
	return (wanted.isEmpty()
		|| (BaseLanguageCode(wanted) == TestSampleLanguage()))
		? TestFallbackLanguage()
		: wanted;
}

// The other half of the same problem: a service that hands the sample straight
// back has not translated it, whatever status it said so with. An engine that
// echoes its input, an LLM that answered "Hello, world!" because the prompt
// was lost, and a proxy that returns the request body all land here rather
// than being reported as a pass. Case and run-length of whitespace are
// ignored, because neither of those is a translation either.
[[nodiscard]] bool LooksUntranslated(
		const QString &sample,
		const QString &translated) {
	const auto flatten = [](const QString &text) {
		return text.simplified().toCaseFolded();
	};
	return (flatten(sample) == flatten(translated));
}

// The "Test failed: <reason>" shape, shared by the two paths that report one:
// the engine's own error, and a reply that came back unchanged.
[[nodiscard]] QString TestFailedWith(const QString &reason) {
	return Tr(u"LuminaTranslateTestFailed"_q) + u": "_q + reason;
}

// `keyed` is true when the request went out carrying an API key, and `status`
// is the HTTP status it came back with - 0 when it never got one, and also 0
// for a failure that never reached HTTP at all.
//
// Both are needed because ErrorForStatus() maps every status onto three
// enumerators, and the failures this row exists to tell apart land in only two
// of them: a rejected key is 401, or on DeepL 403; an exhausted quota is 429
// or 456, or again on DeepL 403; a host that never answered carries no status
// whatsoever. The enum alone therefore cannot separate "your key is wrong"
// from "your wifi is off", which is the single most common pair of afternoons
// this row is pressed on.
[[nodiscard]] QString TestFailureText(
		TranslateError error,
		bool keyed,
		int status) {
	const auto reason = [&] {
		switch (error) {
		case TranslateError::NoKey:
			return Tr(u"LuminaTranslateNoKey"_q);
		case TranslateError::RateLimited:
			// 429 and 456 are quota codes and nothing else, so they say so
			// whether or not a key was involved. 403 is the ambiguous one: it
			// is also what DeepL answers for a key it does not accept, so
			// with a key in play it has to name the key first - otherwise the
			// likeliest reason a user presses this row, a mistyped key, is
			// answered with "try again later" and they wait instead of
			// fixing it.
			return (keyed && (status != 429) && (status != 456))
				? Tr(u"LuminaTranslateTestQuotaKeyed"_q)
				: Tr(u"LuminaTranslateTestQuota"_q);
		case TranslateError::Network:
			// Everything that is not a quota code arrives here, so this is
			// where the status earns its keep.
			if (status == 401) {
				return Tr(u"LuminaTranslateTestKeyRejected"_q);
			} else if (!status || (status >= 500)) {
				// Nothing answered at all, or the service answered that it
				// is broken. Neither has anything to do with the key, and
				// blaming the key here sends the user off to re-enter a key
				// that was fine.
				return Tr(u"LuminaTranslateTestNetwork"_q);
			}
			return keyed
				? Tr(u"LuminaTranslateTestKeyRejected"_q)
				: Tr(u"LuminaTranslateTestNetwork"_q);
		case TranslateError::Unavailable:
			return Tr(u"LuminaTranslateTestUnavailable"_q);
		case TranslateError::BadResponse:
		case TranslateError::None:
			// None with an empty text is TranslateResult::failed() too: the
			// service answered, and the answer had no translation in it.
			return Tr(u"LuminaTranslateTestBadResponse"_q);
		}
		return Tr(u"LuminaTranslateTestUnavailable"_q);
	}();
	return TestFailedWith(reason);
}

[[nodiscard]] QString TestSuccessText(
		const QString &sample,
		const QString &translated,
		const QString &targetName) {
	auto shown = translated.trimmed();
	if (shown.size() > kTestResultMaxLength) {
		shown = shown.left(kTestResultMaxLength) + QChar(0x2026);
	}
	// The target is named, because the result is only evidence to somebody
	// who can see which language was asked for.
	return Tr(u"LuminaTranslateTestSuccess"_q)
		+ u"\n"_q
		+ sample
		+ u" → "_q
		+ shown
		+ u" ("_q
		+ targetName
		+ u")"_q;
}

void ShowTestToast(
		not_null<Window::SessionController*> controller,
		const QString &text) {
	controller->showToast(Ui::Toast::Config{
		.text = TextWithEntities{ text },
		.duration = kTestToastDuration,
	});
}

// Both row builders below make DEPENDENT rows: everything they add is gated on
// the master switch. The master switch itself is built inline in
// AddTranslateRows() and must never go through them, or it would grey itself
// out and could not be switched back on.
not_null<Ui::SettingsButton*> AddToggleRow(
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
	GateRow(button, nullptr);
	return button;
}

// `Settings` names Lumina::Settings inside this namespace, so the settings
// section helpers have to be reached through the global namespace.
not_null<Ui::SettingsButton*> AddValueRow(
		not_null<Ui::VerticalLayout*> container,
		rpl::producer<QString> label,
		Fn<QString()> value,
		Fn<void()> activate) {
	auto rowText = rpl::duplicate(label);
	const auto button = ::Settings::AddButtonWithIcon(
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

	// Ui::InputField::setMaxLength() chops the text the field is ALREADY
	// holding - lib_ui/ui/widgets/fields/input_field.cpp:2500-2517 runs
	// chopByMaxLength() over the whole document the moment the limit is set.
	// Applied to a stored value that is longer than the cap, that deletes the
	// tail before the user has touched anything, the field then shows the
	// truncated text as if that were what was stored, and Save writes the
	// truncation back. A value that long can only have arrived from
	// Settings::importAll() - a backup, or another client with a different
	// limit - and it is still the user's text. Cap what they type, never what
	// they already had; an over-long value stays intact and editable, and the
	// cap starts applying again as soon as it fits.
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
	AddToggleRow(
		container,
		TrValue(u"LuminaFoldOriginalLongMessages"_q),
		[] { return FoldOriginalLongMessages(); },
		[](bool value) { SetFoldOriginalLongMessages(value); });
	// LuminaGram: "in groups, leave messages in a language I already read as
	// their original". The accessor lives in lumina_translate_gating (private
	// there), so the key is read and written directly, exactly as the header
	// documents it - Store::Prefs, default true, multi-user peers only.
	AddToggleRow(
		container,
		TrValue(u"LuminaTranslateGroupSkipMyLanguages"_q),
		[] { return Settings::Instance().getBool(kKeyGroupSkip, true); },
		[](bool value) { Settings::Instance().set(kKeyGroupSkip, value); });
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
	Ui::AddSkip(container);
	Ui::AddDividerText(container, TrValue(u"LuminaTranslateReceiveInfo"_q));
}

// One in-flight test, owned by the page. Destroying it destroys the engine,
// which is what cancels a request the user walked away from: the engine
// contract in lumina_translate_providers.h says a destroyed engine's callbacks
// never run, so nothing here has to be guarded against the page going away.
// The stream carries the label text rather than a change signal, so that
// nothing subscribed to it has to reach back into this struct: the state is
// owned by the page's lifetime, which a QWidget destroys before it deletes its
// children, and a row still alive for those few moments must not read it.
struct TestState {
	rpl::event_stream<QString> label;

	// Which run owns `engine` right now. The deferred release below compares
	// it, so a run that has already been replaced can never drop the engine a
	// later run is still waiting on - which would strand the row on "Testing"
	// with `running` stuck true and the button dead for the rest of the page.
	int generation = 0;
	bool running = false;

	// The only thing that can end a run the engine never answers. Every HTTP
	// engine sets QNetworkRequest::setTransferTimeout() and therefore always
	// reaches its callback, but the Telegram engine does not go over HTTP at
	// all: Ui::CreateMTProtoTranslateProvider() sends through MTP::Sender,
	// and an MTProto request made while the connection is down is queued
	// rather than failed - neither .done() nor .fail() runs until it is sent.
	// Without this, pressing Test on provider "Telegram" while offline left
	// the row reading "Testing..." with `running` stuck true and the button
	// dead for the rest of the page, which is the exact confusion the row
	// exists to end.
	base::Timer watchdog;

	// Last, so that it is destroyed first: whatever the engine does on the way
	// out, the rest of this struct is still there while it does it.
	std::unique_ptr<TranslateEngine> engine;
};

// Android's ITEM_TEST, and the whole reason this section is worth having: a
// service that silently does nothing is otherwise indistinguishable from a
// service that is not selected, a key that was never saved, and a chat that
// simply has translation off.
void AddTestRow(
		not_null<Ui::VerticalLayout*> container,
		not_null<Window::SessionController*> controller) {
	const auto state = container->lifetime().make_state<TestState>();
	const auto button = ::Settings::AddButtonWithIcon(
		container,
		TrValue(u"LuminaTranslateTest"_q),
		st::settingsButtonNoIcon);
	GateRow(button, AddRightLabel(
		button,
		TrValue(u"LuminaTranslateTest"_q),
		rpl::single(QString()) | rpl::then(state->label.events())));
	button->setClickedCallback([=] {
		if (state->running) {
			return;
		}
		const auto provider = CurrentProvider();
		const auto keyed = provider.needsKey;
		if (keyed && ProviderApiKey(provider.id).isEmpty()) {
			ShowTestToast(
				controller,
				TestFailureText(TranslateError::NoKey, false, 0));
			return;
		}

		// MakeTranslateEngine() and deliberately not
		// MakeCurrentTranslateEngine(): the latter wraps the selection in the
		// Telegram fallback, which is right for a message and wrong for a
		// test - it would answer "it works" for a service that never replied,
		// which is exactly the confusion this row exists to end.
		auto engine = MakeTranslateEngine(provider.id, &controller->session());
		if (!engine) {
			ShowTestToast(
				controller,
				TestFailureText(TranslateError::Unavailable, false, 0));
			return;
		}
		state->engine = std::move(engine);
		state->running = true;
		const auto generation = ++state->generation;
		state->label.fire(Tr(u"LuminaTranslateTestRunning"_q));

		// Armed before the request goes out, because an engine may answer
		// from inside translate() itself - an unparseable base url makes
		// SendHttp() call back synchronously - and the cancel below has to be
		// able to disarm a timer that is already running.
		//
		// Dropping the engine from here is safe in a way that dropping it
		// from the result callback is not: this runs from the event loop and
		// not from inside a network reply, so there is no live reply object
		// underneath. Destroying it is also what cancels the request, so a
		// late answer can no longer arrive and contradict the toast.
		state->watchdog.setCallback([=] {
			if (!state->running || (state->generation != generation)) {
				return;
			}
			state->running = false;
			state->label.fire(QString());
			state->engine = nullptr;
			ShowTestToast(
				controller,
				TestFailureText(TranslateError::Network, keyed, 0));
		});
		state->watchdog.callOnce(kTestTimeoutMs);

		const auto sample = TestSampleText();
		const auto target = TestTargetLanguage();
		state->engine->translate(sample, target, [=](
				TranslateResult result) {
			state->watchdog.cancel();
			state->running = false;
			state->label.fire(QString());
			ShowTestToast(controller, result.failed()
				? TestFailureText(result.error, keyed, result.httpStatus)
				: LooksUntranslated(sample, result.text)
				? TestFailedWith(Tr(u"LuminaTranslateTestNoChange"_q))
				: TestSuccessText(
					sample,
					result.text,
					TranslateLanguageName(target)));

			// This runs inside the engine's own network reply, so the engine
			// cannot be dropped from here - that would delete the reply, and
			// with it the lambda currently executing. Release it from a later
			// main thread turn, guarded by the row: if the page closed first
			// this never runs, and the engine died with the page anyway.
			crl::on_main(button, [=] {
				if (state->generation == generation) {
					state->engine = nullptr;
				}
			});
		});
	});
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

	AddTestRow(container, controller);

	Ui::AddSkip(container);
	Ui::AddDividerText(
		container,
		TrValue(u"LuminaTranslateProviderSecurityInfo"_q));
}

// The do-not-translate list editor, modelled on lumina_text_replace_settings'
// rule box: it writes through Glossary::SetGlossaryTerms() and nothing else,
// and the list below rebuilds from Glossary::GlossaryChanges(), so this box
// never needs a pointer back into the list that opened it. A term is identified
// by its own text, which SetGlossaryTerms() keeps unique (case-insensitively),
// so editing or deleting one finds it by value.
void EditGlossaryTermBox(not_null<Ui::GenericBox*> box, QString original) {
	const auto adding = original.isEmpty();
	box->setTitle(TrValue(adding
		? u"LuminaGlossaryAdd"_q
		: u"LuminaGlossaryEdit"_q));

	const auto field = box->addRow(object_ptr<Ui::InputField>(
		box,
		st::defaultInputField,
		Ui::InputField::Mode::SingleLine,
		TrValue(u"LuminaGlossaryTermPlaceholder"_q),
		original));
	field->setMaxLength(kGlossaryTermMaxLength);

	box->setFocusCallback([=] {
		field->setFocusFast();
	});

	const auto save = [=] {
		const auto text = field->getLastText().trimmed();
		if (text.isEmpty()) {
			field->showError();
			return;
		}
		auto terms = Glossary::GlossaryTerms();
		const auto index = adding ? -1 : int(terms.indexOf(original));
		if (index >= 0) {
			terms[index] = text;
		} else {
			terms.append(text);
		}
		Glossary::SetGlossaryTerms(terms);
		box->closeBox();
	};
	field->submits() | rpl::on_next(save, field->lifetime());

	box->addButton(tr::lng_settings_save(), save);
	box->addButton(tr::lng_cancel(), [=] {
		box->closeBox();
	});
	if (!adding) {
		box->addLeftButton(tr::lng_box_delete(), [=] {
			auto terms = Glossary::GlossaryTerms();
			const auto index = int(terms.indexOf(original));
			if (index >= 0) {
				terms.removeAt(index);
				Glossary::SetGlossaryTerms(terms);
			}
			box->closeBox();
		}, st::attentionBoxButton);
	}
}

void GlossaryBox(not_null<Ui::GenericBox*> box) {
	box->setStyle(st::layerBox);
	box->setWidth(st::boxWideWidth);
	box->setTitle(TrValue(u"LuminaGlossaryTitle"_q));

	// The list is rebuilt wholesale, so it gets a layout of its own rather than
	// clearing the box's - GenericBox owns its content layout during prepare().
	const auto content = box->verticalLayout()->add(
		object_ptr<Ui::VerticalLayout>(box));

	const auto rebuild = std::make_shared<Fn<void()>>();
	*rebuild = [=] {
		const auto width = content->width();
		content->clear();

		const auto terms = Glossary::GlossaryTerms();

		Ui::AddSkip(content);
		Ui::AddSubsectionTitle(content, TrValue(u"LuminaGlossaryHeader"_q));
		const auto add = ::Settings::AddButtonWithIcon(
			content,
			TrValue(u"LuminaGlossaryAdd"_q),
			st::settingsButtonActive,
			{ &st::menuIconAdd });
		add->setClickedCallback([=] {
			box->uiShow()->show(Box(EditGlossaryTermBox, QString()));
		});
		for (const auto &term : terms) {
			const auto button = ::Settings::AddButtonWithIcon(
				content,
				rpl::single(term),
				st::settingsButtonNoIcon);
			button->setClickedCallback([=] {
				box->uiShow()->show(Box(EditGlossaryTermBox, term));
			});
		}
		Ui::AddSkip(content);
		Ui::AddDividerText(content, TrValue(terms.isEmpty()
			? u"LuminaGlossaryEmpty"_q
			: u"LuminaGlossaryListInfo"_q));
		content->resizeToWidth(width);
	};
	(*rebuild)();

	// Driven by the store, not by the boxes that write to it, and always
	// deferred: Ui::VerticalLayout::clear() deletes its children immediately,
	// and the click that caused the write may still be on the stack inside one
	// of the rows about to go. The subscription lives on `content`.
	Glossary::GlossaryChanges(
	) | rpl::on_next([=] {
		Ui::PostponeCall(content, [=] {
			(*rebuild)();
		});
	}, content->lifetime());

	box->addButton(tr::lng_close(), [=] {
		box->closeBox();
	});
}

// "More": the two rows that use the translation LLM but are not the chat
// translator itself - "Explain this message" (lumina_explain) and the
// do-not-translate list (lumina_glossary). Gated on the master switch like
// every other dependent row on this page, because both lean on the provider
// and key configured in the section above.
void AddMoreRows(
		not_null<Ui::VerticalLayout*> container,
		not_null<Window::SessionController*> controller) {
	Ui::AddSkip(container);
	Ui::AddSubsectionTitle(container, TrValue(u"LuminaTranslateMoreHeader"_q));
	AddToggleRow(
		container,
		TrValue(u"LuminaExplainEnable"_q),
		[] { return ExplainMessageEnabled(); },
		[](bool value) { SetExplainMessageEnabled(value); });
	AddValueRow(
		container,
		TrValue(u"LuminaGlossaryManage"_q),
		[] {
			const auto count = int(Glossary::GlossaryTerms().size());
			return count
				? QString::number(count)
				: Tr(u"LuminaGlossaryNone"_q);
		},
		[=] { controller->show(Box(GlossaryBox)); });
	Ui::AddSkip(container);
	Ui::AddDividerText(container, TrValue(u"LuminaTranslateMoreInfo"_q));
}

} // namespace

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

bool FoldOriginalLongMessages() {
	return Settings::Instance().getBool(kKeyFoldOriginal, true);
}

void SetFoldOriginalLongMessages(bool value) {
	Settings::Instance().set(kKeyFoldOriginal, value);
}

rpl::producer<> FoldOriginalLongMessagesChanges() {
	return Settings::Instance().changesFor(kKeyFoldOriginal);
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
	//
	// Built here rather than through AddToggleRow(), which gates every row it
	// makes on this switch: a master row that greyed itself out could never be
	// switched back on.
	Ui::AddSkip(container);
	const auto master = container->add(object_ptr<Ui::SettingsButton>(
		container,
		TrValue(u"LuminaTranslateEnable"_q),
		st::settingsButtonNoIcon
	))->toggleOn(FlagValue([] { return TranslationFeatureEnabled(); }));
	master->toggledChanges(
	) | rpl::on_next([](bool value) {
		Settings::Instance().set(kKeyFeatureEnabled, value);
	}, master->lifetime());
	Ui::AddSkip(container);
	Ui::AddDividerText(container, TrValue(u"LuminaTranslateEnableInfo"_q));
	AddSendRows(container, controller);
	AddReceiveRows(container, controller);
	AddProviderRows(container, controller);
	AddMoreRows(container, controller);
}

} // namespace Lumina
