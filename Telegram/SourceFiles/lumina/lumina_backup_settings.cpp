/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_backup_settings.h"

#include "base/weak_ptr.h"
#include "core/application.h"
#include "core/file_utilities.h"
#include "lang/lang_keys.h"
#include "lumina/lumina_backup.h"
#include "lumina/lumina_locale.h"
#include "ui/boxes/confirm_box.h"
#include "ui/layers/generic_box.h"
#include "ui/qt_object_factory.h"
#include "ui/rp_widget.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/fields/password_input.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"

#include "styles/style_layers.h"
#include "styles/style_settings.h"
#include "styles/style_widgets.h"

namespace Lumina {
namespace {

// Ui::PasswordInput is a MaskedInputField, which descends from
// RpWidgetBase<QLineEdit> and not from Ui::RpWidget, so GenericBox::addRow()
// will not take one directly. Same wrapper the cloud-password page uses
// (settings/cloud_password/settings_cloud_password_common.cpp AddPasswordField),
// minus its centring, because a box row wants the full width.
[[nodiscard]] not_null<Ui::PasswordInput*> AddPassphraseField(
		not_null<Ui::GenericBox*> box,
		rpl::producer<QString> placeholder) {
	const auto &st = st::defaultInputField;
	auto owned = object_ptr<Ui::RpWidget>(box);
	const auto wrap = owned.data();
	wrap->resize(wrap->width(), st.heightMin);
	const auto field = Ui::CreateChild<Ui::PasswordInput>(
		wrap,
		st,
		std::move(placeholder));
	wrap->sizeValue(
	) | rpl::on_next([=](QSize size) {
		field->resize(size.width(), size.height());
		field->moveToLeft(0, 0);
	}, wrap->lifetime());
	box->addRow(std::move(owned));
	return field;
}

// Closing the box destroys the button that owns the click handler, and with it
// everything that handler captured. Both arguments are taken BY VALUE so that
// what runs after closeBox() lives on this frame and not inside the lambda that
// is being torn down.
void FinishPassphrase(
		not_null<Ui::GenericBox*> box,
		Fn<void(QString)> done,
		QString passphrase) {
	box->closeBox();
	done(passphrase);
}

void ExportPassphraseBox(
		not_null<Ui::GenericBox*> box,
		Fn<void(QString)> done) {
	box->setTitle(TrValue(u"LuminaBackupExportPassphraseTitle"_q));

	const auto first = AddPassphraseField(
		box,
		TrValue(u"LuminaBackupPassphraseHint"_q));
	Ui::AddSkip(box->verticalLayout());
	const auto second = AddPassphraseField(
		box,
		TrValue(u"LuminaBackupPassphraseRepeatHint"_q));
	box->setFocusCallback([=] {
		first->setFocusFast();
	});

	const auto submit = [=] {
		const auto passphrase = first->getLastText();
		if (passphrase.size() < MinimumPassphraseLength()) {
			first->showError();
			box->uiShow()->showBox(Ui::MakeInformBox(
				Tr(u"LuminaBackupPassphraseTooShort"_q)));
			return;
		} else if (second->getLastText() != passphrase) {
			second->showError();
			box->uiShow()->showBox(Ui::MakeInformBox(
				Tr(u"LuminaBackupPassphraseMismatch"_q)));
			return;
		}
		FinishPassphrase(box, done, passphrase);
	};
	QObject::connect(first.get(), &Ui::MaskedInputField::submitted, [=] {
		second->setFocusFast();
	});
	QObject::connect(second.get(), &Ui::MaskedInputField::submitted, submit);

	box->addButton(tr::lng_settings_save(), submit);
	box->addButton(tr::lng_cancel(), [=] {
		box->closeBox();
	});
}

void ImportPassphraseBox(
		not_null<Ui::GenericBox*> box,
		Fn<void(QString)> done) {
	box->setTitle(TrValue(u"LuminaBackupPassphraseTitle"_q));

	const auto field = AddPassphraseField(
		box,
		TrValue(u"LuminaBackupPassphraseHint"_q));
	box->setFocusCallback([=] {
		field->setFocusFast();
	});

	const auto submit = [=] {
		const auto passphrase = field->getLastText();
		if (passphrase.size() < MinimumPassphraseLength()) {
			field->showError();
			box->uiShow()->showBox(Ui::MakeInformBox(
				Tr(u"LuminaBackupPassphraseTooShort"_q)));
			return;
		}
		FinishPassphrase(box, done, passphrase);
	};
	QObject::connect(field.get(), &Ui::MaskedInputField::submitted, submit);

	box->addButton(tr::lng_box_ok(), submit);
	box->addButton(tr::lng_cancel(), [=] {
		box->closeBox();
	});
}

// Both file dialogs are queued (core/file_utilities.cpp InvokeQueued), so every
// callback below has to survive the controller going away before it runs.
void ExportBackup(not_null<Window::SessionController*> controller) {
	const auto weak = base::make_weak(controller);
	controller->show(Box(ExportPassphraseBox, [=](QString passphrase) {
		FileDialog::GetWritePath(
			Core::App().getFileDialogParent(),
			Tr(u"LuminaBackupSaveCaption"_q),
			Tr(u"LuminaBackupFileFilter"_q),
			filedialogDefaultName(
				u"luminagram-backup"_q,
				BackupFileExtension()),
			[=](const QString &path) {
				if (path.isEmpty()) {
					return;
				}
				const auto error = WriteBackup(path, passphrase);
				const auto strong = weak.get();
				if (!strong) {
					return;
				} else if (error == BackupError::None) {
					strong->showToast(Tr(u"LuminaBackupExportDone"_q));
				} else {
					strong->show(Ui::MakeInformBox(BackupErrorText(error)));
				}
			});
	}));
}

void ImportBackup(not_null<Window::SessionController*> controller) {
	const auto weak = base::make_weak(controller);
	FileDialog::GetOpenPath(
		Core::App().getFileDialogParent(),
		Tr(u"LuminaBackupOpenCaption"_q),
		Tr(u"LuminaBackupFileFilter"_q),
		[=](const FileDialog::OpenResult &result) {
			const auto strong = weak.get();
			if (!strong) {
				return;
			}

			// The envelope is read and checked before the passphrase is asked
			// for, so that a file which is not a backup at all says so instead
			// of asking for a passphrase it can never accept.
			auto read = BackupRead();
			if (!result.paths.isEmpty()) {
				read = ReadBackupFile(result.paths.front());
			} else if (!result.remoteContent.isEmpty()) {
				read = ReadBackupBytes(result.remoteContent);
			} else {
				return;
			}
			if (read.error != BackupError::None) {
				strong->show(Ui::MakeInformBox(BackupErrorText(read.error)));
				return;
			}

			const auto envelope = read.envelope;
			strong->show(Box(ImportPassphraseBox, [=](QString passphrase) {
				const auto error = ApplyBackup(envelope, passphrase);
				if (const auto again = weak.get()) {
					again->show(Ui::MakeInformBox(
						(error == BackupError::None)
							? Tr(u"LuminaBackupImportSuccess"_q)
							: BackupErrorText(error)));
				}
			}));
		});
}

void AddActionRow(
		not_null<Ui::VerticalLayout*> container,
		rpl::producer<QString> label,
		Fn<void()> activate) {
	container->add(object_ptr<Ui::SettingsButton>(
		container,
		std::move(label),
		st::settingsButtonNoIcon
	))->setClickedCallback(std::move(activate));
}

} // namespace

void AddBackupRows(
		not_null<Ui::VerticalLayout*> container,
		not_null<Window::SessionController*> controller) {
	Ui::AddSkip(container);
	Ui::AddSubsectionTitle(container, TrValue(u"LuminaBackupTitle"_q));

	AddActionRow(
		container,
		TrValue(u"LuminaBackupExport"_q),
		[=] { ExportBackup(controller); });
	AddActionRow(
		container,
		TrValue(u"LuminaBackupImport"_q),
		[=] { ImportBackup(controller); });

	Ui::AddSkip(container);
	Ui::AddDividerText(container, TrValue(u"LuminaBackupExportInfo"_q));
	Ui::AddSkip(container);
	Ui::AddDividerText(container, TrValue(u"LuminaBackupImportInfo"_q));
}

} // namespace Lumina
