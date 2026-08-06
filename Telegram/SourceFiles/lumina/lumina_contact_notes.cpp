/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_contact_notes.h"

#include "base/weak_ptr.h"
#include "data/data_changes.h"
#include "data/data_user.h"
#include "lang/lang_keys.h"
#include "lumina/lumina_contact_note_box.h"
#include "lumina/lumina_locale.h"
#include "lumina/lumina_settings.h"
#include "main/main_session.h"
#include "ui/widgets/labels.h"
#include "window/window_session_controller.h"

#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
#include <QtCore/QStringList>

namespace Lumina {
namespace {

const auto kEnabledKey = u"showContactNotes"_q;
const auto kDataKey = u"contactNotes"_q;
const auto kNoteField = u"note"_q;
const auto kTagsField = u"tags"_q;

// Every read goes straight to the store and there is deliberately no cache in
// between. A cache would have to be invalidated from Settings::changesFor(),
// and Settings fires that stream SYNCHRONOUSLY from set() - so whether a row
// redrawing on the same signal saw fresh or stale data would depend on which
// of the two subscriptions happened to be registered first. Reads here are
// cheap anyway: getObject() hands back the already-parsed, implicitly shared
// QJsonObject, and looking one id up in it is a binary search.
[[nodiscard]] QJsonObject All() {
	return Settings::Instance().getObject(kDataKey);
}

[[nodiscard]] QString KeyOf(not_null<UserData*> user) {
	const auto id = peerToUser(user->id).bare;
	return id ? QString::number(id) : QString();
}

[[nodiscard]] ContactNote Parse(const QJsonObject &entry) {
	return {
		.note = entry.value(kNoteField).toString(),
		.tags = entry.value(kTagsField).toString(),
	};
}

[[nodiscard]] QJsonObject Serialize(const ContactNote &note) {
	auto result = QJsonObject();
	result.insert(kNoteField, note.note);
	result.insert(kTagsField, note.tags);
	return result;
}

[[nodiscard]] QString NormalizeTags(const QString &tags) {
	auto result = QStringList();
	for (const auto &part : tags.split(QChar(u','))) {
		const auto trimmed = part.trimmed();
		if (!trimmed.isEmpty()) {
			result.push_back(trimmed);
		}
	}
	return result.join(u", "_q);
}

// Android's separator between the note and the tags - two spaces, a middle
// dot, two spaces - kept character for character so a summary reads the same
// on both clients.
[[nodiscard]] QString SummarySeparator() {
	return u"  "_q + QChar(0x00B7) + u"  "_q;
}

[[nodiscard]] QString ShortenNote(const QString &note) {
	auto result = note.simplified();
	if (result.size() <= kContactNoteSummaryMaxLength) {
		return result;
	}
	result = result.left(kContactNoteSummaryMaxLength);
	// Qt 5's non-const QString::back() yields a QCharRef, which has
	// no isHighSurrogate(); at() yields a QChar and does.
	if (!result.isEmpty()
		&& result.at(result.size() - 1).isHighSurrogate()) {
		result.chop(1);
	}
	return result.trimmed() + QChar(0x2026);
}

[[nodiscard]] rpl::producer<> RowRefreshes(not_null<UserData*> user) {
	// peerFlagsValue() emits at once, which is what gives every producer built
	// on this one its initial value. IsContact is in the set because both the
	// row's label and the presence of its note field depend on it: adding
	// somebody to your contacts hands the note back to Telegram's synced one.
	return rpl::merge(
		ContactNoteChanges(),
		ContactNotesEnabledChanges(),
		LangChanges(),
		user->session().changes().peerFlagsValue(
			user,
			Data::PeerUpdate::Flag::IsContact) | rpl::to_empty);
}

[[nodiscard]] QString RowText(not_null<UserData*> user) {
	if (!ContactNotesEnabled() || !ContactNoteRowAvailable(user)) {
		return QString();
	}
	const auto summary = ContactNoteSummary(user);
	if (!summary.isEmpty()) {
		return summary;
	}
	return ContactNoteFieldAvailable(user)
		? Tr(u"LuminaContactNoteEmpty"_q)
		: Tr(u"LuminaContactTagsEmpty"_q);
}

[[nodiscard]] rpl::producer<QString> RowLabel(not_null<UserData*> user) {
	return RowRefreshes(user) | rpl::map([=] {
		return ContactNoteFieldAvailable(user)
			? Tr(u"LuminaContactNote"_q)
			: Tr(u"LuminaContactTags"_q);
	});
}

} // namespace

bool ContactNotesEnabled() {
	return Settings::Instance().getBool(kEnabledKey, false);
}

void SetContactNotesEnabled(bool value) {
	Settings::Instance().set(kEnabledKey, value, Store::Prefs);
}

rpl::producer<> ContactNotesEnabledChanges() {
	return Settings::Instance().changesFor(kEnabledKey);
}

rpl::producer<> ContactNoteChanges() {
	return Settings::Instance().changesFor(kDataKey);
}

ContactNote ContactNoteFor(not_null<UserData*> user) {
	const auto key = KeyOf(user);
	return key.isEmpty()
		? ContactNote()
		: Parse(All().value(key).toObject());
}

void SetContactNote(not_null<UserData*> user, ContactNote note) {
	const auto key = KeyOf(user);
	if (key.isEmpty()) {
		return;
	}
	const auto normalized = ContactNote{
		.note = note.note.trimmed(),
		.tags = NormalizeTags(note.tags),
	};
	auto all = All();
	if (normalized.empty()) {
		if (!all.contains(key)) {
			return;
		}
		all.remove(key);
	} else {
		const auto value = Serialize(normalized);
		if (all.value(key).toObject() == value) {
			return;
		}
		all.insert(key, value);
	}
	if (all.isEmpty()) {
		Settings::Instance().remove(kDataKey);
	} else {
		// Named store, always: kDataKey lives in the private file and the
		// default argument of set() would relocate it into the plain one.
		Settings::Instance().set(kDataKey, all, Store::Private);
	}
}

int ContactNotesCount() {
	const auto all = All();
	auto result = 0;
	for (auto i = all.constBegin(); i != all.constEnd(); ++i) {
		if (!Parse(i.value().toObject()).empty()) {
			++result;
		}
	}
	return result;
}

void ClearContactNotes() {
	Settings::Instance().remove(kDataKey);
}

bool ContactNoteRowAvailable(not_null<UserData*> user) {
	return !user->isSelf();
}

bool ContactNoteFieldAvailable(not_null<UserData*> user) {
	// The last disjunct is what keeps the policy from hiding data: a note
	// written while the user was a stranger stays readable and deletable after
	// they become a contact.
	return user->isBot()
		|| !user->isContact()
		|| !ContactNoteFor(user).note.isEmpty();
}

QString ContactNoteSummary(not_null<UserData*> user) {
	const auto entry = ContactNoteFor(user);
	auto result = entry.note.isEmpty() ? QString() : ShortenNote(entry.note);
	if (!entry.tags.isEmpty()) {
		if (!result.isEmpty()) {
			result += SummarySeparator();
		}
		result += entry.tags;
	}
	return result;
}

void AddContactNoteRow(
		const ProfileRowsContext &context,
		not_null<UserData*> user) {
	if (!ContactNoteRowAvailable(user)) {
		return;
	}
	auto text = RowRefreshes(user) | rpl::map([=] {
		const auto value = RowText(user);
		return value.isEmpty() ? tr::marked() : tr::link(value);
	});
	const auto line = context.addInfoOneLine(
		RowLabel(user),
		std::move(text),
		QString());

	// The whole value is one link and the filter answers false for every
	// button, so nothing is ever handed to ActivateClickHandler - the
	// "internal:action" url exists only to make the text clickable, and is
	// never opened, dragged or copied. Right-click still reaches
	// contextMenuEvent(), which is a separate path, and text selection still
	// suppresses the click the way it does on every other profile row.
	const auto weak = base::make_weak(context.controller);
	line.text->setClickHandlerFilter([=](
			const ClickHandlerPtr &,
			Qt::MouseButton button) {
		if (button == Qt::LeftButton) {
			if (const auto strong = weak.get()) {
				ShowContactNoteBox(strong, user);
			}
		}
		return false;
	});
}

} // namespace Lumina
