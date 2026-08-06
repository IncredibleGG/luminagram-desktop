/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_vault_door.h"

#include <QtCore/QEventLoop>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

namespace Lumina {
namespace {

// Sizes here are plain logical pixels on purpose. The gate runs before
// style::StartManager(), so st:: values do not exist yet; Qt's own high-DPI
// scaling still applies to them, which is all this window needs.
constexpr auto kWindowWidth = 340;
constexpr auto kWindowHeight = 160;
constexpr auto kPadding = 24;
constexpr auto kSpacing = 12;

// DO NOT give this field a setMaxLength(). It had one - 128, chosen to mirror
// the 128 the settings editor caps the code at - and the two numbers do not
// mean the same thing.
//
// The editor is a Ui::InputField, whose _maxLength is enforced in document
// positions (chopByMaxLength() measures with QTextCursor::position()), and an
// emoji is folded into ONE position: the correction pass turns every sequence
// Emoji::Find() recognises into a single object-replacement character
// (input_field.cpp:3520-3534), and getTextWithTags() expands it back to the
// full emoji text on the way out, adjusting the reported length as it goes.
// QLineEdit::setMaxLength() counts UTF-16 units. So a code of 65 ordinary
// emoji is 65 positions in the editor - accepted and stored whole - and 130+
// units here, where it would be truncated to 128 and could then never match.
// The door would drop into the decoy on every attempt, silently and forever,
// with the settings page still reporting the code as "Set".
//
// Qt's own default cap of 32767 units applies instead, which is far above
// anything 128 editor positions can expand to and is not a number this file
// has to keep in sync with anything.
class DoorWindow final : public QWidget {
public:
	explicit DoorWindow(Fn<void(VaultOutcome)> done);

private:
	void confirm();
	void finish(VaultOutcome outcome);

	void closeEvent(QCloseEvent *e) override;
	void keyPressEvent(QKeyEvent *e) override;

	Fn<void(VaultOutcome)> _done;
	QLineEdit *_field = nullptr;
	bool _finished = false;

};

DoorWindow::DoorWindow(Fn<void(VaultOutcome)> done)
: _done(std::move(done)) {
	const auto layout = new QVBoxLayout(this);
	layout->setContentsMargins(kPadding, kPadding, kPadding, kPadding);
	layout->setSpacing(kSpacing);
	layout->addStretch(1);

	_field = new QLineEdit(this);
	_field->setEchoMode(QLineEdit::Password);
	_field->setPlaceholderText(u"Password"_q);
	layout->addWidget(_field);

	const auto confirmButton = new QPushButton(u"OK"_q, this);
	confirmButton->setDefault(true);
	layout->addWidget(confirmButton);
	layout->addStretch(1);

	connect(_field, &QLineEdit::returnPressed, this, [this] {
		confirm();
	});
	connect(confirmButton, &QPushButton::clicked, this, [this] {
		confirm();
	});

	_field->setFocus();
}

void DoorWindow::confirm() {
	// Never log, echo or report the entry: a wrong one has to be
	// indistinguishable from a right one until the decoy appears.
	const auto entered = (_field ? _field->text() : QString()).trimmed();

	// A blank submit does nothing at all, where Android would take it as a
	// wrong code and drop into the decoy. A blank field is not a guess, so
	// treating it as one buys no secrecy and costs the owner a relaunch every
	// time Enter is hit a moment too early.
	if (entered.isEmpty()) {
		return;
	}
	finish(VaultCodeMatches(entered)
		? VaultOutcome::Unlock
		: VaultOutcome::Decoy);
}

void DoorWindow::finish(VaultOutcome outcome) {
	if (_finished) {
		return;
	}
	_finished = true;
	if (_field) {
		_field->clear();
	}
	if (const auto done = _done) {
		done(outcome);
	}
}

void DoorWindow::closeEvent(QCloseEvent *e) {
	finish(VaultOutcome::Closed);
	QWidget::closeEvent(e);
}

void DoorWindow::keyPressEvent(QKeyEvent *e) {
	if (e->key() == Qt::Key_Escape) {
		close();
	} else {
		QWidget::keyPressEvent(e);
	}
}

} // namespace

VaultOutcome RunVaultDoor() {
	auto outcome = VaultOutcome::Closed;
	auto loop = QEventLoop();

	// The window outlives nothing: `outcome` and `loop` are on this stack and
	// the callback can only run while loop.exec() is on it too.
	auto window = std::make_unique<DoorWindow>([&](VaultOutcome result) {
		outcome = result;
		loop.quit();
	});
	if (!VaultDetail::ShowGateWindow(window.get(), {
		.title = u"Unlock"_q,
		.width = kWindowWidth,
		.height = kWindowHeight,
		.iconGlyph = QChar(u'\x2022'),
		.iconColor = QColor(96, 100, 108),
	})) {
		return VaultOutcome::Failed;
	}
	loop.exec();
	window->hide();

	return outcome;
}

} // namespace Lumina
