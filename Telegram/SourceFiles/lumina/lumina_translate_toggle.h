/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "spellcheck/spellcheck_types.h" // LanguageId.

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
// Filler::addTranslate) plus ContinuousTranslationAvailable(), and the extra term
// is deliberate rather than drift. The row is upstream's, gated on Premium
// through ChatTranslationUnlocked(), and it is shown on a Premium account
// whether or not this fork's translation was ever switched on - that is stock
// behaviour and removing it would be a visible change. The top bar button is
// ours and did not exist before, so it may not appear on a profile that never
// opted in: with the master switch off this bar has to be pixel-identical to
// stock. Hence a button that is strictly rarer than the row, never the
// reverse, and never a button offering something the row would refuse.
//
// translateOfferedFrom() is deliberately NOT part of it, and that has a price
// this file has to state rather than let a caller discover. Requiring it hid
// the control exactly when it was most wanted - on a chat tdesktop has not
// decided is foreign - which is the judgement a per-chat control exists to
// overrule. But nothing downstream can act until the judgement arrives:
// History::translateTo() returns on its first line while the History has no
// HistoryTranslation, and only the tracker's recognition creates one. So
// between "available" and "recognised" this returns true for a chat whose
// incoming side cannot yet be pointed anywhere, and a caller that offers a
// language there is offering a write that is dropped. Test
// ChatTranslateIncomingReady() below before offering one, and before offering
// to turn translation on at all; the outgoing side has no such dependency and
// works throughout.
[[nodiscard]] bool ChatTranslateAvailable(not_null<History*> history);

// Whether this chat is being translated right now.
[[nodiscard]] bool ChatTranslating(not_null<History*> history);

// Whether the INCOMING half can be pointed anywhere yet - the second half of
// the note above, as a question a caller can ask instead of reaching into
// History for it.
//
// False means the tracker has not recognised the language this chat is written
// in, so History has no HistoryTranslation, so History::translateTo() returns
// on its first line and BOTH SetChatTranslating(history, true) and
// SetChatTranslatingTo() are silent no-ops. Every control that offers to start
// translating, or to choose a language to read this chat in, has to test this
// first - a control that does not is a control that looks live, writes a
// value, and changes nothing. The outgoing half is unaffected: the send lock
// is this fork's own preference and never touches HistoryTranslation.
//
// Turning translation OFF is always safe: it only runs while it is on, and it
// is on only when a HistoryTranslation exists.
[[nodiscard]] bool ChatTranslateIncomingReady(not_null<History*> history);

// The language THIS chat's incoming messages are being translated into, and the
// one it would use if it has never been told otherwise.
//
// ChatTranslateDefaultTo() is Ui::ChooseTranslateTo() and nothing added, named
// here so that the fork spells that resolution in one place: it already folds
// in LuminaGram's read-language override (lumina/lumina_translate_readlang.h),
// the last language picked in a stock picker, and the "do not translate a chat
// into the language it is already written in" fallback to the skip list.
//
// ChatTranslatingTo() is the EFFECTIVE answer - what this chat is translated
// into now, or, when it is not being translated, what turning it on would use.
// It never returns nothing, so a control built on it never has an empty row.
//
// The two are equal exactly when this chat has no override of its own, which is
// how a "use the default" action knows whether to offer itself.
[[nodiscard]] LanguageId ChatTranslateDefaultTo(not_null<History*> history);
[[nodiscard]] LanguageId ChatTranslatingTo(not_null<History*> history);

// Turns whole-chat translation on or off for this chat. This is exactly what
// the chat menu row does and nothing more - it does not re-check the gates,
// because the row does not either, and a setter that quietly refused would
// leave a control that looks toggled and is not. Callers decide whether to
// offer the action, with ChatTranslateAvailable().
void SetChatTranslating(not_null<History*> history, bool enabled);

// The same write, with the target named instead of resolved. An empty
// LanguageId turns translation off, so SetChatTranslating() is a two-line
// wrapper over this rather than a second copy of the steps - which matters
// because two of those steps are the easy-to-forget ones this file's header
// note is about, and a per-chat language picker that skipped either would set a
// language on a chat whose translate bar then refuses to show it, or leave the
// migrated half of a supergroup on the previous language.
void SetChatTranslatingTo(not_null<History*> history, LanguageId id);

// Emits once immediately, then whenever anything either of the two answers
// above depends on changes - this chat's offered and translated languages, the
// peer's hidden-translation flag, and the global switches the availability is
// gated on. A control refreshes itself from this and stays in step with the
// other entry point, with Settings, and with the same chat open in a second
// window, without any of them knowing about each other.
[[nodiscard]] rpl::producer<> ChatTranslateStateChanges(
	not_null<History*> history);

} // namespace Lumina
