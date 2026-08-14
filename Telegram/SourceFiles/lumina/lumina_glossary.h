/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include <rpl/producer.h>

#include <QtCore/QString>
#include <QtCore/QStringList>

#include <vector>

namespace Lumina::Glossary {

// The result of Protect(): the text with every do-not-translate span replaced
// by a private-use placeholder, plus the table that turns those placeholders
// back into the original spans. `restore[i]` is the original text of the
// placeholder that Protect() wrote for index i, and Restore() undoes exactly
// what Protect() did. An empty `restore` means nothing was masked.
struct Protected {
	QString text;
	std::vector<QString> restore;
};

// Masks the spans that must survive a round-trip through a translation engine
// unchanged: every configured glossary term (see GlossaryTerms()), every
// @username mention and every http(s) URL. Matching is case-insensitive; a
// term made of Latin word characters matches only on word boundaries, while a
// term in a script that does not separate words (Han, Kana, Hangul, ...)
// matches as a plain substring - the same rule lumina_text_replace uses.
//
// Hand the returned text to the engine and feed the engine's answer back
// through Restore() with the same table. When `restore` comes back empty the
// text is byte-for-byte the input and no restore step is needed.
[[nodiscard]] Protected Protect(const QString &text);

// Turns the placeholders in `text` back into the spans Protect() recorded.
// Placeholders the engine dropped simply do not come back; placeholders it
// duplicated come back once per copy. Safe on any text, including one that
// never went through Protect() (returned unchanged) and an empty table.
[[nodiscard]] QString Restore(
	const QString &text,
	const std::vector<QString> &restore);

// The do-not-translate list, one entry per term. Global: one list for every
// chat. Persisted under the "glossaryTerms" key (see the .cpp). A settings
// page edits it through SetGlossaryTerms(), which is the only supported writer.
[[nodiscard]] QStringList GlossaryTerms();
void SetGlossaryTerms(const QStringList &terms);

// Fires when the list above changes, for a settings page to react to.
[[nodiscard]] rpl::producer<> GlossaryChanges();

} // namespace Lumina::Glossary
