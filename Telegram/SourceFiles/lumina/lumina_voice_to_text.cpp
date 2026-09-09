/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_voice_to_text.h"

#include "api/api_transcribes.h"
#include "apiwrap.h"
#include "base/flat_map.h"
#include "base/weak_ptr.h"
#include "chat_helpers/compose/compose_show.h"
#include "core/file_location.h"
#include "data/data_document.h"
#include "data/data_document_media.h"
#include "data/data_file_origin.h"
#include "data/data_media_types.h"
#include "data/data_session.h"
#include "history/history.h"
#include "history/history_item.h"
#include "lang/lang_keys.h"
#include "lumina/lumina_locale.h"
#include "lumina/lumina_message_menu.h"
#include "lumina/lumina_settings.h"
#include "lumina/lumina_transcribers.h"
#include "lumina/lumina_translate_providers.h"
#include "lumina/lumina_translate_readlang.h"
#include "main/main_session.h"
#include "settings/sections/settings_lumina_voice.h"
#include "spellcheck/platform/platform_language.h"
#include "ui/boxes/confirm_box.h"
#include "ui/layers/generic_box.h"
#include "ui/text/text_entity.h"
#include "ui/vertical_list.h"
#include "ui/widgets/labels.h"
#include "ui/widgets/popup_menu.h"
#include "ui/wrap/slide_wrap.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"

#include "styles/style_layers.h"
#include "styles/style_menu_icons.h"

#include <QtCore/QFile>

namespace Lumina {
namespace {

// Android's key names, unchanged, so a settings backup means the same thing
// on both platforms.
const auto kKeyEnabled = u"voiceToTextEnabled"_q;
const auto kKeyAutoTranslate = u"sttAutoTranslate"_q;

[[nodiscard]] QString ErrorText(TranscribeError error) {
	switch (error) {
	case TranscribeError::NoKey:
		return Tr(u"LuminaSttUiNoKey"_q);
	case TranscribeError::TooLarge:
		return Tr(u"LuminaSttUiTooLarge"_q);
	case TranscribeError::UnsupportedMedia:
		return Tr(u"LuminaSttUiRoundUnsupported"_q);
	case TranscribeError::NoSpeech:
		return Tr(u"LuminaSttUiNoText"_q);
	case TranscribeError::Unavailable:
	case TranscribeError::RateLimited:
	case TranscribeError::Network:
	case TranscribeError::BadResponse:
	case TranscribeError::None:
		break;
	}
	// Everything that is about the service rather than about this particular
	// voice message reads the same way it does on Android, and deliberately
	// carries no part of the response: an error body from these endpoints can
	// echo back the request, which here is the contents of a private message.
	return Tr(u"LuminaSttUiError"_q);
}

// The language the user READS. Same resolution the rest of the fork uses: the
// LuminaGram read-language override first (it is the only one that can carry
// a dialect), then the interface language.
[[nodiscard]] QString ReadingLanguage() {
	const auto stored = NormalizeLanguageCode(ReadLanguageCode());
	return stored.isEmpty() ? InterfaceLanguageCode() : stored;
}

struct State {
	std::shared_ptr<Data::DocumentMedia> media;
	std::unique_ptr<TranscribeEngine> engine;
	rpl::variable<QString> status;
	rpl::variable<QString> transcript;
	rpl::variable<QString> translation;
	rpl::lifetime downloading;
	bool downloadRequested = false;
};

// The audio exactly as Telegram stored it. A voice note is saveToCache(), so
// it normally arrives as bytes in the media view and never touches the disk;
// a round video is a real file. Both are handled, in that order, because
// either one can be the empty half.
[[nodiscard]] QByteArray ReadContent(
		not_null<DocumentData*> document,
		const std::shared_ptr<Data::DocumentMedia> &media) {
	if (media) {
		auto bytes = media->bytes();
		if (!bytes.isEmpty()) {
			return bytes;
		}
	}
	// accessEnable() / accessDisable() and not a bare QFile: on sandboxed
	// macOS the path alone is not permission to read it.
	const auto &location = document->location(true);
	if (location.isEmpty() || !location.accessEnable()) {
		return QByteArray();
	}
	auto result = QByteArray();
	auto file = QFile(location.name());
	if (file.open(QIODevice::ReadOnly)) {
		result = file.readAll();
		file.close();
	}
	location.accessDisable();
	return result;
}

void StartTranslation(
		not_null<Ui::GenericBox*> box,
		not_null<Main::Session*> session,
		not_null<State*> state,
		const QString &transcript) {
	if (!VoiceToTextAutoTranslate()) {
		return;
	}
	const auto target = ReadingLanguage();
	if (target.isEmpty()) {
		return;
	}

	// The quota saver, and the only reason this is not just an unconditional
	// second request: a transcript that is already in the reading language
	// has nothing to translate, so nothing is spent. Recognize() is the same
	// detector HistoryView::TranslateTracker uses to decide whether to offer
	// translation at all.
	//
	// An unrecognised language never counts as a match - we would rather
	// spend one request than leave text on screen the user cannot read.
	const auto detected = Platform::Language::Recognize(transcript);
	if (detected.known()
		&& (BaseLanguageCode(detected.twoLetterCode())
			== BaseLanguageCode(target))) {
		return;
	}

	// The ordinary LuminaGram translation entry point, so the provider, key
	// and quota chosen on the translation settings page apply here unchanged
	// and no second API is wired up for voice. TranslateText() owns its
	// engine and releases it safely, so `done` may do anything.
	TranslateText(session, transcript, target, crl::guard(box, [=](
			TranslateResult result) {
		if (result.failed()) {
			// Fail-safe: the transcript is already on screen, so a failed
			// translation degrades to "transcript only" rather than to an
			// empty box.
			return;
		}
		const auto text = result.text.trimmed();
		if (text.isEmpty() || (text == transcript.trimmed())) {
			// The provider echoed the source back. Nothing worth a second
			// segment, and showing it would look like a broken translation.
			return;
		}
		state->translation = text;
	}));
}

void StartTranscription(
		not_null<Ui::GenericBox*> box,
		not_null<Main::Session*> session,
		not_null<State*> state,
		not_null<DocumentData*> document) {
	auto content = ReadContent(document, state->media);
	if (content.isEmpty()) {
		state->status = ErrorText(TranscribeError::Unavailable);
		return;
	}
	state->engine = MakeCurrentTranscribeEngine();
	if (!state->engine) {
		state->status = ErrorText(TranscribeError::Unavailable);
		return;
	}
	const auto roundVideo = document->isVideoMessage();
	state->status = Tr(u"LuminaSttUiTranscribing"_q);
	state->engine->transcribe({
		.content = std::move(content),
		.fileName = roundVideo ? u"round.mp4"_q : u"voice.ogg"_q,
		.mimeType = roundVideo ? u"video/mp4"_q : u"audio/ogg"_q,
		// The spoken-language hint for Apple's recogniser (mac). whisper
		// (Win/Linux) ignores it and auto-detects. Use the APP UI language, i.e.
		// the user's own language: it is right for the user's own voice and for a
		// same-language chat, and unlike the earlier choices it is stable. NOT
		// ReadingLanguage() (that is the TRANSLATE TARGET -- a chat set to Japanese
		// made Chinese voice transcribe as Japanese), and NOT empty (that let Apple
		// use the DEVICE locale, e.g. English, turning Chinese voice into English).
		.langHint = InterfaceLanguageCode(),
		.roundVideo = roundVideo,
	}, crl::guard(box, [=](TranscribeResult result) {
		if (result.failed()) {
			state->status = ErrorText(result.error);
		} else {
			const auto transcript = result.text.trimmed();
			state->status = QString();

			// Published first and unconditionally - see the header.
			state->transcript = transcript;
			StartTranslation(box, session, state, transcript);
		}

		// This runs inside the engine's own network reply, so dropping the
		// engine here would delete the reply and with it the lambda that is
		// currently executing. Release it from a later main-thread turn,
		// guarded by the box: if the box closed first this never runs, and
		// the engine died with the box anyway.
		crl::on_main(box, [=] {
			state->engine = nullptr;
		});
	}));
}

void StartWhenLoaded(
		not_null<Ui::GenericBox*> box,
		not_null<Main::Session*> session,
		not_null<State*> state,
		FullMsgId itemId) {
	const auto item = session->data().message(itemId);
	const auto document = VoiceToTextDocument(item);
	if (!document) {
		state->status = ErrorText(TranscribeError::Unavailable);
		return;
	}
	StartTranscription(box, session, state, document);
}

void Start(
		not_null<Ui::GenericBox*> box,
		not_null<Window::SessionController*> controller,
		not_null<State*> state,
		FullMsgId itemId) {
	const auto session = &controller->session();
	const auto item = session->data().message(itemId);
	const auto document = VoiceToTextDocument(item);
	if (!document) {
		state->status = ErrorText(TranscribeError::Unavailable);
		return;
	}
	state->media = document->createMediaView();
	if (state->media->loaded()) {
		StartTranscription(box, session, state, document);
		return;
	}

	// Not on this device yet. Android's v1 simply refused in this case; a
	// desktop client streams voice rather than downloading it, so refusing
	// would refuse most of the time. save() with an empty target name is the
	// same silent "into the cache / into a temp file" load the player uses,
	// and it never opens a file dialog.
	state->status = Tr(u"LuminaSttUiFetching"_q);
	state->downloadRequested = true;
	document->save(Data::FileOrigin(itemId), QString());
	if (!state->media->loaded() && !document->loading()) {
		// The loader could not even be started - no connection, or the
		// message no longer carries a usable file reference.
		state->status = ErrorText(TranscribeError::Network);
		return;
	}
	rpl::merge(
		session->downloaderTaskFinished(),
		session->data().documentLoadProgress() | rpl::to_empty
	) | rpl::on_next(crl::guard(box, [=] {
		// The flag and not lifetime.destroy(): this handler is owned by that
		// lifetime, so tearing it down from inside would delete the lambda
		// that is currently running. The subscription simply goes quiet and
		// dies with the box.
		if (!state->downloadRequested) {
			return;
		} else if (state->media->loaded()) {
			state->downloadRequested = false;
			state->status = QString();
			StartWhenLoaded(box, session, state, itemId);
		} else if (!document->loading()) {
			// Cancelled, or failed. Either way nothing else is coming.
			state->downloadRequested = false;
			state->status = ErrorText(TranscribeError::Network);
		}
	}), state->downloading);
}

void VoiceToTextBox(
		not_null<Ui::GenericBox*> box,
		not_null<Window::SessionController*> controller,
		FullMsgId itemId) {
	box->setTitle(TrValue(u"LuminaSttUiPickTitle"_q));

	const auto state = box->lifetime().make_state<State>();

	// Every section is slide-wrapped rather than emptied, so the box grows
	// from "Transcribing..." to transcript to transcript + translation
	// without ever leaving a hole where a section has no text yet.
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
	addText(&state->transcript)->setSelectable(true);

	const auto translated = box->addRow(
		object_ptr<Ui::SlideWrap<Ui::VerticalLayout>>(
			box,
			object_ptr<Ui::VerticalLayout>(box)),
		style::margins()
	)->toggleOn(
		state->translation.value() | rpl::map([](const QString &text) {
			return !text.isEmpty();
		})
	)->finishAnimating()->entity();
	Ui::AddSkip(translated);
	Ui::AddSubsectionTitle(translated, TrValue(u"LuminaSttUiTranslation"_q));
	translated->add(
		object_ptr<Ui::FlatLabel>(
			translated,
			state->translation.value(),
			st::boxLabel),
		st::boxRowPadding)->setSelectable(true);

	box->addButton(tr::lng_box_ok(), [=] {
		box->closeBox();
	});

	// One copy button for the whole answer: the transcript alone while that
	// is all there is, and both segments separated by a blank line once the
	// translation lands - which is exactly the string Android writes into the
	// bubble.
	const auto show = controller->uiShow();
	box->addLeftButton(tr::lng_mediaview_copy(), [=] {
		const auto transcript = state->transcript.current();
		if (transcript.isEmpty()) {
			return;
		}
		const auto translation = state->translation.current();
		TextUtilities::SetClipboardText(TextForMimeData::Simple(
			translation.isEmpty()
				? transcript
				: (transcript + u"\n\n"_q + translation)));
		show->showToast(tr::lng_text_copied(tr::now));
	});

	Start(box, controller, state, itemId);
}

// LuminaGram inline transcription (the on-bubble button target). The
// DURABLE result is written into Api::Transcribes (the stock inline slot the
// premium path renders); the machinery below owns only the TRANSIENT engine,
// media and download subscription for the duration of one run.

struct InlineJob {
	std::shared_ptr<Data::DocumentMedia> media;
	std::unique_ptr<TranscribeEngine> engine;
	rpl::lifetime downloading;
	bool downloadRequested = false;
};

// Session-scoped registry of in-flight jobs: keyed by message so a re-click
// cannot start a second run for the same bubble, and cleaned up with the
// account, which tears down any engine still in flight (destroying a
// TranscribeEngine cancels its request).
[[nodiscard]] base::flat_map<FullMsgId, std::unique_ptr<InlineJob>> &InlineJobs(
		not_null<Main::Session*> session) {
	static auto sessions = base::flat_map<
		Main::Session*,
		base::flat_map<FullMsgId, std::unique_ptr<InlineJob>>>();
	auto i = sessions.find(session.get());
	if (i == sessions.end()) {
		i = sessions.emplace(
			session.get(),
			base::flat_map<FullMsgId, std::unique_ptr<InlineJob>>()).first;
		session->lifetime().add([raw = session.get()] {
			sessions.remove(raw);
		});
	}
	return i->second;
}

// Drop the job - and with it the engine - from a LATER main-thread turn. It is
// called from inside the engine callback and the download subscription, both
// owned by the job; freeing the job inline would delete the object whose
// method is currently on the stack.
void FinishInlineJob(not_null<Main::Session*> session, FullMsgId itemId) {
	crl::on_main(session, [=] {
		InlineJobs(session).remove(itemId);
	});
}

void FailInline(
		base::weak_ptr<Window::SessionController> weak,
		not_null<Main::Session*> session,
		FullMsgId itemId,
		TranscribeError error) {
	// Revert the button to idle and say why in a toast, rather than parking an
	// error string under the bubble.
	if (const auto item = session->data().message(itemId)) {
		session->api().transcribes().luminaFailInline(item);
	}
	if (const auto controller = weak.get()) {
		controller->uiShow()->showToast(ErrorText(error));
	}
	FinishInlineJob(session, itemId);
}

void InlineTranslate(
		not_null<Main::Session*> session,
		FullMsgId itemId,
		const QString &transcript) {
	if (!VoiceToTextAutoTranslate()) {
		return;
	}
	const auto target = ReadingLanguage();
	if (target.isEmpty()) {
		return;
	}
	// Same quota saver as the box: a transcript already in the reading language
	// has nothing to translate. See StartTranslation above.
	const auto detected = Platform::Language::Recognize(transcript);
	if (detected.known()
		&& (BaseLanguageCode(detected.twoLetterCode())
			== BaseLanguageCode(target))) {
		return;
	}
	// TranslateText owns and releases its own engine; guard by the session so a
	// late reply cannot touch a closed account. The transcript is already inline
	// (luminaShowInline ran first), so any failure degrades to transcript-only.
	TranslateText(session, transcript, target, crl::guard(session, [=](
			TranslateResult result) {
		if (result.failed()) {
			return;
		}
		const auto text = result.text.trimmed();
		if (text.isEmpty() || (text == transcript.trimmed())) {
			return;
		}
		if (const auto item = session->data().message(itemId)) {
			// The two-segment layout the box copy button and Android use.
			session->api().transcribes().luminaShowInline(
				item,
				transcript + u"\n\n"_q + text);
		}
	}));
}

void InlineTranscribe(
		base::weak_ptr<Window::SessionController> weak,
		not_null<Main::Session*> session,
		not_null<InlineJob*> job,
		FullMsgId itemId,
		not_null<DocumentData*> document) {
	auto content = ReadContent(document, job->media);
	if (content.isEmpty()) {
		FailInline(weak, session, itemId, TranscribeError::Unavailable);
		return;
	}
	job->engine = MakeCurrentTranscribeEngine();
	if (!job->engine) {
		FailInline(weak, session, itemId, TranscribeError::Unavailable);
		return;
	}
	const auto roundVideo = document->isVideoMessage();
	job->engine->transcribe({
		.content = std::move(content),
		.fileName = roundVideo ? u"round.mp4"_q : u"voice.ogg"_q,
		.mimeType = roundVideo ? u"video/mp4"_q : u"audio/ogg"_q,
		// App UI language as the spoken-language hint - see the box path.
		.langHint = InterfaceLanguageCode(),
		.roundVideo = roundVideo,
	}, crl::guard(session, [=](TranscribeResult result) {
		if (result.failed()) {
			FailInline(weak, session, itemId, result.error);
			return;
		}
		const auto transcript = result.text.trimmed();
		if (const auto item = session->data().message(itemId)) {
			// Published first and unconditionally - a translation that never
			// returns can only leave the plain transcript on screen.
			session->api().transcribes().luminaShowInline(item, transcript);
			InlineTranslate(session, itemId, transcript);
		}
		// Free the engine from a later main turn; this runs inside the reply.
		FinishInlineJob(session, itemId);
	}));
}

void StartInline(
		base::weak_ptr<Window::SessionController> weak,
		not_null<Main::Session*> session,
		not_null<InlineJob*> job,
		FullMsgId itemId,
		not_null<DocumentData*> document) {
	job->media = document->createMediaView();
	if (job->media->loaded()) {
		InlineTranscribe(weak, session, job, itemId, document);
		return;
	}
	// Not on this device yet - stream it into the cache exactly as the box does
	// (save() with an empty target never opens a file dialog), then transcribe.
	job->downloadRequested = true;
	document->save(Data::FileOrigin(itemId), QString());
	if (!job->media->loaded() && !document->loading()) {
		FailInline(weak, session, itemId, TranscribeError::Network);
		return;
	}
	rpl::merge(
		session->downloaderTaskFinished(),
		session->data().documentLoadProgress() | rpl::to_empty
	) | rpl::on_next(crl::guard(session, [=] {
		if (!job->downloadRequested) {
			return;
		} else if (job->media->loaded()) {
			job->downloadRequested = false;
			const auto item = session->data().message(itemId);
			const auto doc = VoiceToTextDocument(item);
			if (!doc) {
				FailInline(weak, session, itemId, TranscribeError::Unavailable);
			} else {
				InlineTranscribe(weak, session, job, itemId, doc);
			}
		} else if (!document->loading()) {
			job->downloadRequested = false;
			FailInline(weak, session, itemId, TranscribeError::Network);
		}
	}), job->downloading);
}

} // namespace

bool VoiceToTextEnabled() {
	// Default ON, exactly as Android's LuminaVoiceToTextActivity defaults it.
	// The menu row is additionally gated on an engine that can actually run
	// (see AddVoiceToTextMenuRow), so an untouched desktop profile keeps the
	// stock message menu and never grows a row that could only fail.
	return Settings::Instance().getBool(kKeyEnabled, true);
}

void SetVoiceToTextEnabled(bool value) {
	Settings::Instance().set(kKeyEnabled, value);
}

rpl::producer<> VoiceToTextEnabledChanges() {
	return Settings::Instance().changesFor(kKeyEnabled);
}

bool VoiceToTextButtonAvailable() {
	if (!VoiceToTextEnabled()) {
		return false;
	}
	// On-device free engines (Apple on mac, whisper.cpp on Win/Linux) have
	// needsKey == false and qualify regardless of whether their model has been
	// downloaded - the click (ShowVoiceToText) fetches it on demand, so the
	// button is discoverable without a prior right-click transcription. A
	// needs-key cloud engine still requires its key. TranscriberConfigured()
	// is intentionally NOT used for the on-device case, to avoid its
	// WhisperModelReady() chicken-and-egg trap.
	return !CurrentTranscriber().needsKey
		|| TranscriberConfigured(CurrentTranscriberId());
}

bool VoiceToTextAutoTranslate() {
	return Settings::Instance().getBool(kKeyAutoTranslate, true);
}

void SetVoiceToTextAutoTranslate(bool value) {
	Settings::Instance().set(kKeyAutoTranslate, value);
}

rpl::producer<> VoiceToTextAutoTranslateChanges() {
	return Settings::Instance().changesFor(kKeyAutoTranslate);
}

DocumentData *VoiceToTextDocument(HistoryItem *item) {
	if (!item) {
		return nullptr;
	}
	const auto media = item->media();
	if (!media) {
		return nullptr;
	} else if (media->ttlSeconds() > 0) {
		// A self-destructing voice message. Uploading one to a third-party
		// transcription service is the one thing its sender asked us not to
		// do, and no setting makes that acceptable.
		return nullptr;
	}
	const auto document = media->document();
	if (!document) {
		return nullptr;
	}
	return (document->isVoiceMessage() || document->isVideoMessage())
		? document
		: nullptr;
}

void ShowVoiceToText(
		not_null<Window::SessionController*> controller,
		not_null<HistoryItem*> item) {
	controller->show(
		Box(VoiceToTextBox, controller, item->fullId()));
}

void ToggleVoiceToTextInline(
		not_null<Window::SessionController*> controller,
		not_null<HistoryItem*> item) {
	const auto session = &controller->session();
	auto &transcribes = session->api().transcribes();
	const auto &entry = transcribes.entry(item);

	// Free-path spinner already up (requestId sentinel) - ignore extra clicks.
	if (entry.requestId) {
		return;
	}
	// Already have a transcript - just flip inline visibility, never re-run.
	if (!entry.result.isEmpty()) {
		transcribes.luminaToggleInline(item);
		return;
	}
	// Nothing cached - run the free on-device engine.
	const auto document = VoiceToTextDocument(item);
	if (!document) {
		controller->uiShow()->showToast(
			ErrorText(TranscribeError::Unavailable));
		return;
	}
	const auto itemId = item->fullId();
	auto &jobs = InlineJobs(session);
	if (jobs.contains(itemId)) {
		// A previous run is still tearing down (deferred removal pending); the
		// next click after it clears starts fresh.
		return;
	}
	const auto roundview = document->isVideoMessage();
	const auto job = jobs.emplace(
		itemId,
		std::make_unique<InlineJob>()).first->second.get();
	transcribes.luminaStartInline(item, roundview);
	StartInline(
		base::make_weak(controller),
		session,
		job,
		itemId,
		document);
}

void AddVoiceToTextMenuRow(
		not_null<Ui::PopupMenu*> menu,
		const MessageMenuContext &context) {
	if (!VoiceToTextEnabled() || context.hasSelection) {
		return;
	}
	const auto item = context.item;

	// The one guard that makes this row voice-only: no voice note and no
	// round video means no row, on a text message as much as on a photo.
	if (!VoiceToTextDocument(item)) {
		return;
	}

	// Desktop's default engine is free and on-device (offline whisper.cpp /
	// Apple Speech), so a fresh profile is ready with no key. Two engines are
	// NOT ready, and they are handled differently:
	//  - a cloud engine (OpenAI Whisper / Google) the user picked but gave no
	//    key: we still offer the row, and on click say what is missing with a
	//    one-tap jump to the page that fixes it, rather than failing silently;
	//  - the offline engine with its model not yet downloaded: that funnel
	//    lives on the settings page, so the row stays hidden as before.
	if (!TranscriberConfigured(CurrentTranscriberId())
		&& !CurrentTranscriber().needsKey) {
		return;
	}
	const auto session = &item->history()->session();
	const auto itemId = item->fullId();
	const auto controller = context.controller;

	// The two menu builders own the menu they hand us, so they outlive it and
	// the raw capture is safe; the message is resolved again on click because
	// it can be deleted while the menu is open.
	menu->addAction(Tr(u"LuminaSttUiMenuItem"_q), crl::guard(controller, [=] {
		const auto item = session->data().message(itemId);
		if (!item) {
			return;
		}
		// Re-checked on click, not captured: the engine or its key can change
		// while the menu is open. The row is only offered for a ready engine
		// or a needs-key engine, so an unconfigured engine here always means
		// "no API key" - hence LuminaSttUiNoKey is the right message.
		if (!TranscriberConfigured(CurrentTranscriberId())) {
			controller->show(Ui::MakeConfirmBox({
				.text = Tr(u"LuminaSttUiNoKey"_q),
				.confirmed = [=](Fn<void()> close) {
					close();
					controller->showSettings(::Settings::LuminaVoiceId());
				},
				.confirmText = Tr(u"LuminaVoiceToTextTitle"_q),
			}));
			return;
		}
		ShowVoiceToText(controller, item);
	}), &st::menuIconTranslate);
}

} // namespace Lumina
