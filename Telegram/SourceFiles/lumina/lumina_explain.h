/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "base/basic_types.h"

#include <rpl/producer.h>

#include <QtCore/QString>

class HistoryItem;

namespace Ui {
class PopupMenu;
} // namespace Ui

namespace Window {
class SessionController;
} // namespace Window

namespace Lumina {

struct MessageMenuContext;

// LuminaGram "Explain this": the cultural-note reading of a single message,
// the desktop port of Android's LuminaExplain. The user right-clicks a message
// and asks what it really means; the message text is handed to the user's own
// LLM, which breaks it into four parts - the literal meaning, the actual tone,
// the cultural / slang notes, and how they might reply - answered in the
// language the user reads, and the answer is shown in a scrollable box.
//
// It reuses the SAME LLM the translation settings own: the API key, base URL
// and model are exactly the ones on Settings > LuminaGram > Translation
// (lumina/lumina_translate_providers.h), so nothing about the LLM is
// configured twice and nothing new is stored. With no key set it does not run
// and does not crash - the box says to add one on that page.

// Master switch, Android's `explainMessage`, default ON as there.
[[nodiscard]] bool ExplainMessageEnabled();
void SetExplainMessageEnabled(bool value);
[[nodiscard]] rpl::producer<> ExplainMessageEnabledChanges();

// The plain text of `item` that would be explained, trimmed, or empty when the
// item is null or carries no text (a bare photo, a sticker, a voice note).
// This is also the row's guard: no text, no row.
[[nodiscard]] QString ExplainMessageText(HistoryItem *item);

// Opens the explain box for `item` and starts the request. Safe for any item;
// it answers with a message in the box rather than doing anything drastic when
// there is no key, no text, or the request fails.
void ShowExplainMessage(
	not_null<Window::SessionController*> controller,
	not_null<HistoryItem*> item);

// The message-menu row. Signature matches the AddExplainRow stub in
// lumina/lumina_message_menu.h.
void AddExplainMenuRow(
	not_null<Ui::PopupMenu*> menu,
	const MessageMenuContext &context);

} // namespace Lumina
