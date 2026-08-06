/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_translate_selection.h"

#include "lang/translate_mtproto_provider.h"
#include "lang/translate_provider.h"
#include "lumina/lumina_translate_providers.h"
#include "lumina/lumina_translate_readlang.h"

namespace Lumina {
namespace {

// Ui::SkipTranslate()'s own budget, kept as it was: only the first 100
// characters are scanned for a letter, and anything longer than that is taken
// to contain one somewhere.
constexpr auto kFirstChunk = 100;

// LanguageId::known() is true for QLocale::C, which LanguageId::language()
// then reports as English. A target that really only means "QLocale could not
// parse this" should fall through to the interface language rather than pin
// every answer to English.
[[nodiscard]] bool Usable(LanguageId id) {
	return id && (id.value != QLocale::C);
}

} // namespace

bool OnDemandTranslateAllowed(const QString &text) {
	if (text.isEmpty()) {
		return false;
	}
	const auto scanned = std::min(int(text.size()), kFirstChunk);
	for (auto i = 0; i != scanned; ++i) {
		if (text.at(i).isLetter()) {
			return true;
		}
	}
	return (text.size() >= kFirstChunk);
}

auto CreateOnDemandTranslateProvider(not_null<Main::Session*> session)
-> std::unique_ptr<Ui::TranslateProvider> {
	auto result = Ui::CreateTranslateProvider(session);
	return result
		? std::move(result)
		: Ui::CreateMTProtoTranslateProvider(session);
}

LanguageId OnDemandTranslateTo(LanguageId chosen) {
	if (Usable(chosen)) {
		return chosen;
	}
	const auto stored = ReadLanguage();
	if (Usable(stored)) {
		return stored;
	}
	const auto app = LanguageId::FromName(InterfaceLanguageCode());
	return Usable(app) ? app : LanguageId{ QLocale::English };
}

} // namespace Lumina
