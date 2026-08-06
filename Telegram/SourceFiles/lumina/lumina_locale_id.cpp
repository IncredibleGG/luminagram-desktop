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
		{ u"LuminaTranslateModeHeader"_q, u"Terjemahkan pesan masuk"_q },
		{ u"LuminaTranslateModeAll"_q, u"Di setiap obrolan"_q },
		{ u"LuminaTranslateModeManual"_q,
			u"Hanya obrolan yang saya aktifkan"_q },
		{ u"LuminaTranslateModeInfo"_q, u"Dengan «Hanya obrolan yang saya "
			u"aktifkan», buka sebuah obrolan lalu gunakan tombol "
			u"«Terjemahkan» di bagian atasnya untuk mengaktifkannya di sana; "
			u"obrolan lain dibiarkan apa adanya. «Di setiap obrolan» "
			u"mengirim satu permintaan per pesan ke layanan terjemahan Anda "
			u"— jika kunci Anda berkuota, biarkan pada «Hanya obrolan yang "
			u"saya aktifkan»."_q },
		{ u"LuminaTranslateReceiveHeader"_q, u"Penerimaan"_q },
		{ u"LuminaDualLanguageDisplay"_q,
			u"Tampilkan teks asli dan terjemahan bersamaan"_q },
		{ u"LuminaTranslateReadLang"_q, u"Bahasa baca"_q },
		{ u"LuminaTranslateReadLangFollow"_q, u"Bahasa antarmuka"_q },
		{ u"LuminaTranslateReceiveInfo"_q, u"Pesan masuk tetap menampilkan "
			u"teks aslinya dalam ukuran penuh, dengan terjemahan di "
			u"bawahnya."_q },
		{ u"LuminaTranslateScopeHeader"_q, u"Cakupan"_q },
		{ u"LuminaTranslateScopePrivate"_q, u"Obrolan pribadi"_q },
		{ u"LuminaTranslateScopeGroup"_q, u"Grup dan kanal"_q },
		{ u"LuminaTranslateScopeInfo"_q, u"Obrolan di luar cakupan dibiarkan "
			u"apa adanya di kedua arah: pesannya tidak diterjemahkan saat Anda "
			u"baca, dan pesan yang Anda kirim di sana dikirim tanpa "
			u"terjemahan. Anda tetap bisa menerjemahkan pesan mana pun secara "
			u"manual."_q },
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
	};
}

[[maybe_unused]] const auto kRegistered = RegisterLocaleTable(
	"id",
	Build);

} // namespace
} // namespace Lumina
