/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_translate_gating.h"

#include "core/application.h"
#include "core/core_settings.h"
#include "data/data_peer.h"
#include "data/data_peer_values.h" // Data::AmPremiumValue.
#include "history/history.h"
#include "lumina/lumina_settings.h"
#include "lumina/lumina_translate_providers.h" // UsingOwnProvider.
#include "lumina/lumina_translate_readlang.h" // ReadLanguageOr.
#include "main/main_session.h"

namespace Lumina {
namespace {

[[nodiscard]] QString ModeKey() {
	return u"trMode"_q;
}

[[nodiscard]] QString ModeAll() {
	return u"all"_q;
}

[[nodiscard]] QString ModeManual() {
	return u"manual"_q;
}

[[nodiscard]] QString ScopePrivateKey() {
	return u"trScopePrivate"_q;
}

[[nodiscard]] QString ScopeGroupKey() {
	return u"trScopeGroup"_q;
}

// TranslateProviderChanges() covers every key on the provider page, not just
// the provider id, so this recomputes on an API key edit as well; the value
// only actually changes when the selected provider does, which is what
// distinct_until_changed() below leaves for the subscriber.
[[nodiscard]] rpl::producer<bool> OwnProviderValue() {
	return rpl::single(
		rpl::empty
	) | rpl::then(
		TranslateProviderChanges()
	) | rpl::map([] {
		return UsingOwnProvider();
	}) | rpl::distinct_until_changed();
}

// The master opt-in. The default provider is the keyless google_web engine, so
// without this a fresh profile would already satisfy UsingOwnProvider() and a
// non-Premium account would see the translate bar, an unlocked "Translate
// chats" switch and the per-chat row where stock shows none - a visible change
// with nothing asked for. Requiring an explicit opt-in keeps a fresh profile
// identical to stock while still letting the free engine work the moment the
// user turns the feature on.
[[nodiscard]] QString FeatureEnabledKey() {
	return u"translateEnabled"_q;
}

} // namespace

bool TranslationFeatureEnabled() {
	return Settings::Instance().getBool(FeatureEnabledKey(), false);
}

rpl::producer<bool> TranslationFeatureEnabledValue() {
	return rpl::single(
		rpl::empty
	) | rpl::then(
		Settings::Instance().changesFor(FeatureEnabledKey())
	) | rpl::map([] {
		return TranslationFeatureEnabled();
	}) | rpl::distinct_until_changed();
}

bool ChatTranslationUnlocked(not_null<Main::Session*> session) {
	return session->premium()
		|| (TranslationFeatureEnabled() && UsingOwnProvider());
}

rpl::producer<bool> ChatTranslationUnlockedValue(
		not_null<Main::Session*> session) {
	using namespace rpl::mappers;
	return rpl::combine(
		Data::AmPremiumValue(session),
		TranslationFeatureEnabledValue(),
		OwnProviderValue(),
		_1 || (_2 && _3));
}

bool AutoTranslateEverything() {
	return (Settings::Instance().getString(ModeKey(), ModeManual())
		== ModeAll());
}

bool TranslateScopeAllows(not_null<PeerData*> peer) {
	return Settings::Instance().getBool(
		peer->isUser() ? ScopePrivateKey() : ScopeGroupKey(),
		true);
}

bool ShouldAutoTranslate(not_null<History*> history) {
	const auto peer = history->peer;
	using Flag = PeerData::TranslationFlag;
	return TranslationFeatureEnabled()
		&& AutoTranslateEverything()
		&& TranslateScopeAllows(peer)
		&& (peer->translationFlag() == Flag::Enabled)
		&& Core::App().settings().translateChatEnabled()
		&& ChatTranslationUnlocked(&history->session());
}

std::optional<std::vector<LanguageId>> AutoTranslateOfferSkip(
		not_null<History*> history) {
	if (!ShouldAutoTranslate(history)) {
		return std::nullopt;
	}
	const auto to = ReadLanguageOr(Core::App().settings().translateTo());
	auto result = std::vector<LanguageId>();
	if (to) {
		result.push_back(to);
	}
	return result;
}

rpl::producer<> AutoTranslatePolicyChanges() {
	const auto &settings = Settings::Instance();
	return rpl::merge(
		settings.changesFor(FeatureEnabledKey()),
		settings.changesFor(ModeKey()),
		settings.changesFor(ScopePrivateKey()),
		settings.changesFor(ScopeGroupKey()),
		TranslateProviderChanges(),
		ReadLanguageCodeValue() | rpl::skip(1) | rpl::to_empty,
		// The read language is an OVERRIDE: with none set, and that is the
		// default, AutoTranslateOfferSkip() resolves the target through
		// Core::Settings::translateTo(). Ui::ChooseTranslateToBox() writes
		// that one on every pick and NoteReadLanguageChosen() is a no-op
		// while there is no override, so without this term picking a new
		// translate-to language moves the target the offer filter is built
		// from and nothing re-evaluates the filter: a chat already offered
		// in the new target keeps being offered - and in "all" mode keeps
		// being translated into itself - while a chat in the old target is
		// never offered at all. Both only correct themselves when some other
		// key here changes or the chat is reopened.
		Core::App().settings().translateToValue()
			| rpl::skip(1)
			| rpl::to_empty);
}

} // namespace Lumina
