/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include <rpl/producer.h>

namespace Ui {
class VerticalLayout;
} // namespace Ui

namespace Window {
class SessionController;
} // namespace Window

namespace Lumina {

// One translation system at a time.
//
// Telegram Desktop 7.0.8 ships its own AI editor (boxes/compose_ai_box.*, the
// "Ai" button in the composer and the ComposeAiApplyInPlace shortcut) whose
// Translate tab does the same job as ours. Its output is not wrong here - it
// already goes out through HistoryWidget::sendWithTextOverride ->
// sendTextWithTags, where Lumina::InterceptSend lives, so a message written
// with it is translated by our pipeline exactly like any other. This is not a
// correctness fix.
//
// It is a product decision by the project owner: turning LuminaGram
// translation on is the moment someone commits to translated conversation, and
// two overlapping tools live at once invites the wrong one being reached for.
// So while our translation is on, upstream's editor is not offered. A user who
// prefers Telegram's simply leaves ours off and keeps theirs, unchanged.
//
// Nothing here reaches inside compose_ai_box or changes what the editor does.
// It answers one question - may it be offered - and every entry point asks the
// same function, so the policy cannot drift between the button and the key.

// Whether the OFFICIAL (upstream) AI editor may be offered right now.
//
// True unless LuminaGram translation is on, and true again whenever the user
// has asked to keep the editor anyway (see KeepOfficialAiEditor below).
//
// The condition is the master opt-in, Lumina::TranslationFeatureEnabled(), and
// that is the owner's rule spelled literally: "when LuminaGram translation is
// switched on". It defaults to FALSE, so on a stock profile this returns true
// and every AI entry point behaves exactly as upstream wrote it.
[[nodiscard]] bool OfficialAiEditorAvailable();

// The same value as a stream, for controls that have to hide themselves the
// moment the preference changes rather than at the next relayout. Emits once
// immediately and then on every change to either key it depends on.
[[nodiscard]] rpl::producer<bool> OfficialAiEditorAvailableValue();

// The escape hatch, key `aiEditorKeep`, default FALSE - the owner's rule.
//
// Turning it on keeps upstream's AI editor available with our translation on,
// for someone who wants both and should not have to give up a tool they like
// to use ours. It is the only thing that can override the rule above, and it
// does nothing at all while our translation is off, because nothing is being
// suppressed then.
[[nodiscard]] bool KeepOfficialAiEditor();
void SetKeepOfficialAiEditor(bool value);
[[nodiscard]] rpl::producer<bool> KeepOfficialAiEditorValue();

// The whole sub-page, in the F-02 sub-page shape: one call, the section .cpp
// holds this and nothing else.
void AddAiEditorRows(
	not_null<Ui::VerticalLayout*> container,
	not_null<Window::SessionController*> controller);

} // namespace Lumina
