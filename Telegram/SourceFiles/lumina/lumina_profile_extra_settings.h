/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "base/basic_types.h"

namespace Ui {
class VerticalLayout;
} // namespace Ui

namespace Window {
class SessionController;
} // namespace Window

namespace Lumina {

// The three extra profile rows on the LuminaGram privacy sub-page, in the F-02
// sub-page shape: settings/sections/settings_lumina_privacy.cpp holds this one
// call and nothing else for W6-B.
//
// One toggle each for the registration date, the datacenter and the group /
// channel creation date. Android splits them across two of its pages - the
// registration date sits under Privacy > Profile and the other two under
// Chats > Info density - but all three add a line to the same profile block,
// so on desktop they are one group. What each row means, and where its number
// comes from, is documented on the preference in lumina_registration_date.h,
// lumina_profile_dc_id.h and lumina_profile_chat_date.h.
//
// All three default to off, so a fresh install shows the profile exactly as
// stock tdesktop does.
void AddProfileExtraRows(
	not_null<Ui::VerticalLayout*> container,
	not_null<Window::SessionController*> controller);

} // namespace Lumina
