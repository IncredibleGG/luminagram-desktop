/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_translate_toggle.h"

#include "boxes/translate_box.h" // Ui::ChooseTranslateTo.
#include "core/application.h"
#include "core/core_settings.h"
#include "data/data_changes.h"
#include "data/data_peer.h"
#include "history/history.h"
#include "lumina/lumina_translate_gating.h"
#include "main/main_session.h"
#include "spellcheck/spellcheck_types.h" // LanguageId.

namespace Lumina {

bool ChatTranslateAvailable(not_null<History*> history) {
	// Deliberately NOT gated on history->translateOfferedFrom().
	//
	// That flag means "tdesktop decided this chat is in a foreign language",
	// and it is the same detection that suppresses anything the user is
	// assumed to already know. Requiring it hid this button exactly when it
	// was most wanted: the user looking at a chat the app does not think
	// needs translating, wanting to translate it anyway. The point of a
	// per-chat control is to overrule that judgement, so it has to be
	// reachable before the judgement is made - and it stays reachable if the
	// judgement never comes.
	return ContinuousTranslationAvailable()
		&& Core::App().settings().translateChatEnabled()
		&& ChatTranslationUnlocked(&history->session());
}

bool ChatTranslating(not_null<History*> history) {
	return history->translatedTo().known();
}

LanguageId ChatTranslateDefaultTo(not_null<History*> history) {
	return Ui::ChooseTranslateTo(history);
}

LanguageId ChatTranslatingTo(not_null<History*> history) {
	const auto now = history->translatedTo();
	return now ? now : ChatTranslateDefaultTo(history);
}

void SetChatTranslating(not_null<History*> history, bool enabled) {
	SetChatTranslatingTo(
		history,
		enabled ? ChatTranslateDefaultTo(history) : LanguageId());
}

bool ChatTranslationExcluded(not_null<History*> history) {
	using Flag = PeerData::TranslationFlag;
	return (history->peer->translationFlag() == Flag::Disabled);
}

void SetChatTranslationExcluded(
		not_null<History*> history,
		bool excluded) {
	using Flag = PeerData::TranslationFlag;
	const auto peer = history->peer;
	if (excluded == (peer->translationFlag() == Flag::Disabled)) {
		return;
	} else if (excluded) {
		// Stop it here as well as excluding it. The flag withdraws the OFFER,
		// and a chat already being translated keeps its own translation until
		// something clears it - so without this the chat the user just told
		// us to leave alone would go on being translated until it was closed.
		SetChatTranslatingTo(history, LanguageId());
	}
	peer->saveTranslationDisabled(excluded);
}

void SetChatTranslatingTo(not_null<History*> history, LanguageId id) {
	const auto peer = history->peer;
	using Flag = PeerData::TranslationFlag;
	if (id && (peer->translationFlag() == Flag::Disabled)) {
		peer->saveTranslationDisabled(false);
	}
	// History::translateTo() drops the language on the floor unless the app has
	// already offered to translate this chat, and it never offers one written
	// in the language being read into - which is precisely the chat this
	// feature exists for: the one the app sees no reason to translate, that
	// the user wants translated anyway. Asking by hand is reason enough, so
	// the offer is opened here rather than waited for.
	//
	// The source named is the chat's own if detection settled on one, and the
	// read language otherwise. It is what the translate bar says the text is
	// being translated from; the engine detects the real source itself, so a
	// wrong guess here costs a word in a bar, not a wrong translation.
	if (id && !history->translation()) {
		const auto detected = history->translateOfferedFrom();
		history->translateOfferFrom(detected ? detected : id);
	}
	history->translateTo(id);
	if (const auto migrated = history->migrateFrom()) {
		migrated->translateTo(id);
	}
}

rpl::producer<> ChatTranslateStateChanges(not_null<History*> history) {
	using HistoryFlag = Data::HistoryUpdate::Flag;
	using PeerFlag = Data::PeerUpdate::Flag;
	const auto session = &history->session();
	return rpl::combine(
		session->changes().historyFlagsValue(
			history,
			(HistoryFlag::TranslateFrom | HistoryFlag::TranslatedTo)),
		session->changes().peerFlagsValue(
			history->peer,
			PeerFlag::TranslationDisabled),
		ContinuousTranslationAvailableValue(),
		ChatTranslationUnlockedValue(session),
		Core::App().settings().translateChatEnabledValue()
	) | rpl::to_empty;
}

} // namespace Lumina
