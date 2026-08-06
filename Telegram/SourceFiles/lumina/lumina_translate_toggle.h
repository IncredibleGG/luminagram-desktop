/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include <rpl/producer.h>

class History;

namespace Lumina {

// The per-chat translation switch, as one bit of state with one setter.
//
// The master opt-in (lumina/lumina_translate_gating.h) only says the service
// may be used at all; whether a particular conversation is translated is a
// separate, per-conversation decision, and it now has two entry points: the
// row in the chat menu that window/window_peer_menu.cpp already had, and the
// top bar button (history/view/history_view_top_bar_widget.cpp) that mirrors
// Android's b24. Two entry points onto one bit is fine; two implementations of
// what that bit means is not, which is why the reading, the writing and the
// "may this chat be translated at all" question live here rather than being
// spelled out again next to the button.
//
// SetChatTranslating() is deliberately the same steps the chat menu row
// performs, in the same order, including the two that are easy to forget: a
// peer whose translation the user previously hid has to be un-hidden before
// translating it, or History::translateTo() applies to a chat the translate
// bar still refuses to show; and a migrated (pre-supergroup) history has to be
// switched along with its successor, or scrolling back past the migration
// point reverts to untranslated text halfway up the chat.
//
// The chat menu row in window/window_peer_menu.cpp still spells those steps
// out inline, because that file is not this change's to edit. Pointing its
// Filler::addTranslate() at these functions is the whole of what is left, and
// until that happens this file is the definition and that one is the copy.

// Whether a per-chat translate control may be offered for this chat at all.
//
// This is the chat menu row's condition (window/window_peer_menu.cpp,
// Filler::addTranslate) plus TranslationFeatureEnabled(), and the extra term
// is deliberate rather than drift. The row is upstream's, gated on Premium
// through ChatTranslationUnlocked(), and it is shown on a Premium account
// whether or not this fork's translation was ever switched on - that is stock
// behaviour and removing it would be a visible change. The top bar button is
// ours and did not exist before, so it may not appear on a profile that never
// opted in: with the master switch off this bar has to be pixel-identical to
// stock. Hence a button that is strictly rarer than the row, never the
// reverse, and never a button offering something the row would refuse.
//
// translateOfferedFrom() is part of it because nothing downstream can act
// without it - History::translateTo() returns immediately for a history that
// was never offered a source language, so a control shown without one would be
// a button that does nothing.
[[nodiscard]] bool ChatTranslateAvailable(not_null<History*> history);

// Whether this chat is being translated right now.
[[nodiscard]] bool ChatTranslating(not_null<History*> history);

// Turns whole-chat translation on or off for this chat. This is exactly what
// the chat menu row does and nothing more - it does not re-check the gates,
// because the row does not either, and a setter that quietly refused would
// leave a control that looks toggled and is not. Callers decide whether to
// offer the action, with ChatTranslateAvailable().
void SetChatTranslating(not_null<History*> history, bool enabled);

// Emits once immediately, then whenever anything either of the two answers
// above depends on changes - this chat's offered and translated languages, the
// peer's hidden-translation flag, and the global switches the availability is
// gated on. A control refreshes itself from this and stays in step with the
// other entry point, with Settings, and with the same chat open in a second
// window, without any of them knowing about each other.
[[nodiscard]] rpl::producer<> ChatTranslateStateChanges(
	not_null<History*> history);

} // namespace Lumina
