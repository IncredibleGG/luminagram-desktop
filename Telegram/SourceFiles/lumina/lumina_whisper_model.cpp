/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_whisper_model.h"

#include "settings.h" // cWorkingDir()

#include <rpl/event_stream.h>

#include <algorithm>
#include <memory>

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QCryptographicHash>
#include <QtCore/QPointer>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

namespace Lumina {
namespace {

// The exact model, hosted on the project's own R2 bucket. The size is the
// readiness gate (a truncated download is not `kModelSize` bytes), and the
// sha256 is verified once on finish so a corrupted or substituted file is
// deleted rather than loaded into whisper.cpp.
constexpr auto kModelSize = qint64(59707625);
const auto kModelUrl = QString::fromLatin1(
	"https://pub-d6a54d2e5f5947e2b0b23fb8e27ce0a5.r2.dev"
	"/models/ggml-base-q5_1.bin");
const auto kModelSha256 = QByteArray(
	"422f1ae452ade6f30a004d7e5c6a43195e4433bc370bf23fac9cc591f01a8898");

[[nodiscard]] QString ModelsDir() {
	return cWorkingDir() + u"tdata/luminagram-models"_q;
}

// A single main-thread owner of the download: the QNetworkAccessManager, the
// running reply, the open .tmp file, the running hash and the state the UI
// reads. Everything here touches the network reply's signals, which fire on
// the main thread, so no locking is needed.
class Manager final {
public:
	[[nodiscard]] static Manager &Instance() {
		static Manager instance;
		return instance;
	}

	[[nodiscard]] WhisperModelState state() const {
		return _state;
	}
	[[nodiscard]] rpl::producer<> changes() const {
		return _changes.events();
	}

	void download();
	void remove();

private:
	void setState(WhisperModelState state);
	void fail();
	void cleanupReply();

	QNetworkAccessManager _network;
	QPointer<QNetworkReply> _reply;
	std::unique_ptr<QFile> _tmp;
	QCryptographicHash _hash{ QCryptographicHash::Sha256 };
	WhisperModelState _state;
	rpl::event_stream<> _changes;

};

void Manager::setState(WhisperModelState state) {
	_state = state;
	_changes.fire({});
}

void Manager::cleanupReply() {
	if (_reply) {
		_reply->disconnect();
		_reply->deleteLater();
		_reply = nullptr;
	}
	if (_tmp) {
		_tmp->close();
		_tmp = nullptr;
	}
}

void Manager::fail() {
	const auto tmpPath = WhisperModelPath() + u".tmp"_q;
	cleanupReply();
	QFile::remove(tmpPath);
	setState({ .stage = WhisperModelState::Stage::Absent });
}

void Manager::download() {
	if (_reply) {
		return; // Already in flight.
	} else if (WhisperModelReady()) {
		setState({ .stage = WhisperModelState::Stage::Ready });
		return;
	}
	QDir().mkpath(ModelsDir());

	const auto tmpPath = WhisperModelPath() + u".tmp"_q;
	QFile::remove(tmpPath);
	_tmp = std::make_unique<QFile>(tmpPath);
	if (!_tmp->open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		fail();
		return;
	}
	_hash.reset();

	setState({ .stage = WhisperModelState::Stage::Downloading, .progress = 0 });

	auto request = QNetworkRequest(QUrl(kModelUrl));
	request.setAttribute(
		QNetworkRequest::RedirectPolicyAttribute,
		QNetworkRequest::NoLessSafeRedirectPolicy);
	_reply = _network.get(request);

	QObject::connect(_reply, &QNetworkReply::readyRead, _reply, [=] {
		if (!_reply || !_tmp) {
			return;
		}
		const auto chunk = _reply->readAll();
		if (chunk.isEmpty()) {
			return;
		}
		_hash.addData(chunk);
		if (_tmp->write(chunk) != chunk.size()) {
			fail(); // Disk full or unwritable; stop rather than keep a partial.
		}
	});
	QObject::connect(_reply, &QNetworkReply::downloadProgress, _reply, [=](
			qint64 received,
			qint64 total) {
		if (_state.stage != WhisperModelState::Stage::Downloading) {
			return;
		}
		const auto known = (total > 0) ? total : kModelSize;
		const auto percent = (known > 0)
			? int((received * 100) / known)
			: 0;
		setState({
			.stage = WhisperModelState::Stage::Downloading,
			.progress = std::clamp(percent, 0, 100),
		});
	});
	QObject::connect(_reply, &QNetworkReply::finished, _reply, [=] {
		if (!_reply || !_tmp) {
			return;
		}
		const auto failure = (_reply->error() != QNetworkReply::NoError);
		// Any bytes still buffered after the last readyRead.
		const auto tail = _reply->readAll();
		if (!tail.isEmpty()) {
			_hash.addData(tail);
			_tmp->write(tail);
		}
		_tmp->flush();
		_tmp->close();
		const auto digest = _hash.result().toHex();
		const auto size = QFileInfo(tmpPath).size();

		if (failure || size != kModelSize || digest != kModelSha256) {
			fail();
			return;
		}
		// Verified. Atomic rename into place; drop any stale target first so
		// the rename cannot fail on an existing file.
		const auto finalPath = WhisperModelPath();
		QFile::remove(finalPath);
		cleanupReply();
		if (!QFile::rename(tmpPath, finalPath)) {
			QFile::remove(tmpPath);
			setState({ .stage = WhisperModelState::Stage::Absent });
			return;
		}
		setState({ .stage = WhisperModelState::Stage::Ready });
	});
}

void Manager::remove() {
	if (_reply) {
		// A running download counts as the model too; cancel it first.
		_reply->abort();
	}
	cleanupReply();
	QFile::remove(WhisperModelPath() + u".tmp"_q);
	QFile::remove(WhisperModelPath());
	setState({ .stage = WhisperModelState::Stage::Absent });
}

} // namespace

QString WhisperModelPath() {
	return cWorkingDir() + u"tdata/luminagram-models/ggml-base-q5_1.bin"_q;
}

bool WhisperModelReady() {
	const auto info = QFileInfo(WhisperModelPath());
	return info.exists() && (info.size() == kModelSize);
}

WhisperModelState CurrentWhisperModelState() {
	auto &manager = Manager::Instance();
	// A model that is already on disk reads as Ready even if this process has
	// never touched the manager (a download from a previous run).
	if (manager.state().stage != WhisperModelState::Stage::Downloading
		&& WhisperModelReady()) {
		return { .stage = WhisperModelState::Stage::Ready };
	}
	return manager.state();
}

rpl::producer<> WhisperModelChanges() {
	return Manager::Instance().changes();
}

void DownloadWhisperModel() {
	Manager::Instance().download();
}

void DeleteWhisperModel() {
	Manager::Instance().remove();
}

} // namespace Lumina
