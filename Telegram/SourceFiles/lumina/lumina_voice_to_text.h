/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "base/basic_types.h"

#include <rpl/producer.h>

class DocumentData;
class HistoryItem;

namespace Ui {
class PopupMenu;
} // namespace Ui

namespace Window {
class SessionController;
} // namespace Window

namespace Lumina {

struct MessageMenuContext;

// LuminaGram voice-to-text: transcribe a received voice note or round video
// with the user's own engine, then carry straight on into translation.
//
// This is the desktop port of Android's LuminaVoiceToText, and the pipeline is
// the same one: resolve the audio -> run the selected engine
// (lumina/lumina_transcribers.h) -> publish the transcript -> when it is not
// already in the reading language, translate it through the ordinary
// LuminaGram translation entry point and publish both segments.
//
// Fail-safe by construction: the transcript is published FIRST and
// unconditionally, so a translation that errors out, rate-limits or never
// returns can only leave the user with the plain transcript.
//
// THREE DELIBERATE DIFFERENCES FROM ANDROID, all forced by the platform:
//
//  * No offline engine. See lumina/lumina_transcribers.h - Vosk is a native
//    library plus a 50 MB model on three platforms, and the settings page
//    says out loud that it is not here rather than pretending otherwise.
//
//  * The transcript is shown in a box, not inline under the bubble. Android
//    writes into TLRPC.Message.voiceTranscription and reuses Telegram's own
//    render path; desktop has no equivalent seam, because Api::Transcribes
//    keeps its results in a private map reachable only through its own MTProto
//    request, so we do NOT reuse that inline slot for the transcript itself.
//    We DO reuse the stock on-bubble transcribe button: countOptimalSize() in
//    history/view/media/history_view_document.cpp (voice notes) and
//    ensureTranscribeButton() in history/view/media/history_view_gif.cpp
//    (round videos) are edited to un-gate that button for on-device free
//    engines - visible even before the model is downloaded, see
//    VoiceToTextButtonAvailable() - and to reroute its click to
//    ShowVoiceToText() below. One left-click on the bubble opens this box; no
//    Premium, no trial, and no api/api_transcribes.{h,cpp} change. A box is
//    the same shape tdesktop already uses for on-demand translation.
//
//  * The chat's own translation is never deferred to. Android skips its
//    translation step when the dialog is already being translated, because
//    TranslateController picks the transcript up out of the message itself.
//    Nothing on desktop can see a transcript that lives in our box, so
//    deferring would leave the user with an untranslated transcript and a
//    chat translation that never fires.

// Master switch, Android's `voiceToTextEnabled`, default ON as there.
[[nodiscard]] bool VoiceToTextEnabled();
void SetVoiceToTextEnabled(bool value);
[[nodiscard]] rpl::producer<> VoiceToTextEnabledChanges();

// True when the on-bubble transcribe button should be offered for the current
// engine. Deliberately LOOSER than TranscriberConfigured(): an on-device free
// engine (Apple on mac, whisper.cpp on Win/Linux; needsKey == false) qualifies
// even when its model is not downloaded yet, because the click path
// (ShowVoiceToText) fetches the model on demand - otherwise the button would
// stay hidden until the user had already transcribed once via the right-click
// menu. A needs-key cloud engine still requires its key, so the button never
// offers a control that could only fail silently.
[[nodiscard]] bool VoiceToTextButtonAvailable();

// Android's `sttAutoTranslate`, default ON as there.
[[nodiscard]] bool VoiceToTextAutoTranslate();
void SetVoiceToTextAutoTranslate(bool value);
[[nodiscard]] rpl::producer<> VoiceToTextAutoTranslateChanges();

// The voice note / round video of `item`, or null when it has none, when it
// is a self-destructing one (whose media must not be uploaded anywhere), or
// when `item` itself is null.
[[nodiscard]] DocumentData *VoiceToTextDocument(HistoryItem *item);

// Downloads the media if needed, transcribes it, translates the transcript
// when that is called for, and shows all of it - including every failure - in
// one box. Safe to call for any item; it answers with an error box rather
// than doing anything drastic.
void ShowVoiceToText(
	not_null<Window::SessionController*> controller,
	not_null<HistoryItem*> item);

// The F-04 row. Signature matches the AddVoiceToTextRow stub in
// lumina/lumina_message_menu.h.
void AddVoiceToTextMenuRow(
	not_null<Ui::PopupMenu*> menu,
	const MessageMenuContext &context);

} // namespace Lumina
