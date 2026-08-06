/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_text_replace.h"

#include "base/random.h"
#include "lumina/lumina_settings.h"
#include "ui/text/text_entity.h"

#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
#include <QtCore/QRegularExpression>

#include <optional>

namespace Lumina {
namespace {

const auto kKeyEnabled = u"textReplaceEnabled"_q;

// The Android key name, kept so that a backup taken there and imported here
// (W6-E) lands on the rules instead of on a dead key.
const auto kKeyRules = u"textReplacements"_q;

const auto kFieldId = u"id"_q;
const auto kFieldFrom = u"from"_q;
const auto kFieldTo = u"to"_q;

// The rules array lives in Store::Bookmarks, not Store::Prefs, so that
// editing a rule does not rewrite the file every other preference shares
// (DESKTOP-PORT-PLAN F-01, requirement 1). Store::Bookmarks is named after
// its first tenant; what it actually is, is the file for the growable arrays.
//
// Because Settings::set() defaults to Store::Prefs and silently RELOCATES a
// key written through that default, WriteRules() below is the only place in
// the tree that writes kKeyRules, and it always names the store.
constexpr auto kRulesStore = Store::Bookmarks;

// A rule that grows the text can be chained with another that grows it again.
// Nothing here recurses - each rule advances past its own replacement - but N
// rules can still multiply, so the whole substitution is abandoned (the
// original text is sent) once the working string passes this. Far above any
// real message: the premium message length limit is measured in thousands.
// It is also the point past which a text is not looked at in the first place.
constexpr auto kMaxResultLength = 100 * 1024;

// Every substitution whose replacement is a different length from what it
// replaces has to move the tail of the string, so cost is (substitutions x
// length) and both factors are attacker-shaped: the length is whatever the
// user pasted, and the count is whatever their own rules happen to match.
// This is the send path on the main thread, so the product is capped rather
// than trusted. Ten thousand substitutions is orders of magnitude past any
// real message; passing it means a rule is pathological, and the message is
// then sent exactly as typed.
constexpr auto kMaxReplacements = 10000;

// Deliberately the ASCII \w set and not QChar::isLetterOrNumber(). This is
// only ever asked "would a word boundary be meaningful on this side", and for
// a script that does not separate words - Han, Hiragana, Thai - the honest
// answer is no: requiring a boundary there means the rule can never fire (the
// Android bug this feature's header describes), while requiring none gives
// plain substring matching, which is what a user writing such a rule means.
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

// Spans a substitution must not reach into: URLs, e-mail addresses,
// @mentions, #hashtags, $cashtags and /commands. With no markdown flag
// ParseEntities() only adds entities and never touches the text, and the
// types it can produce under these flags are exactly the ones worth
// protecting - so every entity it returns is taken as protected, with no
// EntityType switch to fall out of date.
[[nodiscard]] std::vector<Range> ProtectedRanges(const QString &text) {
	auto result = std::vector<Range>();
	const auto parsed = TextUtilities::ParseEntities(
		text,
		(TextParseLinks
			| TextParseMentions
			| TextParseHashtags
			| TextParseBotCommands));
	result.reserve(size_t(parsed.entities.size()));
	for (const auto &entity : parsed.entities) {
		const auto from = entity.offset();
		const auto length = entity.length();
		if (length > 0) {
			result.push_back({ from, from + length });
		}
	}
	return result;
}

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

// Returns std::nullopt when the rule matched nothing, so that the caller can
// skip re-deriving the protected ranges.
//
// A rule scans left to right and always advances past its own replacement, so
// it can never re-enter what it just wrote and the loop always terminates: a
// non-match advances by one, a match advances to the end of the replacement,
// and an empty replacement shortens the string.
//
// `budget` is the substitution allowance shared by every rule applied to this
// one message. It is decremented per substitution and the rule stops when it
// runs out; the caller sees the exhausted budget and abandons the whole
// substitution, so a rule that runs out never leaves a half-rewritten string.
[[nodiscard]] std::optional<QString> ApplyOneRule(
		const QString &text,
		const QString &from,
		const QString &to,
		const std::vector<Range> &protectedRanges,
		int &budget) {
	if (from.isEmpty() || text.isEmpty() || from.size() > text.size()) {
		return std::nullopt;
	}
	const auto anchorLeft = IsWordChar(from.at(0));
	const auto anchorRight = IsWordChar(from.at(from.size() - 1));
	const auto fromLength = int(from.size());
	const auto toLength = int(to.size());
	const auto delta = toLength - fromLength;

	auto spans = protectedRanges;
	auto changed = false;
	auto result = text;
	auto position = 0;
	while (true) {
		const auto at = int(result.indexOf(from, position, Qt::CaseSensitive));
		if (at < 0) {
			break;
		}
		const auto after = at + fromLength;
		const auto boundaries = (!anchorLeft
				|| (at == 0)
				|| !IsWordChar(result.at(at - 1)))
			&& (!anchorRight
				|| (after == int(result.size()))
				|| !IsWordChar(result.at(after)));
		if (!boundaries || Intersects(spans, at, after)) {
			position = at + 1;
			continue;
		}
		result.replace(at, fromLength, to);
		changed = true;
		position = at + toLength;

		// The protected ranges were measured against the string as it was
		// before this replacement. Everything that starts after the match
		// has just moved; nothing overlapping it can exist, because such a
		// match was skipped above.
		if (delta) {
			for (auto &range : spans) {
				if (range.from >= after) {
					range.from += delta;
					range.till += delta;
				}
			}
		}
		if (--budget <= 0 || result.size() > kMaxResultLength) {
			break;
		}
	}
	return changed ? std::optional(result) : std::nullopt;
}

// ApiWrap::sendMessage() is not only the composer's path. It is also how
// ApiWrap::sendBotStart() sends the literal "/start" (or "/start@botname")
// that opens a bot, with no tags and nothing else in the message. That text
// is protected as a bot-command entity below, but the bot name after the "@"
// is not always parsed as a mention, so a rule that happens to match it would
// still land. Nothing is lost by refusing to rewrite a message that is one
// bare command and nothing else.
[[nodiscard]] bool IsBareBotCommand(const QString &text) {
	static const auto expression = QRegularExpression(
		u"^/[A-Za-z0-9_]{1,64}(@[A-Za-z0-9_]{1,32})?$"_q);
	return expression.match(text.trimmed()).hasMatch();
}

[[nodiscard]] QJsonArray ReadRulesArray() {
	auto &settings = Settings::Instance();
	const auto array = settings.getArray(kKeyRules);
	if (!array.isEmpty()) {
		return array;
	}

	// Android stores this key as a *string* holding a JSON array
	// (LuminaConfig.getTextReplacements()). A backup taken there and imported
	// into a desktop profile therefore arrives as a string, and dropping it
	// would look exactly like "my rules did not come across".
	const auto raw = settings.getString(kKeyRules);
	if (raw.isEmpty()) {
		return QJsonArray();
	}
	const auto document = QJsonDocument::fromJson(raw.toUtf8());
	return document.isArray() ? document.array() : QJsonArray();
}

void WriteRules(const QJsonArray &array) {
	Settings::Instance().set(kKeyRules, array, kRulesStore);
}

} // namespace

QString MakeTextReplaceRuleId() {
	return QString::number(base::RandomValue<uint64>(), 16);
}

bool TextReplaceEnabled() {
	return Settings::Instance().getBool(kKeyEnabled);
}

void SetTextReplaceEnabled(bool value) {
	Settings::Instance().set(kKeyEnabled, value, Store::Prefs);
}

std::vector<TextReplaceRule> TextReplaceRules() {
	const auto array = ReadRulesArray();
	auto result = std::vector<TextReplaceRule>();
	result.reserve(size_t(array.size()));
	for (const auto &entry : array) {
		if (!entry.isObject()) {
			continue;
		}
		const auto object = entry.toObject();
		auto rule = TextReplaceRule{
			.id = object.value(kFieldId).toString(),
			.from = object.value(kFieldFrom).toString(),
			.to = object.value(kFieldTo).toString(),
		};
		if (rule.from.isEmpty()) {
			continue;
		}
		result.push_back(std::move(rule));
		if (int(result.size()) >= kTextReplaceMaxStoredRules) {
			break;
		}
	}
	return result;
}

void SetTextReplaceRules(std::vector<TextReplaceRule> rules) {
	auto array = QJsonArray();
	for (auto &rule : rules) {
		if (rule.from.isEmpty()) {
			continue;
		} else if (array.size() >= kTextReplaceMaxStoredRules) {
			break;
		}
		if (rule.id.isEmpty()) {
			rule.id = MakeTextReplaceRuleId();
		}
		auto object = QJsonObject();
		object.insert(kFieldId, rule.id);
		object.insert(kFieldFrom, rule.from.left(kTextReplaceFromMaxLength));
		object.insert(kFieldTo, rule.to.left(kTextReplaceToMaxLength));
		array.append(object);
	}
	WriteRules(array);
}

rpl::producer<> TextReplaceChanges() {
	return rpl::merge(
		Settings::Instance().changesFor(kKeyEnabled),
		Settings::Instance().changesFor(kKeyRules));
}

QString ApplyTextReplaceRules(const QString &text) {
	if (text.isEmpty() || text.size() > kMaxResultLength) {
		return text;
	}
	const auto rules = TextReplaceRules();
	if (rules.empty()) {
		return text;
	}
	const auto count = std::min(int(rules.size()), kTextReplaceMaxRules);
	auto budget = kMaxReplacements;
	auto result = text;
	auto spans = ProtectedRanges(result);
	for (auto i = 0; i != count; ++i) {
		const auto &rule = rules[i];
		auto replaced = ApplyOneRule(
			result,
			rule.from,
			rule.to,
			spans,
			budget);
		if (!replaced) {
			continue;
		} else if (budget <= 0 || replaced->size() > kMaxResultLength) {
			return text;
		}
		result = std::move(*replaced);
		spans = ProtectedRanges(result);
	}
	return result;
}

void ApplyOutgoingTextReplacements(TextWithTags &textWithTags) {
	// The guard this whole feature is built around. A tag is an absolute
	// {offset, length} into the text; a substitution of a different length
	// moves the characters out from under every tag that follows it, and
	// nothing downstream notices. See the header.
	if (!textWithTags.tags.isEmpty()) {
		return;
	} else if (textWithTags.text.isEmpty() || !TextReplaceEnabled()) {
		return;
	} else if (IsBareBotCommand(textWithTags.text)) {
		return;
	}
	auto replaced = ApplyTextReplaceRules(textWithTags.text);
	if (replaced != textWithTags.text) {
		textWithTags.text = std::move(replaced);
	}
}

} // namespace Lumina
