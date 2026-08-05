/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_translate_readlang.h"

#include "lumina/lumina_translate_settings.h"

namespace Lumina {
namespace {

// LanguageId::FromName() answers QLocale::C for anything QLocale does not
// recognise, and LanguageId::language() then reports C as English while
// LanguageId::known() still says true. Left alone, a code QLocale cannot parse
// would silently pin the whole read path to English instead of falling back to
// the interface language, so C is treated as unknown here.
[[nodiscard]] LanguageId Parse(const QString &code) {
	if (code.isEmpty()) {
		return LanguageId();
	}
	const auto result = LanguageId::FromName(code);
	return (result.value == QLocale::AnyLanguage
		|| result.value == QLocale::C)
		? LanguageId()
		: result;
}

} // namespace

QString ReadLanguageCode() {
	return TranslateReadLanguage();
}

LanguageId ReadLanguage() {
	return Parse(ReadLanguageCode());
}

LanguageId ReadLanguageOr(LanguageId fallback) {
	const auto id = ReadLanguage();
	return id ? id : fallback;
}

void NoteReadLanguageChosen(LanguageId id) {
	const auto now = ReadLanguage();
	if (!now || !id || now == id) {
		return;
	}
	// LanguageId::twoLetterCode() is locale().name().mid(0, 2), so a language
	// whose ISO tag is longer than two letters comes back as a DIFFERENT
	// language's code - Gusii ("guz") becomes "gu", which is Gujarati, and
	// Teso ("teo") becomes "te", which is Telugu. Both are in tdesktop's own
	// picker list. Only write back a code that reads back as the language that
	// was actually picked; otherwise leave the stored override alone rather
	// than silently repointing it at some other language.
	const auto code = id.twoLetterCode();
	if (Parse(code) == id) {
		SetTranslateReadLanguage(code);
	}
}

rpl::producer<QString> ReadLanguageCodeValue() {
	// TranslateSettingsChanges() covers every translation key, not just this
	// one; distinct_until_changed() below is what keeps the extra wakeups from
	// reaching a subscriber. Using it rather than a private changesFor() keeps
	// the key string spelled in exactly one place.
	return rpl::single(
		rpl::empty
	) | rpl::then(
		TranslateSettingsChanges()
	) | rpl::map([] {
		return ReadLanguageCode();
	}) | rpl::distinct_until_changed();
}

} // namespace Lumina
