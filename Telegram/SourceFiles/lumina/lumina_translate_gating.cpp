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
#include "lang/lang_keys.h" // Lang::Id.
#include "lumina/lumina_settings.h"
#include "lumina/lumina_translate_providers.h" // UsingOwnProvider.
#include "lumina/lumina_translate_readlang.h" // ReadLanguageOr.
#include "main/main_session.h"

namespace Lumina {
namespace {

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

// LuminaGram: on a group or channel the relaxed offer below normally covers
// every language except the read one, so a message written in a language the
// user already reads is offered for translation exactly like a foreign one.
// This toggle (default ON, multi-user peers only) adds "the languages I read" -
// the interface language and the trReadLang override - back to the skip list
// for those peers, leaving such messages as their original and offering only
// what the user cannot read. A one-to-one chat is never touched.
[[nodiscard]] QString GroupSkipMyLanguagesKey() {
	return u"groupSkipMyLanguages"_q;
}

[[nodiscard]] bool GroupSkipMyLanguages() {
	return Settings::Instance().getBool(GroupSkipMyLanguagesKey(), true);
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

bool ContinuousTranslationAvailable() {
	// A licence or entitlement check belongs on this line and on no other. See
	// the note above the declaration in lumina_translate_gating.h.
	return TranslationFeatureEnabled();
}

rpl::producer<bool> ContinuousTranslationAvailableValue() {
	return TranslationFeatureEnabledValue();
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

std::optional<std::vector<LanguageId>> TranslateOfferSkip(
		not_null<History*> history) {
	const auto peer = history->peer;
	using Flag = PeerData::TranslationFlag;
	if (!ContinuousTranslationAvailable()
		|| (peer->translationFlag() != Flag::Enabled)
		|| !Core::App().settings().translateChatEnabled()
		|| !ChatTranslationUnlocked(&history->session())) {
		return std::nullopt;
	}
	const auto to = ReadLanguageOr(Core::App().settings().translateTo());
	auto result = std::vector<LanguageId>();
	const auto add = [&](LanguageId id) {
		if (id && !ranges::contains(result, id)) {
			result.push_back(id);
		}
	};
	add(to);
	if ((peer->isChat() || peer->isChannel()) && GroupSkipMyLanguages()) {
		add(LanguageId::FromName(Lang::Id()));
		add(ReadLanguage());
	}
	return result;
}

rpl::producer<> TranslateOfferPolicyChanges() {
	return rpl::merge(
		ContinuousTranslationAvailableValue() | rpl::to_empty,
		TranslateProviderChanges(),
		ReadLanguageCodeValue() | rpl::skip(1) | rpl::to_empty,
		Settings::Instance().changesFor(GroupSkipMyLanguagesKey()),
		// The read language is an OVERRIDE: with none set, and that is the
		// default, TranslateOfferSkip() resolves the target through
		// Core::Settings::translateTo(). Ui::ChooseTranslateToBox() writes
		// that one on every pick and NoteReadLanguageChosen() is a no-op
		// while there is no override, so without this term picking a new
		// translate-to language moves the target the offer filter is built
		// from and nothing re-evaluates the filter: a chat already offered
		// in the new target keeps being offered - and translated into itself
		// while it is switched on - while a chat in the old target is never
		// offered at all. Both only correct themselves when some other key
		// here changes or the chat is reopened.
		Core::App().settings().translateToValue()
			| rpl::skip(1)
			| rpl::to_empty);
}

} // namespace Lumina
