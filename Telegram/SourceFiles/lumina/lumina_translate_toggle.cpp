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

bool ChatTranslateIncomingReady(not_null<History*> history) {
	return (history->translation() != nullptr);
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

void SetChatTranslatingTo(not_null<History*> history, LanguageId id) {
	const auto peer = history->peer;
	using Flag = PeerData::TranslationFlag;
	if (id && (peer->translationFlag() == Flag::Disabled)) {
		peer->saveTranslationDisabled(false);
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
