/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_file_guard.h"

#include "base/flat_set.h"
#include "boxes/abstract_box.h"
#include "data/data_document.h"
#include "lang/lang_keys.h"
#include "lumina/lumina_locale.h"
#include "lumina/lumina_settings.h"
#include "ui/layers/generic_box.h"
#include "ui/text/text_entity.h"
#include "ui/widgets/labels.h"
#include "ui/widgets/buttons.h"
#include "window/window_session_controller.h"

#include "styles/style_layers.h"

#include <QtCore/QChar>
#include <QtCore/QStringList>
#include <QtGui/QKeyEvent>

#include <algorithm>

namespace Lumina {
namespace {

const auto kKeyEnabled = u"fileMasqueradeGuard"_q;

// A cross-platform set of extensions that name something the operating system
// treats as code to run rather than data to view. Wider than Core::NameType's
// per-platform executable list on purpose: a Windows .exe is a masqueraded
// file worth naming even on a Mac, because the user may forward it on. Lower
// case, no leading dot.
[[nodiscard]] const base::flat_set<QString> &ExecutableExtensions() {
	static const auto result = base::flat_set<QString>{
		u"apk"_q, u"app"_q, u"appx"_q, u"bash"_q, u"bat"_q, u"cmd"_q,
		u"com"_q, u"command"_q, u"cpl"_q, u"deb"_q, u"dmg"_q, u"exe"_q,
		u"gadget"_q, u"hta"_q, u"ipa"_q, u"jar"_q, u"jnlp"_q, u"js"_q,
		u"jse"_q, u"ksh"_q, u"lnk"_q, u"msc"_q, u"msi"_q, u"msp"_q,
		u"pif"_q, u"pkg"_q, u"ps1"_q, u"psm1"_q, u"reg"_q, u"rpm"_q,
		u"run"_q, u"scr"_q, u"sh"_q, u"vbe"_q, u"vbs"_q, u"wsf"_q,
		u"wsh"_q, u"zsh"_q,
	};
	return result;
}

// The benign types a masqueraded executable dresses itself as: images, video,
// audio, documents and the archive extensions that are classic e-mail lures.
[[nodiscard]] const base::flat_set<QString> &DecoyExtensions() {
	static const auto result = base::flat_set<QString>{
		u"avif"_q, u"bmp"_q, u"gif"_q, u"heic"_q, u"heif"_q, u"ico"_q,
		u"jfif"_q, u"jpeg"_q, u"jpg"_q, u"png"_q, u"svg"_q, u"tif"_q,
		u"tiff"_q, u"webp"_q,
		u"3gp"_q, u"3gpp"_q, u"avi"_q, u"flv"_q, u"m4v"_q, u"mkv"_q,
		u"mov"_q, u"mp4"_q, u"mpeg"_q, u"mpg"_q, u"webm"_q, u"wmv"_q,
		u"aac"_q, u"flac"_q, u"m4a"_q, u"mp3"_q, u"ogg"_q, u"opus"_q,
		u"wav"_q, u"wma"_q,
		u"csv"_q, u"doc"_q, u"docx"_q, u"epub"_q, u"odp"_q, u"ods"_q,
		u"odt"_q, u"pdf"_q, u"ppt"_q, u"pptx"_q, u"rtf"_q, u"txt"_q,
		u"xls"_q, u"xlsx"_q,
		u"7z"_q, u"gz"_q, u"rar"_q, u"tar"_q, u"zip"_q,
	};
	return result;
}

[[nodiscard]] const base::flat_set<QString> &ExecutableMimes() {
	static const auto result = base::flat_set<QString>{
		u"application/bat"_q,
		u"application/java-archive"_q,
		u"application/vnd.android.package-archive"_q,
		u"application/vnd.debian.binary-package"_q,
		u"application/vnd.microsoft.portable-executable"_q,
		u"application/x-apple-diskimage"_q,
		u"application/x-bat"_q,
		u"application/x-csh"_q,
		u"application/x-deb"_q,
		u"application/x-elf"_q,
		u"application/x-executable"_q,
		u"application/x-java-archive"_q,
		u"application/x-mach-binary"_q,
		u"application/x-ms-dos-executable"_q,
		u"application/x-ms-shortcut"_q,
		u"application/x-msdos-program"_q,
		u"application/x-msdownload"_q,
		u"application/x-msi"_q,
		u"application/x-rpm"_q,
		u"application/x-sh"_q,
		u"application/x-shellscript"_q,
	};
	return result;
}

[[nodiscard]] bool IsBidiControl(QChar ch) {
	const auto u = ch.unicode();
	return (u >= 0x202A && u <= 0x202E) // LRE RLE PDF LRO RLO
		|| (u >= 0x2066 && u <= 0x2069) // LRI RLI FSI PDI
		|| (u == 0x200E) // LRM
		|| (u == 0x200F) // RLM
		|| (u == 0x061C); // ALM
}

[[nodiscard]] bool HasBidiControl(const QString &name) {
	for (const auto ch : name) {
		if (IsBidiControl(ch)) {
			return true;
		}
	}
	return false;
}

[[nodiscard]] QString BaseName(const QString &fileName) {
	const auto slash = std::max(
		fileName.lastIndexOf(QChar('/')),
		fileName.lastIndexOf(QChar('\\')));
	return (slash >= 0) ? fileName.mid(slash + 1) : fileName;
}

[[nodiscard]] QStringList ExtensionChain(const QString &fileName) {
	return BaseName(fileName).toLower().split(QChar('.'), Qt::SkipEmptyParts);
}

[[nodiscard]] bool HasDoubleExtension(const QString &fileName) {
	const auto parts = ExtensionChain(fileName);
	if (parts.size() < 3) {
		return false;
	} else if (!ExecutableExtensions().contains(parts.back())) {
		return false;
	}
	for (auto i = 1, count = int(parts.size()) - 1; i < count; ++i) {
		if (DecoyExtensions().contains(parts[i])) {
			return true;
		}
	}
	return false;
}

enum class MimeKind {
	Other,
	Executable,
	Media,
};

[[nodiscard]] MimeKind MimeKindOf(const QString &mime) {
	const auto m = mime.toLower().trimmed();
	if (m.isEmpty()) {
		return MimeKind::Other;
	} else if (ExecutableMimes().contains(m)) {
		return MimeKind::Executable;
	} else if (m.startsWith(u"image/"_q)
		|| m.startsWith(u"video/"_q)
		|| m.startsWith(u"audio/"_q)
		|| m.startsWith(u"text/"_q)
		|| m == u"application/pdf"_q) {
		return MimeKind::Media;
	}
	return MimeKind::Other;
}

[[nodiscard]] bool HasTypeMismatch(
		const QString &fileName,
		const QString &mime) {
	const auto parts = ExtensionChain(fileName);
	if (parts.size() < 2) {
		return false;
	}
	const auto &ext = parts.back();
	const auto extExecutable = ExecutableExtensions().contains(ext);
	const auto extDecoy = DecoyExtensions().contains(ext);
	const auto mimeKind = MimeKindOf(mime);
	return (extExecutable && mimeKind == MimeKind::Media)
		|| (extDecoy && mimeKind == MimeKind::Executable);
}

[[nodiscard]] QString ReasonText(FileGuardReason reason) {
	switch (reason) {
	case FileGuardReason::BidiOverride:
		return Tr(u"LuminaFileGuardWarnBidi"_q);
	case FileGuardReason::DoubleExtension:
		return Tr(u"LuminaFileGuardWarnDouble"_q);
	case FileGuardReason::TypeMismatch:
		return Tr(u"LuminaFileGuardWarnMismatch"_q);
	}
	return QString();
}

[[nodiscard]] QString JoinReasons(const std::vector<FileGuardReason> &reasons) {
	auto result = QString();
	for (const auto reason : reasons) {
		const auto text = ReasonText(reason);
		if (text.isEmpty()) {
			continue;
		} else if (!result.isEmpty()) {
			result += u"\n\n"_q;
		}
		result += text;
	}
	return result;
}

// The name as it is safe to print. A bidirectional control character in the
// name is the whole point of one of the warnings, so it must never reach the
// label verbatim - the label would honour it and reproduce the very disguise
// inside the box that exists to expose it. Each control is replaced with
// U+FFFD, which both stops the reordering and marks where it was.
[[nodiscard]] QString DisplayName(const QString &fileName) {
	auto result = QString();
	result.reserve(fileName.size());
	for (const auto ch : fileName) {
		result += IsBidiControl(ch) ? QChar(0xFFFD) : ch;
	}
	return result;
}

void FillBox(
		not_null<Ui::GenericBox*> box,
		const QString &displayName,
		const QString &warnings,
		Fn<void()> proceed) {
	box->setTitle(TrValue(u"LuminaFileGuardTitle"_q));

	box->addRow(object_ptr<Ui::FlatLabel>(
		box,
		rpl::single(warnings),
		st::boxLabel));

	const auto caption = Tr(u"LuminaFileGuardName"_q);
	auto nameText = TextWithEntities{
		.text = caption + u"\n"_q + displayName,
	};
	nameText.entities.push_back(EntityInText(
		EntityType::Bold,
		int(caption.size()) + 1,
		int(displayName.size())));
	const auto label = box->addRow(object_ptr<Ui::FlatLabel>(
		box,
		rpl::single(nameText),
		st::boxLabel));
	label->setSelectable(true);
	label->setBreakEverywhere(true);

	box->addButton(
		TrValue(u"LuminaFileGuardOpen"_q),
		[=] { box->closeBox(); proceed(); },
		st::attentionBoxButton);

	const auto cancel = box->addButton(tr::lng_cancel(), [=] {
		box->closeBox();
	});

	box->setFocusCallback([=] {
		cancel->setFocus();
	});

	// The default confirmation binds Enter to its confirm button; here the
	// confirm button opens a possibly hostile file, so Enter must not reach
	// it. Swallow Enter into a cancel instead, and focus the cancel button
	// above, so opening is only ever a deliberate click on the red button.
	box->events(
	) | rpl::on_next([=](not_null<QEvent*> e) {
		if (e->type() != QEvent::KeyPress) {
			return;
		}
		const auto k = static_cast<QKeyEvent*>(e.get());
		if (k->key() == Qt::Key_Enter || k->key() == Qt::Key_Return) {
			box->closeBox();
		}
	}, box->lifetime());
}

} // namespace

FileGuardResult FileGuardCheck(const QString &fileName, const QString &mime) {
	auto result = FileGuardResult();
	if (fileName.isEmpty()) {
		return result;
	}
	if (HasBidiControl(fileName)) {
		result.reasons.push_back(FileGuardReason::BidiOverride);
	}
	if (HasDoubleExtension(fileName)) {
		result.reasons.push_back(FileGuardReason::DoubleExtension);
	}
	if (HasTypeMismatch(fileName, mime)) {
		result.reasons.push_back(FileGuardReason::TypeMismatch);
	}
	result.suspicious = !result.reasons.empty();
	return result;
}

bool FileGuardEnabled() {
	return Settings::Instance().getBool(kKeyEnabled, true);
}

void SetFileGuardEnabled(bool value) {
	// The store is named rather than defaulted: Settings::set() defaults to
	// Store::Prefs and silently relocates a key written through that default,
	// so a bare set() here would be a trap the day this key moves.
	Settings::Instance().set(kKeyEnabled, value, Store::Prefs);
}

rpl::producer<> FileGuardChanges() {
	return Settings::Instance().changesFor(kKeyEnabled);
}

bool FileGuardIntercept(
		Window::SessionController *controller,
		not_null<DocumentData*> document,
		Fn<void()> proceed) {
	if (!FileGuardEnabled()) {
		return false;
	}
	const auto name = document->filename();
	const auto result = FileGuardCheck(name, document->mimeString());
	if (!result.suspicious) {
		return false;
	}
	auto box = Box(
		FillBox,
		DisplayName(name),
		JoinReasons(result.reasons),
		std::move(proceed));
	if (controller) {
		controller->uiShow()->showBox(std::move(box));
	} else {
		Ui::show(std::move(box));
	}
	return true;
}

} // namespace Lumina
