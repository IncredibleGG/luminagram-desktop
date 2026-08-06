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
		{ u"LuminaTranslateModeHeader"_q, u"Gelen mesajları çevir"_q },
		{ u"LuminaTranslateModeAll"_q, u"Her sohbette"_q },
		{ u"LuminaTranslateModeManual"_q, u"Yalnızca açtığım sohbetler"_q },
		{ u"LuminaTranslateModeInfo"_q, u"«Yalnızca açtığım sohbetler» "
			u"seçiliyken bir sohbeti açın ve üstündeki «Çevir» düğmesine "
			u"basarak o sohbette çeviriyi açın; diğer sohbetlere "
			u"dokunulmaz. «Her sohbette» seçeneği çeviri hizmetinize mesaj "
			u"başına bir istek gönderir; kotalı bir anahtar kullanıyorsanız "
			u"«Yalnızca açtığım sohbetler» seçeneğinde bırakın."_q },
		{ u"LuminaTranslateReceiveHeader"_q, u"Alma"_q },
		{ u"LuminaDualLanguageDisplay"_q,
			u"Orijinali ve çeviriyi birlikte göster"_q },
		{ u"LuminaTranslateReadLang"_q, u"Okuma dili"_q },
		{ u"LuminaTranslateReadLangFollow"_q, u"Arayüz dili"_q },
		{ u"LuminaTranslateReceiveInfo"_q, u"Gelen mesajlar orijinal metnini "
			u"tam boyutta korur, çeviri ise altında gösterilir."_q },
		{ u"LuminaTranslateScopeHeader"_q, u"Kapsam"_q },
		{ u"LuminaTranslateScopePrivate"_q, u"Özel sohbetler"_q },
		{ u"LuminaTranslateScopeGroup"_q, u"Gruplar ve kanallar"_q },
		{ u"LuminaTranslateScopeInfo"_q, u"Kapsam dışındaki sohbetlere her "
			u"iki yönde de dokunulmaz: okurken çevrilmezler ve oraya "
			u"gönderdiğiniz mesajlar çevrilmeden gider. Yine de herhangi bir "
			u"mesajı elle çevirebilirsiniz."_q },
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
	};
}

[[maybe_unused]] const auto kRegistered = RegisterLocaleTable(
	"tr",
	Build);

} // namespace
} // namespace Lumina
