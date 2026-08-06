/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include <rpl/producer.h>

#include <QtCore/QString>

#include <vector>

struct TextWithTags;

namespace Lumina {

// LuminaGram text replacer / outgoing auto-substitution.
//
// A list of user-defined {from, to} rules applied to the plain text of every
// outgoing message, right before ApiWrap::sendMessage() splits it. Android:
// LuminaConfig.applyTextReplacements(), called from
// SendMessagesHelper.sendMessage().
//
// Nothing here leaves the device: the rules are a local pref, and the only
// thing the server ever sees is the message the user would have sent anyway,
// with the substitutions already made.
//
// THE ONE THING THAT MUST NEVER CHANGE
// ------------------------------------
// TextWithTags::Tag carries an absolute {offset, length} pair into
// TextWithTags::text (ui/text/text_entity.h:323-334). Every tag - bold,
// italic, spoiler, mention-name, custom emoji - is an index range, not an
// anchor that follows the characters it covers. A substitution whose
// replacement differs in length from what it replaced silently slides every
// tag after it onto the wrong characters, and there is no assert, no
// exception and no wire error: the message simply arrives with the bold in
// the wrong place, a mention pointing at the wrong word, or a custom emoji
// sitting on a letter. ApplyOutgoingTextReplacements() therefore refuses to
// touch a message whose `tags` vector is non-empty, and that guard comes
// before every other consideration in this feature.
//
// The same reasoning, one level down, is why link-like spans are protected
// inside a plain-text message: see ApplyTextReplaceRules().

struct TextReplaceRule {
	QString id;
	QString from;
	QString to;
};

// Identity is the id, never the text: a user who edits "brb" into "brb!" is
// editing a rule, not creating one, and two rules may legitimately share a
// `from` after a careless edit. Ids are generated on write; a rule restored
// from a backup written before ids existed comes back with an empty id and is
// given one by the next SetTextReplaceRules().
[[nodiscard]] QString MakeTextReplaceRuleId();

// Master switch, default off. With it off this feature costs one bool lookup
// per outgoing message and changes nothing, which is what behaviour
// neutrality means here - an empty rule list would also be a no-op, but the
// switch lets a user keep a list of rules and stop applying them.
[[nodiscard]] bool TextReplaceEnabled();
void SetTextReplaceEnabled(bool value);

// Rules in the order they are applied, which is the order they are shown in.
// Ids may be empty on the way out (a pre-id backup); they are never empty on
// the way in.
[[nodiscard]] std::vector<TextReplaceRule> TextReplaceRules();

// Normalises before writing: drops rules with an empty `from`, fills in
// missing ids and clamps `from` / `to` to the maximum lengths.
//
// It deliberately does NOT drop rules past kTextReplaceMaxRules. Every
// operation in the rules box is a read-modify-write of this whole vector, so
// a cap enforced here would silently delete, on the very first click after a
// cross-import, every rule an Android backup carried past the hundredth -
// a deletion the user never asked for and cannot undo. The cap that actually
// protects the send path is on how many rules one message is put through,
// and it lives in ApplyTextReplaceRules(). The only bound here is
// kTextReplaceMaxStoredRules, so that a corrupt or hostile backup file cannot
// make every outgoing message walk an unbounded list.
void SetTextReplaceRules(std::vector<TextReplaceRule> rules);

[[nodiscard]] rpl::producer<> TextReplaceChanges();

// Where the "Add rule" row stops, and how many rules one message is ever put
// through. Nothing in the UI can grow a list past this; only an import can.
inline constexpr auto kTextReplaceMaxRules = 100;

// What survives a load / save round trip, so that an over-sized import is
// preserved rather than truncated behind the user's back.
inline constexpr auto kTextReplaceMaxStoredRules = 1000;

inline constexpr auto kTextReplaceFromMaxLength = 256;
inline constexpr auto kTextReplaceToMaxLength = 1024;

// The pure transform, without the enabled check and without the tag guard.
// Exposed for callers that already know they are holding plain text.
//
// Rules run in order, each over the output of the previous one, exactly as
// Android does, and at most kTextReplaceMaxRules of them are applied to any
// one message. Matching is case-sensitive.
//
// This runs on the main thread inside the send path, so it is bounded rather
// than merely correct: a text that is already longer than any message can be
// is returned untouched, and if the rules together need more substitutions
// than one message plausibly can, the whole thing is abandoned and the
// original text is returned. Abandoning always means "send exactly what the
// user typed", never a half-substituted message.
//
// Two deliberate departures from Android's regex, both of which are bugs
// there rather than behaviour worth porting:
//
//  * Android builds "\\b" + Pattern.quote(from) + "\\b". Java's \b is the
//    ASCII \w boundary unless UNICODE_CHARACTER_CLASS is set, and it is not,
//    so for a `from` that starts and ends with non-ASCII - every CJK rule, in
//    a fork whose own interface language is Chinese - no boundary can ever
//    exist and the rule never fires. Here a word boundary is required only on
//    the side where `from`'s own edge character is an ASCII word character,
//    so "brb" stays whole-word and a CJK rule matches as a substring.
//  * Nothing on Android stops a rule from rewriting the inside of a URL, an
//    @mention, a #hashtag or a /command. It can, and a one-letter rule
//    quietly corrupts an invite link - ApiWrap::sendMessage() is also the
//    path taken by "send this invite link" (boxes/peers/add_participants_box
//    .cpp:718) and by the share-box comment, both of which arrive here as
//    plain text with no tags. Spans that TextUtilities::ParseEntities() marks
//    as links, mentions, hashtags, cashtags or bot commands are skipped.
[[nodiscard]] QString ApplyTextReplaceRules(const QString &text);

// The send-path entry point. Mutates `textWithTags` in place, or leaves it
// exactly as it was. Called from ApiWrap::sendMessage() before the text is
// converted to entities and split.
//
// Refuses, in order: a message with any formatting tag, an empty message, a
// message sent while the feature is off, and a message that is one bare
// "/command" - ApiWrap::sendBotStart() reaches sendMessage() with exactly
// that and nothing else, and it is machine-written text, not something the
// user typed.
void ApplyOutgoingTextReplacements(TextWithTags &textWithTags);

} // namespace Lumina
