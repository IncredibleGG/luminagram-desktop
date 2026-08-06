/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_translate_preview_bar.h"

#include "base/timer.h"
#include "data/data_premium_limits.h"
#include "history/history.h"
#include "lumina/lumina_settings.h"
#include "lumina/lumina_translate_providers.h"
#include "lumina/lumina_translate_send.h"
#include "lumina/lumina_translate_settings.h"
#include "main/main_session.h"
#include "ui/painter.h"
#include "ui/qt_object_factory.h"
#include "ui/rp_widget.h"
#include "ui/widgets/fields/input_field.h"
#include "styles/style_chat.h"
#include "styles/style_lumina.h"
#include "styles/style_chat_helpers.h"

namespace Lumina {
namespace {

// Android waits the same 500ms in luminaUpdateTranslatePreview(). It is long
// enough that a paused typist gets one request per thought rather than one per
// keystroke, and short enough that the translation is on screen before the
// hand reaches the send key.
constexpr auto kDebounceDelay = crl::time(500);

// LuminaGram-only user-facing text, spelled as a literal for the same reason
// every other lumina/ file does it: lang.strings belongs to upstream and this
// item may not add keys to it.
[[nodiscard]] QString TranslatingText() {
	return u"Translating..."_q;
}

[[nodiscard]] QString FailedText() {
	return u"Translation unavailable"_q;
}

// Owned and written by lumina_translate_send.cpp; named here only so that a
// change to the per-dialog lock can wake the bar up.
[[nodiscard]] QString DialogLanguageKey() {
	return u"trSendLangDialog"_q;
}

class PreviewBar final : public Ui::RpWidget {
public:
	PreviewBar(
		not_null<Ui::RpWidget*> parent,
		not_null<Ui::InputField*> field);
	~PreviewBar();

	void apply(const TranslatePreviewBarState &state);

	// Answered before the host has had a chance to place the bar, because the
	// host needs the height in order to work out where it goes. So the widget
	// itself stays hidden until placeAt() runs, and _shown - not Qt visibility -
	// is what this reports. A host that reserves the height must therefore
	// always place it in the same pass, which both of them do.
	[[nodiscard]] int barHeight() const;
	void placeAt(int left, int top, int width);
	[[nodiscard]] History *history() const;
	[[nodiscard]] QString translationFor(const QString &original) const;

protected:
	int resizeGetHeight(int newWidth) override;

private:
	void paint();
	void refresh();
	void requestTranslation();
	void cancelPending();
	void forgetTranslation();
	void scheduleLayoutNotify();
	void noteForSendPath(
		History *history,
		const QString &source,
		const QString &language,
		const QString &translated);

	[[nodiscard]] QString resolveLanguage() const;
	[[nodiscard]] QString currentText() const;
	[[nodiscard]] QString translationLine() const;
	[[nodiscard]] TranslateEngine *ensureEngine();

	const not_null<Ui::RpWidget*> _owner;
	const not_null<Ui::InputField*> _field;

	// Weak, not raw: a provider reply can land after the account was logged out
	// and every History destroyed, while this composer is still being torn
	// down. crl::guard() keeps the widget alive for that callback but says
	// nothing about the chat it was typed in.
	base::weak_ptr<History> _history;
	Fn<void()> _layoutChanged;
	base::Timer _debounce;

	QString _original;
	QString _language;
	QString _translatedFor;
	QString _translatedTo;
	QString _translatedText;

	std::unique_ptr<TranslateEngine> _engine;
	Main::Session *_engineSession = nullptr;
	QString _engineProvider;
	bool _engineFallback = false;

	int _generation = 0;
	bool _shown = false;
	bool _editing = false;
	bool _layoutNotifyScheduled = false;

};

[[nodiscard]] base::flat_map<const Ui::RpWidget*, PreviewBar*> &Registry() {
	static auto result = base::flat_map<const Ui::RpWidget*, PreviewBar*>();
	return result;
}

PreviewBar::PreviewBar(
	not_null<Ui::RpWidget*> parent,
	not_null<Ui::InputField*> field)
: RpWidget(parent.get())
, _owner(parent)
, _field(field)
, _debounce([=] { requestTranslation(); }) {
	hide();
	resize(parent->width(), st::historyReplyHeight);

	// The bar is created long after the composer ordered its children, so it
	// would otherwise sit on top of the emoji panel, the inline results and
	// the autocomplete, all of which are raised over the composer band and
	// legitimately cover it. Nothing the composer paints reaches down here, so
	// the bottom of the stack is the one position that is always right.
	lower();

	paintRequest(
	) | rpl::on_next([=] {
		paint();
	}, lifetime());

	_field->changes(
	) | rpl::on_next([=] {
		refresh();
	}, lifetime());

	_field->shownValue(
	) | rpl::on_next([=](bool) {
		refresh();
	}, lifetime());
}

PreviewBar::~PreviewBar() {
	Registry().remove(_owner.get());
}

int PreviewBar::resizeGetHeight(int) {
	return st::historyReplyHeight;
}

int PreviewBar::barHeight() const {
	return _shown ? height() : 0;
}

void PreviewBar::placeAt(int left, int top, int width) {
	resizeToWidth(width);
	moveToLeft(left, top);
	if (_shown && isHidden()) {
		show();
	}
}

History *PreviewBar::history() const {
	return _history.get();
}

QString PreviewBar::translationFor(const QString &original) const {
	const auto source = original.trimmed();
	return (_shown
		&& !_translatedText.isEmpty()
		&& (_translatedFor == source)
		&& (_translatedFor == _original)
		&& (_translatedTo == _language))
		? _translatedText
		: QString();
}

void PreviewBar::apply(const TranslatePreviewBarState &state) {
	_layoutChanged = state.layoutChanged;
	_editing = state.editing;
	if (_history.get() != state.history) {
		_history = base::make_weak(state.history);
		cancelPending();
		forgetTranslation();
		_original = QString();
		_language = QString();

		// Nothing is in flight after cancelPending(), and this never runs from
		// inside a provider callback - only from the host's geometry pass - so
		// dropping the engine here is safe in the sense
		// lumina_translate_providers.h means it. Doing it keeps a logged-out
		// Main::Session from being held by a composer that has not been torn
		// down yet.
		if (!state.history) {
			_engine = nullptr;
			_engineSession = nullptr;
			_engineProvider = QString();
			_engineFallback = false;
		}
	}
	refresh();
}

void PreviewBar::refresh() {
	const auto language = resolveLanguage();
	const auto text = language.isEmpty() ? QString() : currentText();
	if (language.isEmpty() || text.isEmpty()) {
		if (!_original.isEmpty() || !_language.isEmpty()) {
			cancelPending();
			_original = QString();
			_language = QString();
		}
		if (_shown) {
			_shown = false;
			hide();
			scheduleLayoutNotify();
		}
		return;
	}
	if ((_original != text) || (_language != language)) {
		_original = text;
		_language = language;
		cancelPending();
		if ((_translatedFor != text) || (_translatedTo != language)) {
			_debounce.callOnce(kDebounceDelay);
		}
		update();
	}
	if (!_shown) {
		_shown = true;
		scheduleLayoutNotify();
	}
}

void PreviewBar::cancelPending() {
	_debounce.cancel();
	++_generation;
}

void PreviewBar::forgetTranslation() {
	_translatedFor = QString();
	_translatedTo = QString();
	_translatedText = QString();
}

void PreviewBar::scheduleLayoutNotify() {
	if (_layoutNotifyScheduled) {
		return;
	}
	_layoutNotifyScheduled = true;
	InvokeQueued(this, [=] {
		_layoutNotifyScheduled = false;
		if (const auto callback = _layoutChanged) {
			callback();
		}
	});
}

QString PreviewBar::currentText() const {
	return _field->getLastText().trimmed();
}

QString PreviewBar::resolveLanguage() const {
	const auto history = _history.get();
	if (!history || _editing || _field->isHidden()) {
		return QString();
	}

	// The send pipeline's own resolver, deliberately not a second copy of it:
	// it answers a language exactly when the next Send here would translate,
	// and exactly the language that Send would use. Anything else here would be
	// a preview of a message the app is not going to send.
	return ResolveSendLanguage(history);
}

QString PreviewBar::translationLine() const {
	return (_original.isEmpty()
		|| (_translatedFor != _original)
		|| (_translatedTo != _language))
		? TranslatingText()
		: _translatedText.isEmpty()
		? FailedText()
		: _translatedText;
}

TranslateEngine *PreviewBar::ensureEngine() {
	const auto history = _history.get();
	const auto session = history ? &history->session() : nullptr;
	if (!session) {
		return nullptr;
	}
	const auto provider = CurrentProviderId();
	const auto fallback = TranslateFallbackToTelegram();
	if (!_engine
		|| (_engineSession != session)
		|| (_engineProvider != provider)
		|| (_engineFallback != fallback)) {
		_engine = MakeCurrentTranslateEngine(session);
		_engineSession = session;
		_engineProvider = provider;
		_engineFallback = fallback;
	}
	return _engine.get();
}

void PreviewBar::requestTranslation() {
	if (!_shown || _original.isEmpty() || _language.isEmpty()) {
		return;
	}
	const auto engine = ensureEngine();
	if (!engine) {
		return;
	}
	const auto generation = ++_generation;
	const auto source = _original;
	const auto language = _language;
	const auto weak = _history;
	engine->translate(source, language, crl::guard(this, [=](
			TranslateResult result) {
		if (generation != _generation) {
			return;
		}
		_translatedFor = source;
		_translatedTo = language;
		_translatedText = result.failed()
			? QString()
			: result.text.trimmed();
		noteForSendPath(weak.get(), source, language, _translatedText);
		if ((_original == source) && (_language == language)) {
			update();
		}
	}));
}

void PreviewBar::noteForSendPath(
		History *history,
		const QString &source,
		const QString &language,
		const QString &translated) {
	if (!history || translated.isEmpty() || (translated == source)) {
		return;
	}

	// The send pipeline reuses this on an exact (chat, source, language) match
	// and then goes straight out, which is what makes the panel the
	// confirmation and what keeps a metered key from being charged twice for
	// the same string. Its reuse branch skips the length check that lives on
	// the branch which made the request itself, so a translation that
	// ApiWrap::sendMessage() would split across several messages - breaking
	// both the look of it and the single sent-text correlation W2-B needs - is
	// not offered here at all. The send then translates it the long way round
	// and hits that same guard.
	const auto limits = Data::PremiumLimits(&history->session());
	if (int(translated.size()) > limits.messageLengthCurrent()) {
		return;
	}
	NoteSendTranslationPreview(history, source, language, translated);
}

void PreviewBar::paint() {
	auto p = Painter(this);
	p.fillRect(rect(), st::historyComposeAreaBg);

	const auto &icon = st::historyTranslateIcon;
	icon.paint(
		p,
		st::historyReplyIconPosition.x(),
		(height() - icon.height()) / 2,
		width());

	const auto left = st::historyReplySkip;
	const auto available = width() - left - st::msgReplyPadding.right();
	if (available <= 0) {
		return;
	}
	const auto top = st::msgReplyPadding.top();
	const auto originalFont = st::luminaPreviewOriginalFont;
	const auto translationFont = st::luminaPreviewTranslationFont;

	p.setPen(st::historyComposeAreaFg);
	p.setFont(originalFont);
	p.drawTextLeft(
		left,
		top,
		width(),
		originalFont->elided(_original, available));

	p.setPen(st::historyComposeAreaFgService);
	p.setFont(translationFont);
	p.drawTextLeft(
		left,
		top + originalFont->height,
		width(),
		translationFont->elided(translationLine(), available));
}

} // namespace

void RefreshTranslatePreviewBar(TranslatePreviewBarState state) {
	auto &registry = Registry();
	const auto i = registry.find(state.parent.get());
	if (i != registry.end()) {
		i->second->apply(state);
		return;
	} else if (!state.history
		|| !TranslateBeforeSendActive(not_null<History*>(state.history))) {
		return;
	}
	const auto bar = Ui::CreateChild<PreviewBar>(
		state.parent.get(),
		state.field);
	registry.emplace(state.parent.get(), bar);
	bar->apply(state);
}

int TranslatePreviewBarHeight(not_null<const Ui::RpWidget*> parent) {
	auto &registry = Registry();
	const auto i = registry.find(parent.get());
	return (i != registry.end()) ? i->second->barHeight() : 0;
}

void MoveTranslatePreviewBar(
		not_null<Ui::RpWidget*> parent,
		int left,
		int top,
		int width) {
	auto &registry = Registry();
	const auto i = registry.find(parent.get());
	if (i == registry.end()) {
		return;
	}
	i->second->placeAt(left, top, width);
}

rpl::producer<> TranslatePreviewRefreshRequests() {
	return rpl::merge(
		TranslateSettingsChanges(),
		Settings::Instance().changesFor(DialogLanguageKey()));
}

bool TranslatePreviewShown(not_null<History*> history) {
	for (const auto &entry : Registry()) {
		if ((entry.second->history() == history.get())
			&& (entry.second->barHeight() > 0)) {
			return true;
		}
	}
	return false;
}

QString TranslatePreviewTranslation(
		not_null<History*> history,
		const QString &original) {
	for (const auto &entry : Registry()) {
		if (entry.second->history() != history.get()) {
			continue;
		}
		auto result = entry.second->translationFor(original);
		if (!result.isEmpty()) {
			return result;
		}
	}
	return QString();
}

} // namespace Lumina
