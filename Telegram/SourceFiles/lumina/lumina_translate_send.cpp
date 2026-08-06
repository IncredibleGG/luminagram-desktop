/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_translate_send.h"

#include "base/flat_map.h"
#include "base/timer.h"
#include "base/weak_ptr.h"
#include "data/data_peer.h"
#include "data/data_premium_limits.h"
#include "history/history.h"
#include "lang/lang_keys.h"
#include "lumina/lumina_send_pipeline.h"
#include "lumina/lumina_settings.h"
#include "lumina/lumina_translate_gating.h"
#include "lumina/lumina_translate_providers.h"
#include "lumina/lumina_translate_settings.h"
#include "main/main_session.h"
#include "menu/menu_checked_action.h"
#include "menu/menu_send_details.h"
#include "ui/boxes/single_choice_box.h"
#include "ui/layers/generic_box.h"
#include "ui/widgets/labels.h"
#include "ui/widgets/popup_menu.h"
#include "window/window_session_controller.h"

#include "styles/style_layers.h"
#include "styles/style_menu_icons.h"

#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
#include <crl/crl_on_main.h>

namespace Lumina {
namespace {

// The stall watchdog. Every HTTP request the provider layer makes already
// carries a 15s timeout of its own (lumina_translate_providers.cpp), so this
// only has to cover what that timeout cannot see: an engine that resolves
// nothing, and a callback lost between the request and its reply. Android
// shipped the in-flight lock with no timeout at all, and one stalled request
// then swallowed every later send in that chat for the rest of the session.
constexpr auto kWatchdogTimeout = crl::time(20000);

// How long a send-menu quick toggle stays armed. It means "this next message",
// so a menu the user opened and walked away from must not quietly change how a
// message typed much later is sent.
constexpr auto kQuickToggleLifetime = crl::time(5 * 60 * 1000);

[[nodiscard]] QString DialogLanguagesKey() {
	return u"trSendLangDialog"_q;
}

// The session id is in the key so that two logged-in accounts cannot share a
// per-chat lock. Peer ids are very nearly unique on their own, which is
// exactly the kind of "nearly" that turns into a cross-account bug later.
[[nodiscard]] QString DialogKey(not_null<History*> history) {
	return QString::number(history->session().uniqueId())
		+ QChar('_')
		+ QString::number(history->peer->id.value);
}

struct QuickToggle {
	uint64 peerId = 0;
	crl::time when = 0;
	bool value = false;
};

[[nodiscard]] QuickToggle &CurrentQuickToggle() {
	static auto result = QuickToggle();
	return result;
}

[[nodiscard]] std::optional<bool> PeekQuickToggle(uint64 peerId) {
	auto &toggle = CurrentQuickToggle();
	if (!toggle.peerId) {
		return std::nullopt;
	} else if (crl::now() - toggle.when > kQuickToggleLifetime) {
		toggle = QuickToggle();
		return std::nullopt;
	} else if (toggle.peerId != peerId) {
		return std::nullopt;
	}
	return toggle.value;
}

// Only the send that actually acted on the override clears it. A send this
// pipeline could not have translated anyway - one that arrived while another
// send in the same chat was still held - leaves it armed for the next one.
void ConsumeQuickToggle(uint64 peerId) {
	auto &toggle = CurrentQuickToggle();
	if (toggle.peerId == peerId) {
		toggle = QuickToggle();
	}
}

void SetQuickToggle(uint64 peerId, bool value) {
	CurrentQuickToggle() = QuickToggle{
		.peerId = peerId,
		.when = crl::now(),
		.value = value,
	};
}

struct PreviewCache {
	QString dialog;
	QString source;
	QString target;
	QString translated;
};

[[nodiscard]] PreviewCache &Preview() {
	static auto result = PreviewCache();
	return result;
}

[[nodiscard]] SendOriginalHook &OriginalHook() {
	static auto result = SendOriginalHook();
	return result;
}

// One held send. `text` points into the Api::MessageToSend that `proceed`
// owns, which is what lumina_send_pipeline.h promises: rewriting it just
// before invoking `proceed` is the supported way to change an outgoing message
// asynchronously, and it dangles the moment the last copy of `proceed` goes.
// So every terminal below moves `proceed` out into a local first and only then
// touches `text`.
struct Request {
	base::weak_ptr<History> history;
	QString original;
	QString target;
	TextWithTags *text = nullptr;
	Fn<void()> proceed;
	base::Timer watchdog;
	uint64 generation = 0;
};

[[nodiscard]] base::flat_map<QString, std::unique_ptr<Request>> &Requests() {
	static auto result = base::flat_map<QString, std::unique_ptr<Request>>();
	return result;
}

[[nodiscard]] uint64 NextGeneration() {
	static auto result = uint64(0);
	return ++result;
}

// Every terminal takes the generation as well as the chat key, because a box
// can outlive the send it was opened for: cancel one, send again in the same
// chat, and the first box's buttons would otherwise finish the second send
// with the first one's translation.
[[nodiscard]] Request *FindRequest(const QString &key, uint64 generation) {
	const auto i = Requests().find(key);
	return (i != Requests().end() && i->second->generation == generation)
		? i->second.get()
		: nullptr;
}

[[nodiscard]] Window::SessionController *ResolveController(
		not_null<History*> history) {
	return history->session().tryResolveWindow(history->peer);
}

// A translation longer than the message limit would be split into several
// messages by ApiWrap::sendMessage(), which both looks wrong and breaks the
// single sent-text correlation W2-B needs. The original already passed the
// composer's own length check, so sending that is always the safe answer.
// This has to cover the W2-D preview reuse path as well, which never goes
// near the provider and would otherwise skip the check entirely.
[[nodiscard]] bool TranslationFits(
		History *history,
		const QString &translated) {
	if (!history) {
		return true;
	}
	const auto limits = Data::PremiumLimits(&history->session());
	return (int(translated.size()) <= limits.messageLengthCurrent());
}

void FinishRequest(
		const QString &key,
		uint64 generation,
		const QString &translated) {
	const auto request = FindRequest(key, generation);
	if (!request) {
		return;
	}
	auto proceed = std::move(request->proceed);
	const auto text = request->text;
	const auto original = request->original;
	const auto history = request->history.get();
	Requests().remove(key);
	if (!proceed) {
		return;
	}
	if (text && !translated.isEmpty() && (translated != original)) {
		text->text = translated;
		if (history) {
			if (const auto hook = OriginalHook()) {
				hook(history, translated, original);
			}
		}
	}
	proceed();
}

// Dropping the request drops `proceed` with it, which is how
// lumina_send_pipeline.h spells "cancel". The composer never cleared its
// field, so the user's text is still sitting in it afterwards.
void CancelRequest(const QString &key, uint64 generation) {
	if (FindRequest(key, generation)) {
		Requests().remove(key);
	}
}

[[nodiscard]] uint64 CreateRequest(
		const QString &key,
		not_null<History*> history,
		TextWithTags &text,
		const QString &original,
		Fn<void()> proceed) {
	const auto generation = NextGeneration();
	auto created = std::make_unique<Request>();
	created->history = base::make_weak(history);
	created->original = original;
	created->text = &text;
	created->proceed = std::move(proceed);
	created->generation = generation;
	Requests().emplace(key, std::move(created));
	return generation;
}

void ApplyTranslation(
		const QString &key,
		uint64 generation,
		const QString &translated);

void ShowTranslationConfirm(
		const QString &key,
		uint64 generation,
		const QString &translated);

void StartTranslation(
		const QString &key,
		uint64 generation,
		const QString &target) {
	const auto request = FindRequest(key, generation);
	if (!request) {
		return;
	}
	request->target = target;
	request->watchdog.setCallback([=] {
		crl::on_main([=] { FinishRequest(key, generation, QString()); });
	});
	request->watchdog.callOnce(kWatchdogTimeout);

	const auto history = request->history.get();
	const auto session = history ? &history->session() : nullptr;
	const auto original = request->original;
	TranslateText(session, original, target, [=](TranslateResult result) {
		const auto text = result.failed()
			? QString()
			: result.text.trimmed();

		// The provider callback runs inside its own network reply, and
		// TranslateText() may even answer synchronously when no engine can be
		// built at all. Everything below sends a message and can open a box,
		// so it belongs on a clean main-thread turn either way.
		crl::on_main([=] { ApplyTranslation(key, generation, text); });
	});
}

void ApplyTranslation(
		const QString &key,
		uint64 generation,
		const QString &translated) {
	const auto request = FindRequest(key, generation);
	if (!request) {
		return;
	}
	request->watchdog.cancel();
	if (translated.isEmpty() || (translated == request->original)) {
		FinishRequest(key, generation, QString());
		return;
	}
	if (!TranslationFits(request->history.get(), translated)) {
		FinishRequest(key, generation, QString());
		return;
	}
	if (TranslateBeforeSendConfirm()) {
		ShowTranslationConfirm(key, generation, translated);
	} else {
		FinishRequest(key, generation, translated);
	}
}

void ShowTranslationConfirm(
		const QString &key,
		uint64 generation,
		const QString &translated) {
	const auto request = FindRequest(key, generation);
	const auto history = request ? request->history.get() : nullptr;
	const auto controller = history ? ResolveController(history) : nullptr;
	if (!controller) {
		FinishRequest(key, generation, translated);
		return;
	}
	const auto original = request->original;
	const auto name = TranslateLanguageName(request->target);
	const auto routed = std::make_shared<bool>(false);
	controller->show(Box([=](not_null<Ui::GenericBox*> box) {
		box->setTitle(rpl::single(u"Translate before sending"_q));
		box->addRow(object_ptr<Ui::FlatLabel>(
			box.get(),
			u"Original\n"_q
				+ original
				+ u"\n\n"_q
				+ name
				+ QChar('\n')
				+ translated,
			st::boxLabel));
		// Nothing runs after closeBox() anywhere below: closing a box may
		// destroy it, and with it the very lambda that is executing. So each
		// outcome is posted to the next main-thread turn and the close is the
		// last statement, which also keeps the send itself out of a layer that
		// is in the middle of going away.
		box->addButton(rpl::single(u"Send translation"_q), [=] {
			*routed = true;
			crl::on_main([=] {
				FinishRequest(key, generation, translated);
			});
			box->closeBox();
		});
		box->addButton(rpl::single(u"Send original"_q), [=] {
			*routed = true;
			crl::on_main([=] {
				FinishRequest(key, generation, QString());
			});
			box->closeBox();
		});
		box->addLeftButton(tr::lng_cancel(), [=] {
			box->closeBox();
		});
		box->lifetime().add([=] {
			if (!*routed) {
				CancelRequest(key, generation);
			}
		});
	}));
}

// `pending` is the send this picker was opened for, or an empty key when it
// was opened from the settings-style entry point that only rewrites the lock.
void ShowLanguagePicker(
		not_null<Window::SessionController*> controller,
		base::weak_ptr<History> weak,
		const QString &current,
		const QString &pending,
		uint64 generation) {
	const auto &languages = TranslateLanguages();
	auto options = std::vector<QString>();
	auto codes = std::vector<QString>();
	options.reserve(languages.size() + 1);
	codes.reserve(languages.size() + 1);
	for (const auto &language : languages) {
		options.push_back(language.name);
		codes.push_back(language.code);
	}

	// A code this build's table does not carry - written by another client, or
	// restored from a backup - gets an entry of its own, so the picker cannot
	// show a selection that disagrees with the lock it is editing.
	if (!current.isEmpty()
		&& (ranges::find(codes, current) == end(codes))) {
		options.push_back(TranslateLanguageName(current));
		codes.push_back(current);
	}
	// SingleChoiceBox only fires its callback when the radio group's value
	// CHANGES, and its own button just closes the box. A pre-selected row is
	// therefore a row that cannot be chosen: picking it does nothing, the box
	// closes, and for a picker opened for a send in flight that means the held
	// message is dropped even though the user believes they answered. A
	// send-time picker starts with nothing selected, and -1 is also the right
	// start when there is no lock yet, so that the first language in the table
	// is not silently presented as the current answer.
	auto selected = -1;
	for (auto i = 0, count = int(codes.size()); i != count; ++i) {
		if (codes[i] == current) {
			selected = i;
			break;
		}
	}
	if (!pending.isEmpty()) {
		selected = -1;
	}
	const auto routed = std::make_shared<bool>(false);
	controller->show(Box([=](not_null<Ui::GenericBox*> box) {
		SingleChoiceBox(box, {
			.title = rpl::single(u"Translate messages into"_q),
			.options = options,
			.initialSelection = selected,
			.callback = [=](int index) {
				if (index < 0 || index >= int(codes.size())) {
					return;
				}
				*routed = true;
				const auto code = codes[index];
				if (const auto history = weak.get()) {
					SetDialogSendLanguage(history, code);
				}
				if (!pending.isEmpty()) {
					crl::on_main([=] {
						StartTranslation(pending, generation, code);
					});
				}
			},
		});
		box->lifetime().add([=] {
			if (!*routed && !pending.isEmpty()) {
				CancelRequest(pending, generation);
			}
		});
	}));
}

// The one-time per-chat confirm. Detection is only a suggestion here - it is
// wrong often enough between close neighbours (Malay and Indonesian being the
// pair that made Android add this box in the first place) that sending on it
// silently is not acceptable - so the answer is locked for the chat and never
// asked again.
void ShowSendLanguageConfirm(
		not_null<Window::SessionController*> controller,
		base::weak_ptr<History> weak,
		const QString &key,
		uint64 generation,
		const QString &code) {
	const auto name = TranslateLanguageName(code);
	const auto routed = std::make_shared<bool>(false);
	const auto weakController = base::make_weak(controller);
	controller->show(Box([=](not_null<Ui::GenericBox*> box) {
		box->setTitle(rpl::single(u"Translate before sending"_q));
		box->addRow(object_ptr<Ui::FlatLabel>(
			box.get(),
			u"This chat looks like it is written in "_q
				+ name
				+ u". Translate the messages you send here into "_q
				+ name
				+ u"? LuminaGram will remember this for this chat."_q,
			st::boxLabel));
		box->addButton(rpl::single(u"Translate"_q), [=] {
			*routed = true;
			if (const auto history = weak.get()) {
				SetDialogSendLanguage(history, code);
			}
			crl::on_main([=] {
				StartTranslation(key, generation, code);
			});
			box->closeBox();
		});
		box->addButton(rpl::single(u"Send as typed"_q), [=] {
			*routed = true;
			crl::on_main([=] {
				FinishRequest(key, generation, QString());
			});
			box->closeBox();
		});
		box->addLeftButton(rpl::single(u"Choose language"_q), [=] {
			*routed = true;
			crl::on_main([=] {
				if (const auto strong = weakController.get()) {
					ShowLanguagePicker(strong, weak, code, key, generation);
				} else {
					FinishRequest(key, generation, QString());
				}
			});
			box->closeBox();
		});
		box->lifetime().add([=] {
			if (!*routed) {
				CancelRequest(key, generation);
			}
		});
	}));
}

void AskSendLanguage(
		not_null<Window::SessionController*> controller,
		not_null<History*> history,
		const QString &key,
		uint64 generation) {
	const auto weak = base::make_weak(history);
	const auto offered = history->translateOfferedFrom();
	const auto detected = offered
		? NormalizeLanguageCode(offered.twoLetterCode())
		: QString();
	if (detected.isEmpty()) {
		ShowLanguagePicker(controller, weak, QString(), key, generation);
	} else {
		ShowSendLanguageConfirm(controller, weak, key, generation, detected);
	}
}

bool Intercept(
		not_null<History*> history,
		TextWithTags &text,
		Fn<void()> proceed) {
	if (!TranslationFeatureEnabled()) {
		return true;
	}
	const auto peer = history->peer;
	const auto original = text.text.trimmed();

	// A tagged message carries bold / mention / custom-emoji ranges as
	// character offsets into this exact string. Replacing the string would
	// leave every one of them pointing at the wrong characters, or past the
	// end, and it fails silently rather than loudly. Dropping the user's
	// formatting instead is no better, so a formatted message is sent as
	// typed. This is checked before the quick toggle is consumed, so a
	// "translate just this one" armed on a message that cannot be translated
	// at all is still there for the next one.
	if (original.isEmpty()
		|| !text.tags.isEmpty()
		|| !TranslateScopeAllows(peer)) {
		return true;
	}
	const auto quick = PeekQuickToggle(peer->id.value);
	if (!quick.value_or(TranslateBeforeSend())) {
		ConsumeQuickToggle(peer->id.value);
		return true;
	}
	const auto key = DialogKey(history);

	// A send is already held for this chat. Ignore this one exactly as Android
	// does: the composer still holds the text, so nothing is lost, and the
	// watchdog guarantees the hold ends.
	if (Requests().contains(key)) {
		return false;
	}
	const auto target = TranslateSendLanguageIsAuto()
		? DialogSendLanguage(history)
		: TranslateSendLanguage();
	if (!target.isEmpty()) {
		const auto &preview = Preview();
		const auto reuse = (preview.dialog == key)
			&& (preview.source == original)
			&& (preview.target == target)
			&& !preview.translated.isEmpty()
			&& TranslationFits(history, preview.translated);
		const auto translated = reuse ? preview.translated : QString();
		if (reuse) {
			// One entry, consumed once. Keeping it would serve the
			// stored translation to a later identical message even
			// after the provider or its key changed, and would leave
			// the last message the user composed sitting in a
			// process-lifetime static.
			Preview() = PreviewCache();
		}
		ConsumeQuickToggle(peer->id.value);
		const auto generation = CreateRequest(
			key,
			history,
			text,
			original,
			std::move(proceed));
		if (reuse) {
			FinishRequest(key, generation, translated);
		} else {
			StartTranslation(key, generation, target);
		}
		return false;
	}
	const auto controller = ResolveController(history);
	if (!controller) {
		return true;
	}
	ConsumeQuickToggle(peer->id.value);
	const auto generation = CreateRequest(
		key,
		history,
		text,
		original,
		std::move(proceed));
	AskSendLanguage(controller, history, key, generation);
	return false;
}

} // namespace

void SetupTranslateSendPipeline() {
	static auto registered = false;
	if (registered) {
		return;
	}
	registered = true;
	RegisterSendInterceptor([](
			not_null<History*> history,
			TextWithTags &text,
			Api::SendOptions,
			Fn<void()> proceed) {
		return Intercept(history, text, std::move(proceed));
	});
}

bool TranslateBeforeSendActive(not_null<History*> history) {
	if (!TranslationFeatureEnabled()) {
		return false;
	}
	const auto peer = history->peer;
	return PeekQuickToggle(peer->id.value).value_or(TranslateBeforeSend())
		&& TranslateScopeAllows(peer);
}

QString DialogSendLanguage(not_null<History*> history) {
	return Settings::Instance().getObject(
		DialogLanguagesKey()
	).value(DialogKey(history)).toString().trimmed();
}

void SetDialogSendLanguage(not_null<History*> history, const QString &code) {
	const auto key = DialogKey(history);
	const auto trimmed = code.trimmed();
	auto object = Settings::Instance().getObject(DialogLanguagesKey());
	if (trimmed.isEmpty()) {
		object.remove(key);
	} else {
		object.insert(key, trimmed);
	}
	if (object.isEmpty()) {
		Settings::Instance().remove(DialogLanguagesKey());
	} else {
		Settings::Instance().set(
			DialogLanguagesKey(),
			object,
			Store::Private);
	}
	auto &preview = Preview();
	if (preview.dialog == key) {
		preview = PreviewCache();
	}
}

QString ResolveSendLanguage(not_null<History*> history) {
	if (!TranslateBeforeSendActive(history)) {
		return QString();
	} else if (!TranslateSendLanguageIsAuto()) {
		return TranslateSendLanguage();
	}
	return DialogSendLanguage(history);
}

void ShowDialogSendLanguagePicker(not_null<History*> history) {
	if (const auto controller = ResolveController(history)) {
		ShowLanguagePicker(
			controller,
			base::make_weak(history),
			DialogSendLanguage(history),
			QString(),
			0);
	}
}

void NoteSendTranslationPreview(
		not_null<History*> history,
		const QString &source,
		const QString &target,
		const QString &translated) {
	auto &preview = Preview();
	preview.dialog = DialogKey(history);
	preview.source = source.trimmed();
	preview.target = target;
	preview.translated = translated.trimmed();
}

void SetSendOriginalHook(SendOriginalHook hook) {
	OriginalHook() = std::move(hook);
}

void AddSendMenuTranslateRow(
		not_null<Ui::PopupMenu*> menu,
		const SendMenu::Details &details) {
	// Everything below the send-behaviour group of the menu is media state:
	// an album, a caption or a paid post never routes through the text send
	// path this row talks about, so any of it being set rules the row out.
	//
	// The reverse is NOT true, and SendMenu::Details cannot express it. The
	// sticker, GIF, inline-result and field-autocomplete panels all build
	// their menu from the composer's own sendMenuDetails(), which leaves every
	// field here unset, so the row is offered there too. It stays correct -
	// the override is per chat and is consumed by that chat's next text send -
	// but it is offered in more places than it reads well in. Narrowing it
	// needs a flag on SendMenu::Details, which is not this item's file.
	if (!TranslationFeatureEnabled()
		|| !details.barePeerId
		|| (details.spoiler != SendMenu::SpoilerState::None)
		|| (details.caption != SendMenu::CaptionState::None)
		|| (details.photoQuality != SendMenu::PhotoQualityState::None)
		|| details.price.has_value()) {
		return;
	}
	const auto peerId = details.barePeerId;
	const auto checked = PeekQuickToggle(peerId).value_or(
		TranslateBeforeSend());
	Menu::AddCheckedAction(
		menu,
		u"Translate before sending"_q,
		[=] { SetQuickToggle(peerId, !checked); },
		&st::menuIconTranslate,
		checked);
}

namespace {

// The registry in lumina_send_pipeline.cpp is a function-local static, so
// registering from dynamic initialization is order-independent, and nothing
// this interceptor reads is touched until a message is actually sent. The
// public SetupTranslateSendPipeline() stays available for an explicit init
// point should one ever be added.
struct Registrar {
	Registrar() {
		SetupTranslateSendPipeline();
	}
};

[[maybe_unused]] const auto kRegistrar = Registrar();

} // namespace

} // namespace Lumina
