/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_homoglyph.h"

#include "lumina/lumina_settings.h"

#include <rpl/rpl.h>

#include <QtCore/QChar>

#include <array>

namespace Lumina {
namespace {

const auto kKey = u"homoglyphWarn"_q;

// One Cyrillic or Greek code point that mimics a Latin letter, paired with the
// Latin letter it impersonates. The Latin partner is not consulted by the
// detector - the mix is found by script, below - it documents each row and
// lets the table double as a skeleton map if a later feature wants one. Kept
// close to Android's LuminaHomoglyph table so a name flags the same on both.
//
// Code points, not literal glyphs, so the file stays ASCII and every row can
// be checked digit by digit against the Unicode charts:
//   Cyrillic block  U+0400..U+04FF (plus U+0500.. supplement)
//   Greek block     U+0370..U+03FF
struct Confusable {
	char16_t code = 0;
	char latin = 0;
};

constexpr auto kConfusables = std::array<Confusable, 59>{ {
	// Cyrillic, capital.
	{ 0x0410, 'A' }, // CYRILLIC CAPITAL LETTER A
	{ 0x0412, 'B' }, // VE
	{ 0x0415, 'E' }, // IE
	{ 0x041A, 'K' }, // KA
	{ 0x041C, 'M' }, // EM
	{ 0x041D, 'H' }, // EN
	{ 0x041E, 'O' }, // O
	{ 0x0420, 'P' }, // ER
	{ 0x0421, 'C' }, // ES
	{ 0x0422, 'T' }, // TE
	{ 0x0423, 'Y' }, // U
	{ 0x0425, 'X' }, // HA
	{ 0x0406, 'I' }, // BYELORUSSIAN-UKRAINIAN I
	{ 0x0408, 'J' }, // JE
	{ 0x0405, 'S' }, // DZE
	{ 0x04C0, 'I' }, // PALOCHKA
	{ 0x051A, 'Q' }, // QA
	{ 0x051C, 'W' }, // WE
	// Cyrillic, small.
	{ 0x0430, 'a' }, // a
	{ 0x0432, 'b' }, // ve
	{ 0x0435, 'e' }, // ie
	{ 0x043A, 'k' }, // ka
	{ 0x043C, 'm' }, // em
	{ 0x043E, 'o' }, // o
	{ 0x0440, 'p' }, // er
	{ 0x0441, 'c' }, // es
	{ 0x0442, 't' }, // te
	{ 0x0443, 'y' }, // u
	{ 0x0445, 'x' }, // ha
	{ 0x0456, 'i' }, // byelorussian-ukrainian i
	{ 0x0458, 'j' }, // je
	{ 0x0455, 's' }, // dze
	{ 0x04CF, 'l' }, // palochka, small
	{ 0x051B, 'q' }, // qa, small
	{ 0x051D, 'w' }, // we, small
	// Greek, capital.
	{ 0x0391, 'A' }, // GREEK CAPITAL LETTER ALPHA
	{ 0x0392, 'B' }, // BETA
	{ 0x0395, 'E' }, // EPSILON
	{ 0x0396, 'Z' }, // ZETA
	{ 0x0397, 'H' }, // ETA
	{ 0x0399, 'I' }, // IOTA
	{ 0x039A, 'K' }, // KAPPA
	{ 0x039C, 'M' }, // MU
	{ 0x039D, 'N' }, // NU
	{ 0x039F, 'O' }, // OMICRON
	{ 0x03A1, 'P' }, // RHO
	{ 0x03A4, 'T' }, // TAU
	{ 0x03A5, 'Y' }, // UPSILON
	{ 0x03A7, 'X' }, // CHI
	{ 0x03F9, 'C' }, // LUNATE SIGMA SYMBOL (capital)
	// Greek, small.
	{ 0x03B1, 'a' }, // alpha
	{ 0x03B9, 'i' }, // iota
	{ 0x03BA, 'k' }, // kappa
	{ 0x03BD, 'v' }, // nu
	{ 0x03BF, 'o' }, // omicron
	{ 0x03C1, 'p' }, // rho
	{ 0x03C5, 'u' }, // upsilon
	{ 0x03C7, 'x' }, // chi
	{ 0x03F2, 'c' }, // lunate sigma (small)
} };

enum class LetterScript {
	Other,
	Latin,
	Cyrillic,
	Greek,
};

[[nodiscard]] LetterScript ScriptOf(QChar ch) {
	if (!ch.isLetter()) {
		return LetterScript::Other;
	}
	switch (ch.script()) {
	case QChar::Script_Latin: return LetterScript::Latin;
	case QChar::Script_Cyrillic: return LetterScript::Cyrillic;
	case QChar::Script_Greek: return LetterScript::Greek;
	default: return LetterScript::Other;
	}
}

[[nodiscard]] bool IsConfusable(QChar ch) {
	const auto code = ch.unicode();
	for (const auto &entry : kConfusables) {
		if (entry.code == code) {
			return true;
		}
	}
	return false;
}

// Full-width Latin letters and digits (the "Halfwidth and Fullwidth Forms"
// block): a name written "google" in full-width forms reads as Latin but is
// not.
[[nodiscard]] bool IsFullwidthLatinOrDigit(QChar ch) {
	const auto u = ch.unicode();
	return (u >= 0xFF10 && u <= 0xFF19)  // full-width 0..9
		|| (u >= 0xFF21 && u <= 0xFF3A)  // full-width A..Z
		|| (u >= 0xFF41 && u <= 0xFF5A); // full-width a..z
}

// A 0 or 1 standing in for a letter, i.e. wedged between letters on BOTH sides
// ("g00gle"). A trailing digit is intentionally NOT caught ("user1" is
// ordinary). A run of 0/1 is treated as one unit, so the double zero in
// "g00gle" still counts even though neither zero itself touches a letter.
[[nodiscard]] bool DigitAsLetter(
		const QString &text,
		qsizetype from,
		qsizetype till) {
	const auto isMark = [](QChar ch) {
		return (ch == QChar(u'0')) || (ch == QChar(u'1'));
	};
	auto i = from;
	while (i != till) {
		if (!isMark(text.at(i))) {
			++i;
			continue;
		}
		auto runEnd = i;
		while (runEnd != till && isMark(text.at(runEnd))) {
			++runEnd;
		}
		const auto before = (i > from) && text.at(i - 1).isLetter();
		const auto after = (runEnd != till) && text.at(runEnd).isLetter();
		if (before && after) {
			return true;
		}
		i = runEnd;
	}
	return false;
}

// One whitespace-delimited word of the name. The mixed-script test is per word
// on purpose: a Cyrillic given name beside a Latin surname is two single-script
// words and must NOT flag, while one word carrying both scripts must.
[[nodiscard]] bool TokenSuspicious(
		const QString &text,
		qsizetype from,
		qsizetype till) {
	auto hasLatin = false;
	auto hasCyrillic = false;
	auto hasGreek = false;
	auto hasAsciiLatin = false;
	auto hasConfusable = false;
	for (auto i = from; i != till; ++i) {
		const auto ch = text.at(i);
		if (IsFullwidthLatinOrDigit(ch)) {
			return true;
		}
		if (IsConfusable(ch)) {
			hasConfusable = true;
		}
		if (ch.unicode() < 0x80 && ch.isLetter()) {
			hasAsciiLatin = true;
		}
		switch (ScriptOf(ch)) {
		case LetterScript::Latin: hasLatin = true; break;
		case LetterScript::Cyrillic: hasCyrillic = true; break;
		case LetterScript::Greek: hasGreek = true; break;
		case LetterScript::Other: break;
		}
	}
	// Two or more of Latin / Cyrillic / Greek inside one word - no natural
	// language mixes any two of them, so this is the primary signal.
	const auto distinct = (hasLatin ? 1 : 0)
		+ (hasCyrillic ? 1 : 0)
		+ (hasGreek ? 1 : 0);
	if (distinct >= 2) {
		return true;
	}
	// A known look-alike next to a real ASCII letter, kept as a backstop for
	// the rare confusable a build's QChar::script() might not place in the
	// block the test above looks at. It can never fire on a wholly non-Latin
	// word, because such a word has no ASCII letter.
	if (hasConfusable && hasAsciiLatin) {
		return true;
	}
	return DigitAsLetter(text, from, till);
}

} // namespace

bool containsSuspicious(const QString &name) {
	const auto size = name.size();
	auto i = qsizetype(0);
	while (i != size) {
		if (name.at(i).isSpace()) {
			++i;
			continue;
		}
		auto tokenEnd = i;
		while (tokenEnd != size && !name.at(tokenEnd).isSpace()) {
			++tokenEnd;
		}
		if (TokenSuspicious(name, i, tokenEnd)) {
			return true;
		}
		i = tokenEnd;
	}
	return false;
}

bool HomoglyphWarnEnabled() {
	return Settings::Instance().getBool(kKey, true);
}

rpl::producer<bool> HomoglyphWarnEnabledValue() {
	return rpl::single(
		rpl::empty
	) | rpl::then(
		Settings::Instance().changesFor(kKey)
	) | rpl::map([] {
		return HomoglyphWarnEnabled();
	});
}

rpl::producer<> HomoglyphWarnChanges() {
	return Settings::Instance().changesFor(kKey);
}

void SetHomoglyphWarnEnabled(bool value) {
	Settings::Instance().set(kKey, value, Store::Prefs);
}

} // namespace Lumina
