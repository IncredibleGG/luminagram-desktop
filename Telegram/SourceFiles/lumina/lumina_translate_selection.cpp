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
	if (auto result = Ui::CreateTranslateProvider(session)) {
		return result;
	}
	// Ui::CreateTranslateProvider() (lang/translate_provider.cpp:34-48) only
	// answers null on the branch it takes for the platform translator: it
	// checks Platform::IsTranslateProviderAvailable() and then calls
	// Platform::CreateTranslateProvider(), which on macOS can still decline
	// (platform/mac/translate_provider_mac.mm:96-101). It returns before ever
	// reaching Lumina::CreateTranslateProvider(), so falling straight to
	// MTProto here would send the selection to Telegram - past the provider
	// the user actually named on the LuminaGram translate page, and past the
	// promise at the top of this file that nothing quietly reroutes a lookup.
	// Ask the named provider first; it answers null itself for "Telegram" and
	// for a provider with no key, and only then is MTProto the right answer.
	if (auto own = Lumina::CreateTranslateProvider(session)) {
		return own;
	}
	return Ui::CreateMTProtoTranslateProvider(session);
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
