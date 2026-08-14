/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_explain.h"

#include "data/data_session.h"
#include "history/history.h"
#include "history/history_item.h"
#include "lang/lang_keys.h"
#include "lumina/lumina_locale.h"
#include "lumina/lumina_message_menu.h"
#include "lumina/lumina_settings.h"
#include "lumina/lumina_translate_providers.h"
#include "lumina/lumina_translate_readlang.h"
#include "main/main_session.h"
#include "ui/layers/generic_box.h"
#include "ui/text/text_entity.h"
#include "ui/widgets/labels.h"
#include "ui/widgets/popup_menu.h"
#include "ui/wrap/slide_wrap.h"
#include "window/window_session_controller.h"

#include "styles/style_layers.h"
#include "styles/style_menu_icons.h"

#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonParseError>
#include <QtCore/QUrl>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

namespace Lumina {
namespace {

// Android's key name, unchanged, so a settings backup means the same thing on
// both platforms.
const auto kKeyEnabled = u"explainMessage"_q;

// The same 15s ceiling every LuminaGram HTTP call uses
// (lumina/lumina_translate_providers.cpp): a dead endpoint must not leave the
// box stuck on "Explaining..." forever.
constexpr auto kRequestTimeoutMs = 15000;

// The language the user READS. Same resolution the rest of the fork uses (see
// lumina/lumina_voice_to_text.cpp): the LuminaGram read-language override
// first, then the interface language. Handed to the prompt as an English name
// ("Traditional Chinese") because that is what an LLM prompt reads best with.
[[nodiscard]] QString ReadingLanguageName() {
	const auto stored = NormalizeLanguageCode(ReadLanguageCode());
	const auto code = stored.isEmpty() ? InterfaceLanguageCode() : stored;
	return LanguageEnglishName(code);
}

// The instruction, kept in code exactly as the translation prompt is
// (DefaultLlmPrompt()): it is an instruction to the model, not something the
// user reads, and the {lang} placeholder is what makes the model answer in the
// reader's own language rather than in English.
[[nodiscard]] QString ExplainPrompt(const QString &languageName) {
	return QString(u"You are a cross-cultural communication assistant. The "
		"user received the chat message below and wants to understand it. "
		"Break it down into exactly these four short, clearly labelled "
		"sections, in this order:\n"
		"1. Literal meaning: what the words say, plainly.\n"
		"2. Actual tone: the real intent, mood or subtext behind the "
		"words.\n"
		"3. Cultural and slang notes: any idioms, slang, memes, references "
		"or cultural context a non-native reader could miss; write a brief "
		"\"none\" if there is nothing notable.\n"
		"4. How to reply: one or two natural ways the user could respond.\n"
		"Keep every section concise. Do not repeat the whole message back. "
		"Write the entire answer, including the section headings, in "
		"{lang}."_q).replace(u"{lang}"_q, languageName);
}

// choices[0].message.content of an OpenAI-compatible chat completion, trimmed,
// or empty for anything unparseable. Same shape LlmEngine::translate() reads.
[[nodiscard]] QString ParseContent(const QByteArray &body) {
	auto error = QJsonParseError{ 0, QJsonParseError::NoError };
	const auto document = QJsonDocument::fromJson(body, &error);
	if (error.error != QJsonParseError::NoError || !document.isObject()) {
		return QString();
	}
	const auto choices = document.object().value(u"choices"_q).toArray();
	if (choices.isEmpty()) {
		return QString();
	}
	const auto message = choices.at(0).toObject().value(
		u"message"_q).toObject();
	return message.value(u"content"_q).toString().trimmed();
}

struct State {
	rpl::variable<QString> status;
	rpl::variable<QString> result;

	// Declared LAST so it is destroyed FIRST when the box closes: its
	// destructor aborts the reply below, and the abort must happen while the
	// variables it would write to are still alive.
	QNetworkAccessManager network;
};

// The one and only LLM call. It reuses the translation LLM's key, base URL and
// model (lumina/lumina_translate_providers.h) rather than storing any of its
// own, and it builds the request the same way LlmEngine::translate() does. It
// never logs: the body carries a private message and the header carries the
// user's key.
void RequestExplain(
		not_null<Ui::GenericBox*> box,
		not_null<State*> state,
		const QString &text) {
	const auto key = ProviderApiKey(LlmProviderId());
	if (key.isEmpty()) {
		// No key: the promise at the top of the header. Say where to add one,
		// and stop - never crash, never fall through to a request with no
		// Authorization header.
		state->status = Tr(u"LuminaExplainNoKey"_q);
		return;
	}
	auto baseUrl = LlmBaseUrl();
	while (baseUrl.endsWith(QChar('/'))) {
		baseUrl.chop(1);
	}
	if (baseUrl.isEmpty()) {
		baseUrl = DefaultLlmBaseUrl();
	}
	const auto url = QUrl(baseUrl + u"/chat/completions"_q);
	if (!url.isValid()) {
		state->status = Tr(u"LuminaExplainError"_q);
		return;
	}
	const auto body = QJsonDocument(QJsonObject{
		{ u"model"_q, LlmModel() },
		{ u"temperature"_q, 0.3 },
		{ u"messages"_q, QJsonArray{
			QJsonObject{
				{ u"role"_q, u"system"_q },
				{ u"content"_q, ExplainPrompt(ReadingLanguageName()) },
			},
			QJsonObject{
				{ u"role"_q, u"user"_q },
				{ u"content"_q, text },
			},
		} },
	}).toJson(QJsonDocument::Compact);

	auto request = QNetworkRequest(url);
	request.setTransferTimeout(kRequestTimeoutMs);
	request.setRawHeader("Authorization", "Bearer " + key.toUtf8());
	request.setRawHeader("Content-Type", "application/json");

	const auto reply = state->network.post(request, body);
	QObject::connect(reply, &QNetworkReply::finished, crl::guard(box, [=] {
		const auto status = reply->attribute(
			QNetworkRequest::HttpStatusCodeAttribute).toInt();
		const auto failure = reply->error();
		auto received = reply->readAll();
		reply->deleteLater();
		if (status >= 400 || failure != QNetworkReply::NoError) {
			state->status = Tr(u"LuminaExplainError"_q);
			return;
		}
		const auto content = ParseContent(received);
		if (content.isEmpty()) {
			state->status = Tr(u"LuminaExplainError"_q);
			return;
		}
		state->status = QString();
		state->result = content;
	}));
}

void ExplainBox(
		not_null<Ui::GenericBox*> box,
		not_null<Window::SessionController*> controller,
		FullMsgId itemId) {
	box->setTitle(TrValue(u"LuminaExplainTitle"_q));

	const auto state = box->lifetime().make_state<State>();

	// Each section is slide-wrapped rather than emptied, so the box grows from
	// "Explaining..." to the answer without leaving a hole, and the status
	// line disappears once the answer is in.
	const auto addText = [&](not_null<rpl::variable<QString>*> value) {
		const auto wrap = box->addRow(
			object_ptr<Ui::SlideWrap<Ui::FlatLabel>>(
				box,
				object_ptr<Ui::FlatLabel>(
					box,
					value->value(),
					st::boxLabel)));
		wrap->toggleOn(value->value() | rpl::map([](const QString &text) {
			return !text.isEmpty();
		}));
		wrap->finishAnimating();
		return wrap->entity();
	};

	addText(&state->status);
	addText(&state->result)->setSelectable(true);

	box->addButton(tr::lng_box_ok(), [=] {
		box->closeBox();
	});

	const auto show = controller->uiShow();
	box->addLeftButton(tr::lng_mediaview_copy(), [=] {
		const auto text = state->result.current();
		if (text.isEmpty()) {
			return;
		}
		TextUtilities::SetClipboardText(TextForMimeData::Simple(text));
		show->showToast(tr::lng_text_copied(tr::now));
	});

	// Resolved again here, not captured from the menu: a message can be
	// deleted while the menu is open.
	const auto session = &controller->session();
	const auto item = session->data().message(itemId);
	const auto text = ExplainMessageText(item);
	if (text.isEmpty()) {
		state->status = Tr(u"LuminaExplainError"_q);
		return;
	}
	state->status = Tr(u"LuminaExplainWorking"_q);
	RequestExplain(box, state, text);
}

} // namespace

bool ExplainMessageEnabled() {
	// Default ON, exactly as Android's LuminaExplain defaults it. The row is
	// still absent on a message with no text, so an untouched profile only
	// ever sees it where it could actually do something.
	return Settings::Instance().getBool(kKeyEnabled, true);
}

void SetExplainMessageEnabled(bool value) {
	Settings::Instance().set(kKeyEnabled, value);
}

rpl::producer<> ExplainMessageEnabledChanges() {
	return Settings::Instance().changesFor(kKeyEnabled);
}

QString ExplainMessageText(HistoryItem *item) {
	return item ? item->originalText().text.trimmed() : QString();
}

void ShowExplainMessage(
		not_null<Window::SessionController*> controller,
		not_null<HistoryItem*> item) {
	controller->show(Box(ExplainBox, controller, item->fullId()));
}

void AddExplainMenuRow(
		not_null<Ui::PopupMenu*> menu,
		const MessageMenuContext &context) {
	if (!ExplainMessageEnabled() || context.hasSelection) {
		return;
	}
	const auto item = context.item;

	// The one guard: no text, no row - on a photo, a sticker or a voice note
	// as much as on an empty item.
	if (ExplainMessageText(item).isEmpty()) {
		return;
	}

	// Unlike voice-to-text, the row is NOT hidden when no LLM key is set: the
	// spec is that clicking it then explains, in the box, that a key has to be
	// added on the translation page. The row is where a user discovers the
	// feature, and hiding it would hide the instruction with it.
	const auto session = &item->history()->session();
	const auto itemId = item->fullId();
	const auto controller = context.controller;

	// The two menu builders own the menu they hand us, so they outlive it and
	// the raw capture is safe; the message is resolved again on click because
	// it can be deleted while the menu is open.
	menu->addAction(Tr(u"LuminaExplainMenuItem"_q), crl::guard(controller, [=] {
		if (const auto item = session->data().message(itemId)) {
			ShowExplainMessage(controller, item);
		}
	}), &st::menuIconInfo);
}

} // namespace Lumina
