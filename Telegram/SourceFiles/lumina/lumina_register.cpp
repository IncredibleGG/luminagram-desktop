/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_register.h"

#include "core/application.h"
#include "data/data_peer.h"
#include "dialogs/dialogs_key.h"
#include "history/history.h"
#include "lumina/lumina_locale.h"
#include "lumina/lumina_settings.h"
#include "lumina/lumina_translate_providers.h"
#include "main/main_session.h"
#include "window/window_controller.h"
#include "window/window_session_controller.h"

#include <algorithm>

namespace Lumina {
namespace {

[[nodiscard]] QString DialogsKey() {
	return u"trRegisterDialog"_q;
}

[[nodiscard]] QString CustomPrefix() {
	return u"custom:"_q;
}

// The account is in the key so that two logged-in accounts cannot share one
// chat's register. Exactly the spelling lumina_translate_send.cpp uses for
// `trSendLangDialog`, so the two per-chat maps line up when read by hand.
[[nodiscard]] QString StorageKey(RegisterDialog dialog) {
	return QString::number(dialog.sessionId)
		+ QChar('_')
		+ QString::number(dialog.peerId);
}

// One line, no quotes: this lands inside a quoted phrase in the system prompt,
// and a newline there would read as the end of the instruction.
[[nodiscard]] QString Sanitize(const QString &description) {
	auto result = QString();
	result.reserve(std::min(
		int(description.size()),
		kRegisterCustomMaxLength));
	for (const auto ch : description) {
		if (int(result.size()) >= kRegisterCustomMaxLength) {
			break;
		} else if (ch == QChar('\n')
			|| ch == QChar('\r')
			|| ch == QChar('\t')) {
			result += QChar(' ');
		} else if (ch == QChar('"') || ch == QChar('\\')) {
			result += QChar('\'');
		} else if (ch.unicode() >= 0x20) {
			result += ch;
		}
	}
	return result.trimmed();
}

// The instruction itself is written in English on purpose: it is read by the
// model, not by the user, and English is the language these models follow
// instructions in most reliably. Each one names the relationship first and then
// spells out what that means in the languages where politeness is grammar
// rather than word choice, because that is exactly where a translation that is
// "correct" still lands wrong.
//
// Kept word for word in step with Android's LuminaRegister.instruction(): a
// register that produces one tone on the phone and another on the desktop is
// worse than one that produces neither.
[[nodiscard]] QString Instruction(const QString &stored) {
	if (stored.isEmpty()) {
		return QString();
	} else if (RegisterIsCustom(stored)) {
		const auto text = RegisterCustomText(stored);
		if (text.isEmpty()) {
			return QString();
		}
		return u"the sender describes this relationship as \""_q
			+ text
			+ u"\". Match the tone, the formality and the vocabulary that "
				"relationship calls for, including the appropriate politeness "
				"level in languages that mark politeness grammatically."_q;
	} else if (stored == RegisterClient()) {
		return u"the other person is a business client or customer. Write the "
			"translation in polite, professional, formal business language. "
			"In languages that mark politeness grammatically, use the formal "
			"or honorific register (Japanese 敬語 with です・ます, Korean "
			"하십시오체, Chinese 您, German Sie, French vous, Spanish usted). "
			"No slang, no over-familiar wording."_q;
	} else if (stored == RegisterColleague()) {
		return u"the other person is a work colleague of roughly equal "
			"standing. Write the translation in ordinary polite workplace "
			"language — courteous but relaxed, not stiff. In languages that "
			"mark politeness grammatically, use the standard polite register "
			"(Japanese です・ます, Korean 해요체, German Sie, French vous). "
			"Everyday workplace shorthand is fine; slang is not."_q;
	} else if (stored == RegisterFriend()) {
		return u"the other person is a close friend. Write the translation in "
			"casual, informal, everyday spoken language, with the "
			"contractions and colloquialisms a friend would actually use. In "
			"languages that mark politeness grammatically, use the plain or "
			"casual register (Japanese 常体 / タメ口, Korean 반말, German du, "
			"French tu, Spanish tú)."_q;
	} else if (stored == RegisterFamily()) {
		return u"the other person is a family member. Write the translation "
			"in warm, familiar, everyday language of the kind used at home. "
			"In languages that mark politeness grammatically, use the plain "
			"or familiar register (Japanese 常体, German du, French tu), and "
			"keep kinship terms natural for the target culture."_q;
	} else if (stored == RegisterElder()) {
		return u"the other person is an elder or a senior the sender owes "
			"respect to. Write the translation in respectful, deferential "
			"language that still sounds warm and personal rather than "
			"corporate. In languages with honorifics, use the honorific "
			"register (Japanese 敬語, Korean 존댓말, Chinese 您) and "
			"respectful forms of address."_q;
	} else if (stored == RegisterRomance()) {
		return u"the other person is someone the sender is romantically "
			"interested in. Write the translation in warm, playful, "
			"affectionate language with a light flirtatious touch — never "
			"crude or explicit. In languages that mark politeness "
			"grammatically, use the soft casual register (Japanese 常体, "
			"Korean 반말 or a gentle 해요체, French tu, Spanish tú)."_q;
	}
	return QString();
}

// The target languages DeepL documents formality support for. Sending the
// parameter for any other target is an error response, i.e. a failed
// translation, so this list is a gate and not an optimisation. The codes are
// spelled as DeepLLanguageCode() spells them.
[[nodiscard]] bool DeepLSupportsFormality(const QString &target) {
	static const auto kLanguages = std::vector<QString>{
		u"DE"_q,
		u"FR"_q,
		u"IT"_q,
		u"ES"_q,
		u"NL"_q,
		u"PL"_q,
		u"PT-BR"_q,
		u"PT-PT"_q,
		u"JA"_q,
		u"RU"_q,
	};
	return (ranges::find(kLanguages, target) != end(kLanguages));
}

} // namespace

QString RegisterNone() {
	return QString();
}

QString RegisterClient() {
	return u"client"_q;
}

QString RegisterColleague() {
	return u"colleague"_q;
}

QString RegisterFriend() {
	return u"friend"_q;
}

QString RegisterFamily() {
	return u"family"_q;
}

QString RegisterElder() {
	return u"elder"_q;
}

QString RegisterRomance() {
	return u"romance"_q;
}

QString RegisterCustom() {
	return u"custom"_q;
}

const std::vector<QString> &RegisterCodes() {
	static const auto result = std::vector<QString>{
		RegisterClient(),
		RegisterColleague(),
		RegisterFriend(),
		RegisterFamily(),
		RegisterElder(),
		RegisterRomance(),
	};
	return result;
}

RegisterDialog RegisterDialogFor(not_null<History*> history) {
	return {
		.sessionId = history->session().uniqueId(),
		.peerId = uint64(history->peer->id.value),
	};
}

RegisterDialog ActiveRegisterDialog() {
	const auto window = Core::App().activeWindow();
	if (!window) {
		return {};
	}
	const auto controller = window->sessionController();
	if (!controller) {
		return {};
	}
	const auto history = controller->activeChatCurrent().history();
	return history ? RegisterDialogFor(history) : RegisterDialog();
}

RegisterDialog ResolveRegisterDialog(RegisterDialog hint) {
	return hint.valid() ? hint : ActiveRegisterDialog();
}

QString DialogRegister(RegisterDialog dialog) {
	if (!dialog.valid()) {
		return RegisterNone();
	}
	return Settings::Instance().getObject(
		DialogsKey()
	).value(StorageKey(dialog)).toString().trimmed();
}

QString DialogRegister(not_null<History*> history) {
	return DialogRegister(RegisterDialogFor(history));
}

void SetDialogRegister(RegisterDialog dialog, const QString &value) {
	if (!dialog.valid()) {
		return;
	}
	const auto key = StorageKey(dialog);
	const auto trimmed = value.trimmed();
	auto object = Settings::Instance().getObject(DialogsKey());
	if (trimmed.isEmpty()) {
		object.remove(key);
	} else {
		object.insert(key, trimmed);
	}
	// An empty map is a key that says nothing, and leaving it behind would
	// carry a "this user once set a register somewhere" fact into every
	// backup for no benefit.
	if (object.isEmpty()) {
		Settings::Instance().remove(DialogsKey());
	} else {
		Settings::Instance().set(DialogsKey(), object, Store::Private);
	}
}

void SetDialogRegister(not_null<History*> history, const QString &value) {
	SetDialogRegister(RegisterDialogFor(history), value);
}

QString CustomRegisterValue(const QString &description) {
	const auto cleaned = Sanitize(description);
	return cleaned.isEmpty() ? RegisterNone() : (CustomPrefix() + cleaned);
}

bool RegisterIsCustom(const QString &stored) {
	return stored.startsWith(CustomPrefix());
}

QString RegisterCustomText(const QString &stored) {
	return RegisterIsCustom(stored)
		? stored.mid(CustomPrefix().size())
		: QString();
}

QString RegisterDisplayName(const QString &stored) {
	if (RegisterIsCustom(stored)) {
		const auto text = RegisterCustomText(stored);
		return text.isEmpty() ? Tr(u"LuminaChatRegisterCustom"_q) : text;
	}
	return Tr(RegisterNameKey(stored));
}

QString RegisterNameKey(const QString &code) {
	if (code == RegisterClient()) {
		return u"LuminaChatRegisterClient"_q;
	} else if (code == RegisterColleague()) {
		return u"LuminaChatRegisterColleague"_q;
	} else if (code == RegisterFriend()) {
		return u"LuminaChatRegisterFriend"_q;
	} else if (code == RegisterFamily()) {
		return u"LuminaChatRegisterFamily"_q;
	} else if (code == RegisterElder()) {
		return u"LuminaChatRegisterElder"_q;
	} else if (code == RegisterRomance()) {
		return u"LuminaChatRegisterRomance"_q;
	} else if (code == RegisterCustom() || RegisterIsCustom(code)) {
		return u"LuminaChatRegisterCustom"_q;
	}
	return u"LuminaChatRegisterNone"_q;
}

QString RegisterInfoKey(const QString &code) {
	if (code == RegisterClient()) {
		return u"LuminaChatRegisterClientInfo"_q;
	} else if (code == RegisterColleague()) {
		return u"LuminaChatRegisterColleagueInfo"_q;
	} else if (code == RegisterFriend()) {
		return u"LuminaChatRegisterFriendInfo"_q;
	} else if (code == RegisterFamily()) {
		return u"LuminaChatRegisterFamilyInfo"_q;
	} else if (code == RegisterElder()) {
		return u"LuminaChatRegisterElderInfo"_q;
	} else if (code == RegisterRomance()) {
		return u"LuminaChatRegisterRomanceInfo"_q;
	} else if (code == RegisterCustom() || RegisterIsCustom(code)) {
		return u"LuminaChatRegisterCustomInfo"_q;
	}
	return u"LuminaChatRegisterNoneInfo"_q;
}

bool RegisterEngineSupportsPrompt() {
	return (CurrentProviderId() == LlmProviderId());
}

bool RegisterEngineIsDeepL() {
	return (CurrentProviderId() == DeepLProviderId());
}

bool RegisterEngineIgnoresRegister() {
	return !RegisterEngineSupportsPrompt() && !RegisterEngineIsDeepL();
}

QString RegisterPromptSuffix(RegisterDialog dialog) {
	const auto body = Instruction(DialogRegister(dialog));
	if (body.isEmpty()) {
		return QString();
	}
	return u"\n\nTone and register for this conversation: "_q
		+ body
		+ u" Adapt only the tone, the politeness level and the choice of "
			"words. Never change the meaning, never add or drop information, "
			"and never mention or explain the tone — output the translation "
			"only."_q;
}

QString RegisterDeepLFormality(
		RegisterDialog dialog,
		const QString &toCode) {
	const auto stored = DialogRegister(dialog);
	const auto value = (stored == RegisterClient()
		|| stored == RegisterColleague()
		|| stored == RegisterElder())
		? u"prefer_more"_q
		: (stored == RegisterFriend()
			|| stored == RegisterFamily()
			|| stored == RegisterRomance())
		? u"prefer_less"_q
		: QString();
	if (value.isEmpty()) {
		return QString();
	}
	// "prefer_" rather than the strict form: if DeepL ever narrows the
	// supported set under us, an unsupported target is then ignored instead
	// of failing the whole request.
	return DeepLSupportsFormality(DeepLLanguageCode(toCode))
		? value
		: QString();
}

} // namespace Lumina
