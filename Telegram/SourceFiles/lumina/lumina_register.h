/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include <QtCore/QString>

#include <vector>

class History;

namespace Lumina {

// LuminaGram: per-chat REGISTER - the relationship a chat stands in, and
// therefore the tone a translation of it has to carry. Android's
// LuminaRegister.java, same keys, same stored values, same prompts.
//
// Across languages the thing that breaks first is not the meaning, it is the
// politeness level. "Can you send that over?" is one sentence in English and
// three different sentences in Japanese depending on whether the reader is a
// client, a colleague or a friend; a translator that does not know which one is
// talking is guessing every time. So each dialog may carry one register, chosen
// once from the chat's translate menu, and every translation of that dialog -
// incoming and outgoing - is asked to speak in it.
//
// What each engine can actually honour:
//
//  * LLM (OpenAI-compatible: GPT / Gemini / DeepSeek / self-hosted) - the
//    register is APPENDED to the user's own system prompt as an extra
//    paragraph, so the global prompt is layered under it rather than replaced.
//  * DeepL - no free-form instruction, but a native `formality` parameter. The
//    six presets collapse onto its formal/informal axis, and only for the
//    target languages DeepL documents as supporting it; everywhere else the
//    parameter is simply omitted, because sending it for an unsupported target
//    is an error response, i.e. a failed translation.
//  * Google (free web) / Telegram - no channel for tone at all. Nothing is
//    sent, and the menu row says so in grey rather than pretending the setting
//    took effect.
//
// The Telegram fallback (lumina_translate_providers.h) is the one seam where a
// register can silently go missing: an LLM request that fails is retried
// through Telegram's engine, which has nowhere to put the tone. That is the
// same trade the fallback already makes for the user's whole prompt, and a
// translation without a tone instruction is still a translation.
//
// Nothing here may throw or assert. A register that cannot be read is a
// register that was never set, and the translation then goes out exactly as it
// does today - that is the fail-safe the whole feature hangs off.

// The stored values. They are persisted and they are shared with Android, so
// they must never change.
[[nodiscard]] QString RegisterNone();      // "" - no register chosen
[[nodiscard]] QString RegisterClient();    // "client"
[[nodiscard]] QString RegisterColleague(); // "colleague"
[[nodiscard]] QString RegisterFriend();    // "friend"
[[nodiscard]] QString RegisterFamily();    // "family"
[[nodiscard]] QString RegisterElder();     // "elder"
[[nodiscard]] QString RegisterRomance();   // "romance"

// Not a stored value on its own: a user-written description is stored as
// "custom:<their sentence>", and this is only the picker entry that leads to
// the box where they write it.
[[nodiscard]] QString RegisterCustom();    // "custom"

// The six presets, in the order the picker lists them.
[[nodiscard]] const std::vector<QString> &RegisterCodes();

// A free-form description is a prompt fragment the user writes themselves,
// against their own API key, so the only real limit is that it stays a
// description: one line, bounded length.
inline constexpr auto kRegisterCustomMaxLength = 200;

// Which chat a register belongs to.
//
// The account is part of the identity, not decoration: two logged-in accounts
// must not share a per-chat setting silently, and a peer id alone is only
// NEARLY unique - which is exactly the kind of "nearly" that turns into a
// cross-account bug report later. Same shape as the per-dialog send-language
// lock in lumina_translate_send.h, and keyed the same way on disk.
//
// An invalid (default-constructed) value means "the caller does not know which
// chat this is", which every reader below treats as "no register".
struct RegisterDialog {
	uint64 sessionId = 0;
	uint64 peerId = 0;

	[[nodiscard]] bool valid() const {
		return (sessionId != 0) && (peerId != 0);
	}
};

[[nodiscard]] RegisterDialog RegisterDialogFor(not_null<History*> history);

// The chat that is open and focused right now, or an invalid value when the
// active window is not showing one.
//
// This is the fallback for the translation paths whose call sites cannot name
// a chat (the send pipeline, the selection translator, the composer preview
// bar). It is Android's ambientDialogId() and it carries Android's caveat: it
// is right because every one of those paths translates the chat you are
// looking at, not because the pipeline knows. Where the chat IS known - the
// per-message and whole-chat translation that runs through
// Ui::TranslateProvider, which is handed a peer id - it is passed explicitly
// instead and this is never consulted. See TranslateEngine::setRegisterDialog.
[[nodiscard]] RegisterDialog ActiveRegisterDialog();

// `hint` when the caller named a chat, the active one when it did not.
[[nodiscard]] RegisterDialog ResolveRegisterDialog(RegisterDialog hint);

// The stored register for a dialog: a preset code, "custom:...", or empty.
//
// Key `trRegisterDialog`, an object of { "<sessionUniqueId>_<peerId>": code },
// in Store::Private - it is a list of who you talk to and what they are to
// you, which is a more personal thing than most of what this fork stores.
//
// !! SetDialogRegister() is that key's ONLY writer. Lumina::Settings::set()
// defaults to Store::Prefs, and a bare set() on this key would relocate the
// whole map into the plaintext pref file.
[[nodiscard]] QString DialogRegister(RegisterDialog dialog);
[[nodiscard]] QString DialogRegister(not_null<History*> history);

// An empty value clears the chat back to "not set".
void SetDialogRegister(RegisterDialog dialog, const QString &value);
void SetDialogRegister(not_null<History*> history, const QString &value);

// The stored form of a user-written description, or an empty string when it
// says nothing. An empty description is not a register: it clears the chat
// rather than storing a "custom" with no content.
[[nodiscard]] QString CustomRegisterValue(const QString &description);

[[nodiscard]] bool RegisterIsCustom(const QString &stored);

// The user's own words out of a "custom:..." value; empty for anything else.
[[nodiscard]] QString RegisterCustomText(const QString &stored);

// Localized name of a stored value; a "custom:..." one shows the user's own
// sentence, and an unset one the "Not set" string.
[[nodiscard]] QString RegisterDisplayName(const QString &stored);

// The lumina_locale key for a code's name and for its one-line explanation, or
// the "not set" keys for an unknown/unset code. Split out so that the picker
// can list the entries without a second table of its own.
[[nodiscard]] QString RegisterNameKey(const QString &code);
[[nodiscard]] QString RegisterInfoKey(const QString &code);

// Full free-form tone control: only the LLM provider has a channel for it.
[[nodiscard]] bool RegisterEngineSupportsPrompt();

// DeepL: a real but narrow knob - formal vs informal, on the languages it
// supports.
[[nodiscard]] bool RegisterEngineIsDeepL();

// True when the selected engine can carry no tone information whatsoever,
// which is what the grey caveat under the menu row says out loud.
[[nodiscard]] bool RegisterEngineIgnoresRegister();

// The paragraph to append to the LLM system prompt for this dialog, or an
// empty string when no register applies. Appended, never substituted: the
// user's global system prompt keeps saying whatever it says, and this only
// adds how it should sound.
[[nodiscard]] QString RegisterPromptSuffix(RegisterDialog dialog);

// DeepL's `formality` value for this dialog and this target language, or an
// empty string when it does not apply - no register, a register DeepL's one
// axis cannot express (a free-form description has no honest projection onto
// formal/informal, and guessing one would be inventing a setting the user did
// not make), or a target language DeepL does not support formality for.
[[nodiscard]] QString RegisterDeepLFormality(
	RegisterDialog dialog,
	const QString &toCode);

} // namespace Lumina
