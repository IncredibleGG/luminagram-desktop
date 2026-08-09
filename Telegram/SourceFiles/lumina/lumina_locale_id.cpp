/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_locale.h"

namespace Lumina {
namespace {

// Indonesian. Wherever LuminaGram for Android already shows the same thing,
// the wording here is the wording that shipped there, so the two platforms
// read the same key by key. Keys Android does not have - and keys whose
// desktop English says something Android's does not - are translated from the
// English table in lumina_locale.cpp.
//
// Quoted row labels inside the divider paragraphs use the guillemets Android's
// Indonesian table uses, and the phrase between them is copied from the row it
// names, so the two always match.
[[nodiscard]] LocaleTable Build() {
	return {
		// Sections of the LuminaGram settings page.
		{ u"LuminaGramTitle"_q, u"LuminaGram"_q },
		{ u"LuminaTranslateTitle"_q, u"Terjemahan"_q },
		{ u"LuminaPrivacyTitle"_q, u"Privasi"_q },
		{ u"LuminaSecurityTitle"_q, u"Keamanan"_q },
		{ u"LuminaChatSettings"_q, u"Obrolan"_q },
		{ u"LuminaGramChatList"_q, u"Daftar obrolan"_q },
		{ u"LuminaAppearanceTitle"_q, u"Tampilan"_q },
		{ u"LuminaToolsTitle"_q, u"Alat"_q },
		{ u"LuminaGramStoredLocallyInfo"_q, u"Opsi LuminaGram hanya disimpan "
			u"di perangkat ini dan tidak pernah disinkronkan dengan "
			u"Telegram."_q },

		// Sub-pages that have no rows yet.
		{ u"LuminaAppearancePlaceholder"_q, u"Opsi pemformatan pesan, stiker, "
			u"dan angka akan muncul di sini."_q },
		{ u"LuminaChatListPlaceholder"_q, u"Opsi tata letak dan lencana "
			u"daftar obrolan akan muncul di sini."_q },
		{ u"LuminaPrivacyPlaceholder"_q, u"Opsi privasi tautan, papan klip, "
			u"dan media keluar akan muncul di sini."_q },
		{ u"LuminaSecurityPlaceholder"_q, u"Opsi brankas penyamaran, hapus "
			u"darurat, dan buka kunci paksaan akan muncul di sini."_q },
		{ u"LuminaToolsPlaceholder"_q, u"Penanda, templat balasan, dan "
			u"cadangan lokal akan muncul di sini."_q },

		// Chats sub-page.
		{ u"LuminaMessageActions"_q, u"Tindakan pesan"_q },
		{ u"LuminaAllowSaveRestricted"_q,
			u"Izinkan simpan / salin dari obrolan terbatas"_q },
		{ u"LuminaAllowSaveRestrictedInfo"_q, u"«Izinkan simpan / salin dari "
			u"obrolan terbatas» hanya memengaruhi tindakan lokal di perangkat "
			u"ini. Sebagian obrolan membatasi penyimpanan karena suatu alasan "
			u"— gunakan dengan bijak."_q },

		// Translation sub-page.
		{ u"LuminaTranslateEnable"_q, u"Aktifkan terjemahan LuminaGram"_q },
		{ u"LuminaTranslateEnableInfo"_q, u"Terjemahkan dengan mesin Anda "
			u"sendiri, bukan dengan layanan Premium milik Telegram. Mesin "
			u"bawaan tidak memerlukan akun maupun kunci API. Selama ini "
			u"nonaktif, LuminaGram membiarkan terjemahan persis seperti "
			u"bawaan Telegram Desktop."_q },
		{ u"LuminaTranslateSendHeader"_q, u"Pengiriman"_q },
		{ u"LuminaTranslateBeforeSend"_q, u"Terjemahkan sebelum kirim"_q },
		{ u"LuminaTranslateSendLang"_q, u"Bahasa kirim"_q },
		{ u"LuminaTranslateSendLangAuto"_q, u"Bahasa lawan bicara"_q },
		{ u"LuminaTranslateBeforeSendConfirm"_q,
			u"Konfirmasi sebelum kirim"_q },
		{ u"LuminaTranslateSendInfo"_q, u"Pesan keluar diterjemahkan ke "
			u"bahasa di atas, dan teks aslinya tetap disimpan bersama "
			u"terjemahannya. Dengan «Bahasa lawan bicara», LuminaGram "
			u"menanyakan sekali di tiap obrolan bahasa apa yang dipakai di "
			u"sana, lalu mengingatnya. «Konfirmasi sebelum kirim» menampilkan "
			u"terjemahan di samping teks asli lebih dulu, sehingga Anda bisa "
			u"mengirim salah satunya; jika dimatikan, terjemahan langsung "
			u"dikirim."_q },
		{ u"LuminaTranslateReceiveHeader"_q, u"Penerimaan"_q },
		{ u"LuminaDualLanguageDisplay"_q,
			u"Tampilkan teks asli dan terjemahan bersamaan"_q },
		{ u"LuminaTranslateReadLang"_q, u"Bahasa baca"_q },
		{ u"LuminaTranslateReadLangFollow"_q, u"Bahasa antarmuka"_q },
		{ u"LuminaTranslateReceiveInfo"_q, u"Pesan masuk tetap menampilkan "
			u"teks aslinya dalam ukuran penuh, dengan terjemahan di "
			u"bawahnya."_q },
		{ u"LuminaTranslateProviderHeader"_q, u"Layanan"_q },
		{ u"LuminaTranslateProvider"_q, u"Layanan terjemahan"_q },
		{ u"LuminaTranslateProviderLlm"_q, u"LLM (kompatibel OpenAI)"_q },
		{ u"LuminaTranslateApiKey"_q, u"Kunci API"_q },
		{ u"LuminaTranslateApiKeyNotSet"_q, u"Belum diatur"_q },
		{ u"LuminaTranslateBaseUrl"_q, u"URL dasar"_q },
		{ u"LuminaTranslateModel"_q, u"Model"_q },
		{ u"LuminaTranslateSystemPrompt"_q, u"Prompt sistem"_q },
		{ u"LuminaTranslatePromptDefault"_q, u"Bawaan"_q },
		{ u"LuminaTranslatePromptCustom"_q, u"Kustom"_q },
		{ u"LuminaTranslateFallbackTelegram"_q,
			u"Gunakan Telegram bila layanan ini gagal"_q },
		{ u"LuminaTranslateTest"_q, u"Uji terjemahan"_q },
		{ u"LuminaTranslateTestRunning"_q, u"Menguji…"_q },
		{ u"LuminaTranslateTestSuccess"_q, u"Terjemahan berfungsi."_q },
		{ u"LuminaTranslateTestFailed"_q, u"Pengujian gagal"_q },
		{ u"LuminaTranslateNoKey"_q, u"Perlu kunci API"_q },
		{ u"LuminaTranslateTestKeyRejected"_q, u"Layanan tidak dapat "
			u"dihubungi, atau layanan menolak kunci API ini. Periksa kunci "
			u"terlebih dahulu, lalu URL dasar dan koneksi Anda."_q },
		{ u"LuminaTranslateTestNetwork"_q, u"Layanan tidak dapat dihubungi. "
			u"Periksa koneksi internet dan proksi Anda, lalu coba lagi."_q },
		{ u"LuminaTranslateTestQuota"_q, u"Layanan menolak permintaan ini: "
			u"terlalu banyak permintaan, atau kuota kunci ini sudah habis. "
			u"Coba lagi nanti."_q },
		{ u"LuminaTranslateTestQuotaKeyed"_q, u"Layanan menolak permintaan "
			u"ini: layanan menolak kunci API ini, kuota kunci sudah habis, "
			u"atau permintaannya terlalu banyak. Periksa kuncinya dulu, lalu "
			u"coba lagi nanti."_q },
		{ u"LuminaTranslateTestBadResponse"_q, u"Layanan menjawab dengan "
			u"sesuatu yang tidak dapat dibaca LuminaGram, atau tidak "
			u"menjawab sama sekali. Periksa URL dasar dan modelnya."_q },
		{ u"LuminaTranslateTestUnavailable"_q, u"Layanan ini tidak dapat "
			u"dijalankan di sini. Pilih layanan lain di atas."_q },
		{ u"LuminaTranslateProviderSecurityInfo"_q, u"Kunci hanya disimpan di "
			u"perangkat ini, dalam berkas terpisah dari pengaturan lainnya, "
			u"dan tidak pernah dikirim ke Telegram. Semua yang Anda "
			u"terjemahkan dikirim ke layanan yang dipilih di sini, jadi "
			u"pilihlah yang Anda percaya."_q },

		// Translate before sending: the boxes and the send menu.
		{ u"LuminaTranslateOriginalLabel"_q, u"Asli"_q },
		{ u"LuminaSendTranslation"_q, u"Kirim terjemahan"_q },
		{ u"LuminaSendOriginal"_q, u"Kirim asli"_q },
		{ u"LuminaTrSendPickerTitle"_q, u"Terjemahkan pesan ke"_q },
		{ u"LuminaTrSendConfirmMessage"_q, u"Sepertinya obrolan ini ditulis "
			u"dalam {1}. Terjemahkan pesan yang Anda kirim di sini ke {1}? "
			u"LuminaGram akan mengingatnya untuk obrolan ini."_q },
		{ u"LuminaTrSendConfirmTranslate"_q, u"Terjemahkan"_q },
		{ u"LuminaTrSendAsTyped"_q, u"Kirim apa adanya"_q },
		{ u"LuminaTrSendChooseOther"_q, u"Pilih bahasa"_q },

		// The translate-before-send preview bar above the composer.
		{ u"LuminaTranslatePreviewTranslating"_q, u"Menerjemahkan…"_q },
		{ u"LuminaTranslatePreviewFailed"_q, u"Terjemahan tidak tersedia"_q },

		// The per-chat translate button in the chat top bar.
		{ u"LuminaTranslateChatToggle"_q, u"Terjemahkan obrolan ini"_q },
		{ u"LuminaTranslateChatShowOriginal"_q, u"Tampilkan asli"_q },

		// The menu that button opens: one row per side of the conversation.
		// Indonesian names the side after the thing it owns, so the rows say
		// «Pesan mereka» and «Pesan saya» — the two words a reader tells apart
		// at a glance, where a direction word told them nothing. Off is a row
		// of its own, so the slot after «ke» only ever holds a language name
		// or «Bahasa lawan bicara», and both of those follow «ke» properly.
		{ u"LuminaChatLangThem"_q, u"Pesan mereka, diterjemahkan ke {1}"_q },
		{ u"LuminaChatLangThemOff"_q, u"Pesan mereka, tidak diterjemahkan"_q },
		{ u"LuminaChatLangMe"_q, u"Pesan saya, diterjemahkan ke {1}"_q },
		{ u"LuminaChatLangMeOff"_q, u"Pesan saya, tidak diterjemahkan"_q },
		{ u"LuminaChatLangThemTitle"_q, u"Pesan mereka, diterjemahkan ke"_q },
		{ u"LuminaChatLangMeTitle"_q, u"Pesan saya, diterjemahkan ke"_q },
		{ u"LuminaChatLangNone"_q, u"Tidak diterjemahkan"_q },

		// Telegram's own AI editor, whose Translate tab overlaps ours.
		{ u"LuminaAiEditorHeader"_q, u"Editor AI Telegram"_q },
		{ u"LuminaAiEditorKeep"_q, u"Pertahankan editor AI Telegram"_q },
		{ u"LuminaAiEditorInfo"_q, u"Telegram memiliki editor AI sendiri di "
			u"kolom pesan, dan tab Terjemahkan di dalamnya tumpang tindih "
			u"dengan terjemahan LuminaGram. Selama terjemahan LuminaGram "
			u"aktif, tombol editor itu beserta pintasan papan ketiknya tidak "
			u"ditawarkan, sehingga hanya ada satu alat terjemahan di hadapan "
			u"Anda. Aktifkan opsi ini jika Anda tetap ingin editor Telegram "
			u"tersedia. Bila terjemahan LuminaGram nonaktif, editor Telegram "
			u"selalu tersedia dan pengaturan ini tidak mengubah apa pun."_q },

		// Names of the languages LuminaGram can translate into, in the order
		// the pickers show them.
		{ u"LuminaLangEn"_q, u"Inggris"_q },
		{ u"LuminaLangZhTw"_q, u"Tionghoa (Tradisional)"_q },
		{ u"LuminaLangZhCn"_q, u"Tionghoa (Sederhana)"_q },
		{ u"LuminaLangJa"_q, u"Jepang"_q },
		{ u"LuminaLangKo"_q, u"Korea"_q },
		{ u"LuminaLangEs"_q, u"Spanyol"_q },
		{ u"LuminaLangFr"_q, u"Prancis"_q },
		{ u"LuminaLangDe"_q, u"Jerman"_q },
		{ u"LuminaLangRu"_q, u"Rusia"_q },
		{ u"LuminaLangPtBr"_q, u"Portugis (Brasil)"_q },
		{ u"LuminaLangPtPt"_q, u"Portugis (Portugal)"_q },
		{ u"LuminaLangIt"_q, u"Italia"_q },
		{ u"LuminaLangAr"_q, u"Arab"_q },
		{ u"LuminaLangHi"_q, u"Hindi"_q },
		{ u"LuminaLangId"_q, u"Indonesia"_q },
		{ u"LuminaLangTh"_q, u"Thai"_q },
		{ u"LuminaLangVi"_q, u"Vietnam"_q },
		{ u"LuminaLangTr"_q, u"Turki"_q },
		{ u"LuminaLangPl"_q, u"Polandia"_q },
		{ u"LuminaLangUk"_q, u"Ukraina"_q },
		{ u"LuminaLangNl"_q, u"Belanda"_q },
		{ u"LuminaLangAf"_q, u"Afrikaans"_q },
		{ u"LuminaLangSq"_q, u"Albania"_q },
		{ u"LuminaLangAm"_q, u"Amhara"_q },
		{ u"LuminaLangHy"_q, u"Armenia"_q },
		{ u"LuminaLangAz"_q, u"Azerbaijan"_q },
		{ u"LuminaLangEu"_q, u"Basque"_q },
		{ u"LuminaLangBe"_q, u"Belarusia"_q },
		{ u"LuminaLangBn"_q, u"Bengali"_q },
		{ u"LuminaLangBs"_q, u"Bosnia"_q },
		{ u"LuminaLangBg"_q, u"Bulgaria"_q },
		{ u"LuminaLangMy"_q, u"Burma"_q },
		{ u"LuminaLangCa"_q, u"Katalan"_q },
		{ u"LuminaLangHr"_q, u"Kroasia"_q },
		{ u"LuminaLangCs"_q, u"Ceko"_q },
		{ u"LuminaLangDa"_q, u"Denmark"_q },
		{ u"LuminaLangEt"_q, u"Estonia"_q },
		{ u"LuminaLangTl"_q, u"Filipino"_q },
		{ u"LuminaLangFi"_q, u"Finlandia"_q },
		{ u"LuminaLangGl"_q, u"Galisia"_q },
		{ u"LuminaLangKa"_q, u"Georgia"_q },
		{ u"LuminaLangEl"_q, u"Yunani"_q },
		{ u"LuminaLangGu"_q, u"Gujarat"_q },
		{ u"LuminaLangHe"_q, u"Ibrani"_q },
		{ u"LuminaLangHu"_q, u"Hungaria"_q },
		{ u"LuminaLangIs"_q, u"Islandia"_q },
		{ u"LuminaLangGa"_q, u"Irlandia"_q },
		{ u"LuminaLangJv"_q, u"Jawa"_q },
		{ u"LuminaLangKn"_q, u"Kannada"_q },
		{ u"LuminaLangKk"_q, u"Kazakh"_q },
		{ u"LuminaLangKm"_q, u"Khmer"_q },
		{ u"LuminaLangKu"_q, u"Kurdi"_q },
		{ u"LuminaLangKy"_q, u"Kirgiz"_q },
		{ u"LuminaLangLo"_q, u"Lao"_q },
		{ u"LuminaLangLv"_q, u"Latvia"_q },
		{ u"LuminaLangLt"_q, u"Lituania"_q },
		{ u"LuminaLangMk"_q, u"Makedonia"_q },
		{ u"LuminaLangMs"_q, u"Melayu"_q },
		{ u"LuminaLangMl"_q, u"Malayalam"_q },
		{ u"LuminaLangMr"_q, u"Marathi"_q },
		{ u"LuminaLangMn"_q, u"Mongolia"_q },
		{ u"LuminaLangNe"_q, u"Nepal"_q },
		{ u"LuminaLangNo"_q, u"Norwegia"_q },
		{ u"LuminaLangPs"_q, u"Pashto"_q },
		{ u"LuminaLangFa"_q, u"Persia"_q },
		{ u"LuminaLangPa"_q, u"Punjabi"_q },
		{ u"LuminaLangRo"_q, u"Rumania"_q },
		{ u"LuminaLangSr"_q, u"Serbia"_q },
		{ u"LuminaLangSi"_q, u"Sinhala"_q },
		{ u"LuminaLangSk"_q, u"Slovak"_q },
		{ u"LuminaLangSl"_q, u"Slovenia"_q },
		{ u"LuminaLangSo"_q, u"Somali"_q },
		{ u"LuminaLangSw"_q, u"Swahili"_q },
		{ u"LuminaLangSv"_q, u"Swedia"_q },
		{ u"LuminaLangTg"_q, u"Tajik"_q },
		{ u"LuminaLangTa"_q, u"Tamil"_q },
		{ u"LuminaLangTe"_q, u"Telugu"_q },
		{ u"LuminaLangUr"_q, u"Urdu"_q },
		{ u"LuminaLangUz"_q, u"Uzbek"_q },
		{ u"LuminaLangCy"_q, u"Wales"_q },
		{ u"LuminaLangYi"_q, u"Yiddish"_q },
		{ u"LuminaLangZu"_q, u"Zulu"_q },
		{ u"LuminaAppearanceStickerSizeHeader"_q, u"Ukuran stiker"_q },
		{ u"LuminaBackupCryptoFailed"_q,
			u"Enkripsi tidak tersedia di sistem ini."_q },
		{ u"LuminaBackupDamaged"_q,
			u"Berkas cadangan rusak dan tidak dapat dipulihkan."_q },
		{ u"LuminaBackupExport"_q, u"Ekspor cadangan terenkripsi"_q },
		{ u"LuminaBackupExportDone"_q, u"Cadangan disimpan."_q },
		{ u"LuminaBackupExportFailed"_q,
			u"Tidak dapat membuat berkas cadangan."_q },
		{ u"LuminaBackupExportInfo"_q, u"Simpan semua yang disimpan "
			u"LuminaGram di perangkat ini — penanda, catatan, templat "
			u"balasan, penggantian teks, dan seluruh pengaturan — ke dalam "
			u"satu berkas, terenkripsi dengan frasa sandi pilihan Anda. "
			u"Berkas ini juga memuat nilai-nilai pribadi: kunci API "
			u"terjemahan, kode brankas dan crash palsu, serta catatan umpan. "
			u"Tidak ada yang dikirim ke Telegram. Pilih frasa sandi yang "
			u"panjang dan simpan di tempat aman — tanpa frasa itu berkas "
			u"tidak dapat dibuka, dan tidak ada cara untuk memulihkannya."_q },
		{ u"LuminaBackupExportPassphraseTitle"_q, u"Tetapkan frasa sandi"_q },
		{ u"LuminaBackupFileFilter"_q, u"Cadangan LuminaGram (*.lgbak)"_q },
		{ u"LuminaBackupImport"_q, u"Impor cadangan"_q },
		{ u"LuminaBackupImportFailed"_q,
			u"Tidak dapat membaca berkas cadangan."_q },
		{ u"LuminaBackupImportInfo"_q, u"Pilih berkas cadangan dan masukkan "
			u"frasa sandinya untuk memulihkan data LuminaGram Anda. "
			u"Pengaturan yang dibawa cadangan menggantikan pengaturan di "
			u"perangkat ini; apa pun yang tidak dibawanya dibiarkan apa "
			u"adanya. Berkas yang tidak dapat diverifikasi ditolak sebelum "
			u"apa pun ditulis, sehingga frasa sandi yang salah tidak pernah "
			u"membuat pemulihan Anda berhenti di tengah jalan."_q },
		{ u"LuminaBackupImportSuccess"_q, u"Cadangan dipulihkan. Mulai ulang "
			u"LuminaGram untuk menerapkan semuanya."_q },
		{ u"LuminaBackupInvalidFile"_q,
			u"Ini bukan berkas cadangan LuminaGram yang valid."_q },
		{ u"LuminaBackupNewerFormat"_q,
			u"Cadangan ini dibuat oleh versi LuminaGram yang lebih baru."_q },
		{ u"LuminaBackupOpenCaption"_q, u"Buka cadangan LuminaGram"_q },
		{ u"LuminaBackupPassphraseHint"_q, u"Frasa sandi"_q },
		{ u"LuminaBackupPassphraseMismatch"_q,
			u"Kedua frasa sandi tidak sama."_q },
		{ u"LuminaBackupPassphraseRepeatHint"_q, u"Ulangi frasa sandi"_q },
		{ u"LuminaBackupPassphraseTitle"_q, u"Masukkan frasa sandi"_q },
		{ u"LuminaBackupPassphraseTooShort"_q,
			u"Pilih frasa sandi minimal 4 karakter."_q },
		{ u"LuminaBackupSaveCaption"_q, u"Simpan cadangan LuminaGram"_q },
		{ u"LuminaBackupTitle"_q, u"Cadangan terenkripsi"_q },
		{ u"LuminaBackupUnauthenticated"_q, u"Cadangan ini memakai format "
			u"lama tanpa perlindungan, yang tidak dapat diperiksa terhadap "
			u"frasa sandi yang salah maupun terhadap perubahan isi. Buat "
			u"cadangan baru dari LuminaGram versi terbaru."_q },
		{ u"LuminaBackupWrongPassphrase"_q,
			u"Frasa sandi salah, atau berkasnya telah diubah."_q },
		{ u"LuminaBookmark"_q, u"Tambahkan penanda"_q },
		{ u"LuminaBookmarkAdded"_q, u"Ditambahkan ke penanda"_q },
		{ u"LuminaBookmarkChatUnavailable"_q, u"Obrolan tidak tersedia"_q },
		{ u"LuminaBookmarkDeleteTitle"_q, u"Hapus penanda?"_q },
		{ u"LuminaBookmarkGone"_q,
			u"Obrolan ini tidak lagi tersedia di perangkat ini."_q },
		{ u"LuminaBookmarkRemove"_q, u"Hapus penanda"_q },
		{ u"LuminaBookmarkRemoved"_q, u"Dihapus dari penanda"_q },
		{ u"LuminaBookmarksAbout"_q, u"Penanda hanyalah penunjuk ke pesan, "
			u"disimpan hanya di perangkat ini dan tidak pernah dikirim ke "
			u"Telegram. Menghapus penanda tidak mengubah pesannya sama "
			u"sekali."_q },
		{ u"LuminaBookmarksEmpty"_q,
			u"Anda belum menyimpan pesan apa pun ke penanda."_q },
		{ u"LuminaBookmarksFull"_q, u"Daftar penanda sudah penuh. Hapus satu "
			u"penanda untuk menambahkan yang lain."_q },
		{ u"LuminaBookmarksList"_q, u"Pesan yang ditandai"_q },
		{ u"LuminaBookmarksListAbout"_q, u"Klik sebuah penanda untuk membuka "
			u"pesannya, atau klik kanan untuk menghapusnya. Penanda tetap ada "
			u"di daftar ini bahkan setelah pesannya dihapus."_q },
		{ u"LuminaBookmarksNone"_q, u"Tidak ada"_q },
		{ u"LuminaBookmarksTitle"_q, u"Penanda"_q },
		{ u"LuminaChatListDensityTitle"_q, u"Kepadatan daftar obrolan"_q },
		{ u"LuminaCompactListRows"_q, u"Baris ringkas"_q },
		{ u"LuminaCompactListRowsInfo"_q, u"Menampilkan lebih banyak obrolan "
			u"di layar dengan memendekkan setiap baris daftar obrolan. Foto "
			u"profil dan pratinjau pesan tetap terlihat."_q },
		{ u"LuminaChatListDotsTitle"_q, u"Titik avatar"_q },
		{ u"LuminaChatListOnlineDot"_q, u"Titik daring"_q },
		{ u"LuminaChatListOnlineDotInfo"_q, u"Menampilkan titik hijau kecil "
			u"pada avatar obrolan pribadi yang kontaknya sedang daring."_q },
		{ u"LuminaChatListRecencyDot"_q, u"Titik terakhir dilihat"_q },
		{ u"LuminaChatListRecencyDotInfo"_q, u"Mewarnai titik avatar pada "
			u"obrolan pribadi berdasarkan seberapa baru kontak terakhir "
			u"terlihat: hijau saat sedang daring, kuning dalam satu jam, "
			u"oranye dalam satu hari. Tidak ada titik untuk terakhir terlihat "
			u"yang lebih lama atau yang disembunyikan. Sakelar titik daring "
			u"tetap mengendalikan kondisi hijau (daring)."_q },
		{ u"LuminaChatListVisibilityAbout"_q, u"Menghilangkan bilah folder "
			u"dari daftar obrolan — baik bilah vertikal di sampingnya maupun "
			u"bilah horizontal di atasnya. Selama tab folder disembunyikan, "
			u"Anda selalu melihat semua obrolan: folder yang sedang Anda buka "
			u"akan ditinggalkan, sedangkan pintasan folder dan gesekan "
			u"antarfolder tidak melakukan apa pun. Menyembunyikan cerita "
			u"hanya menghapus baris di atas daftar obrolan; cerita itu "
			u"sendiri, dan semua cara lain untuk membukanya, tidak "
			u"berubah."_q },
		{ u"LuminaChatListVisibilityTitle"_q, u"Folder dan cerita"_q },
		{ u"LuminaContactNote"_q, u"Catatan pribadi"_q },
		{ u"LuminaContactNoteAbout"_q, u"Hanya Anda yang bisa melihat ini. "
			u"Catatan tetap di perangkat ini, tidak pernah dikirim ke "
			u"Telegram, dan tidak disinkronkan ke perangkat Anda yang "
			u"lain."_q },
		{ u"LuminaContactNoteEmpty"_q,
			u"Klik untuk menambah catatan pribadi"_q },
		{ u"LuminaContactNoteHint"_q,
			u"Catatan (hanya Anda yang bisa melihat)"_q },
		{ u"LuminaContactNotesAbout"_q, u"Menambahkan catatan pribadi dan "
			u"daftar tag ke profil seseorang. Keduanya tetap di perangkat "
			u"ini, tidak pernah dikirim ke Telegram, dan tidak disinkronkan "
			u"ke perangkat Anda yang lain. Catatan kontak milik Telegram "
			u"sendiri dibiarkan apa adanya: catatan pribadi ditawarkan di "
			u"tempat catatan Telegram tidak tersedia — pada bot, dan pada "
			u"orang yang tidak ada di kontak Anda — sedangkan tag ditawarkan "
			u"di mana saja."_q },
		{ u"LuminaContactNotesClear"_q, u"Hapus semua catatan pribadi"_q },
		{ u"LuminaContactNotesClearText"_q, u"Hapus setiap catatan pribadi "
			u"dan tag yang tersimpan di perangkat ini? Tindakan ini tidak "
			u"dapat dibatalkan."_q },
		{ u"LuminaContactNotesClearTitle"_q, u"Hapus catatan pribadi"_q },
		{ u"LuminaContactNotesNone"_q, u"Tidak ada"_q },
		{ u"LuminaContactNotesTitle"_q, u"Catatan kontak pribadi"_q },
		{ u"LuminaContactNotesToggle"_q,
			u"Catatan pribadi dan tag di profil"_q },
		{ u"LuminaContactTags"_q, u"Tag"_q },
		{ u"LuminaContactTagsEmpty"_q, u"Klik untuk menambah tag"_q },
		{ u"LuminaContactTagsHint"_q, u"Tag, dipisahkan koma"_q },
		{ u"LuminaDetailsDate"_q, u"Tanggal"_q },
		{ u"LuminaDetailsForwardedFrom"_q, u"Diteruskan dari"_q },
		{ u"LuminaDetailsFrom"_q, u"Dari"_q },
		{ u"LuminaDetailsMessageId"_q, u"ID pesan"_q },
		{ u"LuminaDetailsOriginalDate"_q, u"Tanggal asli"_q },
		{ u"LuminaExactNumbers"_q,
			u"Tampilkan angka persis (tanpa pembulatan 1.2K)"_q },
		{ u"LuminaExactNumbersInfo"_q, u"Tampilkan jumlah penuh seperti "
			u"1.234.567 alih-alih bentuk singkat seperti 1.2M. Penghitung "
			u"yang sudah tergambar di layar tetap memakai bentuk lamanya "
			u"sampai aplikasi dimulai ulang."_q },
		{ u"LuminaForwardNoAuthor"_q, u"Teruskan tanpa penulis"_q },
		{ u"LuminaForwardNoAuthorTitle"_q, u"Teruskan tanpa penulis"_q },
		{ u"LuminaForwardNoCaption"_q,
			u"Teruskan tanpa penulis dan keterangan"_q },
		{ u"LuminaForwardNoCaptionTitle"_q,
			u"Teruskan tanpa penulis dan keterangan"_q },
		{ u"LuminaHideStories"_q, u"Sembunyikan cerita"_q },
		{ u"LuminaHideTabs"_q, u"Sembunyikan tab folder"_q },
		{ u"LuminaLinkSafetyDestination"_q, u"Tujuan asli"_q },
		{ u"LuminaLinkSafetyInfo"_q, u"Menanyakan sebelum membuka tautan yang "
			u"menyembunyikan tujuan aslinya di balik teks sebelum tanda @, "
			u"yang menulis domainnya dalam punycode, atau yang lewat pemendek "
			u"tautan yang dikenal. Konfirmasinya menampilkan host tujuan asli "
			u"dan alamat lengkapnya. Telegram Desktop sendiri sudah "
			u"menanyakan tentang domain mirip yang ditulis dengan abjad lain. "
			u"Tidak ada pemeriksaan daring — tidak ada alamat yang Anda buka "
			u"meninggalkan perangkat ini."_q },
		{ u"LuminaLinkSafetyRow"_q, u"Pemeriksa keamanan tautan"_q },
		{ u"LuminaLinkSafetyTitle"_q, u"Buka tautan eksternal?"_q },
		{ u"LuminaLinkSafetyWarnMismatch"_q, u"Tautan ini menyembunyikan "
			u"tujuan aslinya di balik teks sebelum tanda «@»."_q },
		{ u"LuminaLinkSafetyWarnPunycode"_q, u"Alamat ini memakai karakter "
			u"tersandi (punycode) yang bisa meniru situs terkenal."_q },
		{ u"LuminaLinkSafetyWarnShortener"_q, u"Ini pemendek tautan — tujuan "
			u"aslinya tetap tersembunyi sampai Anda membukanya."_q },
		{ u"LuminaMediaAutoPauseBgVideo"_q,
			u"Jeda video saat aplikasi berpindah ke latar belakang"_q },
		{ u"LuminaMediaAutoPauseBgVideoInfo"_q, u"Otomatis menjeda video yang "
			u"sedang diputar saat Anda meninggalkan LuminaGram. Mengecilkan "
			u"penampil juga menjedanya. Video yang sengaja Anda pindahkan ke "
			u"jendela tersendiri, atau ke gambar-dalam-gambar, tetap diputar, "
			u"dan musik serta pesan suara juga tetap diputar selama Anda "
			u"pergi."_q },
		{ u"LuminaMediaSaving"_q, u"Media"_q },
		{ u"LuminaMediaTitle"_q, u"Media"_q },
		{ u"LuminaMessageActionsInfo"_q, u"Entri ini hanya muncul di menu "
			u"konteks sebuah pesan. Masing-masing memakai penerusan milik "
			u"Telegram sendiri, jadi obrolan yang membatasi penerusan tetap "
			u"dibatasi. «Teruskan tanpa penulis dan keterangan» dinamai "
			u"sesuai apa yang benar-benar dikirim: Telegram tidak dapat "
			u"membuang keterangan sambil mempertahankan pengirim, sehingga "
			u"memilihnya membuang keduanya. «Detail» hanya membaca apa yang "
			u"sudah disinkronkan perangkat ini — tidak ada yang diminta dan "
			u"tidak ada yang disimpan."_q },
		{ u"LuminaMessageDetails"_q, u"Detail"_q },
		{ u"LuminaOnboardingDualName"_q, u"Dua bahasa sekaligus"_q },
		{ u"LuminaOnboardingDualText"_q, u"Menyimpan teks asli di layar "
			u"berdampingan dengan terjemahannya, baik untuk pesan yang Anda "
			u"terima maupun yang Anda kirim."_q },
		{ u"LuminaOnboardingFooter"_q, u"Buka salah satu bagian di halaman "
			u"pengaturan LuminaGram untuk menyiapkannya. Anda bisa membaca "
			u"kartu ini lagi dari Alat."_q },
		{ u"LuminaOnboardingGotIt"_q, u"Mengerti"_q },
		{ u"LuminaOnboardingHeader"_q, u"Tentang"_q },
		{ u"LuminaOnboardingIntro"_q, u"LuminaGram menambahkan alatnya "
			u"sendiri di atas Telegram Desktop. Setiap opsi di bawah hanya "
			u"disimpan di komputer ini dan tidak pernah disinkronkan dengan "
			u"Telegram."_q },
		{ u"LuminaOnboardingRow"_q, u"Apa yang ditambahkan LuminaGram"_q },
		{ u"LuminaOnboardingRowAbout"_q, u"Menampilkan kartu selamat datang "
			u"yang muncul saat pertama kali Anda membuka pengaturan "
			u"LuminaGram. Kartu itu hanya menjelaskan apa yang ada di sini "
			u"dan tidak mengaktifkan apa pun."_q },
		{ u"LuminaOnboardingSafetyName"_q, u"Pemeriksaan keamanan"_q },
		{ u"LuminaOnboardingSafetyText"_q, u"Memperingatkan sebelum membuka "
			u"tautan yang alamatnya tidak seperti terlihat, menangkap alamat "
			u"kripto yang tertukar saat Anda menempelkannya, dan menghapus "
			u"lokasi dari foto yang Anda kirim."_q },
		{ u"LuminaOnboardingTitle"_q, u"Selamat datang di LuminaGram"_q },
		{ u"LuminaOnboardingTranslateText"_q, u"Menerjemahkan pesan masuk "
			u"dengan layanan terjemahan Anda sendiri, dan menerjemahkan apa "
			u"yang Anda ketik sebelum dikirim."_q },
		{ u"LuminaOnboardingVaultName"_q, u"Brankas penyamaran"_q },
		{ u"LuminaOnboardingVaultText"_q, u"Menyembunyikan LuminaGram di "
			u"balik kalkulator atau catatan yang hanya membuka aplikasi "
			u"aslinya dengan kode rahasia Anda."_q },
		{ u"LuminaProfileCardAbout"_q, u"Kartu lokal tentang diri Anda — "
			u"bahasa yang Anda kuasai, hal yang Anda minati — yang bisa Anda "
			u"salin dan tempel ke sebuah obrolan. Kartu ini tetap di "
			u"perangkat ini."_q },
		{ u"LuminaProfileCardBio"_q, u"Bio singkat"_q },
		{ u"LuminaProfileCardBioHint"_q,
			u"Beberapa kata tentang diri Anda"_q },
		{ u"LuminaProfileCardCopied"_q, u"Kartu disalin"_q },
		{ u"LuminaProfileCardCopy"_q, u"Salin ke papan klip"_q },
		{ u"LuminaProfileCardCopyInfo"_q, u"Membuat ringkasan teks biasa dari "
			u"kartu Anda. Tempelkan ke obrolan mana pun saat Anda ingin "
			u"membagikannya."_q },
		{ u"LuminaProfileCardEdit"_q, u"Kartu saya"_q },
		{ u"LuminaProfileCardEmptyShare"_q,
			u"Isi kartu Anda terlebih dahulu"_q },
		{ u"LuminaProfileCardHeader"_q, u"Kartu saya"_q },
		{ u"LuminaProfileCardInfo"_q, u"Kartu ini hanya disimpan di perangkat "
			u"ini dan tidak pernah dikirim ke Telegram. Kartu ini tidak "
			u"mengubah profil Telegram Anda."_q },
		{ u"LuminaProfileCardInterests"_q, u"Minat / tag"_q },
		{ u"LuminaProfileCardInterestsHint"_q,
			u"mis. musik, mendaki, coding"_q },
		{ u"LuminaProfileCardLanguages"_q, u"Bahasa yang saya kuasai"_q },
		{ u"LuminaProfileCardLanguagesHint"_q,
			u"mis. Indonesia, English, 中文"_q },
		{ u"LuminaProfileCardNotSet"_q, u"Belum diatur"_q },
		{ u"LuminaProfileCardTagline"_q, u"Slogan"_q },
		{ u"LuminaProfileCardTaglineHint"_q,
			u"Kalimat singkat tentang diri Anda"_q },
		{ u"LuminaProfileCardTitle"_q, u"Kartu profil"_q },
		{ u"LuminaProfileChatCreated"_q, u"Dibuat"_q },
		{ u"LuminaProfileDcId"_q, u"Pusat data"_q },
		{ u"LuminaProfileDcIdValue"_q, u"DC{1}"_q },
		{ u"LuminaProfileInfoHeader"_q, u"Profil"_q },
		{ u"LuminaProfileInfoInfo"_q, u"Baris tambahan di halaman profil, "
			u"semuanya dihitung di perangkat ini — tidak ada yang diminta "
			u"dari Telegram. Telegram sendiri menyediakan tanggal registrasi "
			u"untuk sebagian akun; untuk sisanya tanggal itu diperkirakan "
			u"dari nomor akun dan ditampilkan dengan tanda «~». Pusat data "
			u"adalah tempat foto profil disimpan. Tanggal pembuatan adalah "
			u"saat sebuah grup atau kanal dibuat."_q },
		{ u"LuminaProfileRegistrationApprox"_q, u"~ {1}"_q },
		{ u"LuminaProfileRegistrationDate"_q, u"Tanggal registrasi"_q },
		{ u"LuminaProfileShowChatDate"_q, u"Tampilkan tanggal dibuat"_q },
		{ u"LuminaProfileShowDcId"_q, u"Tampilkan pusat data"_q },
		{ u"LuminaProfileShowRegistrationDate"_q,
			u"Tampilkan tanggal registrasi"_q },
		{ u"LuminaRecentLimitsInfo"_q, u"Mencegah perangkat ini membuang "
			u"stiker terbaru dan GIF tersimpan lebih cepat daripada yang "
			u"seharusnya, memakai batas yang sama seperti LuminaGram di "
			u"Android: hingga 200 stiker terbaru dan 500 GIF tersimpan. "
			u"Server Telegram tetap yang menentukan berapa banyak yang "
			u"benar-benar disimpan di akun Anda dan disinkronkan ke perangkat "
			u"Anda yang lain, jadi ini tidak bisa memperbesar daftar di awan "
			u"— pada sinkronisasi berikutnya, apa pun yang tidak lagi "
			u"disimpan server juga hilang di sini, biasanya dalam hitungan "
			u"detik. Selama ini aktif, Telegram Desktop berhenti menghapus "
			u"GIF tersimpan atas inisiatifnya sendiri, sehingga pengingat "
			u"Premium tentang batas GIF tersimpan tidak muncul. Panel stiker "
			u"tetap hanya menampilkan 20 stiker terbaru pertama kecuali Anda "
			u"juga mengaktifkan «Stiker terbaru tanpa batas» di Pengaturan › "
			u"Lanjutan › Pengaturan eksperimental."_q },
		{ u"LuminaRecentLimitsRow"_q,
			u"Simpan lebih banyak stiker terbaru dan GIF"_q },
		{ u"LuminaReplyTemplatesAbout"_q, u"Potongan teks pendek yang Anda "
			u"simpan di perangkat ini dan sisipkan ke kolom pesan. Klik kanan "
			u"tombol emoji di sebuah obrolan untuk memilih salah satunya. "
			u"Templat tidak pernah dikirim ke Telegram."_q },
		{ u"LuminaReplyTemplatesAdd"_q, u"Tambah templat"_q },
		{ u"LuminaReplyTemplatesEdit"_q, u"Edit templat"_q },
		{ u"LuminaReplyTemplatesEmpty"_q, u"Belum ada templat. Tambahkan "
			u"satu, lalu klik kanan tombol emoji di sebuah obrolan untuk "
			u"menyisipkannya."_q },
		{ u"LuminaReplyTemplatesFull"_q, u"Daftar sudah penuh — hapus satu "
			u"templat untuk menambahkan yang lain."_q },
		{ u"LuminaReplyTemplatesInfo"_q, u"Klik kanan tombol emoji di sebuah "
			u"obrolan untuk menyisipkan templat. Klik kanan sebuah templat di "
			u"sini untuk memindahkannya ke atas atau ke bawah; buka salah "
			u"satunya untuk mengedit atau menghapusnya."_q },
		{ u"LuminaReplyTemplatesList"_q, u"Templat"_q },
		{ u"LuminaReplyTemplatesManage"_q, u"Kelola templat…"_q },
		{ u"LuminaReplyTemplatesMoveDown"_q, u"Pindahkan ke bawah"_q },
		{ u"LuminaReplyTemplatesMoveUp"_q, u"Pindahkan ke atas"_q },
		{ u"LuminaReplyTemplatesNone"_q, u"Tidak ada"_q },
		{ u"LuminaReplyTemplatesPlaceholder"_q, u"Teks templat"_q },
		{ u"LuminaReplyTemplatesShow"_q, u"Tawarkan templat di obrolan"_q },
		{ u"LuminaReplyTemplatesTitle"_q, u"Templat balasan"_q },
		{ u"LuminaSaveSticker"_q, u"Simpan stiker"_q },
		{ u"LuminaSaveStickers"_q, u"Simpan stiker"_q },
		{ u"LuminaSaveStickersInfo"_q, u"Menambahkan baris «Simpan stiker» ke "
			u"menu konteks panel stiker. Stiker disimpan ke tempat unduhan "
			u"Anda yang lain, dan baris itu disembunyikan untuk paket stiker "
			u"milik sebuah grup ketika grup tersebut membatasi "
			u"penyimpanan."_q },
		{ u"LuminaSaveToCloud"_q, u"Simpan ke Pesan Tersimpan"_q },
		{ u"LuminaSaveToCloudTitle"_q, u"Simpan ke Pesan Tersimpan"_q },
		{ u"LuminaSecurityPanicConfirmAck"_q,
			u"Saya paham tindakan ini tidak dapat dibatalkan"_q },
		{ u"LuminaSecurityPanicConfirmButton"_q, u"Hapus sekarang"_q },
		{ u"LuminaSecurityPanicConfirmText"_q, u"Setiap akun di perangkat ini "
			u"akan dikeluarkan. Basis data pesan lokal, draf, dan media dalam "
			u"cache akan dihapus, bersama pengaturan LuminaGram sendiri, "
			u"penanda, terjemahan tersimpan, dan kunci API.\n\nAkun Anda "
			u"tidak dihapus. Akun tetap ada di server Telegram, begitu pula "
			u"pesan Anda — Anda bisa masuk lagi dari mana saja.\n\nBerkas "
			u"yang sudah diunduh TIDAK dihapus. LuminaGram tidak menyentuh "
			u"folder unduhan Anda, karena folder itu biasanya folder Unduhan "
			u"biasa dan berisi berkas yang tidak berkaitan. Pindahkan atau "
			u"hapus sendiri apa pun yang sensitif di sana.\n\nTindakan ini "
			u"tidak dapat dibatalkan."_q },
		{ u"LuminaSecurityPanicConfirmTitle"_q, u"Hapus darurat?"_q },
		{ u"LuminaSecurityPanicHeader"_q, u"Hapus darurat"_q },
		{ u"LuminaSecurityPanicWipe"_q, u"Hapus darurat (Kaboom)"_q },
		{ u"LuminaSecurityPanicWipeAbout"_q, u"Mengeluarkan setiap akun di "
			u"perangkat ini dan menghapus basis data pesan lokal, draf, serta "
			u"media dalam cache, bersama pengaturan LuminaGram sendiri, "
			u"penanda, terjemahan tersimpan, dan kunci API. Akun dan pesan "
			u"Anda tetap ada di server Telegram. Berkas yang sudah Anda unduh "
			u"dibiarkan di tempatnya. Tindakan ini tidak dapat "
			u"dibatalkan."_q },
		{ u"LuminaSelectFromAuthor"_q, u"Pilih semua dari penulis"_q },
		{ u"LuminaSelectFromAuthorAbout"_q, u"Menambahkan entri menu pesan di "
			u"grup yang memilih setiap pesan dari pengirim yang Anda klik. "
			u"Entri ini hanya menjangkau pesan yang sudah dimuat jendela ini "
			u"— gulir lebih jauh ke atas lalu ulangi untuk menyertakan pesan "
			u"yang lebih lama. Pesan yang diatribusikan ke obrolan itu "
			u"sendiri, seperti kiriman kanal dan kiriman admin anonim, tidak "
			u"mendapat entri ini."_q },
		{ u"LuminaSelectFromAuthorLimit"_q,
			u"Hanya {1} pesan yang bisa dipilih sekaligus."_q },
		{ u"LuminaSelectFromAuthorTitle"_q, u"Pilih semua dari penulis"_q },
		{ u"LuminaSendOriginalCaption"_q, u"Kirim keterangan asli"_q },
		{ u"LuminaShowBookmarks"_q, u"Tampilkan item menu penanda"_q },
		{ u"LuminaShowMessageDetails"_q, u"Detail pesan"_q },
		{ u"LuminaShowMutedCount"_q,
			u"Selalu tampilkan jumlah belum dibaca"_q },
		{ u"LuminaShowMutedCountInfo"_q, u"Menggambar lencana belum dibaca "
			u"untuk obrolan yang dibisukan dengan warna aksen normal, bukan "
			u"abu-abu bisu."_q },
		{ u"LuminaStickerSavedTo"_q, u"Stiker disimpan ke {1}"_q },
		{ u"LuminaStickerSizeChoice"_q, u"{1}%"_q },
		{ u"LuminaStickerSizeChoiceDefault"_q, u"{1}% (bawaan)"_q },
		{ u"LuminaStickerSizeInfo"_q, u"Seberapa besar stiker digambar di "
			u"obrolan — baik yang Anda kirim maupun yang Anda terima. Emoji "
			u"animasi, dadu, dan stiker hadiah mempertahankan ukurannya "
			u"sendiri. Telegram Desktop mengukur sebuah stiker sekali lalu "
			u"mengingatnya, jadi ukuran baru berlaku saat aplikasi berikutnya "
			u"dijalankan."_q },
		{ u"LuminaTimeWithSeconds"_q, u"Tampilkan detik pada waktu pesan"_q },
		{ u"LuminaTimeWithSecondsAbout"_q, u"Waktu di bawah pesan, waktu pada "
			u"teks yang disalin, dan waktu yang dibacakan pembaca layar "
			u"semuanya menyertakan detik."_q },

		// The tray menu and the taskbar button's jump list. {1} is the
		// application name, always the bare word LuminaGram.
		{ u"LuminaTrayOpen"_q, u"Buka {1}"_q },
		{ u"LuminaTrayQuit"_q, u"Keluar dari {1}"_q },

		{ u"LuminaUndoSendBulletin"_q, u"Mengirim pesan…"_q },
		{ u"LuminaUndoSendTitle"_q, u"Urungkan kirim"_q },
		{ u"LuminaUndoSendUndo"_q, u"Urungkan"_q },
		{ u"LuminaUndoSendWindow"_q, u"Jendela urungkan kirim"_q },
		{ u"LuminaUndoSendWindowInfo"_q, u"Menahan pesan teks biasa selama "
			u"{1} detik di balik tombol Urungkan sebelum benar-benar dikirim. "
			u"Teks Anda tetap berada di kotak pesan selama itu, dan kotaknya "
			u"baru dikosongkan setelah pesan benar-benar terkirim, jadi "
			u"Urungkan cukup membiarkannya di tempatnya — tidak ada yang "
			u"diambil lalu dikembalikan. Mengirim lagi, membuka obrolan lain, "
			u"atau menutup aplikasi akan langsung mengirim pesan yang "
			u"ditahan. Pesan media, pesan suara, pesan hasil edit, pesan yang "
			u"diteruskan, dan pesan terjadwal tidak pernah ditahan, begitu "
			u"pula pesan yang dikirim dari topik forum atau utas "
			u"komentar."_q },
		// clipboard guard + scam watch
		{ u"LuminaPrivacyCryptoClipboardGuard"_q, u"Perlindungan tempel alamat "
			u"kripto"_q },
		{ u"LuminaPrivacyCryptoClipboardGuardInfo"_q, u"Menanyakan sebelum "
			u"menempelkan sesuatu yang tampak seperti alamat dompet kripto ke "
			u"dalam pesan. Malware pembajak papan klip bisa menukar alamat "
			u"yang Anda salin dengan alamat penipu tanpa Anda sadari, dan saat "
			u"menempel adalah kesempatan terakhir untuk menangkapnya. "
			u"Pemeriksaan dilakukan di perangkat ini dan tidak ada yang "
			u"dikirim ke mana pun."_q },
		{ u"LuminaPrivacyScamKeywordWarning"_q, u"Peringatan kata "
			u"penipuan"_q },
		{ u"LuminaPrivacyScamKeywordWarningInfo"_q, u"Saat pesan dari "
			u"seseorang yang tidak ada di kontak Anda menyebutkan taktik "
			u"penipuan umum — transfer uang, kartu hadiah, “investasi” kripto, "
			u"biaya verifikasi, atau meminta kode — sebuah pengingat sekali "
			u"muncul agar Anda berhati-hati. Pesan tidak pernah diblokir atau "
			u"diubah. Pemeriksaan berjalan luring di perangkat Anda."_q },
		// otp guard
		{ u"LuminaOtpGuardTitle"_q, u"Peringatan"_q },
		{ u"LuminaOtpGuardMessage"_q, u"Anda akan mengirim kode masuk Telegram Anda kepada orang lain. Siapa pun yang menerimanya bisa masuk ke akun Anda dan membaca semua pesan Anda. Telegram tidak pernah meminta kode ini."_q },
		{ u"LuminaOtpGuardCancel"_q, u"Jangan kirim"_q },
		{ u"LuminaOtpGuardSendAnyway"_q, u"Saya paham risikonya, tetap kirim"_q },
		{ u"LuminaOtpGuard"_q, u"Perlindungan kode masuk"_q },
		{ u"LuminaOtpGuardInfo"_q, u"Memperingatkan sebelum Anda mengirim pesan yang berisi kode masuk, jika Telegram baru saja mengirimkannya kepada Anda. Pemeriksaan berjalan sepenuhnya di perangkat Anda: tidak ada yang diunggah, dan pesan Anda tidak pernah diubah."_q },
		// link preview + voice confirm
		{ u"LuminaPrivacyDisableLinkPreview"_q, u"Nonaktifkan pratinjau tautan "
			u"secara bawaan"_q },
		{ u"LuminaLinkPreviewInfo"_q, u"Pesan yang Anda tulis dari awal "
			u"dikirim tanpa pratinjau tautan, dan kolom tulis tidak pernah "
			u"meminta Telegram membuka tautan yang ditempel saat Anda "
			u"mengetik. Pratinjau yang sudah terlampir, dan pesan yang Anda "
			u"sunting, dibiarkan apa adanya. Untuk tetap melampirkan pratinjau "
			u"pada satu pesan, beri tombol untuk «Alihkan pratinjau tautan» di "
			u"Pengaturan > Pintasan papan ketik, lalu tekan saat menulis."_q },
		{ u"LuminaVoiceConfirmTitle"_q, u"Pesan suara dan video"_q },
		{ u"LuminaVoiceConfirmRow"_q, u"Konfirmasi sebelum kirim"_q },
		{ u"LuminaVoiceConfirmInfo"_q, u"Bertanya sebelum pesan suara atau "
			u"pesan video bulat yang sudah direkam dikirim. Membatalkan "
			u"pertanyaan itu membuang rekaman, kecuali di pratinjau dengar, "
			u"yang membiarkan rekaman tetap ada. Pengiriman terjadwal tidak "
			u"pernah ditanyakan."_q },
		// exif strip + text replace
		{ u"LuminaExifStripRow"_q, u"Hapus lokasi foto sebelum mengirim"_q },
		{ u"LuminaExifStripInfo"_q, u"Menghapus koordinat GPS yang disimpan "
			u"kamera di dalam foto JPEG sebelum foto itu diunggah — baik saat "
			u"kamu mengirim foto dengan cara terkompresi biasa maupun saat "
			u"mengirimnya sebagai file. Hanya tag lokasi yang hilang: tag "
			u"orientasi, kamera, dan tanggal dibiarkan persis seperti semula, "
			u"jadi foto yang dikirim sebagai file tetap tampil menghadap arah "
			u"yang benar. Salinan file milikmu sendiri di penyimpanan tidak "
			u"pernah diubah. Ini tidak mencakup video, lokasi yang ditulis ke "
			u"dalam foto oleh XMP atau oleh catatan pabrikan kamera itu "
			u"sendiri, maupun salinan kedua gambar yang diselipkan sebagian "
			u"ponsel ke dalam file yang sama."_q },
		{ u"LuminaReplacerTitle"_q, u"Pengganti teks"_q },
		{ u"LuminaReplacerHeader"_q, u"Aturan"_q },
		{ u"LuminaReplacerAdd"_q, u"Tambah aturan"_q },
		{ u"LuminaReplacerEdit"_q, u"Edit aturan"_q },
		{ u"LuminaReplacerFrom"_q, u"Ganti"_q },
		{ u"LuminaReplacerTo"_q, u"Dengan"_q },
		{ u"LuminaReplacerEnable"_q, u"Ganti teks pada pesan yang saya "
			u"kirim"_q },
		{ u"LuminaReplacerNone"_q, u"Tidak ada"_q },
		{ u"LuminaReplacerNothing"_q, u"(kosong)"_q },
		{ u"LuminaReplacerEmpty"_q, u"Belum ada aturan. Tambahkan satu untuk "
			u"mulai mengganti teks pada pesan yang kamu kirim."_q },
		{ u"LuminaReplacerFull"_q, u"Aturan diterapkan berurutan dari atas. "
			u"Daftar sudah penuh — hapus satu aturan untuk menambah yang "
			u"lain."_q },
		{ u"LuminaReplacerListInfo"_q, u"Aturan diterapkan berurutan dari "
			u"atas, dan setiap aturan bekerja pada hasil aturan di "
			u"atasnya."_q },
		{ u"LuminaReplacerAbout"_q, u"Menukar singkatan dengan frasa yang "
			u"kamu maksud di setiap pesan yang kamu kirim — «brb» menjadi «be "
			u"right back». Pencocokan membedakan huruf besar/kecil dan hanya "
			u"berlaku untuk kata utuh. Pesan yang membawa format apa pun "
			u"dikirim persis seperti yang diketik, dan tautan, sebutan, "
			u"tagar, serta perintah bot tidak pernah ditulis ulang. Aturan "
			u"tetap berada di perangkat ini dan tidak pernah dikirim ke "
			u"Telegram."_q },
		// vault + fake crash
		{ u"LuminaVaultHeader"_q, u"Brankas penyamaran"_q },
		{ u"LuminaVaultEnable"_q, u"Aktifkan brankas"_q },
		{ u"LuminaVaultInfo"_q, u"Semua di sini hanya tersimpan di perangkat ini. Brankas "
			u"menyembunyikan LuminaGram di balik aplikasi yang terlihat "
			u"biasa, dan hanya kode rahasia yang membawa Anda kembali ke "
			u"aplikasi asli."_q },
		{ u"LuminaVaultMode"_q, u"Mode brankas"_q },
		{ u"LuminaVaultModePasswordDoor"_q, u"Pintu kata sandi"_q },
		{ u"LuminaVaultModeDecoyApp"_q, u"Aplikasi umpan"_q },
		{ u"LuminaVaultModePasswordDoorInfo"_q, u"Saat LuminaGram dijalankan, kata sandi "
			u"akan diminta. Kode rahasia membuka aplikasi asli; apa pun "
			u"yang lain membuka aplikasi umpan, dan tidak pernah memberi "
			u"tahu bahwa kodenya salah."_q },
		{ u"LuminaVaultModeDecoyAppInfo"_q, u"Saat LuminaGram dijalankan, aplikasi umpan "
			u"langsung terbuka. Masukkan kode rahasia Anda di sana untuk "
			u"masuk ke aplikasi asli."_q },
		{ u"LuminaVaultSkin"_q, u"Gaya aplikasi umpan"_q },
		{ u"LuminaVaultSkinNotepad"_q, u"Catatan"_q },
		{ u"LuminaVaultSkinCalculator"_q, u"Kalkulator"_q },
		{ u"LuminaVaultSkinHintCalculator"_q, u"Untuk membuka dari kalkulator, ketik kode "
			u"rahasia lalu tekan tombol sama dengan."_q },
		{ u"LuminaVaultSkinHintNotepad"_q, u"Untuk membuka dari catatan, jadikan kode "
			u"rahasia sebagai seluruh isi catatan, lalu klik dua kali "
			u"bilah judul Catatan."_q },
		{ u"LuminaVaultSecretCode"_q, u"Kode rahasia"_q },
		{ u"LuminaVaultSecretCodeDialogTitle"_q, u"Kode rahasia"_q },
		{ u"LuminaDisguiseDecoyCodeSet"_q, u"Diatur"_q },
		{ u"LuminaDisguiseDecoyCodeNotSet"_q, u"Belum diatur"_q },
		{ u"LuminaVaultNoCodeInfo"_q, u"Brankas tetap nonaktif sampai Anda mengatur kode "
			u"rahasia."_q },
		{ u"LuminaVaultCalculatorCodeInfo"_q, u"Kode ini tidak dapat diketik pada papan "
			u"tombol kalkulator, jadi catatan umpan yang ditampilkan "
			u"sebagai gantinya. Gunakan hanya angka, satu titik, dan + - "
			u"* / jika Anda ingin kalkulator."_q },
		{ u"LuminaVaultDisclaimer"_q, u"Ini menghalangi lirikan sekilas dari balik bahu "
			u"Anda, bukan pemeriksaan orang yang paham: program ini "
			u"tetap bernama LuminaGram di pengelola tugas, dan siapa pun "
			u"yang bisa membaca berkas perangkat ini bisa menjangkau "
			u"akun Anda. Brankas muncul saat LuminaGram dijalankan "
			u"berikutnya, dan menutup aplikasi umpan berarti keluar dari "
			u"LuminaGram. Jika suatu saat Anda lupa kodenya, hapus "
			u"tdata/luminagram.json dan tdata/luminagram_private.json "
			u"dari folder data LuminaGram: brankas lalu mematikan "
			u"dirinya sendiri dan LuminaGram berjalan normal."_q },
		{ u"LuminaSecurityFakeCrashHeader"_q, u"Buka kunci crash palsu"_q },
		{ u"LuminaSecurityFakeCrashEnable"_q, u"Aktifkan buka kunci crash palsu"_q },
		{ u"LuminaSecurityFakeCrashCode"_q, u"Kode crash palsu"_q },
		{ u"LuminaSecurityFakeCrashCodeDialogTitle"_q, u"Kode crash palsu"_q },
		{ u"LuminaSecurityFakeCrashCodeHint"_q, u"Kode"_q },
		{ u"LuminaSecurityFakeCrashCodeSet"_q, u"Diatur"_q },
		{ u"LuminaSecurityFakeCrashCodeNotSet"_q, u"Belum diatur"_q },
		{ u"LuminaSecurityFakeCrashCodeSameAsPasscode"_q, u"Kode ini tidak boleh sama "
			u"dengan kode sandi kunci Anda."_q },
		{ u"LuminaSecurityFakeCrashInfo"_q, u"Saat aktif, mengetik kode crash palsu di "
			u"layar kunci akan langsung menutup LuminaGram alih-alih "
			u"membukanya, tanpa pesan apa pun dan tanpa meninggalkan apa "
			u"pun di layar — bagi siapa pun yang melihat, aplikasi "
			u"tampak crash. Kode ini terpisah dari kode sandi kunci "
			u"Anda, disimpan hanya di perangkat ini, dan tidak pernah "
			u"dikirim ke mana pun."_q },
		{ u"LuminaSecurityFakeCrashBiometricInfo"_q, u"Selama ini aktif dan kode sudah "
			u"diatur, membuka kunci dengan Touch ID, Windows Hello, atau "
			u"kata sandi sistem dimatikan: sidik jari membuka aplikasi "
			u"tanpa pernah meminta kode sandi, sehingga akan melewati "
			u"kode ini begitu saja. Menghapus kode, atau mematikan opsi "
			u"ini, akan mengembalikannya."_q },
		{ u"LuminaSecurityFakeCrashNoCodeInfo"_q, u"Belum ada kode yang diatur, jadi tidak "
			u"akan terjadi apa-apa di layar kunci."_q },
		{ u"LuminaSecurityFakeCrashNoPasscodeInfo"_q, u"LuminaGram tidak memiliki kode "
			u"sandi kunci, jadi layar kunci tidak pernah muncul dan kode "
			u"ini tidak pernah diminta. Aktifkan dulu kode sandi kunci "
			u"di Privasi dan Keamanan."_q },
		{ u"LuminaSecurityFakeCrashCodeClashInfo"_q, u"Kode ini kini sama dengan kode "
			u"sandi kunci Anda, jadi tidak akan pernah berfungsi — kode "
			u"sandi yang membuka aplikasi. Pilih kode lain."_q },
	};
}

[[maybe_unused]] const auto kRegistered = RegisterLocaleTable(
	"id",
	Build);

} // namespace
} // namespace Lumina
