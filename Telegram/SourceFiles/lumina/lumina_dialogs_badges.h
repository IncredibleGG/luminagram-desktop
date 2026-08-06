/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include <rpl/producer.h>

#include <QtGui/QBrush>

class PeerData;
class UserData;

namespace Lumina {

// The two preferences behind the little dot on a private chat's avatar in the
// chat list (Android: DialogCell's online-dot block and getRecencyDotColor()).
//
//   chatListOnlineDot   default TRUE   the green "is online now" dot, opt-out
//   chatListRecencyDot  default false  colour the dot by how long ago the
//                                      contact was last seen, opt-in
//
// `chatListOnlineDot` is one of the three opt-out keys named in the foundation
// plan, and it is opt-out because the dot is what desktop shows today: the row
// exists to take something away, so its default has to be the current
// behaviour. `chatListRecencyDot` adds a badge that has never been drawn here
// before, so it defaults off like everything else in this port.
//
// THE BUCKETS, transcribed from DialogCell.getRecencyDotColor():
//
//   online now                 green   (this is the ordinary online dot, and
//                                       it stays gated on chatListOnlineDot)
//   last seen within an hour   yellow  #FFC107
//   last seen within a day     orange  #FF9800
//   older, or no timestamp     no dot
//
// "No timestamp" is the whole "recently / last week / last month / hidden"
// family. Android reads status.expires <= 0 for it; desktop's equivalent is
// Data::LastseenStatus::onlineTill() == 0, which is exactly the same set of
// statuses. Those users deliberately do not publish a last-seen time, so there
// is nothing to colour and no dot is drawn - this feature never turns a coarse
// status into a precise one.
//
// WHY THIS IS NOT JUST A COLOUR SWAP. Dialogs::Row draws its whole userpic,
// stories ring and corner badge into ONE cached QImage, and re-renders that
// frame only when a key changes (Row::paintUserpic). Nothing time-derived is in
// that key, so a frame painted while a contact was still "within an hour" would
// keep its yellow dot for as long as the row stayed in the list. Hence
// CornerBadgeDotCacheKeyPart(), which folds the bucket into the frame key the
// same way peer->messagesTTL() is already folded into it.
//
// Changing the key is still only half of it: a key nobody looks at changes
// nothing, and an idle chat list does not repaint. WatchCornerBadgeDot()
// therefore keeps, per session, the time at which each visible contact's dot is
// due to change colour, and at that moment fires the same
// PeerUpdate::Flag::OnlineStatus that Data::Session::watchForOffline() fires
// when somebody goes offline - which is the update Dialogs::InnerWidget already
// listens to in order to repaint exactly that corner. The registration is
// refreshed from the paint path, so a row that scrolls out of the list simply
// stops being watched.
//
// BEHAVIOUR NEUTRALITY. With both preferences at their defaults every entry
// point here is exactly what it replaced: UserChatListDot() reduces to
// Data::IsUserOnline(), the cache key contribution is 0, the brush is
// st::dialogsOnlineBadgeFg, and the watcher returns on its first line.

enum class ChatListDot : uchar {
	None,
	Online,
	WithinHour,
	WithinDay,
};

// Preference `chatListOnlineDot`, Store::Prefs, default TRUE. Main thread only.
[[nodiscard]] bool ChatListOnlineDot();
void SetChatListOnlineDot(bool value);
[[nodiscard]] rpl::producer<bool> ChatListOnlineDotValue();

// Preference `chatListRecencyDot`, Store::Prefs, default false. Same key name
// as Android. Main thread only.
[[nodiscard]] bool ChatListRecencyDot();
void SetChatListRecencyDot(bool value);
[[nodiscard]] rpl::producer<bool> ChatListRecencyDotValue();

// Which dot `user` should get right now, honouring both preferences.
// `now` is a unixtime; pass 0 to have it resolved here.
[[nodiscard]] ChatListDot UserChatListDot(
	not_null<UserData*> user,
	TimeId now = 0);

// Whether any dot at all is shown - the predicate that replaces
// Data::IsUserOnline() in Dialogs::Row's corner-badge layer decision. Note that
// the layer it selects is shared with the linked-community and channel call /
// subscription badges, so only the online disjunct may be routed through here.
[[nodiscard]] bool CornerBadgeDotShown(
	not_null<UserData*> user,
	TimeId now = 0);

// To be added into Dialogs::Row's cached-frame key. Zero unless a recency
// bucket is in effect, so the key is bit-identical to the stock one while the
// preference is off.
[[nodiscard]] uint64 CornerBadgeDotCacheKeyPart(PeerData *peer);

// The fill for the corner dot. Falls back to tdesktop's own online-badge colour
// for everything that is not a recency bucket - including channels, whose call
// badge shares the same paint block.
[[nodiscard]] QBrush CornerBadgeDotBrush(PeerData *peer, bool active);

// Keeps `user`'s dot up to date. Cheap and safe to call from the paint path;
// returns immediately while the recency preference is off.
void WatchCornerBadgeDot(not_null<UserData*> user, TimeId now);

} // namespace Lumina
