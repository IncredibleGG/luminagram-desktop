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
	return TranslationFeatureEnabled()
		&& Core::App().settings().translateChatEnabled()
		&& ChatTranslationUnlocked(&history->session())
		&& history->translateOfferedFrom();
}

bool ChatTranslating(not_null<History*> history) {
	return history->translatedTo().known();
}

void SetChatTranslating(not_null<History*> history, bool enabled) {
	const auto peer = history->peer;
	using Flag = PeerData::TranslationFlag;
	if (enabled && (peer->translationFlag() == Flag::Disabled)) {
		peer->saveTranslationDisabled(false);
	}
	const auto to = enabled
		? Ui::ChooseTranslateTo(history)
		: LanguageId();
	history->translateTo(to);
	if (const auto migrated = history->migrateFrom()) {
		migrated->translateTo(to);
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
		TranslationFeatureEnabledValue(),
		ChatTranslationUnlockedValue(session),
		Core::App().settings().translateChatEnabledValue()
	) | rpl::to_empty;
}

} // namespace Lumina
