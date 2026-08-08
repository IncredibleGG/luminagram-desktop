/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_locale.h"

namespace Lumina {
namespace {

// Turkish. Wherever LuminaGram for Android already shows the same thing, the
// wording here is the wording that shipped there, so the two platforms read
// the same key by key. Keys Android does not have - and keys whose desktop
// English says something Android's does not - are translated from the English
// table in lumina_locale.cpp.
//
// Quoted row labels inside the divider paragraphs use the guillemets Android's
// Turkish table uses, and the phrase between them is copied from the row it
// names, so the two always match.
[[nodiscard]] LocaleTable Build() {
	return {
		// Sections of the LuminaGram settings page.
		{ u"LuminaGramTitle"_q, u"LuminaGram"_q },
		{ u"LuminaTranslateTitle"_q, u"Çeviri"_q },
		{ u"LuminaPrivacyTitle"_q, u"Gizlilik"_q },
		{ u"LuminaSecurityTitle"_q, u"Güvenlik"_q },
		{ u"LuminaChatSettings"_q, u"Sohbetler"_q },
		{ u"LuminaGramChatList"_q, u"Sohbet listesi"_q },
		{ u"LuminaAppearanceTitle"_q, u"Görünüm"_q },
		{ u"LuminaToolsTitle"_q, u"Araçlar"_q },
		{ u"LuminaGramStoredLocallyInfo"_q, u"LuminaGram seçenekleri yalnızca "
			u"bu cihazda saklanır ve hiçbir zaman Telegram ile "
			u"eşitlenmez."_q },

		// Sub-pages that have no rows yet.
		{ u"LuminaAppearancePlaceholder"_q, u"Mesaj, çıkartma ve sayı "
			u"biçimlendirme seçenekleri burada görünecek."_q },
		{ u"LuminaChatListPlaceholder"_q, u"Sohbet listesi düzeni ve rozet "
			u"seçenekleri burada görünecek."_q },
		{ u"LuminaPrivacyPlaceholder"_q, u"Bağlantı, pano ve giden medya "
			u"gizliliği seçenekleri burada görünecek."_q },
		{ u"LuminaSecurityPlaceholder"_q, u"Gizlenme kasası, acil silme ve "
			u"zorla kilit açma seçenekleri burada görünecek."_q },
		{ u"LuminaToolsPlaceholder"_q, u"Yer imleri, yanıt şablonları ve "
			u"yerel yedekleme burada görünecek."_q },

		// Chats sub-page.
		{ u"LuminaMessageActions"_q, u"Mesaj işlemleri"_q },
		{ u"LuminaAllowSaveRestricted"_q,
			u"Kısıtlı sohbetlerden kaydetmeye / kopyalamaya izin ver"_q },
		{ u"LuminaAllowSaveRestrictedInfo"_q, u"«Kısıtlı sohbetlerden "
			u"kaydetmeye / kopyalamaya izin ver» yalnızca bu cihazdaki yerel "
			u"işlemleri etkiler. Bazı sohbetler kaydetmeyi bir nedenle "
			u"kısıtlar — sorumlu davranın."_q },

		// Translation sub-page.
		{ u"LuminaTranslateEnable"_q,
			u"LuminaGram çevirisini etkinleştir"_q },
		{ u"LuminaTranslateEnableInfo"_q, u"Telegram'ın Premium hizmeti "
			u"yerine kendi motorunuzla çevirin. Varsayılan motor ne hesap ne "
			u"de API anahtarı gerektirir. Bu kapalıyken LuminaGram, çeviriyi "
			u"Telegram Desktop'ın sunduğu haliyle bırakır."_q },
		{ u"LuminaTranslateSendHeader"_q, u"Gönderme"_q },
		{ u"LuminaTranslateBeforeSend"_q, u"Göndermeden önce çevir"_q },
		{ u"LuminaTranslateSendLang"_q, u"Gönderme dili"_q },
		{ u"LuminaTranslateSendLangAuto"_q, u"Karşı tarafın dili"_q },
		{ u"LuminaTranslateBeforeSendConfirm"_q,
			u"Göndermeden önce onayla"_q },
		{ u"LuminaTranslateSendInfo"_q, u"Giden mesajlar yukarıdaki dile "
			u"çevrilir ve orijinali çevirinin yanında saklanır. «Karşı "
			u"tarafın dili» seçiliyken LuminaGram her sohbet için hangi dilin "
			u"kullanılacağını bir kez sorar, sonra bunu hatırlar. "
			u"«Göndermeden önce onayla» çeviriyi önce orijinalin yanında "
			u"gösterir, böylece ikisinden birini gönderebilirsiniz; kapalıyken "
			u"çeviri doğrudan gönderilir."_q },
		{ u"LuminaTranslateReceiveHeader"_q, u"Alma"_q },
		{ u"LuminaDualLanguageDisplay"_q,
			u"Orijinali ve çeviriyi birlikte göster"_q },
		{ u"LuminaTranslateReadLang"_q, u"Okuma dili"_q },
		{ u"LuminaTranslateReadLangFollow"_q, u"Arayüz dili"_q },
		{ u"LuminaTranslateReceiveInfo"_q, u"Gelen mesajlar orijinal metnini "
			u"tam boyutta korur, çeviri ise altında gösterilir."_q },
		{ u"LuminaTranslateProviderHeader"_q, u"Hizmet"_q },
		{ u"LuminaTranslateProvider"_q, u"Çeviri hizmeti"_q },
		// The other provider names are the services' own brands and are not
		// translated; this one names a kind of endpoint.
		{ u"LuminaTranslateProviderLlm"_q, u"LLM (OpenAI uyumlu)"_q },
		{ u"LuminaTranslateApiKey"_q, u"API anahtarı"_q },
		{ u"LuminaTranslateApiKeyNotSet"_q, u"Ayarlanmadı"_q },
		{ u"LuminaTranslateBaseUrl"_q, u"Temel URL"_q },
		{ u"LuminaTranslateModel"_q, u"Model"_q },
		{ u"LuminaTranslateSystemPrompt"_q, u"Sistem istemi"_q },
		{ u"LuminaTranslatePromptDefault"_q, u"Varsayılan"_q },
		{ u"LuminaTranslatePromptCustom"_q, u"Özel"_q },
		{ u"LuminaTranslateFallbackTelegram"_q,
			u"Bu hizmet başarısız olursa Telegram'a geri dön"_q },
		{ u"LuminaTranslateTest"_q, u"Çeviriyi test et"_q },
		{ u"LuminaTranslateTestRunning"_q, u"Test ediliyor…"_q },
		{ u"LuminaTranslateTestSuccess"_q, u"Çeviri çalışıyor."_q },
		{ u"LuminaTranslateTestFailed"_q, u"Test başarısız"_q },
		{ u"LuminaTranslateNoKey"_q, u"API anahtarı gerekli"_q },
		{ u"LuminaTranslateTestKeyRejected"_q, u"Hizmete ulaşılamadı ya da "
			u"hizmet bu API anahtarını kabul etmedi. Önce anahtarı, sonra "
			u"temel URL'yi ve bağlantınızı denetleyin."_q },
		{ u"LuminaTranslateTestNetwork"_q, u"Hizmete ulaşılamadı. İnternet "
			u"bağlantınızı ve varsa vekil sunucu ayarlarınızı denetleyip "
			u"yeniden deneyin."_q },
		{ u"LuminaTranslateTestQuota"_q, u"Hizmet isteği geri çevirdi: çok "
			u"fazla istek gönderildi ya da bu anahtarın kotası doldu. Daha "
			u"sonra yeniden deneyin."_q },
		{ u"LuminaTranslateTestQuotaKeyed"_q, u"Hizmet isteği geri çevirdi: "
			u"bu API anahtarını kabul etmedi, anahtarın kotası doldu ya da "
			u"çok fazla istek gönderildi. Önce anahtarı denetleyin, sonra "
			u"daha sonra yeniden deneyin."_q },
		{ u"LuminaTranslateTestBadResponse"_q, u"Hizmet, LuminaGram'ın "
			u"okuyamadığı bir yanıt verdi ya da hiçbir şey döndürmedi. Temel "
			u"URL'yi ve modeli denetleyin."_q },
		{ u"LuminaTranslateTestUnavailable"_q, u"Bu hizmet burada "
			u"çalışamıyor. Yukarıdan başka bir hizmet seçin."_q },
		{ u"LuminaTranslateProviderSecurityInfo"_q, u"Anahtarlar yalnızca bu "
			u"cihazda, diğer ayarlardan ayrı bir dosyada saklanır ve hiçbir "
			u"zaman Telegram'a gönderilmez. Çevirdiğiniz her şey burada "
			u"seçilen hizmete gönderilir, bu yüzden güvendiğiniz bir hizmet "
			u"seçin."_q },

		// Translate before sending: the boxes and the send menu.
		{ u"LuminaTranslateOriginalLabel"_q, u"Orijinal"_q },
		{ u"LuminaSendTranslation"_q, u"Çeviriyi gönder"_q },
		{ u"LuminaSendOriginal"_q, u"Orijinali gönder"_q },
		{ u"LuminaTrSendPickerTitle"_q, u"Mesajları şu dile çevir"_q },
		{ u"LuminaTrSendConfirmMessage"_q, u"Bu sohbet {1} dilinde yazılıyor "
			u"gibi görünüyor. Buraya gönderdiğiniz mesajlar {1} diline "
			u"çevrilsin mi? LuminaGram bunu bu sohbet için hatırlayacak."_q },
		{ u"LuminaTrSendConfirmTranslate"_q, u"Çevir"_q },
		{ u"LuminaTrSendAsTyped"_q, u"Çevirmeden gönder"_q },
		{ u"LuminaTrSendChooseOther"_q, u"Başka bir dil seç"_q },

		// The translate-before-send preview bar above the composer.
		{ u"LuminaTranslatePreviewTranslating"_q, u"Çevriliyor…"_q },
		{ u"LuminaTranslatePreviewFailed"_q, u"Çeviri kullanılamıyor"_q },

		// The per-chat translate button in the chat top bar. Both texts name
		// what pressing the button will do, not what the chat is doing now.
		{ u"LuminaTranslateChatToggle"_q, u"Bu sohbeti çevir"_q },
		{ u"LuminaTranslateChatShowOriginal"_q, u"Orijinali göster"_q },

		// Pressing that button: the two rows that state this conversation's
		// language pair. Each row names whose messages it is about - the other
		// party and myself, never a direction word. Off is its own row now,
		// so a value row always names a real target: a language name, or the
		// recipient's-language answer on the Me row. Neither can carry a case
		// ending here, so the ending sits on the noun "dil" and the value
		// itself follows the colon.
		{ u"LuminaChatLangThem"_q,
			u"Karşı tarafın mesajları şu dile çevrilir: {1}"_q },
		{ u"LuminaChatLangThemOff"_q,
			u"Karşı tarafın mesajları çevrilmiyor"_q },
		{ u"LuminaChatLangMe"_q,
			u"Benim mesajlarım şu dile çevrilir: {1}"_q },
		{ u"LuminaChatLangMeOff"_q, u"Benim mesajlarım çevrilmiyor"_q },
		{ u"LuminaChatLangThemTitle"_q,
			u"Karşı tarafın mesajlarını şu dile çevir"_q },
		{ u"LuminaChatLangMeTitle"_q, u"Mesajlarımı şu dile çevir"_q },
		{ u"LuminaChatLangNone"_q, u"Çevrilmiyor"_q },

		// The tray menu and the taskbar button's jump list, which upstream
		// builds from lng_open_from_tray / lng_quit_from_tray - "Telegram" in
		// every language it serves. The name is substituted bare and cannot
		// take a case ending, so the ending sits on "uygulama" after it.
		{ u"LuminaTrayOpen"_q, u"{1} uygulamasını aç"_q },
		{ u"LuminaTrayQuit"_q, u"{1} uygulamasından çık"_q },

		// Telegram's own AI editor, which carries a Translate tab of its own
		// and so overlaps what LuminaGram translation already does.
		{ u"LuminaAiEditorHeader"_q, u"Telegram yapay zekâ düzenleyicisi"_q },
		{ u"LuminaAiEditorKeep"_q,
			u"Telegram'ın yapay zekâ düzenleyicisini koru"_q },
		{ u"LuminaAiEditorInfo"_q, u"Telegram'ın mesaj kutusunda kendi yapay "
			u"zekâ düzenleyicisi vardır ve onun Çeviri sekmesi LuminaGram'ın "
			u"çevirisiyle çakışır. LuminaGram çevirisi açıkken bu "
			u"düzenleyicinin düğmesi de klavye kısayolu da sunulmaz; böylece "
			u"önünüzde her zaman tek bir çeviri aracı olur. Telegram'ın "
			u"düzenleyicisini yine de kullanılabilir tutmak için bunu açın. "
			u"LuminaGram çevirisi kapalıyken Telegram'ın düzenleyicisi zaten "
			u"her zaman oradadır ve bu ayar hiçbir şeyi değiştirmez."_q },

		// Names of the languages LuminaGram can translate into, in the order
		// the pickers show them.
		{ u"LuminaLangEn"_q, u"İngilizce"_q },
		{ u"LuminaLangZhTw"_q, u"Çince (Geleneksel)"_q },
		{ u"LuminaLangZhCn"_q, u"Çince (Basitleştirilmiş)"_q },
		{ u"LuminaLangJa"_q, u"Japonca"_q },
		{ u"LuminaLangKo"_q, u"Korece"_q },
		{ u"LuminaLangEs"_q, u"İspanyolca"_q },
		{ u"LuminaLangFr"_q, u"Fransızca"_q },
		{ u"LuminaLangDe"_q, u"Almanca"_q },
		{ u"LuminaLangRu"_q, u"Rusça"_q },
		{ u"LuminaLangPtBr"_q, u"Portekizce (Brezilya)"_q },
		{ u"LuminaLangPtPt"_q, u"Portekizce (Portekiz)"_q },
		{ u"LuminaLangIt"_q, u"İtalyanca"_q },
		{ u"LuminaLangAr"_q, u"Arapça"_q },
		{ u"LuminaLangHi"_q, u"Hintçe"_q },
		{ u"LuminaLangId"_q, u"Endonezce"_q },
		{ u"LuminaLangTh"_q, u"Tayca"_q },
		{ u"LuminaLangVi"_q, u"Vietnamca"_q },
		{ u"LuminaLangTr"_q, u"Türkçe"_q },
		{ u"LuminaLangPl"_q, u"Lehçe"_q },
		{ u"LuminaLangUk"_q, u"Ukraynaca"_q },
		{ u"LuminaLangNl"_q, u"Felemenkçe"_q },
		{ u"LuminaLangAf"_q, u"Afrikaanca"_q },
		{ u"LuminaLangSq"_q, u"Arnavutça"_q },
		{ u"LuminaLangAm"_q, u"Amharca"_q },
		{ u"LuminaLangHy"_q, u"Ermenice"_q },
		{ u"LuminaLangAz"_q, u"Azerice"_q },
		{ u"LuminaLangEu"_q, u"Baskça"_q },
		{ u"LuminaLangBe"_q, u"Belarusça"_q },
		{ u"LuminaLangBn"_q, u"Bengalce"_q },
		{ u"LuminaLangBs"_q, u"Boşnakça"_q },
		{ u"LuminaLangBg"_q, u"Bulgarca"_q },
		{ u"LuminaLangMy"_q, u"Birmanca"_q },
		{ u"LuminaLangCa"_q, u"Katalanca"_q },
		{ u"LuminaLangHr"_q, u"Hırvatça"_q },
		{ u"LuminaLangCs"_q, u"Çekçe"_q },
		{ u"LuminaLangDa"_q, u"Danca"_q },
		{ u"LuminaLangEt"_q, u"Estonca"_q },
		{ u"LuminaLangTl"_q, u"Filipince"_q },
		{ u"LuminaLangFi"_q, u"Fince"_q },
		{ u"LuminaLangGl"_q, u"Galiçyaca"_q },
		{ u"LuminaLangKa"_q, u"Gürcüce"_q },
		{ u"LuminaLangEl"_q, u"Yunanca"_q },
		{ u"LuminaLangGu"_q, u"Gucaratça"_q },
		{ u"LuminaLangHe"_q, u"İbranice"_q },
		{ u"LuminaLangHu"_q, u"Macarca"_q },
		{ u"LuminaLangIs"_q, u"İzlandaca"_q },
		{ u"LuminaLangGa"_q, u"İrlandaca"_q },
		{ u"LuminaLangJv"_q, u"Cavaca"_q },
		{ u"LuminaLangKn"_q, u"Kannada"_q },
		{ u"LuminaLangKk"_q, u"Kazakça"_q },
		{ u"LuminaLangKm"_q, u"Khmerce"_q },
		{ u"LuminaLangKu"_q, u"Kürtçe"_q },
		{ u"LuminaLangKy"_q, u"Kırgızca"_q },
		{ u"LuminaLangLo"_q, u"Laoca"_q },
		{ u"LuminaLangLv"_q, u"Letonca"_q },
		{ u"LuminaLangLt"_q, u"Litvanca"_q },
		{ u"LuminaLangMk"_q, u"Makedonca"_q },
		{ u"LuminaLangMs"_q, u"Malayca"_q },
		{ u"LuminaLangMl"_q, u"Malayalamca"_q },
		{ u"LuminaLangMr"_q, u"Marathice"_q },
		{ u"LuminaLangMn"_q, u"Moğolca"_q },
		{ u"LuminaLangNe"_q, u"Nepalce"_q },
		{ u"LuminaLangNo"_q, u"Norveççe"_q },
		{ u"LuminaLangPs"_q, u"Peştuca"_q },
		{ u"LuminaLangFa"_q, u"Farsça"_q },
		{ u"LuminaLangPa"_q, u"Pencapça"_q },
		{ u"LuminaLangRo"_q, u"Rumence"_q },
		{ u"LuminaLangSr"_q, u"Sırpça"_q },
		{ u"LuminaLangSi"_q, u"Seylanca"_q },
		{ u"LuminaLangSk"_q, u"Slovakça"_q },
		{ u"LuminaLangSl"_q, u"Slovence"_q },
		{ u"LuminaLangSo"_q, u"Somalice"_q },
		{ u"LuminaLangSw"_q, u"Svahilice"_q },
		{ u"LuminaLangSv"_q, u"İsveççe"_q },
		{ u"LuminaLangTg"_q, u"Tacikçe"_q },
		{ u"LuminaLangTa"_q, u"Tamilce"_q },
		{ u"LuminaLangTe"_q, u"Teluguca"_q },
		{ u"LuminaLangUr"_q, u"Urduca"_q },
		{ u"LuminaLangUz"_q, u"Özbekçe"_q },
		{ u"LuminaLangCy"_q, u"Galce"_q },
		{ u"LuminaLangYi"_q, u"Yidiş"_q },
		{ u"LuminaLangZu"_q, u"Zuluca"_q },
		{ u"LuminaAppearanceStickerSizeHeader"_q, u"Çıkartma boyutu"_q },
		{ u"LuminaBackupCryptoFailed"_q, u"Bu sistemde şifreleme kullanılamıyor."_q },
		{ u"LuminaBackupDamaged"_q, u"Yedek dosyası hasarlı ve geri yüklenemedi."_q },
		{ u"LuminaBackupExport"_q, u"Şifreli yedeği dışa aktar"_q },
		{ u"LuminaBackupExportDone"_q, u"Yedek kaydedildi."_q },
		{ u"LuminaBackupExportFailed"_q, u"Yedek dosyası oluşturulamadı."_q },
		{ u"LuminaBackupExportInfo"_q, u"LuminaGram'ın bu cihazda sakladığı her şeyi — yer imleri, notlar, yanıt şablonları, metin değiştirmeleri ve tüm ayarlar — seçtiğiniz bir parolayla şifrelenmiş tek bir dosyaya kaydedin. Dosya gizli değerleri de taşır: çeviri API anahtarları, kasa ve sahte çökme kodları, sahte not defterinin içeriği. Telegram'a hiçbir şey gönderilmez. Uzun bir parola seçin ve güvenli bir yerde saklayın — parola olmadan dosya açılamaz ve kurtarmanın hiçbir yolu yoktur."_q },
		{ u"LuminaBackupExportPassphraseTitle"_q, u"Bir parola belirleyin"_q },
		{ u"LuminaBackupFileFilter"_q, u"LuminaGram yedeği (*.lgbak)"_q },
		{ u"LuminaBackupImport"_q, u"Yedeği içe aktar"_q },
		{ u"LuminaBackupImportFailed"_q, u"Yedek dosyası okunamadı."_q },
		{ u"LuminaBackupImportInfo"_q, u"Bir yedek dosyası seçin ve LuminaGram verilerinizi geri yüklemek için parolasını girin. Yedeğin taşıdığı ayarlar bu cihazdakilerin yerine geçer; taşımadıklarına dokunulmaz. Doğrulanamayan bir dosya, hiçbir şey yazılmadan önce geri çevrilir; böylece yanlış bir parola sizi asla yarım geri yüklenmiş bırakamaz."_q },
		{ u"LuminaBackupImportSuccess"_q, u"Yedek geri yüklendi. Her şeyi uygulamak için LuminaGram'ı yeniden başlatın."_q },
		{ u"LuminaBackupInvalidFile"_q, u"Bu geçerli bir LuminaGram yedek dosyası değil."_q },
		{ u"LuminaBackupNewerFormat"_q, u"Bu yedek, LuminaGram'ın daha yeni bir sürümüyle oluşturulmuş."_q },
		{ u"LuminaBackupOpenCaption"_q, u"LuminaGram yedeğini aç"_q },
		{ u"LuminaBackupPassphraseHint"_q, u"Parola"_q },
		{ u"LuminaBackupPassphraseMismatch"_q, u"İki parola birbirinden farklı."_q },
		{ u"LuminaBackupPassphraseRepeatHint"_q, u"Parolayı yeniden girin"_q },
		{ u"LuminaBackupPassphraseTitle"_q, u"Parolayı girin"_q },
		{ u"LuminaBackupPassphraseTooShort"_q, u"En az 4 karakterlik bir parola seçin."_q },
		{ u"LuminaBackupSaveCaption"_q, u"LuminaGram yedeğini kaydet"_q },
		{ u"LuminaBackupTitle"_q, u"Şifreli yedek"_q },
		{ u"LuminaBackupUnauthenticated"_q, u"Bu yedek, yanlış bir parolaya ya da kurcalanmaya karşı denetlenemeyen eski korumasız biçimi kullanıyor. Güncel bir LuminaGram ile yeni bir yedek alın."_q },
		{ u"LuminaBackupWrongPassphrase"_q, u"Yanlış parola ya da dosya değiştirilmiş."_q },
		{ u"LuminaBookmark"_q, u"Yer imi ekle"_q },
		{ u"LuminaBookmarkAdded"_q, u"Yer imlerine eklendi"_q },
		{ u"LuminaBookmarkChatUnavailable"_q, u"Sohbet kullanılamıyor"_q },
		{ u"LuminaBookmarkDeleteTitle"_q, u"Yer imi silinsin mi?"_q },
		{ u"LuminaBookmarkGone"_q, u"Bu sohbet artık bu cihazda kullanılamıyor."_q },
		{ u"LuminaBookmarkRemove"_q, u"Yer imini kaldır"_q },
		{ u"LuminaBookmarkRemoved"_q, u"Yer imlerinden kaldırıldı"_q },
		{ u"LuminaBookmarksAbout"_q, u"Yer imleri, mesajlara giden işaretçilerdir; yalnızca bu cihazda tutulur ve hiçbir zaman Telegram'a gönderilmez. Bir yer imini kaldırmak mesajın kendisine dokunmaz."_q },
		{ u"LuminaBookmarksEmpty"_q, u"Henüz hiçbir mesajı yer imlerine eklemediniz."_q },
		{ u"LuminaBookmarksFull"_q, u"Yer imi listesi dolu. Yeni bir tane eklemek için bir yer imini kaldırın."_q },
		{ u"LuminaBookmarksList"_q, u"Yer imli mesajlar"_q },
		{ u"LuminaBookmarksListAbout"_q, u"Mesajı açmak için bir yer imine tıklayın, kaldırmak için sağ tıklayın. Bir yer imi, mesajı silinse bile bu listede kalır."_q },
		{ u"LuminaBookmarksNone"_q, u"Yok"_q },
		{ u"LuminaBookmarksTitle"_q, u"Yer imleri"_q },
		{ u"LuminaChatListDensityTitle"_q, u"Sohbet listesi yoğunluğu"_q },
		{ u"LuminaCompactListRows"_q, u"Kompakt satırlar"_q },
		{ u"LuminaCompactListRowsInfo"_q, u"Sohbet listesindeki her satırı kısaltarak ekrana daha fazla sohbet sığdırır. Profil fotoğrafları ve mesaj önizlemeleri görünmeye devam eder."_q },
		{ u"LuminaChatListDotsTitle"_q, u"Avatar noktaları"_q },
		{ u"LuminaChatListOnlineDot"_q, u"Çevrimiçi noktası"_q },
		{ u"LuminaChatListOnlineDotInfo"_q, u"Karşı tarafın şu anda çevrimiçi olduğu birebir sohbetlerin avatarında küçük yeşil bir nokta gösterir."_q },
		{ u"LuminaChatListRecencyDot"_q, u"Son görülme noktası"_q },
		{ u"LuminaChatListRecencyDotInfo"_q, u"Kişinin ne kadar süre önce görüldüğüne göre birebir sohbetlerdeki avatar noktasını renklendirir: şu anda çevrimiçiyse yeşil, bir saat içinde sarı, bir gün içinde turuncu. Daha eski ya da gizli son görülme için nokta gösterilmez. Yeşil (çevrimiçi) durumu yine «Çevrimiçi noktası» anahtarıyla denetlenir."_q },
		{ u"LuminaChatListVisibilityAbout"_q, u"Klasör çubuğunu sohbet listesinden kaldırır — hem yanındaki dikey çubuğu hem de üstündeki yatay çubuğu. Klasör sekmeleri gizliyken her zaman tüm sohbetlerinizi görürsünüz: içinde bulunduğunuz klasörden çıkılır, klasör kısayolları ve klasörler arasında kaydırma hiçbir şey yapmaz. Hikâyeleri gizlemek yalnızca sohbet listesinin üstündeki satırı kaldırır; hikâyelerin kendisi ve onları açmanın diğer tüm yolları değişmeden kalır."_q },
		{ u"LuminaChatListVisibilityTitle"_q, u"Klasörler ve hikâyeler"_q },
		{ u"LuminaContactNote"_q, u"Özel not"_q },
		{ u"LuminaContactNoteAbout"_q, u"Bunu yalnızca siz görebilirsiniz. Bu cihazda kalır, hiçbir zaman Telegram'a gönderilmez ve diğer cihazlarınızla eşitlenmez."_q },
		{ u"LuminaContactNoteEmpty"_q, u"Özel not eklemek için tıklayın"_q },
		{ u"LuminaContactNoteHint"_q, u"Not (yalnızca siz görebilirsiniz)"_q },
		{ u"LuminaContactNotesAbout"_q, u"Bir kişinin profiline özel bir not ve etiket listesi ekler. İkisi de bu cihazda kalır, hiçbir zaman Telegram'a gönderilmez ve diğer cihazlarınızla eşitlenmez. Telegram'ın kendi kişi notlarına dokunulmaz: özel not, onların bulunmadığı yerlerde sunulur — botlarda ve kişilerinizde olmayan insanlarda — etiketler ise her yerde sunulur."_q },
		{ u"LuminaContactNotesClear"_q, u"Tüm özel notları sil"_q },
		{ u"LuminaContactNotesClearText"_q, u"Bu cihazda saklanan her özel not ve etiket silinsin mi? Bu işlem geri alınamaz."_q },
		{ u"LuminaContactNotesClearTitle"_q, u"Özel notları sil"_q },
		{ u"LuminaContactNotesNone"_q, u"Yok"_q },
		{ u"LuminaContactNotesTitle"_q, u"Özel kişi notları"_q },
		{ u"LuminaContactNotesToggle"_q, u"Profillerde özel notlar ve etiketler"_q },
		{ u"LuminaContactTags"_q, u"Etiketler"_q },
		{ u"LuminaContactTagsEmpty"_q, u"Etiket eklemek için tıklayın"_q },
		{ u"LuminaContactTagsHint"_q, u"Etiketler, virgülle ayrılmış"_q },
		{ u"LuminaDetailsDate"_q, u"Tarih"_q },
		{ u"LuminaDetailsForwardedFrom"_q, u"Şuradan iletildi"_q },
		{ u"LuminaDetailsFrom"_q, u"Gönderen"_q },
		{ u"LuminaDetailsMessageId"_q, u"Mesaj kimliği"_q },
		{ u"LuminaDetailsOriginalDate"_q, u"Özgün tarih"_q },
		{ u"LuminaExactNumbers"_q, u"Tam sayıları göster (1.2K yuvarlaması yok)"_q },
		{ u"LuminaExactNumbersInfo"_q, u"1.2M gibi kısaltmalar yerine 1.234.567 gibi tam sayıları gösterir. Ekranda hâlihazırda çizilmiş sayaçlar, uygulama yeniden başlatılana kadar mevcut biçimini korur."_q },
		{ u"LuminaForwardNoAuthor"_q, u"Yazarı olmadan ilet"_q },
		{ u"LuminaForwardNoAuthorTitle"_q, u"Yazarı olmadan ilet"_q },
		{ u"LuminaForwardNoCaption"_q, u"Yazarı ve açıklamaları olmadan ilet"_q },
		{ u"LuminaForwardNoCaptionTitle"_q, u"Yazarı ve açıklamaları olmadan ilet"_q },
		{ u"LuminaHideStories"_q, u"Hikâyeleri gizle"_q },
		{ u"LuminaHideTabs"_q, u"Klasör sekmelerini gizle"_q },
		{ u"LuminaLinkSafetyDestination"_q, u"Gerçek hedef"_q },
		{ u"LuminaLinkSafetyInfo"_q, u"Gerçek hedefini «@» işaretinden önceki metnin arkasına gizleyen, alan adını punycode ile yazan ya da bilinen bir bağlantı kısaltıcıdan geçen bir bağlantıyı açmadan önce sorar. Onay penceresi gerçek hedef sunucuyu ve tam adresi gösterir. Başka alfabelerle yazılmış benzer görünen alan adlarını Telegram Desktop zaten kendisi sorar. Hiçbir şey çevrimiçi denetlenmez — açtığınız hiçbir adres bu cihazdan çıkmaz."_q },
		{ u"LuminaLinkSafetyRow"_q, u"Bağlantı güvenlik denetleyicisi"_q },
		{ u"LuminaLinkSafetyTitle"_q, u"Dış bağlantı açılsın mı?"_q },
		{ u"LuminaLinkSafetyWarnMismatch"_q, u"Bu bağlantı gerçek hedefini «@» işaretinden önceki metnin arkasına gizliyor."_q },
		{ u"LuminaLinkSafetyWarnPunycode"_q, u"Bu adres, tanınmış bir siteyi taklit edebilecek kodlanmış (punycode) karakterler kullanıyor."_q },
		{ u"LuminaLinkSafetyWarnShortener"_q, u"Bu bir bağlantı kısaltıcı — gerçek hedef, siz açana kadar gizli kalır."_q },
		{ u"LuminaMediaAutoPauseBgVideo"_q, u"Uygulama arka plana geçtiğinde videoyu duraklat"_q },
		{ u"LuminaMediaAutoPauseBgVideoInfo"_q, u"LuminaGram'dan ayrıldığınızda oynatılan videoyu otomatik olarak duraklatır. İzleyiciyi küçültmek de videoyu duraklatır. Bilerek kendi penceresine ya da pencere içinde pencereye aldığınız bir video oynamayı sürdürür; siz yokken müzik ve sesli mesajlar da çalmayı sürdürür."_q },
		{ u"LuminaMediaSaving"_q, u"Medya"_q },
		{ u"LuminaMediaTitle"_q, u"Medya"_q },
		{ u"LuminaMessageActionsInfo"_q, u"Bu öğeler yalnızca bir mesajın sağ tık menüsünde görünür. Her biri Telegram'ın kendi iletme işlevini kullanır, bu yüzden iletmeyi kısıtlayan bir sohbet kısıtlı kalır. «Yazarı ve açıklamaları olmadan ilet», gerçekte ne gönderildiğine göre adlandırılmıştır: Telegram, göndereni koruyup açıklamaları atamaz, bu yüzden bu seçenek ikisini birden atar. «Ayrıntılar» yalnızca bu cihazın hâlihazırda eşitlediklerini okur — hiçbir şey istenmez ve hiçbir şey saklanmaz."_q },
		{ u"LuminaMessageDetails"_q, u"Ayrıntılar"_q },
		{ u"LuminaOnboardingDualName"_q, u"İki dil bir arada"_q },
		{ u"LuminaOnboardingDualText"_q, u"Aldığınız ve gönderdiğiniz mesajlarda orijinal metni çevirisinin yanında ekranda tutun."_q },
		{ u"LuminaOnboardingFooter"_q, u"Bunlardan herhangi birini ayarlamak için LuminaGram ayarlar sayfasındaki bir bölümü açın. Bu kartı Araçlar bölümünden yeniden okuyabilirsiniz."_q },
		{ u"LuminaOnboardingGotIt"_q, u"Anladım"_q },
		{ u"LuminaOnboardingHeader"_q, u"Hakkında"_q },
		{ u"LuminaOnboardingIntro"_q, u"LuminaGram, Telegram Desktop'ın üzerine kendi araçlarını ekler. Aşağıdaki her seçenek yalnızca bu bilgisayarda saklanır ve hiçbir zaman Telegram ile eşitlenmez."_q },
		{ u"LuminaOnboardingRow"_q, u"LuminaGram neler ekliyor"_q },
		{ u"LuminaOnboardingRowAbout"_q, u"LuminaGram ayarlarını ilk kez açtığınızda çıkan karşılama kartını gösterir. Yalnızca burada nelerin olduğunu anlatır, hiçbir şeyi açmaz."_q },
		{ u"LuminaOnboardingSafetyName"_q, u"Güvenlik denetimleri"_q },
		{ u"LuminaOnboardingSafetyText"_q, u"Adresi göründüğü gibi olmayan bir bağlantıyı açmadan önce uyarır, yapıştırdığınız bir kripto adresi değiştirilmişse yakalar ve gönderdiğiniz fotoğraflardan konum bilgisini siler."_q },
		{ u"LuminaOnboardingTitle"_q, u"LuminaGram'a hoş geldiniz"_q },
		{ u"LuminaOnboardingTranslateText"_q, u"Gelen mesajları kendi çeviri hizmetinizle çevirin, yazdıklarınızı da gönderilmeden önce çevirin."_q },
		{ u"LuminaOnboardingVaultName"_q, u"Gizlenme kasası"_q },
		{ u"LuminaOnboardingVaultText"_q, u"LuminaGram'ı, gerçek uygulamayı yalnızca gizli kodunuzla açan bir hesap makinesinin ya da not defterinin arkasına gizleyin."_q },
		{ u"LuminaProfileCardAbout"_q, u"Kendinizle ilgili yerel bir kart — hangi dilleri konuştuğunuz, nelerle ilgilendiğiniz — kopyalayıp bir sohbete yapıştırabilirsiniz. Bu cihazda kalır."_q },
		{ u"LuminaProfileCardBio"_q, u"Kısa özgeçmiş"_q },
		{ u"LuminaProfileCardBioHint"_q, u"Kendinizle ilgili birkaç kelime"_q },
		{ u"LuminaProfileCardCopied"_q, u"Kart kopyalandı"_q },
		{ u"LuminaProfileCardCopy"_q, u"Panoya kopyala"_q },
		{ u"LuminaProfileCardCopyInfo"_q, u"Kartınızın düz metin özetini oluşturur. Paylaşmak istediğinizde herhangi bir sohbete yapıştırın."_q },
		{ u"LuminaProfileCardEdit"_q, u"Kartım"_q },
		{ u"LuminaProfileCardEmptyShare"_q, u"Önce kartınızı doldurun"_q },
		{ u"LuminaProfileCardHeader"_q, u"Kartım"_q },
		{ u"LuminaProfileCardInfo"_q, u"Bu kart yalnızca bu cihazda saklanır ve hiçbir zaman Telegram'a gönderilmez. Telegram profilinizi değiştirmez."_q },
		{ u"LuminaProfileCardInterests"_q, u"İlgi alanları / etiketler"_q },
		{ u"LuminaProfileCardInterestsHint"_q, u"örn. müzik, doğa yürüyüşü, kodlama"_q },
		{ u"LuminaProfileCardLanguages"_q, u"Konuştuğum diller"_q },
		{ u"LuminaProfileCardLanguagesHint"_q, u"örn. Türkçe, English, Español"_q },
		{ u"LuminaProfileCardNotSet"_q, u"Ayarlanmadı"_q },
		{ u"LuminaProfileCardTagline"_q, u"Slogan"_q },
		{ u"LuminaProfileCardTaglineHint"_q, u"Kendinizle ilgili kısa bir söz"_q },
		{ u"LuminaProfileCardTitle"_q, u"Profil kartı"_q },
		{ u"LuminaProfileChatCreated"_q, u"Oluşturuldu"_q },
		{ u"LuminaProfileDcId"_q, u"Veri merkezi"_q },
		{ u"LuminaProfileDcIdValue"_q, u"DC{1}"_q },
		{ u"LuminaProfileInfoHeader"_q, u"Profil"_q },
		{ u"LuminaProfileInfoInfo"_q, u"Profil sayfalarındaki ek satırlar; hepsi bu cihazda hesaplanır — Telegram'dan hiçbir şey istenmez. Bazı hesaplar için kayıt tarihini Telegram'ın kendisi verir; geri kalanlar için tarih hesap numarasından tahmin edilir ve başında «~» ile gösterilir. Veri merkezi, profil fotoğrafını saklayan merkezdir. Oluşturma tarihi, bir grubun ya da kanalın kurulduğu tarihtir."_q },
		{ u"LuminaProfileRegistrationApprox"_q, u"~ {1}"_q },
		{ u"LuminaProfileRegistrationDate"_q, u"Kayıt tarihi"_q },
		{ u"LuminaProfileShowChatDate"_q, u"Oluşturma tarihini göster"_q },
		{ u"LuminaProfileShowDcId"_q, u"Veri merkezini göster"_q },
		{ u"LuminaProfileShowRegistrationDate"_q, u"Kayıt tarihini göster"_q },
		{ u"LuminaRecentLimitsInfo"_q, u"Bu cihazın son kullanılan çıkartmaları ve kayıtlı GIF'leri gerekenden önce atmasını engeller; Android'deki LuminaGram ile aynı sınırları kullanır: en fazla 200 son çıkartma ve 500 kayıtlı GIF. Hesabınızda gerçekte kaçının saklandığına ve diğer cihazlarınıza kaçının eşitlendiğine yine Telegram'ın sunucuları karar verir, bu yüzden bu ayar size daha büyük bir bulut listesi veremez — bu cihaz bir sonraki eşitlemesinde, sunucunun artık tutmadığı her şey genellikle saniyeler içinde buradan da kaybolur. Bu açıkken Telegram Desktop kayıtlı bir GIF'i kendiliğinden kaldırmayı bırakır, böylece kayıtlı GIF sınırıyla ilgili Premium hatırlatması da susar. Ayarlar › Gelişmiş › Deneysel ayarlar bölümünden «Sınırsız son kullanılan çıkartmalar» seçeneğini de açmadığınız sürece çıkartma paneli yine yalnızca ilk 20 son çıkartmayı gösterir."_q },
		{ u"LuminaRecentLimitsRow"_q, u"Daha fazla son çıkartma ve GIF sakla"_q },
		{ u"LuminaReplyTemplatesAbout"_q, u"Bu cihazda sakladığınız ve mesaj alanına ekleyebileceğiniz kısa metin parçaları. Birini seçmek için bir sohbette emoji düğmesine sağ tıklayın. Şablonlar hiçbir zaman Telegram'a gönderilmez."_q },
		{ u"LuminaReplyTemplatesAdd"_q, u"Şablon ekle"_q },
		{ u"LuminaReplyTemplatesEdit"_q, u"Şablonu düzenle"_q },
		{ u"LuminaReplyTemplatesEmpty"_q, u"Henüz şablon yok. Bir tane ekleyin, sonra bir sohbette emoji düğmesine sağ tıklayıp ekleyin."_q },
		{ u"LuminaReplyTemplatesFull"_q, u"Liste dolu — yenisini eklemek için bir şablonu silin."_q },
		{ u"LuminaReplyTemplatesInfo"_q, u"Bir şablonu eklemek için bir sohbette emoji düğmesine sağ tıklayın. Buradaki bir şablonu yukarı ya da aşağı taşımak için ona sağ tıklayın; düzenlemek ya da silmek için açın."_q },
		{ u"LuminaReplyTemplatesList"_q, u"Şablonlar"_q },
		{ u"LuminaReplyTemplatesManage"_q, u"Şablonları yönet…"_q },
		{ u"LuminaReplyTemplatesMoveDown"_q, u"Aşağı taşı"_q },
		{ u"LuminaReplyTemplatesMoveUp"_q, u"Yukarı taşı"_q },
		{ u"LuminaReplyTemplatesNone"_q, u"Yok"_q },
		{ u"LuminaReplyTemplatesPlaceholder"_q, u"Şablon metni"_q },
		{ u"LuminaReplyTemplatesShow"_q, u"Sohbetlerde şablonları öner"_q },
		{ u"LuminaReplyTemplatesTitle"_q, u"Yanıt şablonları"_q },
		{ u"LuminaSaveSticker"_q, u"Çıkartmayı kaydet"_q },
		{ u"LuminaSaveStickers"_q, u"Çıkartmaları kaydet"_q },
		{ u"LuminaSaveStickersInfo"_q, u"Çıkartma panelinin sağ tık menüsüne bir «Çıkartmayı kaydet» satırı ekler. Çıkartmalar diğer indirmelerinizin gittiği yere gider; bir grup kaydetmeyi kısıtlıyorsa o grubun kendi çıkartma paketinde bu satır gizlenir."_q },
		{ u"LuminaSaveToCloud"_q, u"Kayıtlı Mesajlar'a kaydet"_q },
		{ u"LuminaSaveToCloudTitle"_q, u"Kayıtlı Mesajlar'a kaydet"_q },
		{ u"LuminaSecurityPanicConfirmAck"_q, u"Bunun geri alınamayacağını anlıyorum"_q },
		{ u"LuminaSecurityPanicConfirmButton"_q, u"Şimdi sil"_q },
		{ u"LuminaSecurityPanicConfirmText"_q, u"Bu cihazdaki tüm hesaplardan çıkış yapılacak. Yerel mesaj veritabanı, taslaklar ve önbelleğe alınmış medya, LuminaGram'ın kendi ayarları, yer imleri, kayıtlı çevirileri ve API anahtarlarıyla birlikte silinecek.\n\nHesaplarınız silinmez. Telegram'ın sunucularında kalırlar, mesajlarınız da öyle — istediğiniz yerden yeniden giriş yapabilirsiniz.\n\nDaha önce indirilmiş dosyalar SİLİNMEZ. LuminaGram indirme klasörünüze dokunmaz, çünkü orası genellikle sıradan İndirilenler klasörünüzdür ve ilgisiz dosyalar barındırır. Oradaki hassas her şeyi kendiniz taşıyın ya da silin.\n\nBu işlem geri alınamaz."_q },
		{ u"LuminaSecurityPanicConfirmTitle"_q, u"Acil silme?"_q },
		{ u"LuminaSecurityPanicHeader"_q, u"Acil silme"_q },
		{ u"LuminaSecurityPanicWipe"_q, u"Acil silme (Kaboom)"_q },
		{ u"LuminaSecurityPanicWipeAbout"_q, u"Bu cihazdaki tüm hesaplardan çıkış yapar; yerel mesaj veritabanını, taslakları ve önbelleğe alınmış medyayı, LuminaGram'ın kendi ayarları, yer imleri, kayıtlı çevirileri ve API anahtarlarıyla birlikte siler. Hesaplarınız ve mesajlarınız Telegram'ın sunucularında kalır. Daha önce indirdiğiniz dosyalar oldukları yerde bırakılır. Bu işlem geri alınamaz."_q },
		{ u"LuminaSelectFromAuthor"_q, u"Yazarın tüm mesajlarını seç"_q },
		{ u"LuminaSelectFromAuthorAbout"_q, u"Gruplarda, tıkladığınız gönderenin tüm mesajlarını seçen bir mesaj menüsü öğesi ekler. Yalnızca bu pencerenin hâlihazırda yüklediği mesajlara ulaşır — daha eskilerini de katmak için geriye kaydırıp yeniden uygulayın. Kanal gönderileri ve anonim yöneticilerin gönderileri gibi sohbetin kendisine atfedilen mesajlarda bu öğe görünmez."_q },
		{ u"LuminaSelectFromAuthorLimit"_q, u"Aynı anda yalnızca {1} mesaj seçilebilir."_q },
		{ u"LuminaSelectFromAuthorTitle"_q, u"Yazarın tüm mesajlarını seç"_q },
		{ u"LuminaSendOriginalCaption"_q, u"Orijinal açıklamayı gönder"_q },
		{ u"LuminaShowBookmarks"_q, u"Yer imi menü öğesini göster"_q },
		{ u"LuminaShowMessageDetails"_q, u"Mesaj ayrıntıları menüsü"_q },
		{ u"LuminaShowMutedCount"_q, u"Okunmamış sayısını her zaman göster"_q },
		{ u"LuminaShowMutedCountInfo"_q, u"Sessize alınmış sohbetlerin okunmamış rozetini sessiz gri yerine normal vurgu renginde çizer."_q },
		{ u"LuminaStickerSavedTo"_q, u"Çıkartma şuraya kaydedildi: {1}"_q },
		{ u"LuminaStickerSizeChoice"_q, u"%{1}"_q },
		{ u"LuminaStickerSizeChoiceDefault"_q, u"%{1} (varsayılan)"_q },
		{ u"LuminaStickerSizeInfo"_q, u"Çıkartmaların sohbetlerde ne kadar büyük çizileceğini belirler — gönderdikleriniz ve aldıklarınız. Hareketli emojiler, zar ve hediye çıkartmaları kendi boyutlarını korur. Telegram Desktop bir çıkartmayı bir kez ölçüp hatırlar, bu yüzden yeni boyut uygulamanın bir sonraki açılışında geçerli olur."_q },
		{ u"LuminaTimeWithSeconds"_q, u"Mesaj saatinde saniyeleri göster"_q },
		{ u"LuminaTimeWithSecondsAbout"_q, u"Mesajın altındaki saat, kopyalanan metindeki saat ve ekran okuyucunun seslendirdiği saat saniyeleri de içerir."_q },
		{ u"LuminaUndoSendBulletin"_q, u"Mesaj gönderiliyor…"_q },
		{ u"LuminaUndoSendTitle"_q, u"Göndermeyi geri al"_q },
		{ u"LuminaUndoSendUndo"_q, u"Geri al"_q },
		{ u"LuminaUndoSendWindow"_q, u"Göndermeyi geri alma penceresi"_q },
		{ u"LuminaUndoSendWindowInfo"_q, u"Düz metin mesajlarını gönderilmeden önce {1} saniye boyunca bir «Geri al» düğmesinin arkasında bekletir. Metniniz bu süre boyunca mesaj kutusunda kalır ve kutu ancak mesaj gerçekten gittiğinde temizlenir; bu yüzden «Geri al» metni olduğu yerde bırakır — hiçbir şey alınıp geri konmaz. Yeniden göndermek, başka bir sohbet açmak ya da uygulamadan çıkmak, bekletilen mesajı hemen gönderir. Medya, sesli, düzenlenmiş, iletilen ve zamanlanmış mesajlar hiçbir zaman bekletilmez; bir forum konusundan ya da bir yorum dizisinden gönderilen mesajlar da bekletilmez."_q },
		// vault + fake crash
		{ u"LuminaVaultHeader"_q, u"Gizlenme kasası"_q },
		{ u"LuminaVaultEnable"_q, u"Kasayı etkinleştir"_q },
		{ u"LuminaVaultInfo"_q, u"Buradaki her şey yalnızca bu cihazda kalır. Kasa, LuminaGram'ı zararsız görünen bir uygulamanın arkasına gizler ve gerçek uygulamaya yalnızca gizli kodla dönersiniz."_q },
		{ u"LuminaVaultMode"_q, u"Kasa modu"_q },
		{ u"LuminaVaultModePasswordDoor"_q, u"Parola kapısı"_q },
		{ u"LuminaVaultModeDecoyApp"_q, u"Sahte uygulama"_q },
		{ u"LuminaVaultModePasswordDoorInfo"_q, u"LuminaGram başlatıldığında bir parola sorulur. Gizli kod gerçek uygulamayı açar; başka her şey sahte uygulamayı açar ve yanlış olduğunu asla söylemez."_q },
		{ u"LuminaVaultModeDecoyAppInfo"_q, u"LuminaGram başlatıldığında sahte uygulama doğrudan açılır. Gerçek uygulamaya geçmek için gizli kodunuzu oraya girin."_q },
		{ u"LuminaVaultSkin"_q, u"Sahte uygulama görünümü"_q },
		{ u"LuminaVaultSkinNotepad"_q, u"Not defteri"_q },
		{ u"LuminaVaultSkinCalculator"_q, u"Hesap makinesi"_q },
		{ u"LuminaVaultSkinHintCalculator"_q, u"Hesap makinesinden açmak için gizli kodu yazın ve eşittir tuşuna basın."_q },
		{ u"LuminaVaultSkinHintNotepad"_q, u"Not defterinden açmak için notun tamamını gizli koddan oluşturun, sonra Notlar başlık çubuğuna çift tıklayın."_q },
		{ u"LuminaVaultSecretCode"_q, u"Gizli kod"_q },
		{ u"LuminaVaultSecretCodeDialogTitle"_q, u"Gizli kod"_q },
		{ u"LuminaDisguiseDecoyCodeSet"_q, u"Ayarlandı"_q },
		{ u"LuminaDisguiseDecoyCodeNotSet"_q, u"Ayarlanmadı"_q },
		{ u"LuminaVaultNoCodeInfo"_q, u"Gizli bir kod belirleyene kadar kasa kapalı kalır."_q },
		{ u"LuminaVaultCalculatorCodeInfo"_q, u"Bu kod hesap makinesi tuş takımında yazılamaz, bu yüzden onun yerine sahte not defteri gösterilir. Hesap makinesini istiyorsanız yalnızca rakamları, noktayı ve + - * / işaretlerini kullanın."_q },
		{ u"LuminaVaultDisclaimer"_q, u"Bu, omzunuzun üzerinden atılan bir bakışı engeller, bilgili bir incelemeyi değil: program görev yöneticisinde hâlâ LuminaGram olarak görünür ve bu cihazın dosyalarını okuyabilen herkes hesabınıza ulaşabilir. Kasa, LuminaGram bir sonraki açılışında görünür ve sahte uygulamayı kapatmak LuminaGram'dan çıkar. Kodu unutursanız LuminaGram veri klasörünüzden tdata/luminagram.json ve tdata/luminagram_private.json dosyalarını silin: kasa o zaman kendini kapatır ve LuminaGram normal şekilde başlar."_q },
		{ u"LuminaSecurityFakeCrashHeader"_q, u"Sahte çökme kilidi"_q },
		{ u"LuminaSecurityFakeCrashEnable"_q, u"Sahte çökme kilidini etkinleştir"_q },
		{ u"LuminaSecurityFakeCrashCode"_q, u"Sahte çökme kodu"_q },
		{ u"LuminaSecurityFakeCrashCodeDialogTitle"_q, u"Sahte çökme kodu"_q },
		{ u"LuminaSecurityFakeCrashCodeHint"_q, u"Kod"_q },
		{ u"LuminaSecurityFakeCrashCodeSet"_q, u"Ayarlandı"_q },
		{ u"LuminaSecurityFakeCrashCodeNotSet"_q, u"Ayarlanmadı"_q },
		{ u"LuminaSecurityFakeCrashCodeSameAsPasscode"_q, u"Bu kod parola kilidinizle aynı olamaz."_q },
		{ u"LuminaSecurityFakeCrashInfo"_q, u"Bu açıkken, kilit ekranında sahte çökme kodunu yazmak LuminaGram'ın kilidini açmak yerine onu hemen kapatır; hiçbir ileti gösterilmez ve ekranda hiçbir şey kalmaz — izleyen birine göre uygulama çökmüştür. Bu kod parola kilidinizden ayrıdır, yalnızca bu cihazda tutulur ve hiçbir yere gönderilmez."_q },
		{ u"LuminaSecurityFakeCrashBiometricInfo"_q, u"Bu açıkken ve bir kod ayarlıyken Touch ID, Windows Hello veya sistem parolanızla kilit açma devre dışı bırakılır: parmak izi, parola hiç sorulmadan uygulamayı açar ve bu kodun yanından geçip gider. Kodu silmek veya bunu kapatmak onları geri getirir."_q },
		{ u"LuminaSecurityFakeCrashNoCodeInfo"_q, u"Henüz bir kod ayarlanmadı, bu yüzden kilit ekranında hiçbir şey olmaz."_q },
		{ u"LuminaSecurityFakeCrashNoPasscodeInfo"_q, u"LuminaGram'da parola kilidi yok, bu yüzden kilit ekranı hiç görünmez ve bu kod hiç sorulmaz. Önce Gizlilik ve Güvenlik bölümünden parola kilidini açın."_q },
		{ u"LuminaSecurityFakeCrashCodeClashInfo"_q, u"Bu kod artık parola kilidinizle aynı, bu yüzden hiçbir zaman çalışmaz — uygulamayı parola kilidi açar. Farklı bir kod seçin."_q },
	};
}

[[maybe_unused]] const auto kRegistered = RegisterLocaleTable(
	"tr",
	Build);

} // namespace
} // namespace Lumina
