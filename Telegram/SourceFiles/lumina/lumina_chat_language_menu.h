/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

class History;

namespace Ui {
class PopupMenu;
} // namespace Ui

namespace Window {
class SessionController;
} // namespace Window

namespace Lumina {

// The one place that states, in words, what language pair a conversation is
// using - and lets each half of it be changed without leaving the chat.
//
// It exists because the answer was spread across three surfaces, two of them
// undiscoverable:
//
//   * Settings > Translation holds a GLOBAL send language and a GLOBAL read
//     language, and says nothing about any particular chat;
//   * the top bar button (lumina/lumina_translate_toggle.h) toggled this chat's
//     translation on and off while showing no language at all;
//   * the per-chat SEND language (Lumina::ShowDialogSendLanguagePicker) had one
//     entry point, a row inside the send button's context menu.
//
// So this menu is deliberately NOT a fourth store of anything. Every row reads
// and writes a value that already had exactly one owner:
//
//   incoming target   History::translatedTo() / Lumina::SetChatTranslatingTo()
//                     with Ui::ChooseTranslateTo() as the inherited value
//   outgoing target   Lumina::DialogSendLanguage() / SetDialogSendLanguage()
//                     with Lumina::TranslateSendLanguage() as the global one
//   tone / register   Lumina::DialogRegister() / SetDialogRegister()
//                     (lumina/lumina_register.h), which has no global at all
//
// The third row is not a third direction: it is the one answer that governs
// both of the rows above. What breaks first across languages is not the
// meaning but the politeness level, and neither a source language nor a target
// language says which one this conversation wants - only the relationship
// does. So the row names the relationship, and the translation of everything
// that arrives and everything that is sent is asked to speak in it.
//
// It is also the one row that can be honest about the engine underneath it: an
// engine with no channel for tone (the free Google endpoint, Telegram's own)
// gets a grey line under the row saying so, rather than a setting that reads as
// having worked and silently did nothing.
//
// !! THE TWO DIRECTIONS ARE NOT STORED THE SAME WAY, and the menu cannot hide
// that. The outgoing lock is a preference: it is keyed by session and peer, it
// is written to the private store, and it survives a restart. The incoming
// target is History::translatedTo(), which is runtime state that lives as long
// as the History does - stock tdesktop has never persisted it, the translate
// bar's own picker writes the same field, and Ui::ChooseTranslateTo() is what
// re-derives it on the next launch.
//
// Composing with that field is still the right answer rather than adding a
// persistent per-chat read override beside it: two answers to "what is this
// chat translated into" would disagree the moment the translate bar wrote one
// of them, and everything downstream - the bar, the tracker, the translation
// itself - reads the History. What a restart costs is the override, not the
// setting: the chat comes back on the global read language, which is where it
// would have been anyway.

// Whether a right-click on the top bar translate button has anything to show.
// This is Lumina::ChatTranslateAvailable() and nothing added: a chat that may
// not be translated at all has no pair to state, and with the master opt-in off
// it is false for every chat, so the menu is unreachable.
[[nodiscard]] bool ChatLanguageMenuAvailable(not_null<History*> history);

// Fills `menu` with the rows described above. Adds nothing when
// ChatLanguageMenuAvailable() is false, so a caller may call it unconditionally
// and check Ui::PopupMenu::empty() afterwards.
//
// `controller` is needed for the incoming picker only, which is a box; the
// outgoing picker resolves its own window through the History, exactly as the
// send menu's row does.
void FillChatLanguageMenu(
	not_null<Ui::PopupMenu*> menu,
	not_null<Window::SessionController*> controller,
	not_null<History*> history);

} // namespace Lumina
