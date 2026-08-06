/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include <rpl/producer.h>

namespace Lumina {

// Pause the media viewer's video while LuminaGram is in the background -
// Android's `autoPauseBgVideo` toggle (LuminaMediaActivity), ported.
//
// WHAT ANDROID DOES. PhotoViewer.onPause() pauses the video that is playing
// and remembers that it was the one who paused it (`pausedByBackground`);
// PhotoViewer.onResume() plays it again, unless the user paused it by hand in
// the meantime (`manuallyPaused`). It skips all of that when the video is in
// picture-in-picture or in the floating PipVideoOverlay: a video the user
// deliberately kept on screen while doing something else has to keep playing.
//
// WHAT "BACKGROUND" MEANS HERE. Android's onPause is the whole task leaving
// the foreground, so the desktop counterpart is the whole APPLICATION losing
// activation - Qt's applicationState() != Qt::ApplicationActive, which is what
// Core::App().appDeactivatedValue() reports. It is deliberately NOT the viewer
// window losing focus: window focus is also lost when the user clicks another
// LuminaGram window - the main window, to answer the message they were sent
// the video in - which is not leaving the app, and pausing there would make an
// entirely ordinary action stutter the video.
//
// The viewer being MINIMISED counts as background too, on its own, whether or
// not the application is still active: a minimised window shows nothing.
//
// WHAT IS DELIBERATELY EXEMPT.
//
//  - The viewer in WINDOWED mode, for the application-deactivated trigger.
//    The viewer opens full screen out of the box (Core::Settings'
//    `_mediaViewPosition` defaults to `.maximized = 2`, read by
//    OverlayWidget::initFullScreen()), so a windowed viewer is a state the
//    user asked for with the fullscreen toggle, and the usual reason to ask
//    for it is to keep watching in a corner while working elsewhere. Pausing
//    that is precisely the fight Android avoids with its picture-in-picture
//    check. Minimising a windowed viewer still pauses it.
//
//  - PICTURE-IN-PICTURE, the literal counterpart of Android's PipVideoOverlay
//    check. On desktop OverlayWidget::switchToPip() closes the overlay and
//    hands the stream to a Pip window, so this hook could not reach it in any
//    case; the flag is part of the state below so that the rule is written
//    down rather than left to depend on that.
//
//  - STORIES. Media::Stories::Controller already pauses on window inactivity -
//    its `_windowActive` input to updatePlayingAllowed() - and it owns the
//    resume decision jointly with layers, menus, tooltips and the caption
//    view. A second pauser sharing the same player would resume a story the
//    controller means to keep paused.
//
// WHAT IS NEVER TOUCHED. Music and voice messages play through
// Media::Player::instance(), not through the viewer, and nothing here calls
// into it: audio the user started in order to listen to it keeps playing while
// they work in another app, which is the entire point of it. Only the video
// inside the open media viewer is paused. A live video stream in the viewer
// (OverlayWidget::_videoStream) is untouched as well - it does not use the
// streamed player this hook works on, and a live stream cannot be resumed
// where it was left.
//
// BEHAVIOUR NEUTRALITY. The preference defaults to false, and while it is
// false ShouldPauseBackgroundVideo() is false for every possible state, so the
// viewer does exactly what stock tdesktop does.

// Preference `autoPauseBgVideo`, Store::Prefs, default false. Same key name as
// Android, so the two platforms can be compared without reading both UIs.
// Main thread only.
[[nodiscard]] bool AutoPauseBackgroundVideo();
void SetAutoPauseBackgroundVideo(bool value);
[[nodiscard]] rpl::producer<bool> AutoPauseBackgroundVideoValue();

// Everything about the viewer that the decision depends on. The viewer fills
// this in from what it already knows about itself, so that the policy lives
// here - in one readable place - instead of as a condition buried in an 8000
// line file.
struct ViewerState {
	bool applicationBackgrounded = false;
	bool minimized = false;
	bool windowed = false;
	bool pictureInPicture = false;
	bool stories = false;
};

// Reads the preference itself, so the viewer has exactly one thing to ask.
//
// This answers "should it be paused NOW", not "should it be paused because of
// this event": resuming is not the negation of it. The viewer resumes only a
// video it paused itself, and only while the user has not touched play/pause
// in between - see OverlayWidget::updateBackgroundVideoPause().
[[nodiscard]] bool ShouldPauseBackgroundVideo(ViewerState state);

// Core::App().appDeactivatedValue(), named for what this feature uses it for.
// Emits the current state immediately on subscription.
[[nodiscard]] rpl::producer<bool> ApplicationBackgroundedValue();

} // namespace Lumina
