/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_vault_calculator.h"

#include <QtCore/QEventLoop>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

#include <cmath>
#include <optional>
#include <vector>

namespace Lumina {
namespace {

// Sizes here are plain logical pixels on purpose. The gate runs before
// style::StartManager(), so st:: values do not exist yet; Qt's own high-DPI
// scaling still applies to them, which is all this window needs.
constexpr auto kWindowWidth = 320;
constexpr auto kWindowHeight = 440;
constexpr auto kPadding = 12;
constexpr auto kSpacing = 6;
constexpr auto kDisplayPadding = 8;
constexpr auto kDisplayFontScale = 2.2;
constexpr auto kButtonMinHeight = 44;
constexpr auto kColumns = 4;

// A result is formatted with this many decimals and then trimmed, matching the
// Android decoy so the two behave identically on the same input.
constexpr auto kResultDecimals = 8;
constexpr auto kWholeNumberLimit = 1.e15;

[[nodiscard]] bool IsAsciiDigit(QChar c) {
	return (c >= u'0') && (c <= u'9');
}

[[nodiscard]] bool IsOperator(QChar c) {
	return (c == u'+') || (c == u'-') || (c == u'*') || (c == u'/');
}

[[nodiscard]] bool SegmentHasDot(const QString &expression) {
	for (auto i = int(expression.size()); i != 0;) {
		const auto c = expression.at(--i);
		if (IsOperator(c)) {
			return false;
		} else if (c == u'.') {
			return true;
		}
	}
	return false;
}

void AppendDigit(QString &expression, QChar c) {
	if (c != u'.') {
		expression.append(c);
		return;
	} else if (SegmentHasDot(expression)) {
		return;
	} else if (expression.isEmpty()
		|| IsOperator(expression.at(expression.size() - 1))) {
		expression.append(u"0."_q);
	} else {
		expression.append(c);
	}
}

void AppendOperator(QString &expression, QChar c) {
	if (expression.isEmpty()) {
		if (c == u'-') {
			expression.append(c);
		}
		return;
	}
	const auto last = expression.size() - 1;
	if (IsOperator(expression.at(last))) {
		expression[last] = c;
	} else {
		expression.append(c);
	}
}

[[nodiscard]] bool AppendInput(QString &expression, QChar c) {
	if (IsAsciiDigit(c) || (c == u'.')) {
		AppendDigit(expression, c);
		return true;
	} else if (IsOperator(c)) {
		AppendOperator(expression, c);
		return true;
	}
	return false;
}

[[nodiscard]] std::optional<QString> FormatResult(double value) {
	if (std::isnan(value) || std::isinf(value)) {
		return std::nullopt;
	} else if (std::abs(value) < kWholeNumberLimit
		&& (value == std::rint(value))) {
		return QString::number(qint64(value));
	}
	auto result = QString::number(value, 'f', kResultDecimals);
	if (result.contains(u'.')) {
		auto end = int(result.size());
		while ((end > 0) && (result.at(end - 1) == u'0')) {
			--end;
		}
		if ((end > 0) && (result.at(end - 1) == u'.')) {
			--end;
		}
		result = result.left(end);
	}
	return result;
}

// Evaluates "number (operator number)*" with the usual precedence. Returns
// nullopt for anything the display should show as an error: a malformed
// expression, a division by zero, an overflow.
[[nodiscard]] std::optional<QString> Evaluate(const QString &expression) {
	const auto size = int(expression.size());
	if (!size) {
		return u"0"_q;
	}
	auto numbers = std::vector<double>();
	auto operators = std::vector<QChar>();
	auto segment = QString();
	auto i = 0;
	if (expression.at(0) == u'-') {
		segment.append(u'-');
		i = 1;
	}
	const auto takeSegment = [&] {
		auto ok = false;
		const auto value = segment.toDouble(&ok);
		if (ok) {
			numbers.push_back(value);
		}
		segment.clear();
		return ok;
	};
	for (; i != size; ++i) {
		const auto c = expression.at(i);
		if (!IsOperator(c)) {
			segment.append(c);
			continue;
		} else if (segment.isEmpty()
			|| (segment == u"-"_q)
			|| !takeSegment()) {
			return std::nullopt;
		}
		operators.push_back(c);
	}
	if (segment.isEmpty() || (segment == u"-"_q)) {
		if (!operators.empty()) {
			operators.pop_back();
		}
	} else if (!takeSegment()) {
		return std::nullopt;
	}
	if (numbers.empty()) {
		return u"0"_q;
	} else if (numbers.size() != operators.size() + 1) {
		return std::nullopt;
	}

	auto merged = std::vector<double>{ numbers.front() };
	auto rest = std::vector<QChar>();
	for (auto k = 0, count = int(operators.size()); k != count; ++k) {
		const auto op = operators[k];
		const auto rhs = numbers[k + 1];
		if (op == u'*') {
			merged.back() *= rhs;
		} else if (op == u'/') {
			if (rhs == 0.) {
				return std::nullopt;
			}
			merged.back() /= rhs;
		} else {
			rest.push_back(op);
			merged.push_back(rhs);
		}
	}
	auto result = merged.front();
	for (auto k = 0, count = int(rest.size()); k != count; ++k) {
		if (rest[k] == u'+') {
			result += merged[k + 1];
		} else {
			result -= merged[k + 1];
		}
	}
	return FormatResult(result);
}

class CalculatorWindow final : public QWidget {
public:
	explicit CalculatorWindow(Fn<void(VaultOutcome)> done);

private:
	void addKeypad(not_null<QVBoxLayout*> layout);
	void press(const QString &label);
	void digit(QChar c);
	void oper(QChar c);
	void clear();
	void backspace();
	void equals();
	void updateDisplay();
	void finish(VaultOutcome outcome);

	void closeEvent(QCloseEvent *e) override;
	void keyPressEvent(QKeyEvent *e) override;

	Fn<void(VaultOutcome)> _done;
	QLabel *_display = nullptr;
	QString _expression;
	bool _showingResult = false;
	bool _finished = false;

};

CalculatorWindow::CalculatorWindow(Fn<void(VaultOutcome)> done)
: _done(std::move(done)) {
	const auto layout = new QVBoxLayout(this);
	layout->setContentsMargins(kPadding, kPadding, kPadding, kPadding);
	layout->setSpacing(kSpacing);

	_display = new QLabel(u"0"_q, this);
	_display->setAlignment(Qt::AlignRight | Qt::AlignBottom);
	_display->setContentsMargins(
		kDisplayPadding,
		kDisplayPadding,
		kDisplayPadding,
		kDisplayPadding);
	auto font = _display->font();
	if (font.pointSizeF() > 0) {
		font.setPointSizeF(font.pointSizeF() * kDisplayFontScale);
	} else if (font.pixelSize() > 0) {
		font.setPixelSize(int(font.pixelSize() * kDisplayFontScale));
	}
	_display->setFont(font);
	layout->addWidget(_display, 2);

	addKeypad(layout);

	// Every key is also a keystroke: the keypad buttons take no focus, so the
	// window itself keeps it and keyPressEvent() sees everything typed. On a
	// desktop that matters more than it does on Android - entering the code is
	// how the vault is opened, and nobody wants to click it out digit by
	// digit.
	setFocusPolicy(Qt::StrongFocus);
	setFocus();
}

void CalculatorWindow::addKeypad(not_null<QVBoxLayout*> layout) {
	const auto grid = new QGridLayout();
	grid->setSpacing(kSpacing);

	// The same keypad as the Android decoy, so the two look like one app.
	const auto rows = std::vector<std::vector<QString>>{
		{ u"C"_q, u"/"_q, u"*"_q, u"-"_q },
		{ u"7"_q, u"8"_q, u"9"_q, u"+"_q },
		{ u"4"_q, u"5"_q, u"6"_q, u"="_q },
		{ u"1"_q, u"2"_q, u"3"_q, u"."_q },
		{ u"0"_q },
	};
	for (auto row = 0, count = int(rows.size()); row != count; ++row) {
		const auto &labels = rows[row];
		const auto columns = int(labels.size());
		for (auto column = 0; column != columns; ++column) {
			const auto label = labels[column];
			const auto button = new QPushButton(label, this);
			button->setFocusPolicy(Qt::NoFocus);
			button->setMinimumHeight(kButtonMinHeight);
			button->setSizePolicy(
				QSizePolicy::Expanding,
				QSizePolicy::Expanding);
			connect(button, &QPushButton::clicked, this, [this, label] {
				press(label);
			});
			const auto span = (columns == 1) ? kColumns : 1;
			grid->addWidget(button, row, column, 1, span);
		}
	}
	layout->addLayout(grid, 5);
}

void CalculatorWindow::press(const QString &label) {
	if (label.size() != 1) {
		return;
	}
	const auto c = label.at(0);
	if (c == u'C') {
		clear();
	} else if (c == u'=') {
		equals();
	} else if (IsOperator(c)) {
		oper(c);
	} else {
		digit(c);
	}
}

void CalculatorWindow::digit(QChar c) {
	if (_showingResult) {
		_expression.clear();
		_showingResult = false;
	}
	AppendDigit(_expression, c);
	updateDisplay();
}

void CalculatorWindow::oper(QChar c) {
	_showingResult = false;
	AppendOperator(_expression, c);
	updateDisplay();
}

void CalculatorWindow::clear() {
	_expression.clear();
	_showingResult = false;
	updateDisplay();
}

void CalculatorWindow::backspace() {
	if (_showingResult) {
		_expression.clear();
		_showingResult = false;
	} else if (!_expression.isEmpty()) {
		_expression.chop(1);
	}
	updateDisplay();
}

void CalculatorWindow::equals() {
	// The unlock, and the only thing here that is not a calculator: the entry
	// has to be exactly the secret code, and it has to have been TYPED.
	// Never log or echo it.
	//
	// `_showingResult` is true exactly when `_expression` is a result this
	// window computed rather than something the user entered - every input
	// path (digit, oper, clear, backspace) clears it first. Without the check,
	// a code of "12" is also opened by computing 5+7 and pressing equals
	// twice, so anyone idly using the decoy can fall into the real app by
	// arithmetic. The check cannot cost the owner their unlock: the only
	// documented way in is to type the code, and typing always leaves
	// `_showingResult` false.
	if (!_showingResult && VaultCodeMatches(_expression)) {
		finish(VaultOutcome::Unlock);
		return;
	}
	const auto result = Evaluate(_expression);
	if (!result) {
		_expression.clear();
		_showingResult = false;
		if (_display) {
			_display->setText(u"Error"_q);
		}
		return;
	}
	_expression = *result;
	_showingResult = true;
	updateDisplay();
}

void CalculatorWindow::updateDisplay() {
	if (_display) {
		_display->setText(_expression.isEmpty() ? u"0"_q : _expression);
	}
}

void CalculatorWindow::finish(VaultOutcome outcome) {
	if (_finished) {
		return;
	}
	_finished = true;
	_expression.clear();
	if (const auto done = _done) {
		done(outcome);
	}
}

void CalculatorWindow::closeEvent(QCloseEvent *e) {
	finish(VaultOutcome::Closed);
	QWidget::closeEvent(e);
}

void CalculatorWindow::keyPressEvent(QKeyEvent *e) {
	const auto key = e->key();
	if ((key == Qt::Key_Return)
		|| (key == Qt::Key_Enter)
		|| (key == Qt::Key_Equal)) {
		equals();
		return;
	} else if ((key == Qt::Key_Escape) || (key == Qt::Key_Delete)) {
		// Escape clears, as it does in every desktop calculator. It
		// deliberately does not close the window: closing quits the app, and
		// that is not what a stray Escape should do.
		clear();
		return;
	} else if (key == Qt::Key_Backspace) {
		backspace();
		return;
	}
	const auto text = e->text();
	if (text.size() == 1) {
		const auto c = (text.at(0) == u',') ? QChar(u'.') : text.at(0);
		if (IsAsciiDigit(c) || (c == u'.')) {
			digit(c);
			return;
		} else if (IsOperator(c)) {
			oper(c);
			return;
		}
	}
	QWidget::keyPressEvent(e);
}

} // namespace

VaultOutcome RunVaultCalculator() {
	auto outcome = VaultOutcome::Closed;
	auto loop = QEventLoop();

	// The window outlives nothing: `outcome` and `loop` are on this stack and
	// the callback can only run while loop.exec() is on it too.
	auto window = std::make_unique<CalculatorWindow>([&](VaultOutcome result) {
		outcome = result;
		loop.quit();
	});
	if (!VaultDetail::ShowGateWindow(window.get(), {
		.title = u"Calculator"_q,
		.width = kWindowWidth,
		.height = kWindowHeight,
		.iconGlyph = QChar(u'='),
		.iconColor = QColor(72, 76, 84),
	})) {
		return VaultOutcome::Failed;
	}
	loop.exec();
	window->hide();

	return outcome;
}

bool CalculatorCanType(const QString &text) {
	auto expression = QString();
	for (const auto c : text) {
		if (!AppendInput(expression, c)) {
			return false;
		}
	}
	return (expression == text);
}

} // namespace Lumina
