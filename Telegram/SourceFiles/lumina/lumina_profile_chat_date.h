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

// The "Created" row on a group or channel profile.
//
// The value is the `date` field of the chat / channel constructor, which
// tdesktop already stores as ChatData::date (data/data_chat.h:173) and
// ChannelData::date (data/data_channel.h:588). It is filled from the ordinary
// chat list traffic, so nothing is requested to build this row.
//
// THE LABEL IS ALWAYS "Created", AND THAT IS A DELIBERATE DIVERGENCE.
// Android relabels the same number "Joined" for a channel the user is a member
// of (ProfileActivity.java:13649-13650). There is nothing behind that
// relabelling: it is the same single field, the protocol carries no separate
// join date on this constructor, and there is no per-user component to it at
// all. Presenting one number under two different meanings is worse than
// presenting it under the one meaning it actually has, so this row says
// "Created" everywhere.
//
// (ChannelData::date is only assigned for a non-"min" channel, so a channel
// that has so far only been seen as the author of a forwarded message reads 0
// until it is loaded properly. 0 hides the row rather than showing 1970.)

// Preference `showChatDate`, Store::Prefs, default false. Same key and same
// default as Android (LuminaChatActivity.java:76).
[[nodiscard]] bool ChatCreationDateRowEnabled();
void SetChatCreationDateRowEnabled(bool value);

// Fires when the preference above changes, including through a whole-file
// restore by Settings::importAll().
[[nodiscard]] rpl::producer<> ChatCreationDateRowChanges();

// The creation timestamp of a group or channel, 0 when unknown or when the
// peer is neither.
[[nodiscard]] TimeId ChatCreationDate(not_null<PeerData*> peer);

// What the row shows. Empty - which makes the row slide itself away - when the
// preference is off or there is no date.
[[nodiscard]] QString ChatCreationDateText(not_null<PeerData*> peer);

// Adds the row through the F-05 seam. Call from Lumina::AddChatInfoRows(),
// passing its `topic` through: a forum topic's section describes the topic,
// not the peer, so it gets no peer-level row - the same rule the upstream
// location row follows at info_profile_actions.cpp:1832.
void AddChatCreationDateRow(
	const ProfileRowsContext &context,
	not_null<PeerData*> peer,
	Data::ForumTopic *topic);

} // namespace Lumina
