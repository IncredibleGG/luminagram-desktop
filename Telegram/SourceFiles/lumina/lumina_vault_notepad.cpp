/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_vault_notepad.h"

#include "lumina/lumina_settings.h"

#include <QtCore/QEventLoop>
#include <QtGui/QTextCursor>
#include <QtGui/QTextDocument>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

namespace Lumina {
namespace {

// Sizes here are plain logical pixels on purpose. The gate runs before
// style::StartManager(), so st:: values do not exist yet; Qt's own high-DPI
// scaling still applies to them, which is all this window needs.
constexpr auto kWindowWidth = 560;
constexpr auto kWindowHeight = 480;
constexpr auto kTitlePadding = 14;
constexpr auto kBodyPadding = 10;
constexpr auto kTitleFontScale = 1.2;

class TitleLabel final : public QLabel {
public:
	TitleLabel(
		QWidget *parent,
		const QString &text,
		Fn<void()> doubleClicked);

private:
	void mouseDoubleClickEvent(QMouseEvent *e) override;

	Fn<void()> _doubleClicked;

};

TitleLabel::TitleLabel(
	QWidget *parent,
	const QString &text,
	Fn<void()> doubleClicked)
: QLabel(text, parent)
, _doubleClicked(std::move(doubleClicked)) {
	setAutoFillBackground(true);
	setContentsMargins(
		kTitlePadding,
		kTitlePadding,
		kTitlePadding,
		kTitlePadding);
	auto scaled = font();
	if (scaled.pointSizeF() > 0) {
		scaled.setPointSizeF(scaled.pointSizeF() * kTitleFontScale);
	} else if (scaled.pixelSize() > 0) {
		scaled.setPixelSize(int(scaled.pixelSize() * kTitleFontScale));
	}
	scaled.setBold(true);
	setFont(scaled);
}

void TitleLabel::mouseDoubleClickEvent(QMouseEvent *e) {
	if (const auto handler = _doubleClicked) {
		handler();
	}
	QLabel::mouseDoubleClickEvent(e);
}

class NotepadWindow final : public QWidget {
public:
	explicit NotepadWindow(Fn<void(VaultOutcome)> done);

private:
	void save();
	void tryUnlock();
	void finish(VaultOutcome outcome);

	void closeEvent(QCloseEvent *e) override;
	void mouseDoubleClickEvent(QMouseEvent *e) override;

	Fn<void(VaultOutcome)> _done;
	QPlainTextEdit *_body = nullptr;
	QString _restored;
	bool _finished = false;

};

NotepadWindow::NotepadWindow(Fn<void(VaultOutcome)> done)
: _done(std::move(done))
, _restored(DecoyNotepadContent()) {
	const auto layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);

	layout->addWidget(new TitleLabel(this, u"Notes"_q, [this] {
		tryUnlock();
	}));

	_body = new QPlainTextEdit(this);
	_body->setFrameShape(QFrame::NoFrame);
	_body->document()->setDocumentMargin(kBodyPadding);
	_body->setPlainText(_restored);
	_body->moveCursor(QTextCursor::End);
	layout->addWidget(_body, 1);

	_body->setFocus();
}

// Deliberately NOT Android's save-on-every-keystroke.
//
// Unlocking from this skin means making the note body the secret code, so a
// decoy that persists as you type writes that code - and every prefix of it -
// into decoyNotepadContent, in plaintext, on disk. Android does exactly that
// and the code is then sitting in the note the next time the decoy opens,
// which defeats the whole point. Writing once, on the way out, and never
// writing a body that is the code costs nothing here: the note is a prop, and
// the only thing lost by not saving mid-edit is a prop's edits after a crash.
void NotepadWindow::save() {
	if (!_body) {
		return;
	}
	const auto text = _body->toPlainText();
	if (!VaultCodeMatches(text)) {
		SetDecoyNotepadContent(text);
	} else if (VaultCodeMatches(_restored)) {
		// The body is the code, so it is not written. If what was stored was
		// the code as well - an older build, or a note carried over from
		// Android - drop it, so the decoy stops opening with the code on
		// display.
		SetDecoyNotepadContent(QString());
	}
}

void NotepadWindow::tryUnlock() {
	// The unlock, and the only thing here that is not a notepad. A wrong body
	// does nothing whatsoever - no message, no flash - so a double click gives
	// away neither that a lock exists nor that one was just attempted.
	if (_body && VaultCodeMatches(_body->toPlainText())) {
		finish(VaultOutcome::Unlock);
	}
}

void NotepadWindow::finish(VaultOutcome outcome) {
	if (_finished) {
		return;
	}
	_finished = true;
	save();
	Settings::Instance().saveNow();
	if (const auto done = _done) {
		done(outcome);
	}
}

void NotepadWindow::closeEvent(QCloseEvent *e) {
	finish(VaultOutcome::Closed);
	QWidget::closeEvent(e);
}

// The same unlock attempt, one widget further out, and it is not redundant
// padding.
//
// In VaultMode::DecoyApp with the notepad skin - which is the default skin,
// and the skin the gate substitutes whenever the calculator cannot type the
// code - the double click on the title is the ONLY way the owner ever gets
// back into their account. A single unlock surface that depends on one Qt
// widget receiving one event kind is too narrow a thing to hang an account on.
//
// QLabel does not accept plain mouse events unless it has text interaction
// flags, so a double click on the title is delivered to TitleLabel first and
// then propagates here; a double click anywhere else on the window chrome
// arrives here directly. The body swallows its own, which is correct - a
// double click in a text area selects a word and must keep doing so.
//
// tryUnlock() with a body that is not the code does nothing observable, so
// widening where it can be attempted from gives nothing away.
void NotepadWindow::mouseDoubleClickEvent(QMouseEvent *e) {
	tryUnlock();
	QWidget::mouseDoubleClickEvent(e);
}

} // namespace

VaultOutcome RunVaultNotepad() {
	auto outcome = VaultOutcome::Closed;
	auto loop = QEventLoop();

	// The window outlives nothing: `outcome` and `loop` are on this stack and
	// the callback can only run while loop.exec() is on it too.
	auto window = std::make_unique<NotepadWindow>([&](VaultOutcome result) {
		outcome = result;
		loop.quit();
	});
	if (!VaultDetail::ShowGateWindow(window.get(), {
		.title = u"Notes"_q,
		.width = kWindowWidth,
		.height = kWindowHeight,
		.iconGlyph = QChar(u'N'),
		.iconColor = QColor(214, 158, 46),
	})) {
		return VaultOutcome::Failed;
	}
	loop.exec();
	window->hide();

	return outcome;
}

} // namespace Lumina
