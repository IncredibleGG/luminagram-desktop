/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "base/basic_types.h"

namespace Ui {
class RpWidget;
class VerticalLayout;
} // namespace Ui

namespace Window {
class SessionController;
} // namespace Window

namespace Lumina {

// The first-run card: one welcome box, once per install, listing what this
// fork adds on top of Telegram Desktop.
//
// Android's version lives in LuminaGramSettingsActivity.onResume() and is
// gated on the LuminaConfig flag "onboardingShown"; this uses the same key
// name in the same role, so the two platforms can be compared without reading
// both UIs. Two things are carried over verbatim because they are the whole
// correctness of the feature:
//
//   * the flag is written only once the card has genuinely been on screen -
//     Android tests showDialog()'s return value, we hang the write on the
//     box's showFinishes(), which fires when the layer's show animation has
//     completed. Writing it at schedule time, or even at show() time, burns
//     the one card the user gets on a window that never presented it;
//   * the card is posted with a short delay rather than shown from inside the
//     page's construction, so the section that triggered it has finished
//     sliding in first. It is guarded on that page, which is the desktop
//     equivalent of Android's getParentActivity() == null check: navigate
//     away inside the delay and the card is simply not shown.
//
// It cannot appear before there is an account, because both entry points take
// a Window::SessionController and one of those exists only for a logged-in
// session - there is no such controller behind the intro/login screens. It is
// additionally not shown while the app sits behind the passcode lock, where
// the layer stack is not on screen and showFinishes() would still fire.
//
// *** THE COPY IS DESKTOP'S, NOT ANDROID'S ***
//
// Android's card advertises four features and one of them, the ghost-mode
// privacy block (appear offline / suppress typing / suppress read receipts),
// is parked project-wide on this fork and has no UI at all. Its slot is taken
// by the safety cluster that desktop does ship - link inspector, crypto-paste
// guard, photo location stripping. Do not re-translate the Android strings
// into this card; check the feature exists here first.

// The "About" block of the LuminaGram Tools sub-page, in the F-02 sub-page
// shape: one row that re-opens the welcome card on demand. It also arms the
// first-run card, so the card still appears for a user whose first stop is
// this page rather than the hub.
void AddOnboardingRows(
	not_null<Ui::VerticalLayout*> container,
	not_null<Window::SessionController*> controller);

// Arms the first-run card and nothing else - no rows, no visible effect on a
// user who has already seen it. Meant for one call at the top of the
// LuminaGram hub's setupContent(), which is where Android shows the card;
// `parent` is that page's content widget and owns the pending show.
void SetupFirstRunOnboarding(
	not_null<Ui::RpWidget*> parent,
	not_null<Window::SessionController*> controller);

} // namespace Lumina
