/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_message_details.h"

#include "base/unixtime.h"
#include "chat_helpers/compose/compose_show.h"
#include "data/data_peer.h"
#include "data/data_session.h"
#include "history/history.h"
#include "history/history_item.h"
#include "history/history_item_components.h"
#include "lang/lang_keys.h"
#include "lumina/lumina_locale.h"
#include "lumina/lumina_message_menu.h"
#include "lumina/lumina_settings.h"
#include "ui/layers/generic_box.h"
#include "ui/text/text_entity.h"
#include "ui/widgets/labels.h"
#include "ui/widgets/popup_menu.h"
#include "window/window_session_controller.h"

#include "styles/style_layers.h"
#include "styles/style_menu_icons.h"

#include <QtCore/QLocale>

namespace Lumina {
namespace {

const auto kKey = u"showMessageDetails"_q;

[[nodiscard]] QString FullTimestamp(TimeId when) {
	// QLocale::LongFormat is what the message date tooltip already uses
	// (history_view_element.cpp:686). It is the only in-tree full-precision
	// timestamp, and it carries seconds - which is the whole point of a
	// details popup, and the reason langDateTimeFull() is not used here: that
	// one formats the time with QLocale::ShortFormat and drops them.
	return QLocale().toString(
		base::unixtime::parse(when),
		QLocale::LongFormat);
}

void AppendLine(QString &to, const QString &label, const QString &value) {
	if (value.isEmpty()) {
		return;
	}
	if (!to.isEmpty()) {
		to += '\n';
	}
	to += label + u": "_q + value;
}

void FillDetailsBox(
		not_null<Ui::GenericBox*> box,
		const QString &text,
		Fn<void()> copy) {
	box->setTitle(TrValue(u"LuminaMessageDetails"_q));
	const auto label = box->addRow(object_ptr<Ui::FlatLabel>(
		box,
		rpl::single(text),
		st::boxLabel));
	label->setSelectable(true);
	box->addButton(tr::lng_box_ok(), [=] { box->closeBox(); });
	box->addLeftButton(tr::lng_mediaview_copy(), [=, copy = std::move(copy)] {
		copy();
		box->closeBox();
	});
}

} // namespace

bool MessageDetailsRow() {
	return Settings::Instance().getBool(kKey, false);
}

void SetMessageDetailsRow(bool value) {
	Settings::Instance().set(kKey, value);
}

rpl::producer<> MessageDetailsRowChanges() {
	return Settings::Instance().changesFor(kKey);
}

QString MessageDetailsText(not_null<HistoryItem*> item) {
	auto result = QString();
	AppendLine(
		result,
		Tr(u"LuminaDetailsDate"_q),
		FullTimestamp(item->date()));
	AppendLine(
		result,
		Tr(u"LuminaDetailsMessageId"_q),
		QString::number(item->id.bare));
	AppendLine(
		result,
		Tr(u"LuminaDetailsFrom"_q),
		item->author()->name());

	// Read straight off the component rather than through originalSender() /
	// originalDate(), which both answer for a message that was never
	// forwarded at all - the first with the ordinary sender, the second with
	// the ordinary date - and would print a "Forwarded from" block for every
	// message in the chat.
	if (const auto forwarded = item->Get<HistoryMessageForwarded>()) {
		auto origin = QString();
		if (const auto sender = forwarded->originalSender) {
			origin = sender->name();
		} else if (const auto hidden
				= forwarded->originalHiddenSenderInfo.get()) {
			origin = hidden->name;
		}
		const auto author = forwarded->originalPostAuthor;
		if (!author.isEmpty()) {
			origin = origin.isEmpty()
				? author
				: (origin + u" ("_q + author + ')');
		}
		AppendLine(result, Tr(u"LuminaDetailsForwardedFrom"_q), origin);
		if (forwarded->originalDate > 0) {
			AppendLine(
				result,
				Tr(u"LuminaDetailsOriginalDate"_q),
				FullTimestamp(forwarded->originalDate));
		}
	}
	return result;
}

void AddMessageDetailsMenuRow(
		not_null<Ui::PopupMenu*> menu,
		const MessageMenuContext &context) {
	if (!MessageDetailsRow() || context.hasSelection) {
		return;
	}
	const auto owner = &context.item->history()->owner();
	const auto itemId = context.item->fullId();
	const auto show = context.controller->uiShow();
	menu->addAction(Tr(u"LuminaMessageDetails"_q), [=] {
		const auto item = owner->message(itemId);
		if (!item) {
			return;
		}
		const auto text = MessageDetailsText(item);
		if (text.isEmpty()) {
			return;
		}
		const auto copy = [=] {
			TextUtilities::SetClipboardText(
				TextForMimeData::Simple(text));
			show->showToast(tr::lng_text_copied(tr::now));
		};
		show->showBox(Box(FillDetailsBox, text, Fn<void()>(copy)));
	}, &st::menuIconInfo);
}

} // namespace Lumina
