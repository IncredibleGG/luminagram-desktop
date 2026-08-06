/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_text_replace_settings.h"

#include "lang/lang_keys.h"
#include "lumina/lumina_text_replace.h"
#include "settings/settings_common.h"
#include "ui/layers/generic_box.h"
#include "ui/layers/show.h"
#include "ui/ui_utility.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/fields/input_field.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"

#include "styles/style_layers.h"
#include "styles/style_menu_icons.h"
#include "styles/style_settings.h"
#include "styles/style_widgets.h"

namespace Lumina {
namespace {

// How much of a rule fits on one list row before it is elided. A character
// count, not a dimension - nothing here is measured in pixels.
constexpr auto kPreviewMaxLength = 40;

[[nodiscard]] rpl::producer<QString> LabelValue(Fn<QString()> compute) {
	return rpl::single(
		rpl::empty
	) | rpl::then(
		TextReplaceChanges()
	) | rpl::map([compute = std::move(compute)] {
		return compute();
	});
}

[[nodiscard]] rpl::producer<bool> FlagValue(Fn<bool()> compute) {
	return rpl::single(
		rpl::empty
	) | rpl::then(
		TextReplaceChanges()
	) | rpl::map([compute = std::move(compute)] {
		return compute();
	});
}

[[nodiscard]] QString Preview(const QString &text) {
	auto result = text;
	result.replace(QChar(u'\n'), QChar(u' '));
	result.replace(QChar(u'\r'), QChar(u' '));
	result = result.trimmed();
	if (result.isEmpty()) {
		return u"(nothing)"_q;
	} else if (result.size() > kPreviewMaxLength) {
		return result.left(kPreviewMaxLength) + QChar(0x2026);
	}
	return result;
}

// Rules restored from a backup written before rules had ids come back with an
// empty id, and the list below is keyed by id. Give them one and write it
// straight back, so that identity is settled before the user can click
// anything. Costs a write only on the first open after such a restore.
[[nodiscard]] std::vector<TextReplaceRule> LoadRulesWithIds() {
	auto rules = TextReplaceRules();
	const auto missing = ranges::any_of(rules, [](const auto &rule) {
		return rule.id.isEmpty();
	});
	if (missing) {
		SetTextReplaceRules(rules);
		rules = TextReplaceRules();
	}
	return rules;
}

// Writes through Lumina::SetTextReplaceRules() and nothing else: the list
// behind it rebuilds from TextReplaceChanges(), so this box never needs to
// know who opened it or hold a pointer back into it.
void EditRuleBox(not_null<Ui::GenericBox*> box, TextReplaceRule rule) {
	const auto adding = rule.id.isEmpty();
	box->setTitle(rpl::single(adding
		? u"Add rule"_q
		: u"Edit rule"_q));

	const auto from = box->addRow(object_ptr<Ui::InputField>(
		box,
		st::defaultInputField,
		Ui::InputField::Mode::SingleLine,
		rpl::single(u"Replace"_q),
		rule.from));
	from->setMaxLength(kTextReplaceFromMaxLength);

	const auto to = box->addRow(object_ptr<Ui::InputField>(
		box,
		st::defaultInputField,
		Ui::InputField::Mode::SingleLine,
		rpl::single(u"With"_q),
		rule.to));
	to->setMaxLength(kTextReplaceToMaxLength);

	box->setFocusCallback([=] {
		from->setFocusFast();
	});

	const auto save = [=] {
		const auto fromText = from->getLastText().trimmed();
		if (fromText.isEmpty()) {
			from->showError();
			return;
		}
		const auto toText = to->getLastText();
		auto rules = TextReplaceRules();
		if (adding) {
			rules.push_back(TextReplaceRule{
				.id = MakeTextReplaceRuleId(),
				.from = fromText,
				.to = toText,
			});
		} else {
			const auto i = ranges::find(
				rules,
				rule.id,
				&TextReplaceRule::id);
			if (i == end(rules)) {
				return;
			}
			i->from = fromText;
			i->to = toText;
		}
		SetTextReplaceRules(std::move(rules));
		box->closeBox();
	};
	from->submits() | rpl::on_next(save, from->lifetime());
	to->submits() | rpl::on_next(save, to->lifetime());

	box->addButton(tr::lng_settings_save(), save);
	box->addButton(tr::lng_cancel(), [=] {
		box->closeBox();
	});
	if (!adding) {
		box->addLeftButton(tr::lng_box_delete(), [=] {
			auto rules = TextReplaceRules();
			const auto i = ranges::find(
				rules,
				rule.id,
				&TextReplaceRule::id);
			if (i != end(rules)) {
				rules.erase(i);
				SetTextReplaceRules(std::move(rules));
			}
			box->closeBox();
		}, st::attentionBoxButton);
	}
}

void RulesBox(not_null<Ui::GenericBox*> box) {
	box->setStyle(st::layerBox);
	box->setWidth(st::boxWideWidth);
	box->setTitle(rpl::single(u"Text replacer"_q));

	// The list is rebuilt wholesale, so it gets a layout of its own rather
	// than clearing the box's - GenericBox owns its content layout and wraps
	// it in an Ui::OverrideMargins during prepare().
	const auto content = box->verticalLayout()->add(
		object_ptr<Ui::VerticalLayout>(box));

	const auto rebuild = std::make_shared<Fn<void()>>();
	*rebuild = [=] {
		const auto width = content->width();
		content->clear();

		const auto rules = LoadRulesWithIds();
		const auto full = (int(rules.size()) >= kTextReplaceMaxRules);

		Ui::AddSkip(content);
		Ui::AddSubsectionTitle(content, rpl::single(u"Rules"_q));
		if (!full) {
			const auto add = ::Settings::AddButtonWithIcon(
				content,
				rpl::single(u"Add rule"_q),
				st::settingsButtonActive,
				{ &st::menuIconAdd });
			add->setClickedCallback([=] {
				box->uiShow()->show(Box(EditRuleBox, TextReplaceRule()));
			});
		}
		for (const auto &rule : rules) {
			const auto button = ::Settings::AddButtonWithLabel(
				content,
				rpl::single(Preview(rule.from)),
				rpl::single(Preview(rule.to)),
				st::settingsButtonNoIcon);
			button->setClickedCallback([=] {
				box->uiShow()->show(Box(EditRuleBox, rule));
			});
		}
		Ui::AddSkip(content);
		Ui::AddDividerText(content, rpl::single(rules.empty()
			? u"No rules yet. Add one to start replacing text in the "
				"messages you send."_q
			: full
			? u"Rules are applied in order, from the top. The list is "
				"full - delete a rule to add another."_q
			: u"Rules are applied in order, from the top, and each one "
				"works on what the one above it produced."_q));
		content->resizeToWidth(width);
	};
	(*rebuild)();

	// The rebuild is driven by the store, not by the boxes that write to it,
	// and it is always deferred: Ui::VerticalLayout::clear() deletes its
	// children immediately, and the click that caused the write may still be
	// on the stack inside one of the rows about to go. The subscription lives
	// on `content`, so nothing here outlives the list it rebuilds.
	TextReplaceChanges(
	) | rpl::on_next([=] {
		Ui::PostponeCall(content, [=] {
			(*rebuild)();
		});
	}, content->lifetime());

	box->addButton(tr::lng_close(), [=] {
		box->closeBox();
	});
}

} // namespace

void AddTextReplaceRows(
		not_null<Ui::VerticalLayout*> container,
		not_null<Window::SessionController*> controller) {
	Ui::AddSkip(container);
	Ui::AddSubsectionTitle(container, rpl::single(u"Text replacer"_q));

	const auto toggle = container->add(object_ptr<Ui::SettingsButton>(
		container,
		rpl::single(u"Replace text in messages I send"_q),
		st::settingsButtonNoIcon
	))->toggleOn(FlagValue([] { return TextReplaceEnabled(); }));
	toggle->toggledChanges(
	) | rpl::on_next([](bool value) {
		SetTextReplaceEnabled(value);
	}, toggle->lifetime());

	::Settings::AddButtonWithLabel(
		container,
		rpl::single(u"Rules"_q),
		LabelValue([] {
			const auto count = int(TextReplaceRules().size());
			return count ? QString::number(count) : u"None"_q;
		}),
		st::settingsButtonNoIcon
	)->setClickedCallback([=] {
		controller->show(Box(RulesBox));
	});

	Ui::AddSkip(container);
	Ui::AddDividerText(
		container,
		rpl::single(u"Swap a shorthand for the phrase you meant in every "
			"message you send - \"brb\" becomes \"be right back\". Matching "
			"is case-sensitive and matches whole words. A message that "
			"carries any formatting is sent exactly as typed, and links, "
			"mentions, hashtags and bot commands are never rewritten. Rules "
			"stay on this device and are never sent to Telegram."_q));
}

} // namespace Lumina
