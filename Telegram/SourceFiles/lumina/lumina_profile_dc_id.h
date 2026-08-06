/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "lumina/lumina_profile_rows.h"

#include <rpl/producer.h>

#include <QtCore/QString>

class PeerData;

namespace Data {
class ForumTopic;
} // namespace Data

namespace Lumina {

// The "Datacenter" row, on user, group and channel profiles alike.
//
// WHAT THIS NUMBER IS, AND WHAT IT IS NOT. Telegram stores files on numbered
// datacenters, and the download location of a peer's profile photo names the
// one holding that photo. That is the number shown here, and it is the same
// number Android shows (ProfileActivity.java:10547-10560 reads
// `user.photo.dc_id` / `chat.photo.dc_id`).
//
// It is a property of the PHOTO, not of the account. It is commonly the dc the
// account was registered on, which is why it is often described as a rough
// geographic hint, but a photo can be stored elsewhere and an account can be
// migrated, so nothing about the owner can be concluded from it. A peer with
// no photo has no number and the row is not shown at all - which is the same
// condition Android uses (`getProfileDcId() > 0`).
//
// Purely local: the location is already in memory because the app has to know
// where to fetch the avatar from. Nothing is requested to build this row.

// Preference `showDcId`, Store::Prefs, default false. Same key and same
// default as Android (LuminaChatActivity.java:75).
[[nodiscard]] bool ProfileDcIdRowEnabled();
void SetProfileDcIdRowEnabled(bool value);

// Fires when the preference above changes, including through a whole-file
// restore by Settings::importAll().
[[nodiscard]] rpl::producer<> ProfileDcIdRowChanges();

// The datacenter of the peer's current profile photo, 0 when there is none.
[[nodiscard]] int ProfileDcId(not_null<PeerData*> peer);

// What the row shows. Empty - which makes the row slide itself away - when the
// preference is off or the peer has no photo.
[[nodiscard]] QString ProfileDcIdText(not_null<PeerData*> peer);

// Adds the row through the F-05 seam. Call from Lumina::AddUserInfoRows() with
// the default `topic`, and from Lumina::AddChatInfoRows() passing its own
// `topic` through: a forum topic's section describes the topic, not the peer,
// so it gets no peer-level row - the same rule the upstream location row
// follows at info_profile_actions.cpp:1832.
void AddProfileDcIdRow(
	const ProfileRowsContext &context,
	not_null<PeerData*> peer,
	Data::ForumTopic *topic = nullptr);

} // namespace Lumina
