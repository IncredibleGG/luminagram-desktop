/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_quick_replies.h"

#include "base/event_filter.h"
#include "base/random.h"
#include "base/unique_qptr.h"
#include "base/weak_ptr.h"
#include "lumina/lumina_locale.h"
#include "lumina/lumina_quick_replies_section.h"
#include "lumina/lumina_settings.h"
#include "ui/rp_widget.h"
#include "ui/widgets/fields/input_field.h"
#include "ui/widgets/popup_menu.h"
#include "window/window_session_controller.h"

#include "styles/style_widgets.h"

#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
#include <QtGui/QCursor>

#include <algorithm>
#include <utility>

namespace Lumina {
namespace {

const auto kKeyEnabled = u"showReplyTemplates"_q;

// The Android key name, kept so that a backup taken there and imported here
// (W6-E) lands on the templates instead of on a dead key.
const auto kKeyTemplates = u"quickReplies"_q;

const auto kFieldId = u"id"_q;
const auto kFieldText = u"text"_q;

// Settings::set() defaults to Store::Prefs and silently RELOCATES a key
// written through that default, so WriteTemplates() below is the only place in
// the tree that writes kKeyTemplates, and it always names the store.
constexpr auto kTemplatesStore = Store::Bookmarks;

[[nodiscard]] QJsonArray ReadTemplatesArray() {
	auto &settings = Settings::Instance();
	const auto array = settings.getArray(kKeyTemplates);
	if (!array.isEmpty()) {
		return array;
	}

	// Android stores this key as a *string* holding a JSON array of bare
	// template strings (LuminaQuickRepliesActivity.getTemplates()). A backup
	// taken there and imported into a desktop profile therefore arrives as a
	// string, and dropping it would look exactly like "my templates did not
	// come across".
	const auto raw = settings.getString(kKeyTemplates);
	if (raw.isEmpty()) {
		return QJsonArray();
	}
	const auto document = QJsonDocument::fromJson(raw.toUtf8());
	return document.isArray() ? document.array() : QJsonArray();
}

void WriteTemplates(const QJsonArray &array) {
	Settings::Instance().set(kKeyTemplates, array, kTemplatesStore);
}

[[nodiscard]] QString Normalized(const QString &text) {
	auto result = text.trimmed();
	if (result.size() > kReplyTemplateMaxLength) {
		result = result.left(kReplyTemplateMaxLength);
		if (!result.isEmpty() && result.back().isHighSurrogate()) {
			result.chop(1);
		}
		result = result.trimmed();
	}
	return result;
}

[[nodiscard]] auto FindById(std::vector<ReplyTemplate> &list, uint64 id) {
	return std::find_if(list.begin(), list.end(), [&](const auto &entry) {
		return (entry.id == id);
	});
}

// Ui::Menu renders '&' as a mnemonic marker and swallows it (ParseMenuItem,
// lib_ui/ui/widgets/menu/menu_action.cpp:17-40), so a template reading "Q&A"
// would show as "QA" with an underlined A. Doubling it renders one '&'.
[[nodiscard]] QString EscapedForMenu(const QString &text) {
	auto result = text;
	return result.replace(QChar(u'&'), u"&&"_q);
}

void InsertIntoField(
		not_null<Ui::InputField*> field,
		const QString &text) {
	if (text.isEmpty()) {
		return;
	}

	// Exactly HistoryWidget::supportInsertText() (history_widget.cpp:1596),
	// which is this tree's way of typing text into the composer for the user:
	// the cursor's own insert replaces the selection when there is one and
	// leaves the caret after the inserted text, which is what Android's
	// luminaInsertQuickReply() spells out by hand.
	field->setFocus();
	field->textCursor().insertText(text);
	field->ensureCursorVisible();
}

} // namespace

bool ReplyTemplatesEnabled() {
	return Settings::Instance().getBool(kKeyEnabled, true);
}

void SetReplyTemplatesEnabled(bool value) {
	Settings::Instance().set(kKeyEnabled, value, Store::Prefs);
}

uint64 MakeReplyTemplateId() {
	auto list = ReplyTemplates();
	while (true) {
		const auto result = base::RandomValue<uint64>();
		if (result && FindById(list, result) == list.end()) {
			return result;
		}
	}
}

std::vector<ReplyTemplate> ReplyTemplates() {
	const auto array = ReadTemplatesArray();
	auto result = std::vector<ReplyTemplate>();
	result.reserve(size_t(array.size()));
	for (const auto &entry : array) {
		auto parsed = ReplyTemplate();
		if (entry.isString()) {
			parsed.text = Normalized(entry.toString());
		} else if (entry.isObject()) {
			const auto object = entry.toObject();
			auto ok = false;
			const auto id = uint64(
				object.value(kFieldId).toString().toULongLong(&ok));
			parsed.id = ok ? id : 0;
			parsed.text = Normalized(object.value(kFieldText).toString());
		} else {
			continue;
		}
		if (parsed.text.isEmpty()) {
			continue;
		} else if (parsed.id
			&& (FindById(result, parsed.id) != result.end())) {
			parsed.id = 0;
		}
		result.push_back(std::move(parsed));
		if (int(result.size()) >= kMaxStoredReplyTemplates) {
			break;
		}
	}
	return result;
}

int ReplyTemplatesCount() {
	return int(ReplyTemplates().size());
}

std::vector<ReplyTemplate> ReplyTemplatesWithIds() {
	auto result = ReplyTemplates();
	const auto missing = std::any_of(
		result.begin(),
		result.end(),
		[](const ReplyTemplate &entry) { return !entry.id; });
	if (missing) {
		SetReplyTemplates(result);
		result = ReplyTemplates();
	}
	return result;
}

rpl::producer<> ReplyTemplateChanges() {
	return rpl::merge(
		Settings::Instance().changesFor(kKeyEnabled),
		Settings::Instance().changesFor(kKeyTemplates));
}

void SetReplyTemplates(std::vector<ReplyTemplate> list) {
	auto array = QJsonArray();
	auto written = std::vector<ReplyTemplate>();
	written.reserve(list.size());
	for (auto &entry : list) {
		entry.text = Normalized(entry.text);
		if (entry.text.isEmpty()) {
			continue;
		} else if (array.size() >= kMaxStoredReplyTemplates) {
			break;
		}
		while (!entry.id || FindById(written, entry.id) != written.end()) {
			entry.id = base::RandomValue<uint64>();
		}
		auto object = QJsonObject();
		object.insert(kFieldId, QString::number(entry.id));
		object.insert(kFieldText, entry.text);
		array.append(object);
		written.push_back(entry);
	}
	WriteTemplates(array);
}

void AddReplyTemplate(const QString &text) {
	const auto normalized = Normalized(text);
	if (normalized.isEmpty()) {
		return;
	}
	auto list = ReplyTemplates();
	if (int(list.size()) >= kMaxReplyTemplates) {
		return;
	}
	list.push_back(ReplyTemplate{
		.id = MakeReplyTemplateId(),
		.text = normalized,
	});
	SetReplyTemplates(std::move(list));
}

void UpdateReplyTemplate(uint64 id, const QString &text) {
	const auto normalized = Normalized(text);
	if (!id || normalized.isEmpty()) {
		return;
	}
	auto list = ReplyTemplates();
	const auto i = FindById(list, id);
	if (i == list.end()) {
		return;
	}
	i->text = normalized;
	SetReplyTemplates(std::move(list));
}

void RemoveReplyTemplate(uint64 id) {
	if (!id) {
		// A zero id is "not settled yet", not "the first unsettled entry" -
		// ReplyTemplates() hands one to every Android-imported entry, so
		// FindById(list, 0) would find and destroy an unrelated template.
		return;
	}
	auto list = ReplyTemplates();
	const auto i = FindById(list, id);
	if (i == list.end()) {
		return;
	}
	list.erase(i);
	SetReplyTemplates(std::move(list));
}

void MoveReplyTemplate(uint64 id, int delta) {
	if (!id) {
		return; // See RemoveReplyTemplate().
	}
	auto list = ReplyTemplates();
	const auto i = FindById(list, id);
	if (i == list.end()) {
		return;
	}
	const auto from = int(i - list.begin());
	const auto till = from + delta;
	if (till < 0 || till >= int(list.size())) {
		return;
	}
	std::swap(list[from], list[till]);
	SetReplyTemplates(std::move(list));
}

QString ReplyTemplatePreview(const QString &text) {
	auto result = text.simplified();
	if (result.size() > kReplyTemplatePreviewLength) {
		result = result.left(kReplyTemplatePreviewLength);
		if (!result.isEmpty() && result.back().isHighSurrogate()) {
			result.chop(1);
		}
		result = result.trimmed() + QChar(0x2026);
	}
	return result;
}

void InstallReplyTemplatesMenu(
		not_null<Ui::RpWidget*> button,
		not_null<Ui::InputField*> field,
		Window::SessionController *controller) {
	const auto menu = button->lifetime().make_state<
		base::unique_qptr<Ui::PopupMenu>>();
	const auto weak = base::make_weak(controller);
	base::install_event_filter(button, [=](not_null<QEvent*> e) {
		if (e->type() != QEvent::ContextMenu || !ReplyTemplatesEnabled()) {
			return base::EventFilterResult::Continue;
		}
		const auto list = ReplyTemplates();
		if (list.empty()) {
			// Nothing to offer, so the right click keeps doing what it does
			// in a stock build, which is nothing at all.
			return base::EventFilterResult::Continue;
		}
		// Deliberately the plain menu style and no icons: this is Android's
		// chooser, a list of the user's own sentences, and the with-icons
		// style would indent every one of them past an empty icon column.
		*menu = base::make_unique_q<Ui::PopupMenu>(button.get());
		for (const auto &entry : list) {
			const auto text = entry.text;
			(*menu)->addAction(
				EscapedForMenu(ReplyTemplatePreview(text)),
				[=] { InsertIntoField(field, text); });
		}
		if (weak.get()) {
			(*menu)->addSeparator();
			(*menu)->addAction(Tr(u"LuminaReplyTemplatesManage"_q), [=] {
				if (const auto strong = weak.get()) {
					strong->showSettings(
						::Settings::LuminaReplyTemplatesId());
				}
			});
		}
		(*menu)->popup(QCursor::pos());
		return base::EventFilterResult::Cancel;
	});
}

} // namespace Lumina
