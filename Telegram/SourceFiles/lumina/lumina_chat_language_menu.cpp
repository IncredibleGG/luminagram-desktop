/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_chat_language_menu.h"

#include "base/weak_ptr.h"
#include "history/history.h"
#include "lumina/lumina_locale.h"
#include "lumina/lumina_translate_send.h"
#include "lumina/lumina_translate_settings.h"
#include "lumina/lumina_translate_toggle.h"
#include "ui/boxes/choose_language_box.h"
#include "ui/layers/generic_box.h"
#include "ui/text/text_entity.h"
#include "ui/widgets/menu/menu_multiline_action.h"
#include "ui/widgets/popup_menu.h"
#include "window/window_session_controller.h"
#include "styles/style_menu_icons.h"
#include "styles/style_widgets.h"

namespace Lumina {
namespace {

// What the outgoing half of the pair actually resolves to for this chat, in the
// same order Lumina::ResolveSendLanguage() resolves it - which is the only way
// the row can be trusted. The three states that are not Off are separate
// because the user has to be able to tell a language that is remembered for
// this chat from the one every chat gets, and neither of those from "the
// pipeline will ask before the first send".
enum class Outgoing {
	Off,
	Global,
	Chat,
	Ask,
};

struct OutgoingState {
	Outgoing kind = Outgoing::Off;
	QString name;
};

[[nodiscard]] OutgoingState ResolveOutgoing(not_null<History*> history) {
	if (!TranslateBeforeSendActive(history)) {
		return { Outgoing::Off, QString() };
	}
	const auto code = DialogSendLanguage(history);
	if (!code.isEmpty()) {
		return { Outgoing::Chat, TranslateLanguageName(code) };
	} else if (!TranslateSendLanguageIsAuto()) {
		return {
			Outgoing::Global,
			TranslateLanguageName(TranslateSendLanguage()),
		};
	}
	return { Outgoing::Ask, QString() };
}

// A row wide enough to say a whole sentence. Ui::Menu::Action elides at the
// menu's width, and every row here is a statement that stops meaning anything
// once its end is cut off, so the rows wrap instead.
//
// A null `callback` makes an inert row: it is painted in the subdued label
// colour so it does not read as something to press, and it keeps the menu open
// when it is pressed anyway. Those are the rows that state a value this menu
// cannot change - the global send language, and "not translated" - where the
// alternative would be a control that looks live and writes a value nothing
// reads.
void AddRow(
		not_null<Ui::PopupMenu*> menu,
		const QString &text,
		const style::icon *icon,
		Fn<void()> callback) {
	const auto &stMenu = menu->st().menu;
	auto item = base::make_unique_q<Ui::Menu::MultilineAction>(
		menu->menu(),
		stMenu,
		callback ? st::defaultFlatLabel : st::defaultSubTextLabel,
		QPoint(stMenu.itemPadding.left(), stMenu.itemPadding.top()),
		TextWithEntities{ text },
		icon);
	if (callback) {
		item->setActionTriggered(std::move(callback));
	} else {
		item->setPreventClose(true);
	}
	menu->addAction(std::move(item));
}

// The incoming picker. Deliberately NOT Ui::ChooseTranslateToBox(): that one
// writes Core::Settings::setTranslateTo() and Lumina::NoteReadLanguageChosen()
// on the way out, so using it here would move every other chat's read language
// as well as this one's - the exact thing this menu promises not to do. The
// same list, the same rows, without the two global writes.
void ShowIncomingPicker(
		not_null<Window::SessionController*> controller,
		not_null<History*> history) {
	const auto weak = base::make_weak(history);
	const auto current = ChatTranslatingTo(history);
	auto selected = std::vector<LanguageId>();
	if (current) {
		selected.push_back(current);
	}
	controller->show(Box(
		Ui::ChooseLanguageBox,
		TrValue(u"LuminaChatLangIncomingPicker"_q),
		[=](const std::vector<LanguageId> &ids) {
			if (ids.empty()) {
				return;
			} else if (const auto strong = weak.get()) {
				SetChatTranslatingTo(strong, ids.front());
			}
		},
		std::move(selected),
		false,
		nullptr));
}

void FillIncoming(
		not_null<Ui::PopupMenu*> menu,
		not_null<Window::SessionController*> controller,
		not_null<History*> history) {
	const auto effective = Ui::LanguageName(ChatTranslatingTo(history));
	const auto choose = [=] { ShowIncomingPicker(controller, history); };
	if (!ChatTranslating(history)) {
		AddRow(
			menu,
			Tr(u"LuminaChatLangTurnOn"_q, effective),
			&st::menuIconTranslate,
			[=] { SetChatTranslating(history, true); });
		AddRow(
			menu,
			Tr(u"LuminaChatLangIncomingOff"_q),
			&st::menuIconDownload,
			choose);
		return;
	}
	AddRow(
		menu,
		Tr(u"LuminaChatLangIncoming"_q, effective),
		&st::menuIconDownload,
		choose);
}

void FillOutgoing(
		not_null<Ui::PopupMenu*> menu,
		not_null<History*> history) {
	const auto state = ResolveOutgoing(history);
	const auto choose = [=] { ShowDialogSendLanguagePicker(history); };
	switch (state.kind) {
	case Outgoing::Off:
		AddRow(
			menu,
			Tr(u"LuminaChatLangOutgoingOff"_q),
			&st::menuIconSend,
			choose);
		return;
	case Outgoing::Global:
		AddRow(
			menu,
			Tr(u"LuminaChatLangOutgoingGlobal"_q, state.name),
			&st::menuIconSend,
			choose);
		return;
	case Outgoing::Chat:
		AddRow(
			menu,
			Tr(u"LuminaChatLangOutgoingChat"_q, state.name),
			&st::menuIconSend,
			choose);
		return;
	case Outgoing::Ask:
		AddRow(
			menu,
			Tr(u"LuminaChatLangOutgoingAuto"_q),
			&st::menuIconSend,
			choose);
		return;
	}
}

// The way back. Without these two an override is a one-way door: a language
// picked by mistake, or picked for one conversation months ago, can be changed
// to another language but never handed back to the setting it came from.
//
// Each row names the value it would restore, because "use the default" on its
// own is a promise the user has no way to check before pressing it.
void FillDefaults(
		not_null<Ui::PopupMenu*> menu,
		not_null<History*> history) {
	const auto inherited = ChatTranslateDefaultTo(history);
	const auto incoming = ChatTranslating(history)
		&& (ChatTranslatingTo(history) != inherited);
	const auto outgoing = (ResolveOutgoing(history).kind == Outgoing::Chat);
	if (!incoming && !outgoing) {
		return;
	}
	menu->addSeparator();
	if (incoming) {
		const auto name = Ui::LanguageName(inherited);
		AddRow(
			menu,
			Tr(u"LuminaChatLangIncomingDefault"_q, name),
			&st::menuIconRestore,
			[=] { SetChatTranslating(history, true); });
	}
	if (outgoing) {
		AddRow(
			menu,
			Tr(u"LuminaChatLangOutgoingDefault"_q),
			&st::menuIconRestore,
			[=] { SetDialogSendLanguage(history, QString()); });
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
	FillOutgoing(menu, history);
	FillDefaults(menu, history);
}

} // namespace Lumina
