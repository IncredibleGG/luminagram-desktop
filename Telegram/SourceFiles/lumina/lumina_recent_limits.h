/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include <rpl/producer.h>

namespace Lumina {

// Android's moreRecentStickers, ported.
//
// WHAT ANDROID DOES. MediaDataController.java:958 has one helper,
// `luminaBoost(base, floor) = enabled ? max(base, floor) : base`, and calls it
// at exactly four trim points: the recent-sticker cap in addRecentSticker()
// (floor 200), the saved-GIF cap in addRecentGif() (floor 500), and the two
// caps inside processLoadedRecentDocuments(), which is the routine that writes
// the web_recent_v3 cache (floors 500 for GIFs, 200 for stickers). The floors
// are raised, never lowered - a server limit larger than the floor still wins.
// The same two floors are used here, so the two clients agree on the numbers.
//
// WHERE THE FOUR SITES ARE ON DESKTOP, AND WHY ONLY THREE ARE PATCHED. This
// was checked by reading, not assumed:
//
//   1. Data::Stickers::incrementSticker(), data_stickers.cpp - trims the
//      legacy local recent pack (`cRefRecentStickers()`) once the cloud recent
//      set plus that pack exceeds MTP::ConfigFields::stickersRecentLimit
//      (server value, 30 by default). PATCHED. This one is purely local: the
//      pack is persisted inside the session settings blob under
//      dbiRecentStickers, no server ever sees it, so the extra entries survive
//      restarts.
//   2. Data::Stickers::addSavedGif(), data_stickers.cpp - trims _savedGifs at
//      Data::PremiumLimits::gifsCurrent(). PATCHED, see the toast note below.
//   3. Storage::Account::importOldRecentStickers(), storage_account.cpp - the
//      one-shot migration out of the pre-cloud _recentStickersKeyOld file caps
//      the pack it rebuilds at the same stickersRecentLimit. PATCHED, so a
//      migration performed while the preference is on does not silently
//      undercut site 1 for the rest of that account's life.
//   4. The fourth site is NOT a number and is deliberately left alone.
//      Stickers::gifsReceived() (and Stickers::specialSetReceived() for the
//      cloud recent sticker set) clears the local list and rebuilds it from
//      the server's answer, then calls writeSavedGifs(). So anything this
//      device kept beyond what the account actually holds is dropped by the
//      next successful messages.getSavedGifs, and the truncation is persisted.
//      Retaining the surplus there is not a two-line change and not a safe
//      one: Api::CountSavedGifsHash() hashes the whole local vector, so a
//      locally padded list can never match the server's hash again - every
//      poll would download the full list and log an API Error, forever. The
//      storage layer itself was cleared of suspicion at the same time:
//      writeSavedGifs()/readSavedGifs() and writeStickerSets()/
//      readStickerSets() carry the whole list, and kMaxSavedStickerSetsCount
//      is a sanity bound on the number of *sets*, not on stickers.
//
//      HOW SOON SITE 4 UNDOES THE PADDING - read this before believing the
//      GIF half does much. addSavedGif() itself ends with
//      setLastSavedGifsUpdate(0) and session->api().updateSavedGifs(), and
//      ApiWrap::requestSavedGifs() then sends messages.getSavedGifs
//      immediately with Api::CountSavedGifsHash() of the padded local list.
//      For an account already at its server GIF limit that hash cannot match,
//      so the server answers with the full list instead of
//      messages.savedGifsNotModified and gifsReceived() truncates us back
//      within one round trip. Two consequences: the surplus GIFs live for a
//      round trip, not for a session, and while the list is padded every
//      saved GIF costs a full list download rather than a not-modified. Both
//      only happen while the preference is on.
//
// SO WHAT DOES THE GIF HALF ACTUALLY BUY? Not "more GIFs than your account
// holds" - the server decides that and site 4 enforces it. What it buys is
// that this client stops throwing away GIFs the server still has: gifsCurrent()
// is `isPremium() ? gifsPremium() : gifsDefault()`, both read out of the app
// config with hard-coded fallbacks (200/400), so early in a session, or when
// the premium flag or the app config has not arrived yet, the stock code can
// pop a GIF the account was perfectly entitled to keep. The settings row says
// this in plain words instead of promising a bigger cloud list.
//
// THE PREMIUM UPSELL TOAST. data_stickers.cpp shows SavedGifsToast() exactly
// when addSavedGif() pops the tail. Raising the cap therefore silences it. That
// is intended, and it is not a limit bypass: messages.saveGif is still the
// server's call, and this fork cannot and does not raise anyone's account
// limit. Suppressing an upsell whose numbers (200/400) no longer describe the
// local cap (500) is the honest reading. The toast is kept byte-for-byte when
// the preference is off - which is the default - and SavedGifsLimit() returning
// its argument unchanged is exactly how the call site detects that case.
//
// BEHAVIOUR NEUTRALITY. The preference defaults to false, and with it false
// both functions below return their argument, so every patched site evaluates
// to the identical expression it had before. Android defaults this one to true;
// desktop deliberately does not, because the GIF half changes when a stock
// Premium prompt appears.
//
// HOW MUCH OF THIS IS ACTUALLY VISIBLE ON DESKTOP - read this before promising
// anything. Two more facts were established by reading the tree, and neither is
// true on Android:
//
//   * The recent sticker list a desktop user sees is the CLOUD recent set, and
//     nothing here caps it: incrementSticker() only ever pushes to its front,
//     and specialSetReceived() replaces it wholesale from the server. The pack
//     site 1 protects, cRefRecentStickers(), is the pre-cloud legacy pack, and
//     the only code that ever fills it is site 3's migration. On an account
//     that never migrated, that pack is empty and site 1 is inert.
//   * The sticker panel *displays* only the first kRecentDisplayLimit (20)
//     recents unless tdesktop's own experimental option
//     "unlimited-recent-stickers" is on (chat_helpers/stickers_list_widget.cpp,
//     collectRecentStickers()).
//
// So the change that would make "more recent stickers" mean something for a
// desktop user is that display cap, not a retention cap - and it lives in a
// file this item does not own. RecentStickersLimit() is shaped to be its hook:
// `RecentStickersLimit(kRecentDisplayLimit)` yields 20 with the preference off
// and 200 with it on, which is exactly the intended behaviour, with no second
// preference and no extra branch.
//
// Main thread only, like the rest of Lumina::Settings.

// The floors, matching MediaDataController.java:1024/1119/2093/2100.
inline constexpr auto kRecentStickersFloor = 200;
inline constexpr auto kSavedGifsFloor = 500;

// Preference `moreRecentStickers`, Store::Prefs, default false.
[[nodiscard]] bool MoreRecentEnabled();
void SetMoreRecentEnabled(bool value);
[[nodiscard]] rpl::producer<> MoreRecentChanges();

// Both take the limit tdesktop would have used and return the one to use.
// While the preference is off, and whenever the stock limit is already the
// larger of the two, they return `stockLimit` unchanged - so a call site can
// test `RecentStickersLimit(x) == x` to ask "is this still tdesktop's own
// limit?" without reading the preference a second time.
[[nodiscard]] int RecentStickersLimit(int stockLimit);
[[nodiscard]] int SavedGifsLimit(int stockLimit);

} // namespace Lumina
