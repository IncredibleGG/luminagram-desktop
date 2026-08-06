/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_quick_replies_section.h"

#include "base/unique_qptr.h"
#include "lang/lang_keys.h"
#include "lumina/lumina_locale.h"
#include "lumina/lumina_quick_replies.h"
#include "settings/settings_common.h"
#include "ui/layers/generic_box.h"
#include "ui/rp_widget.h"
#include "ui/ui_utility.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/fields/input_field.h"
#include "ui/widgets/popup_menu.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"

#include "styles/style_layers.h"
#include "styles/style_menu_icons.h"
#include "styles/style_settings.h"
#include "styles/style_widgets.h"

#include <QtGui/QCursor>

namespace Settings {
namespace {

// Add (id == 0) or edit one template. Writes through the store and nothing
// else, so it never needs to know which list opened it: the page behind it
// rebuilds from Lumina::ReplyTemplateChanges().
//
// Delete lives here rather than in the row's context menu, exactly as it does
// on Android (LuminaQuickRepliesActivity.showEditDialog() puts it on the
// dialog's neutral button): the row is already one click away from this box,
// and a destructive action that closes the surface it was invoked from cannot
// pull the ground out from under itself.
void EditTemplateBox(
		not_null<Ui::GenericBox*> box,
		Lumina::ReplyTemplate entry) {
	const auto adding = !entry.id;
	box->setTitle(Lumina::TrValue(adding
		? u"LuminaReplyTemplatesAdd"_q
		: u"LuminaReplyTemplatesEdit"_q));
	box->setWidth(st::boxWideWidth);

	const auto field = box->addRow(object_ptr<Ui::InputField>(
		box,
		st::defaultInputField,
		Ui::InputField::Mode::MultiLine,
		Lumina::TrValue(u"LuminaReplyTemplatesPlaceholder"_q),
		entry.text));
	field->setMaxLength(Lumina::kReplyTemplateMaxLength);
	box->setFocusCallback([=] {
		field->setFocusFast();
	});

	const auto save = [=] {
		const auto text = field->getLastText().trimmed();
		if (text.isEmpty()) {
			field->showError();
			return;
		} else if (adding) {
			Lumina::AddReplyTemplate(text);
		} else {
			Lumina::UpdateReplyTemplate(entry.id, text);
		}
		box->closeBox();
	};
	field->submits() | rpl::on_next(save, field->lifetime());

	box->addButton(tr::lng_settings_save(), save);
	box->addButton(tr::lng_cancel(), [=] {
		box->closeBox();
	});
	if (!adding) {
		box->addLeftButton(tr::lng_box_delete(), [=] {
			Lumina::RemoveReplyTemplate(entry.id);
			box->closeBox();
		}, st::attentionBoxButton);
	}
}

void AppendTemplateRow(
		not_null<Ui::VerticalLayout*> container,
		not_null<Window::SessionController*> controller,
		const Lumina::ReplyTemplate &entry,
		bool canMoveUp,
		bool canMoveDown) {
	const auto id = entry.id;
	const auto button = AddButtonWithIcon(
		container,
		rpl::single(Lumina::ReplyTemplatePreview(entry.text)),
		st::settingsButtonNoIcon);
	button->setClickedCallback([=] {
		controller->show(Box(EditTemplateBox, entry));
	});
	if (!canMoveUp && !canMoveDown) {
		return;
	}

	// Android reorders by dragging the row. Desktop's equivalent of a long
	// press is a right click, and the two moves are all it carries - the list
	// rebuilds underneath the menu, and it is parented to the row, so the
	// rebuild takes the menu with it. That rebuild is postponed by the page,
	// which is what keeps this from deleting the menu inside its own handler.
	const auto menu = button->lifetime().make_state<
		base::unique_qptr<Ui::PopupMenu>>();
	button->events(
	) | rpl::filter([](not_null<QEvent*> e) {
		return e->type() == QEvent::ContextMenu;
	}) | rpl::on_next([=](not_null<QEvent*> e) {
		*menu = base::make_unique_q<Ui::PopupMenu>(
			button,
			st::popupMenuWithIcons);
		if (canMoveUp) {
			(*menu)->addAction(
				Lumina::Tr(u"LuminaReplyTemplatesMoveUp"_q),
				[=] { Lumina::MoveReplyTemplate(id, -1); },
				&st::menuIconReorder);
		}
		if (canMoveDown) {
			(*menu)->addAction(
				Lumina::Tr(u"LuminaReplyTemplatesMoveDown"_q),
				[=] { Lumina::MoveReplyTemplate(id, 1); },
				&st::menuIconReorder);
		}
		(*menu)->popup(QCursor::pos());
		e->accept();
	}, button->lifetime());
}

} // namespace

Type LuminaReplyTemplatesId() {
	return LuminaReplyTemplates::Id();
}

LuminaReplyTemplates::LuminaReplyTemplates(
	QWidget *parent,
	not_null<Window::SessionController*> controller)
: Section(parent, controller) {
	const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);
	setupContent(content);
	Ui::ResizeFitChild(this, content);
}

LuminaReplyTemplates::~LuminaReplyTemplates() = default;

rpl::producer<QString> LuminaReplyTemplates::title() {
	return Lumina::TrValue(u"LuminaReplyTemplatesTitle"_q);
}

void LuminaReplyTemplates::setupContent(
		not_null<Ui::VerticalLayout*> container) {
	const auto controller = this->controller();

	// The list is rebuilt wholesale, so it gets a layout of its own rather
	// than clearing the one the section was handed.
	const auto list = container->add(
		object_ptr<Ui::VerticalLayout>(container));

	const auto rebuild = container->lifetime().make_state<Fn<void()>>();
	*rebuild = [=] {
		const auto width = list->width();
		list->clear();

		// WithIds, not the plain read: every row hands its id to a click that
		// happens later, and an id minted per call would not survive the trip.
		const auto templates = Lumina::ReplyTemplatesWithIds();
		const auto count = int(templates.size());
		const auto full = (count >= Lumina::kMaxReplyTemplates);

		Ui::AddSkip(list);
		Ui::AddSubsectionTitle(
			list,
			Lumina::TrValue(u"LuminaReplyTemplatesList"_q));
		if (!full) {
			AddButtonWithIcon(
				list,
				Lumina::TrValue(u"LuminaReplyTemplatesAdd"_q),
				st::settingsButtonActive,
				{ &st::menuIconAdd }
			)->setClickedCallback([=] {
				controller->show(
					Box(EditTemplateBox, Lumina::ReplyTemplate()));
			});
		}
		for (auto i = 0; i != count; ++i) {
			AppendTemplateRow(
				list,
				controller,
				templates[i],
				(i > 0),
				(i + 1 < count));
		}
		Ui::AddSkip(list);
		Ui::AddDividerText(list, Lumina::TrValue(templates.empty()
			? u"LuminaReplyTemplatesEmpty"_q
			: full
			? u"LuminaReplyTemplatesFull"_q
			: u"LuminaReplyTemplatesInfo"_q));
		list->resizeToWidth(width);
	};
	(*rebuild)();

	// Driven by the store, not by whoever wrote to it, so a template added
	// from another window updates this page too. Always deferred:
	// Ui::VerticalLayout::clear() deletes its children immediately, and the
	// click that caused the write may still be on the stack inside one of the
	// rows about to go.
	rpl::merge(
		Lumina::ReplyTemplateChanges(),
		Lumina::LangChanges()
	) | rpl::on_next([=] {
		Ui::PostponeCall(list, [=] {
			(*rebuild)();
		});
	}, list->lifetime());
}

} // namespace Settings
