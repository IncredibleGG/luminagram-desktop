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
#include "lumina/lumina_translate_settings.h"
#include "lumina/lumina_translate_toggle.h"
#include "ui/boxes/choose_language_box.h"
#include "ui/text/text_entity.h"
#include "ui/widgets/menu/menu_multiline_action.h"
#include "ui/widgets/popup_menu.h"
#include "window/window_session_controller.h"
#include "styles/style_menu_icons.h"
#include "styles/style_widgets.h"

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
[[nodiscard]] QString OutgoingRow() {
	if (!TranslateBeforeSend()) {
		return Tr(u"LuminaChatLangMeOff"_q);
	}
	return Tr(u"LuminaChatLangMe"_q, TranslateSendLanguageIsAuto()
		? Tr(u"LuminaTranslateSendLangAuto"_q)
		: TranslateLanguageName(TranslateSendLanguage()));
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
		not_null<Window::SessionController*> controller) {
	AddRow(
		menu,
		OutgoingRow(),
		&st::menuIconSend,
		[=] {
			ShowLanguagePicker(
				controller,
				Tr(u"LuminaChatLangMeTitle"_q),
				Tr(u"LuminaChatLangNone"_q),
				OffCode(),
				(TranslateBeforeSend()
					? TranslateSendLanguage()
					: OffCode()),
				[](QString code) {
					if (code == OffCode()) {
						SetTranslateBeforeSend(false);
						return;
					}
					SetTranslateSendLanguage(code);
					SetTranslateBeforeSend(true);
				});
		});
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
	FillOutgoing(menu, controller);
}

} // namespace Lumina
