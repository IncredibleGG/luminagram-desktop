/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "base/flat_map.h"

#include <rpl/producer.h>

#include <QtCore/QString>

namespace Lumina {

// LuminaGram's own localisation layer.
//
// Telegram Desktop's tr::lng_* strings are served by Telegram's CLOUD language
// pack: lang.strings only provides the English originals and the generated key
// ids, and every translation of them is downloaded from the server. A key this
// fork invents is therefore untranslatable through that system - the server has
// never heard of it - which is why the LuminaGram pages read English no matter
// which in-app language is selected.
//
// So our strings live in an in-code table keyed by the in-app language, which
// is what Android's LuminaLocale.java does for the same reason. The keys are
// the same key names wherever the two platforms show the same thing, so a
// string can be compared across them without reading both UIs.
//
// English is the base table, it lives in lumina_locale.cpp, and it is also the
// fallback: an unknown in-app language, a language whose table is not linked
// into this build, and a key some table happens to be missing all resolve to
// the English text. A LuminaGram string therefore never renders empty and
// never renders as its own key, and with English selected the app reads exactly
// as it did before this layer existed.

// One language's table: our key -> the text in that language.
using LocaleTable = base::flat_map<QString, QString>;

// Builds one language's table. Called at most once per language, the first
// time a string is looked up while that language is selected - never during
// static initialisation.
using LocaleTableFactory = LocaleTable(*)();

// Adds one language to the layer. Returns true (the value is meaningless; it
// exists so that the call can initialise a namespace-scope constant).
//
// `languageCode` must be one of the codes LocaleCode() below resolves to, in
// lowercase, and must stay valid forever - pass a string literal:
//
//   "zh-hans"  Chinese (Simplified)     "tr"     Turkish
//   "zh-hant"  Chinese (Traditional)    "es"     Spanish
//   "ar"       Arabic                   "pt-br"  Portuguese (Brazil)
//   "ru"       Russian                  "id"     Indonesian
//   "fa"       Persian
//
// English needs no registration: it is the base table in lumina_locale.cpp.
// Registering the same code twice keeps the first registration and ignores the
// second.
//
// SAFE TO CALL DURING STATIC INITIALISATION, and that is how it is meant to be
// called: it only appends the pointer pair to a function-local static vector.
// It does NOT read the pointed-to strings, does not build the table, does not
// touch preferences, and does not touch Core::App(). A registrar must not do
// any of those things either - constructing a QObject (a base::Timer, anything
// that reads Lumina::Settings) before QApplication exists crashes the app
// before it can open its log.
//
// A whole language is therefore one self-contained .cpp file that nothing else
// includes and that never needs this file to change:
//
//   /*
//   This file is part of LuminaGram,
//   a fork of Telegram Desktop.
//
//   For license and copyright information please follow this link:
//   https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
//   */
//   #include "lumina/lumina_locale.h"
//
//   namespace Lumina {
//   namespace {
//
//   [[nodiscard]] LocaleTable Build() {
//   	return {
//   		{ u"LuminaGramTitle"_q, u"LuminaGram"_q },
//   		{ u"LuminaTranslateTitle"_q, u"翻譯"_q },
//   		...
//   	};
//   }
//
//   [[maybe_unused]] const auto kRegistered = RegisterLocaleTable(
//   	"zh-hant",
//   	Build);
//
//   } // namespace
//   } // namespace Lumina
//
// A table may carry any subset of the English keys; every key it leaves out
// falls back to English on its own. Keys it carries that English does not are
// simply never asked for. The file must be added to Telegram/CMakeLists.txt by
// whoever owns that file.
bool RegisterLocaleTable(const char *languageCode, LocaleTableFactory factory);

// The LuminaGram string for `key`, in the current in-app language.
//
// Falls back to the English text, so the result is empty only for a key that
// the English table in lumina_locale.cpp does not carry at all - which is a
// typo at the call site, not a missing translation.
[[nodiscard]] QString Tr(const QString &key);

// The same, with every "{1}" in the text replaced by `arg1`. A text may use
// "{1}" more than once (languages disagree about how often a name has to be
// repeated in a sentence), and every occurrence is replaced.
[[nodiscard]] QString Tr(const QString &key, const QString &arg1);

// Tr(key) as a producer that re-emits when the in-app language changes. Use it
// wherever the widget takes an rpl::producer<QString> anyway.
[[nodiscard]] rpl::producer<QString> TrValue(const QString &key);

// Fires when the in-app language changes. For rows whose text is computed
// rather than looked up, merge this into whatever already refreshes them.
[[nodiscard]] rpl::producer<> LangChanges();

// The table code the in-app language resolves to - one of the codes listed on
// RegisterLocaleTable(), or "en" for English and for every language this fork
// does not translate. Never empty.
[[nodiscard]] QString LocaleCode();

} // namespace Lumina
