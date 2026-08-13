/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_chat_language_menu.h"

#include "base/weak_ptr.h"
#include "history/history.h"
#include "lang/lang_keys.h"
#include "lumina/lumina_locale.h"
#include "lumina/lumina_register.h"
#include "lumina/lumina_translate_send.h"
#include "lumina/lumina_translate_settings.h"
#include "lumina/lumina_translate_toggle.h"
#include "ui/boxes/choose_language_box.h"
#include "ui/layers/generic_box.h"
#include "ui/text/text_entity.h"
#include "ui/vertical_list.h"
#include "ui/widgets/checkbox.h"
#include "ui/widgets/fields/input_field.h"
#include "ui/widgets/labels.h"
#include "ui/widgets/menu/menu_multiline_action.h"
#include "ui/widgets/popup_menu.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"
#include "styles/style_layers.h"
#include "styles/style_menu_icons.h"
#include "styles/style_widgets.h"

#include <vector>

namespace Lumina {
namespace {

// The code the "not translated" entry stores. It has to be one no language
// owns, and it has to differ from the empty string, which already means
// "follow the interface language" everywhere the read language is read.
[[nodiscard]] QString OffCode() {
	return u"off"_q;
}

// Rows wide enough to say "Them, translated into Traditional Chinese" without
// eliding: Ui::Menu::Action cuts a row off at the menu's width, and the whole
// point of these two rows is the language at the end of them.
void AddRow(
		not_null<Ui::PopupMenu*> menu,
		const QString &text,
		const style::icon *icon,
		Fn<void()> callback) {
	const auto &stMenu = menu->st().menu;
	auto item = base::make_unique_q<Ui::Menu::MultilineAction>(
		menu->menu(),
		stMenu,
		st::defaultFlatLabel,
		QPoint(stMenu.itemPadding.left(), stMenu.itemPadding.top()),
		TextWithEntities{ text },
		icon);
	item->setActionTriggered(std::move(callback));
	menu->addAction(std::move(item));
}

// A row that only explains something, in the grey the rest of the app uses for
// explanations. Transparent to the mouse, because it is a caveat and not a
// choice - the same thing tdesktop's own explanatory menu rows do
// (HistoryView::AddEphemeralAboutAction).
void AddAbout(not_null<Ui::PopupMenu*> menu, const QString &text) {
	const auto &stMenu = menu->st().menu;
	auto item = base::make_unique_q<Ui::Menu::MultilineAction>(
		menu->menu(),
		stMenu,
		st::boxDividerLabel,
		QPoint(stMenu.itemPadding.left(), stMenu.itemPadding.top()),
		TextWithEntities{ text });
	item->setAttribute(Qt::WA_TransparentForMouseEvents);
	menu->addAction(std::move(item));
}

// What the incoming half reads, as a whole row.
//
// Off is its own string rather than the word "off" substituted into the "into
// {1}" one, which would say "translated into not translated".
//
// The stored language is empty until it has been chosen at least once, and the
// row still has to name one: empty means the interface language, and that is
// what the messages would actually arrive in.
[[nodiscard]] QString IncomingRow(not_null<History*> history) {
	if (ChatTranslationExcluded(history) || !ChatTranslating(history)) {
		return Tr(u"LuminaChatLangThemOff"_q);
	}
	const auto stored = TranslateReadLanguage();
	return Tr(u"LuminaChatLangThem"_q, stored.isEmpty()
		? Ui::LanguageName(ChatTranslateDefaultTo(history))
		: TranslateLanguageName(stored));
}

// The same for the outgoing half. "Recipient's language" is an answer rather
// than a missing one - it means the send pipeline asks each chat once - so the
// row names it instead of naming whichever language was last used.
[[nodiscard]] QString OutgoingRow(not_null<History*> history) {
	// Off unless the global capability is on AND this chat is switched on (the
	// per-chat switch, default off). The global switch alone no longer
	// translates anything - the send-button menu and Android read it the same.
	if (!TranslateBeforeSend() || !DialogSendTranslateOn(history)) {
		return Tr(u"LuminaChatLangMeOff"_q);
	}
	// The language this chat outgoing messages actually go out in, resolved the
	// way the send path does: the chat lock wins, then the global language;
	// empty means neither is settled and the send flow asks once.
	const auto chat = DialogSendLanguage(history);
	const auto code = !chat.isEmpty()
		? chat
		: (TranslateSendLanguageIsAuto() ? QString() : TranslateSendLanguage());
	return Tr(u"LuminaChatLangMe"_q, code.isEmpty()
		? Tr(u"LuminaTranslateSendLangAuto"_q)
		: TranslateLanguageName(code));
}

void FillIncoming(
		not_null<Ui::PopupMenu*> menu,
		not_null<Window::SessionController*> controller,
		not_null<History*> history) {
	const auto weak = base::make_weak(history);
	AddRow(
		menu,
		IncomingRow(history),
		&st::menuIconDownload,
		[=] {
			// Read through the weak pointer rather than the captured History:
			// the row outlives the menu it was built in, and the chat behind
			// it can be gone by the time it is pressed.
			const auto opened = weak.get();
			if (!opened) {
				return;
			}
			ShowLanguagePicker(
				controller,
				Tr(u"LuminaChatLangThemTitle"_q),
				Tr(u"LuminaChatLangNone"_q),
				OffCode(),
				((ChatTranslating(opened)
					&& !ChatTranslationExcluded(opened))
					? TranslateReadLanguage()
					: OffCode()),
				[=](QString code) {
					const auto strong = weak.get();
					if (!strong) {
						return;
					} else if (code == OffCode()) {
						// Excluding rather than merely stopping. Stopping
						// lasts until the next message this chat is
						// recognised from, and the offer comes straight
						// back - which reads as the choice not having
						// been taken.
						SetChatTranslationExcluded(strong, true);
						return;
					}
					// Order matters: the language is what this chat is about
					// to be translated into, so it has to be stored before
					// the translation starts reading it. Naming a language
					// also takes the chat back off the excluded list, which
					// SetChatTranslatingTo() does on the way through.
					SetTranslateReadLanguage(code);
					SetChatTranslating(strong, true);
				});
		});
}

void FillOutgoing(
		not_null<Ui::PopupMenu*> menu,
		not_null<Window::SessionController*> controller,
		not_null<History*> history) {
	const auto weak = base::make_weak(history);
	AddRow(
		menu,
		OutgoingRow(history),
		&st::menuIconSend,
		[=] {
			// Read through the weak pointer, as the incoming row does: the
			// row outlives the menu it was built in.
			const auto opened = weak.get();
			if (!opened) {
				return;
			}
			ShowLanguagePicker(
				controller,
				Tr(u"LuminaChatLangMeTitle"_q),
				Tr(u"LuminaChatLangNone"_q),
				OffCode(),
				((TranslateBeforeSend() && DialogSendTranslateOn(opened))
					? DialogSendLanguage(opened)
					: OffCode()),
				[=](QString code) {
					const auto strong = weak.get();
					if (!strong) {
						return;
					} else if (code == OffCode()) {
						// This chat only; the global capability and every
						// other chat are left untouched.
						SetDialogSendTranslateOn(strong, false);
						return;
					}
					// The per-chat send language, which the send path prefers
					// over the global one, so it governs this chat alone and
					// touches no other.
					SetDialogSendLanguage(strong, code);
					SetDialogSendTranslateOn(strong, true);
					// Turn the global capability on if it was off, so the
					// per-chat switch it gates takes effect.
					if (!TranslateBeforeSend()) {
						SetTranslateBeforeSend(true);
					}
				});
		});
}

// ---- register ------------------------------------------------------------

// The third row names the relationship this chat is in, because that is what
// decides how both halves above should sound.
//
// Unset is its own string for the same reason the two language rows have one:
// "Tone: not set" is a state, and substituting a word for "none" into
// "Tone: {1}" would read like a choice had been made.
[[nodiscard]] QString RegisterRow(not_null<History*> history) {
	const auto stored = DialogRegister(history);
	return stored.isEmpty()
		? Tr(u"LuminaChatRegisterOff"_q)
		: Tr(u"LuminaChatRegister"_q, RegisterDisplayName(stored));
}

// What the selected engine can actually do with a register, said plainly, or
// nothing when there is nothing to warn about (no register set, or an engine
// that honours it fully).
//
// A register the user set and the engine silently drops is worse than no
// register at all, because it reads as having worked.
[[nodiscard]] QString RegisterCaveat(not_null<History*> history) {
	if (DialogRegister(history).isEmpty()) {
		return QString();
	} else if (RegisterEngineIgnoresRegister()) {
		return Tr(u"LuminaChatRegisterUnsupported"_q);
	} else if (RegisterEngineIsDeepL()) {
		return Tr(u"LuminaChatRegisterDeepL"_q);
	}
	return QString();
}

// "Not set" first, then the six presets, then "Custom…".
[[nodiscard]] std::vector<QString> RegisterOptions() {
	auto result = std::vector<QString>();
	result.reserve(RegisterCodes().size() + 2);
	result.push_back(RegisterNone());
	for (const auto &code : RegisterCodes()) {
		result.push_back(code);
	}
	result.push_back(RegisterCustom());
	return result;
}

void ShowCustomRegisterBox(
	not_null<Window::SessionController*> controller,
	base::weak_ptr<History> weak);

// "Custom" is the escape hatch for every relationship the six presets do not
// name - a thesis advisor, a landlord, an ex. The sentence the user writes is
// handed to the model as-is.
void CustomRegisterBox(
		not_null<Ui::GenericBox*> box,
		base::weak_ptr<History> weak) {
	const auto opened = weak.get();
	if (!opened) {
		box->closeBox();
		return;
	}
	box->setTitle(TrValue(u"LuminaChatRegisterCustomTitle"_q));

	const auto field = box->addRow(object_ptr<Ui::InputField>(
		box,
		st::defaultInputField,
		Ui::InputField::Mode::SingleLine,
		TrValue(u"LuminaChatRegisterCustomHint"_q),
		RegisterCustomText(DialogRegister(opened))));
	// Safe to cap the field rather than the stored value: what is already
	// stored went through the same cap on the way in, so this can never chop
	// a description the user cannot get back.
	field->setMaxLength(kRegisterCustomMaxLength);
	box->setFocusCallback([=] {
		field->setFocusFast();
	});

	const auto save = [=] {
		if (const auto strong = weak.get()) {
			// An empty description is not a register: CustomRegisterValue()
			// answers with the empty value, which clears the chat back to
			// "not set" rather than storing a custom that says nothing.
			SetDialogRegister(
				strong,
				CustomRegisterValue(field->getLastText()));
		}
		box->closeBox();
	};
	field->submits() | rpl::on_next(save, field->lifetime());

	Ui::AddSkip(box->verticalLayout());
	Ui::AddDividerText(
		box->verticalLayout(),
		TrValue(u"LuminaChatRegisterCustomInfo"_q));

	box->addButton(tr::lng_settings_save(), save);
	box->addButton(tr::lng_cancel(), [=] {
		box->closeBox();
	});
}

// The register list. Each entry carries a sentence of its own, because "Client"
// and "Colleague" differ only in what they do to the translation - the name
// alone does not tell anyone which one they want.
void RegisterPickerBox(
		not_null<Ui::GenericBox*> box,
		not_null<Window::SessionController*> controller,
		base::weak_ptr<History> weak) {
	const auto opened = weak.get();
	if (!opened) {
		box->closeBox();
		return;
	}
	const auto current = DialogRegister(opened);
	const auto codes = RegisterOptions();

	// A stored "custom:…" selects the "Custom…" entry, which is the only one
	// that can stand for it: the value itself is the user's own sentence and
	// is never one of the listed codes.
	auto selected = 0;
	for (auto i = 0, count = int(codes.size()); i != count; ++i) {
		const auto matches = RegisterIsCustom(current)
			? (codes[i] == RegisterCustom())
			: (codes[i] == current);
		if (matches) {
			selected = i;
			break;
		}
	}

	box->setTitle(TrValue(u"LuminaChatRegisterTitle"_q));
	box->addButton(tr::lng_cancel(), [=] {
		box->closeBox();
	});

	const auto group = std::make_shared<Ui::RadiobuttonGroup>(selected);
	const auto layout = box->verticalLayout();
	const auto left = st::boxPadding.left() + st::boxOptionListPadding.left();
	// The explanation lines up with the option's own text rather than with its
	// radio, so the two read as one entry.
	const auto indent = left
		+ st::defaultBoxCheckbox.checkPosition.x()
		+ st::defaultRadio.diameter
		+ st::defaultBoxCheckbox.textPosition.x();
	Ui::AddSkip(layout);
	for (auto i = 0, count = int(codes.size()); i != count; ++i) {
		const auto code = codes[i];
		const auto row = layout->add(
			object_ptr<Ui::Radiobutton>(
				layout,
				group,
				i,
				Tr(RegisterNameKey(code)),
				st::defaultBoxCheckbox),
			QMargins(left, 0, st::boxPadding.right(), 0));
		layout->add(
			object_ptr<Ui::FlatLabel>(
				layout,
				Tr(RegisterInfoKey(code)),
				st::boxDividerLabel),
			QMargins(
				indent,
				0,
				st::boxPadding.right(),
				st::boxOptionListSkip));
		if (code == RegisterCustom()) {
			// Every other entry is answered by the group's change callback,
			// which is silent when the value did not change - and for this one
			// it has to not be: a chat that already carries a description
			// opens the picker with this row already selected, and pressing it
			// is how the description gets edited.
			row->clicks() | rpl::on_next([=] {
				ShowCustomRegisterBox(controller, weak);
			}, row->lifetime());
		}
	}
	group->setChangedCallback([=](int value) {
		if (value < 0 || value >= int(codes.size())) {
			return;
		}
		const auto code = codes[value];
		if (code == RegisterCustom()) {
			// Handled by the row's own click above, so that selecting it and
			// re-pressing it both open the description box exactly once.
			return;
		} else if (const auto strong = weak.get()) {
			SetDialogRegister(strong, code);
		}
		box->closeBox();
	});
}

void ShowCustomRegisterBox(
		not_null<Window::SessionController*> controller,
		base::weak_ptr<History> weak) {
	// Shown over the picker rather than after closing it: cancelling the
	// description must not leave the user with no box and a register they
	// never chose.
	controller->show(Box(CustomRegisterBox, weak));
}

void FillRegister(
		not_null<Ui::PopupMenu*> menu,
		not_null<Window::SessionController*> controller,
		not_null<History*> history) {
	const auto weak = base::make_weak(history);
	AddRow(
		menu,
		RegisterRow(history),
		&st::menuIconCustomize,
		[=] {
			// Read through the weak pointer rather than the captured History,
			// exactly as the two rows above do: the row outlives the menu it
			// was built in.
			const auto opened = weak.get();
			if (!opened) {
				return;
			}
			controller->show(Box(RegisterPickerBox, controller, weak));
		});
	const auto caveat = RegisterCaveat(history);
	if (!caveat.isEmpty()) {
		AddAbout(menu, caveat);
	}
}

} // namespace

bool ChatLanguageMenuAvailable(not_null<History*> history) {
	return ChatTranslateAvailable(history);
}

void FillChatLanguageMenu(
		not_null<Ui::PopupMenu*> menu,
		not_null<Window::SessionController*> controller,
		not_null<History*> history) {
	if (!ChatLanguageMenuAvailable(history)) {
		return;
	}
	FillIncoming(menu, controller, history);
	FillOutgoing(menu, controller, history);
	FillRegister(menu, controller, history);
}

} // namespace Lumina
