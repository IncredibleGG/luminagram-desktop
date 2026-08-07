/*
This file is part of Telegram Desktop,
the official desktop application for the Telegram messaging service.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "history/view/history_view_translate_tracker.h"

#include "apiwrap.h"
#include "api/api_transcribes.h"
#include "core/application.h"
#include "core/core_settings.h"
#include "data/data_changes.h"
#include "data/data_channel.h"
#include "data/data_flags.h"
#include "data/data_peer.h"
#include "data/data_session.h"
#include "history/history.h"
#include "history/history_item.h"
#include "history/history_item_components.h"
#include "history/view/history_view_element.h"
#include "iv/iv_rich_page.h"
#include "lang/translate_provider.h"
#include "lumina/lumina_translate_gating.h"
#include "lumina/lumina_translate_providers.h" // TranslateProviderChanges.
#include "main/main_session.h"
#include "spellcheck/platform/platform_language.h"

namespace HistoryView {
namespace {

constexpr auto kEnoughForRecognition = 10;
constexpr auto kEnoughForTranslation = 6;
constexpr auto kMaxCheckInBunch = 100;
constexpr auto kRequestLengthLimit = 24 * 1024;
constexpr auto kRequestCountLimit = 20;

// LuminaGram: how many recognised messages an offer needs while
// Lumina::TranslateOfferSkip() is in force. One, because the offer is what
// gives the per-chat translate control something to write to - History has no
// HistoryTranslation until a language is offered, and History::translateTo()
// returns on its first line while there is none - so waiting for
// kEnoughForTranslation is a button that does nothing for the first five
// messages of every new conversation. It is not zero: a message whose language
// was not recognised, and a message that is only emoji and spaces (dropped in
// add() before it ever reaches recognition), both count for nothing here, so a
// chat that has said nothing recognisable yet still offers nothing and asks the
// provider for nothing.
constexpr auto kEnoughForRelaxedOffer = 1;

using OfferSkip = std::optional<std::vector<LanguageId>>;

} // namespace

TranslateTracker::TranslateTracker(not_null<History*> history)
: _history(history)
, _provider(Ui::CreateTranslateProvider(&_history->session()))
, _api(&_history->session().mtp())
, _limit(kEnoughForRecognition) {
	setup();
}

TranslateTracker::~TranslateTracker() {
	cancelToRequest();
	cancelSentRequest();
}

rpl::producer<bool> TranslateTracker::trackingLanguage() const {
	return _trackingLanguage.value();
}

void TranslateTracker::setup() {
	const auto peer = _history->peer;
	peer->updateFull();

	// The revocation half of Lumina::TranslateOfferSkip(). An offer this file
	// made on relaxed terms has to disappear the moment the terms are
	// withdrawn, and checkRecognized() alone will not do it: below
	// kEnoughForTranslation messages it deliberately keeps whatever offer is
	// already there, which is what carries a real offer across a chat being
	// reopened. So the whole policy, and not just the languages in it, goes
	// through distinct_until_changed(): what arrives here is the transition,
	// and nullopt is "upstream's rules again" - the one edge on which a
	// relaxed offer is dropped, and the only one on which anything is dropped
	// at all.
	const auto offerPolicyChanged = [=](const OfferSkip &now) {
		const auto wasOfferedFrom = _history->translateOfferedFrom();
		const auto wasTranslatedTo = _history->translatedTo();
		if (!now && wasOfferedFrom) {
			_history->translateOfferFrom({});
		}
		checkRecognized();
		if (wasTranslatedTo
			&& wasOfferedFrom
			&& !_history->translateOfferedFrom()) {
			stopAndRevert();
		}
	};

	const auto channel = peer->asChannel();
	auto autoTranslationValue = (channel
		? (channel->flagsValue() | rpl::type_erased)
		: rpl::single(Data::Flags<ChannelDataFlags>::Change({}, {}))
		) | rpl::map([=](Data::Flags<ChannelDataFlags>::Change data) {
		return (data.value & ChannelDataFlag::AutoTranslation);
	}) | rpl::distinct_until_changed();

	using namespace rpl::mappers;
	_trackingLanguage = rpl::combine(
		Core::App().settings().translateChatEnabledValue(),
		Lumina::ChatTranslationUnlockedValue(&_history->session()),
		std::move(autoTranslationValue),
		_1 && (_2 || _3));
	_trackingLanguage.value() | rpl::on_next([=](bool tracking) {
		_trackingLifetime.destroy();
		if (tracking) {
			recognizeCollected();
			trackSkipLanguages();
			trackTranslationDisabled();
			// EVERY input of Lumina::TranslateOfferSkip() has to reach this
			// stream, not only the ones that file owns.
			// distinct_until_changed() keeps one remembered value per
			// subscription and updates it only when a value actually flows
			// through here, so an input that moves the answer WITHOUT waking
			// this stream leaves that memory holding something the predicate
			// no longer returns - and the next real change is then compared
			// against a lie and dropped as "not a change".
			//
			// PeerData::translationFlag() is exactly such an input. It is
			// Unknown until the peer's full info comes back, so a chat opened
			// at launch subscribes here while the answer is still nullopt,
			// and the flag arriving Enabled turns it into a real list through
			// trackTranslationDisabled() - which does not pass through here.
			// Turning the translation switch back OFF then maps to nullopt,
			// compares equal to the remembered nullopt and is swallowed: the
			// relaxed offer is never withdrawn, stopAndRevert() never runs,
			// and the chat keeps translating every new message, one provider
			// request each, after the switch that stops it was turned off.
			//
			// translateChatEnabledValue() and ChatTranslationUnlockedValue()
			// are here for the same reason. They usually move
			// _trackingLanguage as well, which rebuilds this subscription and
			// its memory from scratch - but not always: in a channel carrying
			// ChannelDataFlag::AutoTranslation the tracking flag stays true on
			// its own, and Premium lapsing underneath it would be one more
			// silently remembered lie.
			//
			// None of the three needs skip(1). They emit their current value
			// on subscribe, it maps to the same answer rpl::single() below
			// already produced, and distinct_until_changed() collapses it;
			// what they are here for is to keep the memory honest afterwards.
			auto policyChanges = rpl::merge(
				Lumina::TranslateOfferPolicyChanges(),
				_history->session().changes().peerFlagsValue(
					peer,
					Data::PeerUpdate::Flag::TranslationDisabled
				) | rpl::to_empty,
				Core::App().settings().translateChatEnabledValue()
					| rpl::to_empty,
				Lumina::ChatTranslationUnlockedValue(&_history->session())
					| rpl::to_empty);

			// skip(1) drops the current value, which is not a change;
			// trackSkipLanguages() has already evaluated it by then.
			rpl::single(
				rpl::empty
			) | rpl::then(
				std::move(policyChanges)
			) | rpl::map([=] {
				return Lumina::TranslateOfferSkip(_history);
			}) | rpl::distinct_until_changed(
			) | rpl::skip(1) | rpl::on_next(
				offerPolicyChanged,
				_trackingLifetime);
		} else {
			checkRecognized({});
			stopAndRevert();
		}
	}, _lifetime);

	// The tier boundary being withdrawn while a chat is translating.
	//
	// Everything above reacts to the OFFER, and with the continuous tier off
	// the detector may well keep offering this chat on upstream's own terms -
	// a foreign-language chat is exactly what upstream offers. A chat the user
	// had switched on would then keep translating every new message, one
	// provider request each, after the switch that pays for it was turned off.
	// Only ChatTranslationUnlockedValue() dropping catches that today, and it
	// does not drop for a Premium account.
	//
	// Only the transition acts. skip(1) drops the current value, so a profile
	// that never opted in - the default - is not touched at all, and a channel
	// Telegram itself auto-translates is left alone because that is Telegram's
	// own feature and upstream restarts it from the next offer regardless.
	Lumina::ContinuousTranslationAvailableValue(
	) | rpl::skip(1) | rpl::filter([](bool available) {
		return !available;
	}) | rpl::on_next([=] {
		if (_history->translatedTo() && !_history->peer->autoTranslation()) {
			stopAndRevert();
		}
	}, _lifetime);

	// _provider is resolved once, in the constructor's initialiser list. That
	// is a WHERE THE TEXT GOES decision, not a cached setting: with an open
	// chat behind a settings window - a separate chat window, or simply
	// coming back to the same one - picking a different service, or pasting
	// the API key that was missing, changed the row on the settings page and
	// nothing else. Every further message in that chat kept going to the
	// engine that was current when the chat was opened, including the keyless
	// Google one after the user had explicitly moved to Telegram. Rebuilding
	// here is what makes the provider row mean anything while a chat is open.
	//
	// Order is load bearing. cancelSentRequest() first, because destroying
	// the old provider destroys the QNetworkAccessManager that owns the reply
	// in flight and the finished handler then never runs: without this
	// _requestInProcess would stay true for the lifetime of the chat and
	// requestSome() would refuse every later batch - the provider switch
	// would turn translation off instead of redirecting it. It also puts the
	// items it had taken back into a state the next paint bunch re-queues,
	// through translationShowRequiresCheck(), so nothing is dropped; the
	// requestSome() below covers whatever is still queued without waiting for
	// that bunch.
	//
	// Safe to destroy the provider from here: every writer of the keys this
	// stream watches is a settings row (SetCurrentProviderId,
	// SetProviderApiKey and friends in lumina/lumina_translate_settings.cpp),
	// so this never runs inside a reply callback owned by the object being
	// destroyed.
	Lumina::TranslateProviderChanges(
	) | rpl::on_next([=] {
		cancelSentRequest();
		_provider = Ui::CreateTranslateProvider(&_history->session());
		requestSome();
	}, _lifetime);
}

bool TranslateTracker::enoughForRecognition() const {
	return _itemsForRecognize.size() >= kEnoughForRecognition;
}

void TranslateTracker::startBunch() {
	_addedInBunch = 0;
	_bunchTranslatedTo = _history->translatedTo();
	++_generation;
}

bool TranslateTracker::add(not_null<Element*> view) {
	const auto item = view->data();
	const auto only = view->isOnlyEmojiAndSpaces();
	if (only != OnlyEmojiAndSpaces::Unknown) {
		item->cacheOnlyEmojiAndSpaces(only == OnlyEmojiAndSpaces::Yes);
	}
	return add(item, false);
}

bool TranslateTracker::add(not_null<HistoryItem*> item) {
	return add(item, false);
}

bool TranslateTracker::add(
		not_null<HistoryItem*> item,
		bool skipDependencies) {
	Expects(_addedInBunch >= 0);

	if ((item->out() && !item->history()->peer->autoTranslation())
		|| item->isService()
		|| !item->isRegular()
		|| item->isOnlyEmojiAndSpaces()) {
		return false;
	}
	if (item->translationShowRequiresCheck(_bunchTranslatedTo)) {
		_switchTranslations[item] = _bunchTranslatedTo;
	}
	if (!skipDependencies) {
		if (const auto reply = item->Get<HistoryMessageReply>()) {
			if (const auto to = reply->resolvedMessage.get()) {
				add(to, true);
			}
		}
#if 0 // I hope this is not needed, although I'm not sure.
		if (item->groupId()) {
			if (const auto group = _history->owner().groups().find(item)) {
				for (const auto &other : group->items) {
					if (other != item) {
						add(other, true);
					}
				}
			}
		}
#endif
	}
	const auto id = item->fullId();
	const auto i = _itemsForRecognize.find(id);
	if (i != end(_itemsForRecognize)) {
		i->second.generation = _generation;
		return true;
	}
	const auto &text = item->originalText().text;
	_itemsForRecognize.emplace(id, ItemForRecognize{
		.generation = _generation,
		.id = (_trackingLanguage.current()
			? Platform::Language::Recognize(text)
			: MaybeLanguageId{ text }),
	});
	++_addedInBunch;
	return true;
}

void TranslateTracker::switchTranslation(
		not_null<HistoryItem*> item,
		LanguageId id) {
	_history->session().api().transcribes().checkSummaryToTranslate(
		item->fullId());
	if (item->translationShowRequiresRequest(id)) {
		_itemsToRequest.emplace(item->fullId(), ItemToRequest{
			.length = int(item->originalText().text.size()),
			.rich = (_provider->supportsMessageId()
				&& (item->richPage() != nullptr)),
		});
	}
}

void TranslateTracker::finishBunch() {
	if (_addedInBunch > 0) {
		accumulate_max(_limit, _addedInBunch + kEnoughForRecognition);
		_addedInBunch = -1;
		applyLimit();
		if (_trackingLanguage.current()) {
			checkRecognized();
		}
	}
	if (!_switchTranslations.empty()) {
		auto switching = base::take(_switchTranslations);
		for (const auto &[item, id] : switching) {
			switchTranslation(item, id);
		}
		_switchTranslations = std::move(switching);
		_switchTranslations.clear();
	}
	requestSome();
}

void TranslateTracker::addBunchFromBlocks() {
	if (enoughForRecognition()) {
		return;
	}
	startBunch();
	const auto guard = gsl::finally([&] {
		finishBunch();
	});

	auto check = kMaxCheckInBunch;
	for (const auto &block : _history->blocks) {
		for (const auto &view : block->messages) {
			if (!check-- || (add(view.get()) && enoughForRecognition())) {
				return;
			}
		}
	}
}

void TranslateTracker::addBunchFrom(
		const std::vector<not_null<Element*>> &views) {
	if (enoughForRecognition()) {
		return;
	}
	startBunch();
	const auto guard = gsl::finally([&] {
		finishBunch();
	});

	auto check = kMaxCheckInBunch;
	for (const auto &view : views) {
		if (!check-- || (add(view.get()) && enoughForRecognition())) {
			return;
		}
	}
}

void TranslateTracker::cancelToRequest() {
	if (!_itemsToRequest.empty()) {
		const auto owner = &_history->owner();
		for (const auto &[id, entry] : base::take(_itemsToRequest)) {
			if (const auto item = owner->message(id)) {
				item->translationShowRequiresRequest({});
			}
		}
	}
}

void TranslateTracker::cancelSentRequest() {
	if (_requestInProcess) {
		const auto owner = &_history->owner();
		for (const auto &id : base::take(_requested)) {
			if (const auto item = owner->message(id)) {
				item->translationShowRequiresRequest({});
			}
		}
		++_requestToken;
		_requestInProcess = false;
	}
}

void TranslateTracker::stopAndRevert() {
	cancelToRequest();
	cancelSentRequest();
	const auto owner = &_history->owner();
	for (const auto &[id, entry] : _itemsForRecognize) {
		if (const auto item = owner->message(id)) {
			if (item->translation()
				&& item->translationShowRequiresCheck({})) {
				item->translationShowRequiresRequest({});
			}
		}
	}
	_history->translateTo({});
	if (const auto migrated = _history->migrateFrom()) {
		migrated->translateTo({});
	}
}

void TranslateTracker::requestSome() {
	if (_requestInProcess || _itemsToRequest.empty()) {
		return;
	}
	const auto to = _history->translatedTo();
	if (!to) {
		cancelToRequest();
		return;
	}
	_requested.clear();
	_requested.reserve(_itemsToRequest.size());
	const auto session = &_history->session();
	const auto peerId = _itemsToRequest.back().first.peer;
	const auto rich = _itemsToRequest.back().second.rich;
	auto length = 0;
	for (auto i = _itemsToRequest.end(); i != _itemsToRequest.begin();) {
		--i;
		if (i->first.peer != peerId
			|| i->second.rich != rich) {
			break;
		}
		length += i->second.length;
		_requested.push_back(i->first);
		i = _itemsToRequest.erase(i);
		if (_requested.size() >= kRequestCountLimit
			|| length >= kRequestLengthLimit) {
			break;
		}
	}
	if (_requested.empty()) {
		return;
	}
	if (rich) {
		requestSomeRich(to, peerId);
		return;
	}
	const auto owner = &session->data();
	auto requests = std::vector<Ui::TranslateProviderRequest>();
	requests.reserve(_requested.size());
	auto ids = std::vector<FullMsgId>();
	ids.reserve(_requested.size());
	for (const auto &id : _requested) {
		if (const auto item = owner->message(id)) {
			requests.push_back(Ui::PrepareTranslateProviderRequest(
				_provider.get(),
				session->data().peer(id.peer),
				id.msg,
				item->originalText()));
			ids.push_back(id);
		}
	}
	_requested = std::move(ids);
	if (_requested.empty()) {
		requestSome();
		return;
	}
	_requestInProcess = true;
	const auto requestToken = ++_requestToken;
	_provider->requestBatch(
		std::move(requests),
		to,
		[=](int index, Ui::TranslateProviderResult result) {
			if (!_requestInProcess || (_requestToken != requestToken)) {
				return;
			}
			if (index < 0 || index >= _requested.size()) {
				return;
			}
			const auto &id = _requested[index];
			if (const auto item = owner->message(id)) {
				item->translationDone(
					to,
					result.text.value_or(TextWithEntities()));
			}
		},
		[=] {
			if (!_requestInProcess || (_requestToken != requestToken)) {
				return;
			}
			_requestInProcess = false;
			_requested.clear();
			requestSome();
		});
}

void TranslateTracker::requestSomeRich(LanguageId to, PeerId peerId) {
	const auto session = &_history->session();
	const auto owner = &session->data();
	const auto peer = owner->peerLoaded(peerId);
	if (!peer) {
		for (const auto &id : base::take(_requested)) {
			if (const auto item = owner->message(id)) {
				item->translationDone(to, TextWithEntities());
			}
		}
		requestSome();
		return;
	}
	auto mtpIds = QVector<MTPint>();
	mtpIds.reserve(_requested.size());
	auto ids = std::vector<FullMsgId>();
	ids.reserve(_requested.size());
	for (const auto &id : _requested) {
		const auto item = owner->message(id);
		if (item && item->richPage()) {
			mtpIds.push_back(MTP_int(id.msg));
			ids.push_back(id);
		}
	}
	_requested = std::move(ids);
	if (_requested.empty()) {
		requestSome();
		return;
	}
	_requestInProcess = true;
	const auto requestToken = ++_requestToken;
	const auto finish = [=] {
		_requestInProcess = false;
		_requested.clear();
		requestSome();
	};
	using Flag = MTPmessages_TranslateRichMessage::Flag;
	_api.request(MTPmessages_TranslateRichMessage(
		MTP_flags(Flag::f_peer | Flag::f_id),
		peer->input(),
		MTP_vector<MTPint>(mtpIds),
		MTPVector<MTPInputRichMessage>(),
		MTP_string(to.twoLetterCode()),
		MTPstring()
	)).done([=](const MTPmessages_TranslatedRichMessage &result) {
		if (!_requestInProcess || (_requestToken != requestToken)) {
			return;
		}
		const auto &list = result.data().vresult().v;
		for (auto i = 0, count = int(_requested.size()); i != count; ++i) {
			if (const auto item = owner->message(_requested[i])) {
				item->translationDone(to, (i < list.size())
					? Iv::ParseRichPage(session, list[i])
					: nullptr);
			}
		}
		finish();
	}).fail([=](const MTP::Error &) {
		if (!_requestInProcess || (_requestToken != requestToken)) {
			return;
		}
		for (const auto &id : _requested) {
			if (const auto item = owner->message(id)) {
				item->translationDone(to, TextWithEntities());
			}
		}
		finish();
	}).send();
}

void TranslateTracker::applyLimit() {
	const auto generationProjection = [](const auto &pair) {
		return pair.second.generation;
	};
	const auto owner = &_history->owner();

	// Erase starting with oldest generation till items count is not too big.
	while (_itemsForRecognize.size() > _limit) {
		const auto oldest = ranges::min_element(
			_itemsForRecognize,
			ranges::less(),
			generationProjection
		)->second.generation;
		for (auto i = begin(_itemsForRecognize)
			; i != end(_itemsForRecognize);) {
			if (i->second.generation == oldest) {
				if (const auto j = _itemsToRequest.find(i->first)
					; j != end(_itemsToRequest)) {
					if (const auto item = owner->message(i->first)) {
						item->translationShowRequiresRequest({});
					}
					_itemsToRequest.erase(j);
				}
				i = _itemsForRecognize.erase(i);
			} else {
				++i;
			}
		}
	}
}

void TranslateTracker::recognizeCollected() {
	for (auto &[id, entry] : _itemsForRecognize) {
		if (const auto text = std::get_if<QString>(&entry.id)) {
			entry.id = Platform::Language::Recognize(*text);
		}
	}
}

void TranslateTracker::trackSkipLanguages() {
	Core::App().settings().skipTranslationLanguagesValue(
	) | rpl::on_next([=](const std::vector<LanguageId> &skip) {
		const auto wasOfferedFrom = _history->translateOfferedFrom();
		const auto wasTranslatedTo = _history->translatedTo();
		checkRecognized(skip);
		if (wasTranslatedTo
			&& wasOfferedFrom
			&& !_history->translateOfferedFrom()) {
			stopAndRevert();
		}
	}, _trackingLifetime);
}

// PeerData::translationFlag() is Unknown until the peer's full info comes back
// from the server, and setup() only asks for it. Lumina::TranslateOfferSkip()
// requires Enabled, so on a chat opened right after launch the first
// checkRecognized() runs while the answer is still in flight and finds no
// policy - and nothing re-runs it afterwards, because finishBunch() only calls
// it when a bunch actually added new messages and a repaint of the same
// messages adds none. That is a chat that recognised its language and then sat
// there with no offer, which is a per-chat translate control with nothing to
// write to. So the offer is re-evaluated here, where the flag finally arrives,
// and only while the policy is in force: with the tier off this is the upstream
// handler unchanged.
void TranslateTracker::trackTranslationDisabled() {
	using PeerFlag = Data::PeerUpdate::Flag;
	_history->session().changes().peerFlagsValue(
		_history->peer,
		PeerFlag::TranslationDisabled
	) | rpl::skip(1) | rpl::on_next([=] {
		using TranslationFlag = PeerData::TranslationFlag;
		const auto disabled = (_history->peer->translationFlag()
			== TranslationFlag::Disabled);
		if (!disabled) {
			if (Lumina::TranslateOfferSkip(_history)) {
				checkRecognized();
			}
		} else if (_history->translatedTo()) {
			stopAndRevert();
		}
	}, _trackingLifetime);
}

void TranslateTracker::checkRecognized() {
	checkRecognized(Core::App().settings().skipTranslationLanguages());
}

// LuminaGram: while Lumina::TranslateOfferSkip() holds a value, this chat is
// one the user may switch on by hand, and both of the guesses upstream makes
// about whether an offer is wanted at all are replaced. Only then:
//
//  * the skip list becomes the one language an offer would be pointless for,
//    the read language itself, instead of every language tdesktop assumes the
//    user knows - which by default holds the interface language, so a peer
//    writing it was the case that produced no offer, a per-chat control that
//    wrote into nothing, and no explanation;
//  * the count threshold becomes kEnoughForRelaxedOffer, so the control is
//    live from the chat's first recognised message rather than its sixth.
//
// The "don't change offer by small amount of messages" branch is kept for the
// unrelaxed case unchanged: it is what carries an offer across a chat being
// reopened with only a few messages loaded, and dropping it would make offers
// flicker for everyone. Withdrawing a relaxed offer is a transition, not a
// count, and is handled where the policy is watched in setup().
void TranslateTracker::checkRecognized(const std::vector<LanguageId> &skip) {
	if (!_trackingLanguage.current()) {
		_history->translateOfferFrom({});
		return;
	}
	const auto relaxed = Lumina::TranslateOfferSkip(_history);
	const auto &effectiveSkip = relaxed ? *relaxed : skip;
	auto languages = base::flat_map<LanguageId, int>();
	for (const auto &[id, entry] : _itemsForRecognize) {
		if (const auto id = std::get_if<LanguageId>(&entry.id)) {
			// QLocale::C is what Qt hands back for a name it cannot parse,
			// and CLD3 has labels Qt does not know ("bh", and the Latin
			// transliteration entries). LanguageId::known() still says true
			// for it and LanguageId::operator== normalises it to English
			// (lib_spellcheck/spellcheck/spellcheck_types.h), so an
			// unrecognised answer is counted as a recognised ENGLISH message.
			// Upstream needs kEnoughForTranslation of those before it acts;
			// under the relaxed policy one is enough, which is a single
			// mislabelled two-word message offering a chat that may well be
			// in the read language already, and a translate bar for it. Only
			// dropped where the threshold is 1; the unrelaxed path stays
			// byte-for-byte upstream.
			if (*id
				&& (!relaxed || (id->value != QLocale::C))
				&& !ranges::contains(effectiveSkip, *id)) {
				++languages[*id];
			}
		}
	}
	using namespace base;
	const auto count = int(_itemsForRecognize.size());
	constexpr auto p = &flat_multi_map_pair_type<LanguageId, int>::second;
	const auto threshold = relaxed
		? kEnoughForRelaxedOffer
		: (count > kEnoughForRecognition)
		? (count * kEnoughForTranslation / kEnoughForRecognition)
		: _allLoaded
		? std::min(count, kEnoughForTranslation)
		: kEnoughForTranslation;
	const auto translatable = ranges::accumulate(
		languages,
		0,
		ranges::plus(),
		p);
	if (!relaxed && count < kEnoughForTranslation) {
		// Don't change offer by small amount of messages.
	} else if (translatable >= threshold) {
		_history->translateOfferFrom(
			ranges::max_element(languages, ranges::less(), p)->first);
	} else {
		_history->translateOfferFrom({});
	}
}

} // namespace HistoryView
