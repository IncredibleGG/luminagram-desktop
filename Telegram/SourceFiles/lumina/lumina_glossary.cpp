/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_glossary.h"

#include "lumina/lumina_settings.h"
#include "ui/text/text_entity.h"

#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonValue>
#include <QtCore/QSet>

#include <algorithm>

namespace Lumina::Glossary {
namespace {

// The same key Android stores this list under, so that a backup taken there
// and imported here (lumina_backup) lands on the list instead of a dead key.
const auto kKeyTerms = u"glossaryTerms"_q;

// The list lives in Store::Bookmarks - the file tdesktop uses for the growable
// arrays - so that editing it does not rewrite the file every other preference
// shares, exactly as lumina_text_replace does with its rules. Settings::set()
// defaults to Store::Prefs and silently RELOCATES a key written through that
// default, so SetGlossaryTerms() is the only writer of kKeyTerms and it always
// names the store.
constexpr auto kTermsStore = Store::Bookmarks;

// Placeholders live in the Basic Multilingual Plane private-use area
// (U+E000..U+F8FF): 6400 single-QChar code points, none of which a real
// message or a translation engine has any reason to carry. The i-th masked
// span becomes QChar(kPlaceholderBase + i), and Restore() maps it straight
// back through the table index. The count is capped so it never leaves the
// area.
constexpr auto kPlaceholderBase = char16_t(0xE000);
constexpr auto kPlaceholderCount = int(0xF8FF - 0xE000 + 1); // 6400.

// Above this the text is passed through untouched: it is far past any real
// message (the premium length limit is measured in thousands), and Protect()
// runs on the main-thread send path.
constexpr auto kMaxProtectLength = 100 * 1024;

// Guards on the stored list, so a pathological import cannot make every send
// walk a runaway list. Both are far above any real glossary.
constexpr auto kMaxStoredTerms = 2000;
constexpr auto kMaxTermLength = 128;

// Deliberately the ASCII \w set and not QChar::isLetterOrNumber(): this is only
// ever asked "would a word boundary be meaningful on this side", and for a
// script that does not separate words (Han, Kana, Hangul, Thai) the honest
// answer is no. Requiring a boundary there would mean such a term never
// matches; requiring none gives plain substring matching, which is what a user
// who typed such a term means. Same rule as lumina_text_replace.
[[nodiscard]] bool IsWordChar(QChar ch) {
	const auto code = ch.unicode();
	return (code >= u'a' && code <= u'z')
		|| (code >= u'A' && code <= u'Z')
		|| (code >= u'0' && code <= u'9')
		|| (code == u'_');
}

struct Range {
	int from = 0;
	int till = 0;
};

[[nodiscard]] bool Intersects(
		const std::vector<Range> &spans,
		int from,
		int till) {
	for (const auto &range : spans) {
		if (from < range.till && range.from < till) {
			return true;
		}
	}
	return false;
}

[[nodiscard]] QJsonArray ReadTermsArray() {
	auto &settings = Settings::Instance();
	const auto array = settings.getArray(kKeyTerms);
	if (!array.isEmpty()) {
		return array;
	}
	// Android stores this key as a *string* holding a JSON array. A backup
	// taken there and imported into a desktop profile therefore arrives as a
	// string, and dropping it would look exactly like "my list did not come
	// across".
	const auto raw = settings.getString(kKeyTerms);
	if (raw.isEmpty()) {
		return QJsonArray();
	}
	const auto document = QJsonDocument::fromJson(raw.toUtf8());
	return document.isArray() ? document.array() : QJsonArray();
}

// URLs and @mentions, via tdesktop's own entity parser. With no markdown flag
// ParseEntities() only adds entities and never rewrites the text, and the two
// types it produces under these flags are exactly the ones the spec protects.
void AppendEntitySpans(const QString &text, std::vector<Range> &spans) {
	const auto parsed = TextUtilities::ParseEntities(
		text,
		(TextParseLinks | TextParseMentions));
	for (const auto &entity : parsed.entities) {
		const auto from = entity.offset();
		const auto length = entity.length();
		if (length > 0) {
			spans.push_back({ from, from + length });
		}
	}
}

// Every glossary term's matches that do not fall inside a span already taken.
void AppendTermSpans(const QString &text, std::vector<Range> &spans) {
	auto terms = GlossaryTerms();

	// Longest first, so "iPhone Pro" claims its span before "Pro" can; the
	// shorter match then intersects the taken span and is skipped.
	std::sort(
		terms.begin(),
		terms.end(),
		[](const QString &a, const QString &b) {
			return a.size() > b.size();
		});
	for (const auto &term : terms) {
		if (term.isEmpty() || term.size() > text.size()) {
			continue;
		}
		const auto anchorLeft = IsWordChar(term.at(0));
		const auto anchorRight = IsWordChar(term.at(term.size() - 1));
		const auto length = int(term.size());
		auto position = 0;
		while (true) {
			const auto at = int(
				text.indexOf(term, position, Qt::CaseInsensitive));
			if (at < 0) {
				break;
			}
			const auto after = at + length;
			const auto boundaries = (!anchorLeft
					|| (at == 0)
					|| !IsWordChar(text.at(at - 1)))
				&& (!anchorRight
					|| (after == int(text.size()))
					|| !IsWordChar(text.at(after)));
			if (boundaries && !Intersects(spans, at, after)) {
				spans.push_back({ at, after });
			}
			position = at + 1;
		}
	}
}

} // namespace

Protected Protect(const QString &text) {
	auto result = Protected{ .text = text };
	if (text.isEmpty() || text.size() > kMaxProtectLength) {
		return result;
	}

	auto spans = std::vector<Range>();
	AppendEntitySpans(text, spans);
	AppendTermSpans(text, spans);
	if (spans.empty()) {
		return result;
	}

	std::sort(spans.begin(), spans.end(), [](const Range &a, const Range &b) {
		return a.from < b.from;
	});

	auto masked = QString();
	masked.reserve(text.size());
	auto position = 0;
	for (const auto &span : spans) {
		if (span.from < position) {
			continue; // Overlapped a span already emitted; skip it.
		} else if (int(result.restore.size()) >= kPlaceholderCount) {
			break; // Out of placeholders; leave the rest of the text as-is.
		}
		masked += text.mid(position, span.from - position);
		const auto index = int(result.restore.size());
		masked += QChar(char16_t(kPlaceholderBase + index));
		result.restore.push_back(text.mid(span.from, span.till - span.from));
		position = span.till;
	}
	masked += text.mid(position);
	result.text = std::move(masked);
	return result;
}

QString Restore(const QString &text, const std::vector<QString> &restore) {
	if (restore.empty() || text.isEmpty()) {
		return text;
	}
	const auto count = int(restore.size());
	auto result = QString();
	result.reserve(text.size());
	for (const auto ch : text) {
		const auto index = int(ch.unicode()) - int(kPlaceholderBase);
		if (index >= 0 && index < count) {
			result += restore[index];
		} else {
			result += ch;
		}
	}
	return result;
}

QStringList GlossaryTerms() {
	const auto array = ReadTermsArray();
	auto result = QStringList();
	result.reserve(array.size());
	for (const auto &entry : array) {
		const auto term = entry.toString().trimmed();
		if (term.isEmpty()) {
			continue;
		}
		result.push_back(term.left(kMaxTermLength));
		if (result.size() >= kMaxStoredTerms) {
			break;
		}
	}
	return result;
}

void SetGlossaryTerms(const QStringList &terms) {
	auto array = QJsonArray();
	auto seen = QSet<QString>();
	for (const auto &raw : terms) {
		const auto term = raw.trimmed().left(kMaxTermLength);
		if (term.isEmpty()) {
			continue;
		} else if (array.size() >= kMaxStoredTerms) {
			break;
		} else if (!seen.contains(term.toLower())) {
			seen.insert(term.toLower()); // Case-insensitive dedupe.
			array.append(term);
		}
	}
	Settings::Instance().set(kKeyTerms, array, kTermsStore);
}

rpl::producer<> GlossaryChanges() {
	return Settings::Instance().changesFor(kKeyTerms);
}

} // namespace Lumina::Glossary
