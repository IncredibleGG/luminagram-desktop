/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "info/profile/info_profile_text.h"
#include "ui/text/text_variant.h"

class PeerData;
class UserData;

namespace Data {
class ForumTopic;
} // namespace Data

namespace Ui {
class VerticalLayout;
} // namespace Ui

namespace Window {
class SessionController;
} // namespace Window

namespace Lumina {

// The single seam LuminaGram has into the profile "details" block built by
// Info::Profile::DetailsFiller::makeInfo() (info/profile/info_profile_actions
// .cpp). That file is 3000+ lines of upstream-hot code, so it holds exactly two
// calls into this header and nothing else; every LuminaGram profile row is
// added from here instead.
//
// Mirrors the `addInfoOneLine` factory local to makeInfo(). Rows created
// through it are added to the same Ui::VerticalLayout and tracked by the same
// Ui::MultiSlideTracker as their upstream neighbours, so a row whose text
// producer emits an empty value slides itself away, and the details block as a
// whole still collapses when every row inside it is empty.
//
// Arguments are (label, text, contextCopyText); the padding and popup menu
// style arguments of the upstream factory keep their defaults.
//
// !! CALL-SCOPED ONLY. The wrapped lambda holds references into makeInfo()'s
// stack frame - in particular to its local Ui::MultiSlideTracker, which is
// destroyed as soon as makeInfo() returns. Call it synchronously from
// AddUserInfoRows() / AddChatInfoRows() and never copy it into a click
// handler, an rpl subscription, a lifetime-owned state or any other callback
// that runs later.
using InfoOneLineFactory = Fn<Info::Profile::TextWithLabel(
	v::text::data label,
	rpl::producer<TextWithEntities> text,
	QString contextCopyText)>;

// Also call-scoped: makeInfo() passes a temporary, so the reference dies at
// the end of the call. Copy out the individual members you need to keep - both
// not_null pointers outlive the section, `addInfoOneLine` does not.
struct ProfileRowsContext {
	// The details block's own Ui::VerticalLayout.
	//
	// Prefer `addInfoOneLine` for anything row-shaped. A widget added straight
	// to `container` is NOT registered with makeInfo()'s MultiSlideTracker,
	// and the whole block is toggled on `tracker.atLeastOneShownValue()` - so
	// on a peer where every upstream row happens to be empty an untracked
	// Lumina row is built but never shown. If a future row really cannot go
	// through the factory, wrap it in a Ui::SlideWrap and extend this struct
	// with a tracking callback rather than adding it bare.
	not_null<Ui::VerticalLayout*> container;
	not_null<Window::SessionController*> controller;
	InfoOneLineFactory addInfoOneLine;
};

// Both functions are called once per built details section, from the matching
// branch of makeInfo().
//
// Rows added here must be reactive exactly the way their neighbours are: build
// the text producer from `peer->session().changes().peerFlagsValue(...)` (or
// another rpl source) rather than from a value computed once, or the row will
// never update after the peer is refreshed from the server, and an initially
// empty row will stay hidden forever.

// The user branch. Deliberately called for bots as well - private contact
// notes have to be available on bot profiles too - so a row that does not
// apply to bots must check `user->isBot()` itself. Likewise it is called for
// `user->isSelf()` and for Saved Messages sublist sections (where makeInfo()
// builds the details of `sublist->sublistPeer()`); a row that should not show
// up there has to test for it.
void AddUserInfoRows(
	const ProfileRowsContext &context,
	not_null<UserData*> user);

// The chat / channel branch. `topic` is not null when the section describes a
// forum topic rather than the peer itself; peer-level rows must skip that case,
// the same way the upstream location row does.
void AddChatInfoRows(
	const ProfileRowsContext &context,
	not_null<PeerData*> peer,
	Data::ForumTopic *topic);

} // namespace Lumina
