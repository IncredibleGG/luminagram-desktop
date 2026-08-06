/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "base/basic_types.h"

#include <rpl/producer.h>

#include <QtCore/QString>

#include <vector>

namespace Ui {
class InputField;
class RpWidget;
} // namespace Ui

namespace Window {
class SessionController;
} // namespace Window

namespace Lumina {

// LuminaGram reply templates - Android's LuminaQuickRepliesActivity.
//
//
// WHY NOT "QUICK REPLIES"
//
// Telegram Desktop already ships a feature called exactly that: the Telegram
// Business quick replies (settings/business/settings_quick_replies.cpp), which
// are server-side shortcuts, hard premium-gated, and have their own composer
// syntax. Two things with one name is a support problem, so everything the
// user sees here says "reply templates". The files keep the Android name so
// that the two trees still line up when read side by side.
//
//
// WHAT A TEMPLATE IS
//
// A piece of plain text the user keeps on this device and inserts into the
// message field. Nothing is sent anywhere, nothing is registered with the
// server, and inserting one is exactly equivalent to typing the text: the
// message that eventually goes out is an ordinary message.
//
//
// IDENTITY
//
// A random non-zero uint64, minted on the first write and never reused. Text
// is NOT identity: Android compares by reference (indexOfIdentity()), which is
// a Java-only trick that has no equivalent here, and comparing by value would
// mean that editing a template into the text of another one merges the two,
// and that two templates with the same text can never be told apart. Every
// edit, move and delete addresses a template by its id.
//
// The id is stored as a decimal *string*, like every other 64-bit id this fork
// persists: JSON numbers are doubles, and a uint64 above 2^53 does not survive
// the round trip.
//
//
// WHERE IT IS PERSISTED
//
// Store::Bookmarks (tdata/luminagram_bookmarks.json), under Android's key name
// `quickReplies`. Store::Bookmarks is the file for the growable arrays - see
// lumina_text_replace.cpp, which explains the same choice - so that editing a
// template does not rewrite the file every other preference shares.
//
// Android writes that key as a *string* holding a JSON array of bare strings
// (LuminaQuickRepliesActivity.getTemplates()). A backup taken there and
// imported here (W6-E) therefore arrives in that shape, and it is read: bare
// strings are accepted and given ids on the next write, which is the same
// courtesy lumina_text_replace.cpp extends to Android's rules.
//
//
// HOW ONE IS INSERTED, AND WHY IT IS A RIGHT CLICK
//
// Android long-presses the emoji button. Desktop has no long press, so the
// same button answers a right click instead - QEvent::ContextMenu on
// Ui::EmojiButton, which is a plain RippleButton and neither consumes nor
// re-purposes that event today (verified: no contextMenuEvent() override and
// no context-menu policy anywhere in its chain). The menu is installed with
// base::install_event_filter, the pattern the send button already uses for its
// own right-click menu (menu/menu_send.cpp:858-864).
//
//
// BEHAVIOUR NEUTRALITY
//
// The menu is offered only when the user has at least one template, and the
// list is empty until the user creates one. So on a fresh profile, and on
// every profile that never opens this page, right-clicking the emoji button
// does exactly what it does today: nothing.
//
// That is a deliberate departure from Android, which opens the chooser on an
// empty list too, with "Manage templates" as its only row. Here that would
// mean every user in the world gets a new menu on a button they already use,
// for a feature they have never touched.

struct ReplyTemplate {
	uint64 id = 0;
	QString text;
};

// A template is a snippet, not a whole message; the shortest premium message
// limit is far above this. It bounds what one paste into the edit box can put
// into the pref file.
inline constexpr auto kReplyTemplateMaxLength = 4096;

// Where the "Add template" row stops. The chooser is a popup menu, and a menu
// with hundreds of rows is not a chooser.
inline constexpr auto kMaxReplyTemplates = 50;

// What survives a load / save round trip, so that an over-sized import is
// preserved rather than truncated behind the user's back.
inline constexpr auto kMaxStoredReplyTemplates = 500;

// How much of a template fits on a row before it is elided. A character count,
// not a dimension - nothing here is measured in pixels.
inline constexpr auto kReplyTemplatePreviewLength = 48;

// Whether the composer offers the templates at all. Opt-OUT, default true.
//
// The justification for the one default-true key in this item: with no
// templates stored - which is the state of every profile until the user adds
// one - this switch has no observable effect whatsoever, because the chooser
// is not offered for an empty list either way. It exists so that a user who
// has built a list can stop the composer from reacting to a right click
// without deleting the list, which is the only thing the switch can ever
// change. Android has no such switch and is always on.
[[nodiscard]] bool ReplyTemplatesEnabled();
void SetReplyTemplatesEnabled(bool value);

// Non-zero, and unique against the stored list.
[[nodiscard]] uint64 MakeReplyTemplateId();

// In the order they are shown in, which is the order the chooser offers them
// in. Reads only - never writes, because the composer chooser is on this path
// and a right click must not touch the disk.
//
// An id may be ZERO here: an entry that came from an Android backup, from a
// hand-edited file, or that duplicated another entry's id has none yet. Such
// an entry can be inserted (which needs only its text) but cannot be addressed
// by the mutators below.
[[nodiscard]] std::vector<ReplyTemplate> ReplyTemplates();
[[nodiscard]] int ReplyTemplatesCount();

// The same list with every id settled, writing them back when any was missing.
//
// Anything that hands ids to a later click must read through this, or an
// Android-imported list edits nothing: ids minted per call would differ
// between the read that built the row and the read the mutator does, and every
// lookup would miss. Costs one write on the first open after such an import,
// and nothing on every open after that. (lumina_text_replace.cpp's
// LoadRulesWithIds() is the same fix for the same reason.)
[[nodiscard]] std::vector<ReplyTemplate> ReplyTemplatesWithIds();

// Fires on any change to the list or to the switch, including a W6-E backup
// import.
[[nodiscard]] rpl::producer<> ReplyTemplateChanges();

// Normalises before writing: trims, drops empty templates, clamps to
// kReplyTemplateMaxLength, mints missing and duplicate ids, and keeps at most
// kMaxStoredReplyTemplates entries.
void SetReplyTemplates(std::vector<ReplyTemplate> list);

// Each is a read-modify-write of the whole list. Adding past
// kMaxReplyTemplates does nothing - the UI hides the row that would reach it.
// A zero id addresses nothing and is a no-op in all three: it means "not
// settled yet" (see ReplyTemplates() above), never "the first unsettled entry".
void AddReplyTemplate(const QString &text);
void UpdateReplyTemplate(uint64 id, const QString &text);
void RemoveReplyTemplate(uint64 id);

// `delta` is -1 for up and 1 for down. A move off either end does nothing.
void MoveReplyTemplate(uint64 id, int delta);

// One line, whitespace collapsed, elided to kReplyTemplatePreviewLength
// without splitting a surrogate pair. Shared by the settings list and the
// composer chooser so that a template reads the same in both.
[[nodiscard]] QString ReplyTemplatePreview(const QString &text);

// Puts the chooser on `button`'s right click and inserts into `field`.
//
// `controller` may be null - a composer outside a regular window has nowhere
// to push a settings page - and then the chooser simply carries no "Manage
// templates" row. It is held weakly either way.
//
// Safe to call once per composer, and only once: the filter it installs lives
// as long as the button.
void InstallReplyTemplatesMenu(
	not_null<Ui::RpWidget*> button,
	not_null<Ui::InputField*> field,
	Window::SessionController *controller);

} // namespace Lumina
