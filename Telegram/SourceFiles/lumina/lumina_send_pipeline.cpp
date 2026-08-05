/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_send_pipeline.h"

namespace Lumina {
namespace {

[[nodiscard]] std::vector<SendInterceptor> &Interceptors() {
	static auto result = std::vector<SendInterceptor>();
	return result;
}

// Runs the interceptors starting at `index` and reports whether the send may
// go ahead. Nothing is registered yet, so today this always returns true on
// the first check and the composers send exactly the way they did before.
[[nodiscard]] bool RunFrom(
		int index,
		not_null<History*> history,
		TextWithTags &text,
		const Api::SendOptions &options,
		const Fn<void()> &proceed) {
	auto &list = Interceptors();

	// The registry is append-only, so an index still means the same
	// interceptor when a held send resumes the chain much later.
	for (auto i = index; i != int(list.size()); ++i) {
		const auto resumeFrom = i + 1;

		// Owned by `proceed`, which `resume` keeps alive.
		const auto resumeText = &text;

		auto resume = [=] {
			if (RunFrom(resumeFrom, history, *resumeText, options, proceed)) {
				proceed();
			}
		};

		// A copy: an interceptor is allowed to register another one, which
		// would reallocate the list out from under us.
		auto interceptor = list[i];
		if (!interceptor(history, text, options, std::move(resume))) {
			return false;
		}
	}
	return true;
}

} // namespace

bool InterceptSend(
		not_null<History*> history,
		TextWithTags &text,
		Api::SendOptions options,
		Fn<void()> proceed) {
	Expects(proceed != nullptr);

	return RunFrom(0, history, text, options, proceed);
}

void RegisterSendInterceptor(SendInterceptor interceptor) {
	Expects(interceptor != nullptr);

	Interceptors().push_back(std::move(interceptor));
}

} // namespace Lumina
