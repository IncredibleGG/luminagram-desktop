/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_brand.h"

#include "core/version.h"
#include "lang/lang_keys.h"
#include "lumina/lumina_locale.h"

namespace Lumina {
namespace {

// Every key here is the app speaking about itself. See lumina_brand.h for the
// rule that decides what may join this list and what may not.
constexpr ushort kBrandedKeys[] = {
	// Startup, updating, quitting, restarting.
	tr::lng_error_start_minimized_passcoded.base,
	tr::lng_new_version_wrap.base,
	tr::lng_outdated_now.base,
	tr::lng_outdated_soon.base,
	tr::lng_settings_add_sendto.base,
	tr::lng_settings_auto_start.base,
	tr::lng_settings_auto_start_disabled_uwp.base,
	tr::lng_sure_save_language.base,
	tr::lng_tray_icon_text.base,
	tr::lng_update_telegram.base,

	// Locking and unlocking this app on this computer.
	tr::lng_passcode_about.base,
	tr::lng_passcode_about2.base,
	tr::lng_passcode_about3.base,
	tr::lng_passcode_winhello_unlock.base,
	tr::lng_settings_use_applewatch_about.base,
	tr::lng_settings_use_systempwd_about.base,
	tr::lng_settings_use_touchid_about.base,
	tr::lng_settings_use_winhello_about.base,
	tr::lng_theme_editor_need_unlock.base,

	// Permissions the user grants to this app in the system settings, where
	// the entry to look for is named after this app.
	tr::lng_group_call_mac_access.base,
	tr::lng_group_call_mac_accessibility.base,
	tr::lng_group_call_mac_input.base,
	tr::lng_group_call_mac_recording.base,
	tr::lng_group_call_mac_screencast_access.base,
	tr::lng_no_mic_permission.base,

	// This build's own limits, storage and state.
	tr::lng_bad_photo.base,
	tr::lng_bot_share_location_unavailable.base,
	tr::lng_export_progress.base,
	tr::lng_local_storage_device_telegram.base,
	tr::lng_local_storage_device_usage.base,
	tr::lng_passport_app_out_of_date.base,
	tr::lng_proxy_unsupported.base,
	tr::lng_screen_reader_bar_text.base,
	tr::lng_screen_reader_confirm_text.base,
	tr::lng_settings_passkeys_unsigned_error.base,
	tr::lng_stories_unsupported.base,
};

[[nodiscard]] bool Branded(ushort key) {
	for (const auto branded : kBrandedKeys) {
		if (branded == key) {
			return true;
		}
	}
	return false;
}

} // namespace

QString BrandedLangValue(ushort key, QString value) {
	if (!Branded(key)) {
		return value;
	}
	// "Telegram Desktop" collapses to the whole app name rather than to
	// "LuminaGram Desktop", because this app is called LuminaGram and there is
	// no second edition of it to tell apart. The longer form has to go first,
	// or the shorter rule would leave a stray "Desktop" behind. All-lowercase
	// "telegram" is deliberately not rewritten: as a word it never appears on
	// its own, and as a substring it is almost always a telegram.org address
	// that a translation put there and that we cannot honestly redirect.
	const auto name = AppName.utf16();
	const auto shout = name.toUpper();
	value.replace(u"Telegram Desktop"_q, name);
	value.replace(u"TELEGRAM DESKTOP"_q, shout);
	value.replace(u"Telegram"_q, name);
	value.replace(u"TELEGRAM"_q, shout);
	return value;
}

QString BrandIntroAbout() {
	return Tr(u"LuminaBrandAboutIntro"_q);
}

TextWithEntities BrandAboutSummary(TextWithEntities apiLink) {
	auto result = TextWithEntities{
		Tr(u"LuminaBrandAboutSummaryPre"_q)
	};
	result.append(std::move(apiLink));
	result.append(Tr(u"LuminaBrandAboutSummaryPost"_q));
	return result;
}

} // namespace Lumina
