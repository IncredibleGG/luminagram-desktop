/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_ai_editor.h"

#include "lumina/lumina_locale.h"
#include "lumina/lumina_settings.h"
#include "lumina/lumina_translate_gating.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "ui/wrap/vertical_layout.h"

#include "styles/style_settings.h"

#include <QtCore/QJsonValue>

namespace Lumina {
namespace {

[[nodiscard]] QString KeepKey() {
	return u"aiEditorKeep"_q;
}

// lumina_locale.cpp is owned by another workflow, so the keys below may not be
// in the English table yet. Tr() answers empty for a key that table does not
// carry, and an empty settings row is worse than an untranslated one, so every
// string here names its own English text as well: the row reads correctly
// today and starts translating itself the moment the key is added, with no
// change needed on this side.
[[nodiscard]] QString TrOr(const QString &key, const QString &english) {
	const auto value = Tr(key);
	return value.isEmpty() ? english : value;
}

[[nodiscard]] rpl::producer<QString> TrOrValue(QString key, QString english) {
	return rpl::single(
		rpl::empty
	) | rpl::then(
		LangChanges()
	) | rpl::map([key = std::move(key), english = std::move(english)] {
		return TrOr(key, english);
	});
}

} // namespace

bool OfficialAiEditorAvailable() {
	return !TranslationFeatureEnabled() || KeepOfficialAiEditor();
}

rpl::producer<bool> OfficialAiEditorAvailableValue() {
	return rpl::combine(
		TranslationFeatureEnabledValue(),
		KeepOfficialAiEditorValue()
	) | rpl::map([](bool translation, bool keep) {
		return !translation || keep;
	}) | rpl::distinct_until_changed();
}

bool KeepOfficialAiEditor() {
	return Settings::Instance().getBool(KeepKey(), false);
}

void SetKeepOfficialAiEditor(bool value) {
	Settings::Instance().set(KeepKey(), value, Store::Prefs);
}

rpl::producer<bool> KeepOfficialAiEditorValue() {
	return rpl::single(
		rpl::empty
	) | rpl::then(
		Settings::Instance().changesFor(KeepKey())
	) | rpl::map([] {
		return KeepOfficialAiEditor();
	}) | rpl::distinct_until_changed();
}

void AddAiEditorRows(
		not_null<Ui::VerticalLayout*> container,
		not_null<Window::SessionController*>) {
	Ui::AddSkip(container);
	Ui::AddSubsectionTitle(
		container,
		TrOrValue(
			u"LuminaAiEditorHeader"_q,
			u"Telegram AI editor"_q));
	const auto button = container->add(object_ptr<Ui::SettingsButton>(
		container,
		TrOrValue(
			u"LuminaAiEditorKeep"_q,
			u"Keep Telegram's AI editor"_q),
		st::settingsButtonNoIcon
	))->toggleOn(KeepOfficialAiEditorValue());
	button->toggledChanges(
	) | rpl::on_next([](bool value) {
		SetKeepOfficialAiEditor(value);
	}, button->lifetime());
	Ui::AddSkip(container);
	Ui::AddDividerText(
		container,
		TrOrValue(
			u"LuminaAiEditorInfo"_q,
			u"Telegram has an AI editor of its own in the composer, with a "
			u"Translate tab that overlaps LuminaGram's translation. While "
			u"LuminaGram translation is on, its button and its keyboard "
			u"shortcut are not offered, so there is only ever one translation "
			u"tool in front of you. Turn this on to keep Telegram's editor "
			u"available anyway. With LuminaGram translation off, Telegram's "
			u"editor is always there and this setting changes nothing."_q));
}

} // namespace Lumina
