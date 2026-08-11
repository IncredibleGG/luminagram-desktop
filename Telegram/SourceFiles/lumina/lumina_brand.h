/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "ui/text/text_entity.h"

#include <QtCore/QString>

namespace Lumina {

// LuminaGram's brand layer over Telegram's cloud language pack.
//
// Telegram Desktop's tr::lng_* strings do not come from this repository once
// the app is logged in: lang.strings only carries the English originals and
// the generated key ids, and the values actually shown are downloaded from
// Telegram's language pack server. Editing lang.strings therefore fixes
// nothing a user ever sees - the cloud value wins - which is why a few stock
// strings kept calling this app "Telegram Desktop", and one of them called it
// "the official" one. Android hit the same wall and solved it the same way, in
// LocaleController.getStringInternal(); this is the desktop twin of that fix.
//
// The hook is Lang::details::Current(ushort) in lang/lang_instance.cpp. Every
// tr::lng_* read funnels through it - the immediate tr::now form, the reactive
// producer form (Lang::Value() calls Current()), and each plural shift - so
// one call site covers the whole app. It is deliberately NOT hooked in
// Instance::getValue(): that would also rewrite values on their way to and
// from local storage and to the language-name lookups, and the brand layer has
// no business anywhere except on the way to a screen.
//
// BrandedLangValue() is a pure function: no state, no allocation before it has
// decided to act, no dependency on Lumina::Settings or anything else that
// exists only after QApplication does. Language strings are read from more
// than one thread, and this must stay safe there.
//
// ---------------------------------------------------------------------------
// THE RULE FOR THE LIST BELOW - read this before adding a key.
// ---------------------------------------------------------------------------
//
// 435 stock strings mention Telegram. Almost none of them may be touched. The
// test is not "does it say Telegram", it is WHO IS SPEAKING ABOUT WHAT:
//
//   * The app describing ITSELF - what it is, what it cannot do, what it needs
//     permission for, what the user has to update, quit, unlock or allow in
//     the operating system's own settings. That app is LuminaGram. REWRITE.
//
//       "Launch Telegram when system starts"          -> ours, rewrite
//       "Click it to lock Telegram Desktop"           -> ours, rewrite
//       "Telegram does not have access to screen ..." -> ours, rewrite
//
//   * The NETWORK, the service, or anything Telegram Messenger operates. Our
//     users really are on Telegram; saying otherwise would claim we run a
//     network of our own, which is a lie and an invitation to a trademark
//     problem. LEAVE IT ALONE.
//
//       "Anyone on Telegram will be able to join ..."  -> theirs, leave
//       "Contact joined Telegram"                      -> theirs, leave
//       "All media will stay in the Telegram cloud"    -> theirs, leave
//
//   * Telegram's products and platform - Premium, Stars, Gifts, Business,
//     Passport, Fragment, sponsored messages, ad revenue, official-account
//     badges, the Terms of Service, the API and FAQ links. NEVER. Renaming
//     those would misdescribe someone else's paid product to our users, and it
//     is the exact direction that gets a fork thrown off a platform.
//
// Two further reasons to leave a self-referential string alone, both of which
// cost real keys below:
//
//   * The sentence mixes both voices and a blind word swap would hit the wrong
//     half - lng_terms_delete_warning talks about "your Telegram account" and
//     "the Telegram cloud" in the same breath as "open the latest version of
//     Telegram Desktop".
//   * The sentence hands the user a telegram.org address that we have no
//     replacement for - lng_message_unsupported tells the reader to install
//     the app from a link the caller supplies.
//
// When in doubt, leave it. A missed rebrand is a cosmetic bug; a wrong one is
// a false statement about somebody else's service.
//
// Keys named here must not be plural keys. A plural entry occupies six
// consecutive indices and .base is only the first of them; none of the keys
// below is plural, and a future one would have to enumerate all six.
//
// Returns `value` untouched unless `key` is on the list.
[[nodiscard]] QString BrandedLangValue(ushort key, QString value);

// The two strings the mechanical swap cannot fix, because the problem is not
// the name in them - it is the claim.
//
// "Welcome to the official Telegram Desktop app" and "Official free messaging
// app based on the Telegram API" both survive a Telegram -> LuminaGram swap
// with the word "official" still in them, and we are not the official anything
// - see TRADEMARK.md. They are rewritten whole instead, at the two call sites
// that show them (intro/intro_start.cpp and boxes/about_box.cpp), rather than
// through the langpack hook: a whole-value override would have to reproduce
// the internal tag encoding that Lang::ValueParser puts in place of {api_link},
// and the sentence needs rebuilding anyway.
//
// English only, on purpose. These say what the app legally is, and a wrong
// translation of that is worse than an untranslated one.
[[nodiscard]] QString BrandIntroAbout();

[[nodiscard]] TextWithEntities BrandAboutSummary(TextWithEntities apiLink);

} // namespace Lumina
