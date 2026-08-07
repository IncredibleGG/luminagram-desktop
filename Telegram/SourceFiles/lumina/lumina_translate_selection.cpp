/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_translate_selection.h"

#include "core/application.h"
#include "core/core_settings.h"
#include "lang/lang_keys.h" // tr::marked.
#include "lang/translate_mtproto_provider.h"
#include "lang/translate_provider.h"
#include "lumina/lumina_translate_gating.h"
#include "lumina/lumina_translate_providers.h"
#include "lumina/lumina_translate_readlang.h"
#include "platform/platform_translate_provider.h"

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

// The Ui::TranslateProvider face of a LuminaGram engine, for the one caller
// that must not be tier-gated.
//
// lumina_translate_providers.cpp has the same adapter, and that is the one
// tdesktop's shared factory uses; this is a second one only because that file's
// Lumina::CreateTranslateProvider() answers null while the continuous tier is
// off - correctly, since the whole-chat path builds its provider from the same
// hook - and a free-tier lookup must still reach the engine the user named.
// Keep the two bodies identical. supportsMessageId() is false and must stay
// false for the reason spelled out on CreateTranslateProvider() in
// lumina_translate_providers.h: a true here would leave the request carrying a
// message id and no text, which our engines cannot use.
class OnDemandEngineProvider final : public Ui::TranslateProvider {
public:
	explicit OnDemandEngineProvider(std::unique_ptr<TranslateEngine> engine)
	: _engine(std::move(engine)) {
	}

	[[nodiscard]] bool supportsMessageId() const override {
		return false;
	}

	void request(
			Ui::TranslateProviderRequest request,
			LanguageId to,
			Fn<void(Ui::TranslateProviderResult)> done) override {
		const auto text = request.text.text;
		if (text.isEmpty()) {
			done({ .error = Ui::TranslateProviderError::Unknown });
			return;
		}
		_engine->translate(
			text,
			TargetLanguageCode(to),
			[done](TranslateResult result) {
				if (result.failed()) {
					done({ .error = Ui::TranslateProviderError::Unknown });
				} else {
					done({ .text = tr::marked(result.text) });
				}
			});
	}

private:
	const std::unique_ptr<TranslateEngine> _engine;

};

// The OS translator, which Ui::CreateTranslateProvider()
// (lang/translate_provider.cpp) offers ahead of any LuminaGram engine. It is
// re-stated here so that the bypass below cannot quietly demote it: it is a
// "use this engine" choice exactly as the Service row is, it is on-device, and
// it must keep winning where it wins today.
//
// That function's other early branch, the experimental translate-url-template
// option, is deliberately NOT re-stated. Reading it needs its literal option id
// in a second place, and base::options::value() aborts on an id it does not
// know - a hard crash on every context menu if upstream ever renames it. The
// bypass below is the only path that skips it, it is taken only while the
// continuous tier is off, and the combination it costs is an experimental
// developer option plus a LuminaGram engine selected plus the switch off, where
// the Service row is the answer a user can actually see.
[[nodiscard]] bool PlatformTranslatorChosen() {
	return Core::App().settings().usePlatformTranslation()
		&& Platform::IsTranslateProviderAvailable();
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
	// THE FREE TIER IS FREE, and this is what makes it so.
	//
	// The engine named on the Service row, whenever it is one of ours and
	// usable. UsingOwnProvider() is the same predicate
	// Lumina::CreateTranslateProvider() applies - it is false for "Telegram"
	// and for a provider whose API key is missing - MINUS that function's tier
	// term, and the missing term is the entire point.
	//
	// Ui::CreateTranslateProvider() reaches a LuminaGram engine only through
	// that function, which answers null while ContinuousTranslationAvailable()
	// is false. That is right for the whole-chat path, which builds its
	// provider from the same hook, and wrong here twice over: with the switch
	// off - the default, and the state most users are in - a selected-text or
	// single-message lookup would fall through to Telegram's own MTProto
	// service. The free tier would stop using the keyless default engine that
	// is what makes it free with no key to obtain, and the lookup would go
	// somewhere other than the service the user named, which the top of this
	// file promises can never happen.
	//
	// The tier is read here for ONE thing only: whether the shared factory
	// already gives the right answer. It never decides whether this feature is
	// available - with the tier on, the branch below is not taken and the
	// resolution is upstream's, unchanged, in upstream's own order.
	const auto ownEngine = [&]() -> std::unique_ptr<Ui::TranslateProvider> {
		if (!UsingOwnProvider()) {
			return nullptr;
		}
		auto engine = MakeCurrentTranslateEngine(session);
		return engine
			? std::make_unique<OnDemandEngineProvider>(std::move(engine))
			: nullptr;
	};
	// With the continuous tier off, a one-off lookup follows the engine the
	// user NAMED, and only that. On a profile that never opened the Service
	// row, the default is not a choice, so the lookup goes where stock sends
	// it - to Telegram - rather than to a third party the user never picked.
	if (!ContinuousTranslationAvailable()
		&& !PlatformTranslatorChosen()
		&& ProviderExplicitlyChosen()) {
		if (auto own = ownEngine()) {
			return own;
		}
	}
	if (auto result = Ui::CreateTranslateProvider(session)) {
		return result;
	}
	// Ui::CreateTranslateProvider() (lang/translate_provider.cpp) only answers
	// null on the branch it takes for the platform translator: it checks
	// Platform::IsTranslateProviderAvailable() and then calls
	// Platform::CreateTranslateProvider(), which on macOS can still decline
	// (platform/mac/translate_provider_mac.mm). It returns before ever
	// reaching a LuminaGram engine, so falling straight to MTProto here would
	// send the selection to Telegram - past the provider the user actually
	// named on the LuminaGram translate page, and past the promise at the top
	// of this file that nothing quietly reroutes a lookup. Only when there is
	// no own engine to ask is MTProto the right answer.
	if (auto own = ownEngine()) {
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
