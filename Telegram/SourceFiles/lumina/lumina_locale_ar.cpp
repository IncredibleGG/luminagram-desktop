/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_locale.h"

namespace Lumina {
namespace {

// Arabic is written right-to-left, so a text that names a value the user picked
// reads better with the placeholder late in the clause; where the sentence has
// to repeat the language name, "{1}" is simply written twice.
//
// Quoted row labels use the Arabic quotation marks «...», which is what the
// Arabic Telegram UI uses; the words inside them are the same words as the rows
// they quote, which is what has to match.
[[nodiscard]] LocaleTable Build() {
	return {
		// Sections of the LuminaGram settings page.
		{ u"LuminaGramTitle"_q, u"LuminaGram"_q },
		{ u"LuminaTranslateTitle"_q, u"الترجمة"_q },
		{ u"LuminaPrivacyTitle"_q, u"الخصوصية"_q },
		{ u"LuminaSecurityTitle"_q, u"الأمان"_q },
		{ u"LuminaChatSettings"_q, u"الدردشات"_q },
		{ u"LuminaGramChatList"_q, u"قائمة الدردشات"_q },
		{ u"LuminaAppearanceTitle"_q, u"المظهر"_q },
		{ u"LuminaToolsTitle"_q, u"الأدوات"_q },
		{ u"LuminaGramStoredLocallyInfo"_q,
			u"تُحفظ خيارات LuminaGram على هذا الجهاز فقط، ولا تتم مزامنتها مع "
			u"Telegram أبدًا."_q },

		// Sub-pages that have no rows yet.
		{ u"LuminaAppearancePlaceholder"_q,
			u"ستظهر هنا خيارات تنسيق الرسائل والملصقات والأرقام."_q },
		{ u"LuminaChatListPlaceholder"_q,
			u"ستظهر هنا خيارات تخطيط قائمة الدردشات والشارات."_q },
		{ u"LuminaPrivacyPlaceholder"_q,
			u"ستظهر هنا خيارات الخصوصية الخاصة بالروابط والحافظة والوسائط "
			u"الصادرة."_q },
		{ u"LuminaSecurityPlaceholder"_q,
			u"ستظهر هنا خيارات خزنة التمويه والمحو الطارئ والفتح تحت "
			u"الإكراه."_q },
		{ u"LuminaToolsPlaceholder"_q,
			u"ستظهر هنا الإشارات المرجعية وقوالب الردود والنسخ الاحتياطي "
			u"المحلي."_q },

		// Chats sub-page.
		{ u"LuminaMessageActions"_q, u"إجراءات الرسائل"_q },
		{ u"LuminaAllowSaveRestricted"_q,
			u"السماح بالحفظ / النسخ من الدردشات المقيَّدة"_q },
		{ u"LuminaAllowSaveRestrictedInfo"_q,
			u"لا يؤثر خيار «السماح بالحفظ / النسخ من الدردشات المقيَّدة» إلا "
			u"على الإجراءات المحلية على هذا الجهاز. بعض الدردشات تُقيّد الحفظ "
			u"لسبب وجيه — استخدمه بمسؤولية."_q },

		// Translation sub-page.
		{ u"LuminaTranslateEnable"_q, u"تفعيل ترجمة LuminaGram"_q },
		{ u"LuminaTranslateEnableInfo"_q,
			u"ترجم باستخدام محرّك الترجمة الخاص بك بدلاً من خدمة Telegram "
			u"Premium. المحرّك الافتراضي لا يحتاج إلى حساب ولا إلى مفتاح API. "
			u"وما دام هذا الخيار متوقفًا، يترك LuminaGram الترجمة كما تأتي في "
			u"Telegram Desktop تمامًا."_q },
		{ u"LuminaTranslateSendHeader"_q, u"الإرسال"_q },
		{ u"LuminaTranslateBeforeSend"_q, u"الترجمة قبل الإرسال"_q },
		{ u"LuminaTranslateSendLang"_q, u"لغة الإرسال"_q },
		{ u"LuminaTranslateSendLangAuto"_q, u"لغة المُستقبِل"_q },
		{ u"LuminaTranslateBeforeSendConfirm"_q, u"التأكيد قبل الإرسال"_q },
		{ u"LuminaTranslateSendInfo"_q,
			u"تُترجَم الرسائل الصادرة إلى اللغة المحددة أعلاه، ويُحتفظ بالنص "
			u"الأصلي إلى جانب الترجمة. وعند اختيار «لغة المُستقبِل» يسألك "
			u"LuminaGram مرة واحدة في كل دردشة عن اللغة التي يستخدمها فيها، ثم "
			u"يتذكّرها. ويعرض خيار «التأكيد قبل الإرسال» الترجمة بجانب النص "
			u"الأصلي أولاً، لتتمكن من إرسال أيّهما شئت؛ وعند إيقافه تُرسَل "
			u"الترجمة مباشرةً."_q },
		{ u"LuminaTranslateReceiveHeader"_q, u"الاستقبال"_q },
		{ u"LuminaDualLanguageDisplay"_q,
			u"عرض النص الأصلي والترجمة معًا"_q },
		{ u"LuminaTranslateReadLang"_q, u"لغة القراءة"_q },
		{ u"LuminaTranslateReadLangFollow"_q, u"لغة الواجهة"_q },
		{ u"LuminaTranslateModeHeader"_q, u"ترجمة الرسائل الواردة"_q },
		{ u"LuminaTranslateModeAll"_q, u"في كل الدردشات"_q },
		{ u"LuminaTranslateModeManual"_q, u"الدردشات التي أُفعّلها فقط"_q },
		{ u"LuminaTranslateReceiveInfo"_q,
			u"تحتفظ الرسائل الواردة بنصها الأصلي بالحجم الكامل، مع عرض الترجمة "
			u"تحته. ويرسل خيار «في كل الدردشات» طلبًا واحدًا لكل رسالة إلى "
			u"خدمة الترجمة لديك — فإذا كان مفتاحك محدود الاستخدام، فأبقِه على "
			u"«الدردشات التي أُفعّلها فقط»."_q },
		{ u"LuminaTranslateScopeHeader"_q, u"النطاق"_q },
		{ u"LuminaTranslateScopePrivate"_q, u"الدردشات الخاصة"_q },
		{ u"LuminaTranslateScopeGroup"_q, u"المجموعات والقنوات"_q },
		{ u"LuminaTranslateScopeInfo"_q,
			u"الدردشات خارج النطاق لا تتأثر في الاتجاهين: فلا تُترجَم أثناء "
			u"قراءتك لها، والرسائل التي ترسلها فيها تخرج دون ترجمة. ويظل "
			u"بإمكانك ترجمة أي رسالة بمفردها يدويًا."_q },
		{ u"LuminaTranslateProviderHeader"_q, u"الخدمة"_q },
		{ u"LuminaTranslateProvider"_q, u"خدمة الترجمة"_q },
		// The other provider names are the services' own brands and are not
		// translated; this one names a kind of endpoint.
		{ u"LuminaTranslateProviderLlm"_q, u"LLM (متوافق مع OpenAI)"_q },
		{ u"LuminaTranslateApiKey"_q, u"مفتاح API"_q },
		{ u"LuminaTranslateApiKeyNotSet"_q, u"غير محدد"_q },
		{ u"LuminaTranslateBaseUrl"_q, u"عنوان URL الأساسي"_q },
		{ u"LuminaTranslateModel"_q, u"النموذج"_q },
		{ u"LuminaTranslateSystemPrompt"_q, u"توجيه النظام"_q },
		{ u"LuminaTranslatePromptDefault"_q, u"افتراضي"_q },
		{ u"LuminaTranslatePromptCustom"_q, u"مخصص"_q },
		{ u"LuminaTranslateFallbackTelegram"_q,
			u"الرجوع إلى Telegram عند فشل هذه الخدمة"_q },
		{ u"LuminaTranslateProviderSecurityInfo"_q,
			u"تُحفظ المفاتيح على هذا الجهاز فقط، في ملف منفصل عن بقية "
			u"الإعدادات، ولا تُرسَل إلى Telegram أبدًا. وكل ما تترجمه يُرسَل "
			u"إلى الخدمة المحددة هنا، فاختر خدمة تثق بها."_q },

		// Translate before sending: the boxes and the send menu.
		{ u"LuminaTranslateOriginalLabel"_q, u"النص الأصلي"_q },
		{ u"LuminaSendTranslation"_q, u"إرسال الترجمة"_q },
		{ u"LuminaSendOriginal"_q, u"إرسال النص الأصلي"_q },
		{ u"LuminaTrSendPickerTitle"_q, u"ترجمة الرسائل إلى"_q },
		{ u"LuminaTrSendConfirmMessage"_q,
			u"يبدو أن هذه الدردشة مكتوبة بـ {1}. هل تريد ترجمة الرسائل التي "
			u"ترسلها هنا إلى {1}؟ سيتذكّر LuminaGram هذا الاختيار لهذه "
			u"الدردشة."_q },
		{ u"LuminaTrSendConfirmTranslate"_q, u"ترجمة"_q },
		{ u"LuminaTrSendAsTyped"_q, u"إرسال كما كُتبت"_q },
		{ u"LuminaTrSendChooseOther"_q, u"اختيار لغة أخرى"_q },

		// The translate-before-send preview bar above the composer.
		{ u"LuminaTranslatePreviewTranslating"_q, u"جارٍ الترجمة…"_q },
		{ u"LuminaTranslatePreviewFailed"_q, u"الترجمة غير متاحة"_q },

		// Names of the languages LuminaGram can translate into, in the order
		// the pickers show them.
		{ u"LuminaLangEn"_q, u"الإنجليزية"_q },
		{ u"LuminaLangZhTw"_q, u"الصينية (التقليدية)"_q },
		{ u"LuminaLangZhCn"_q, u"الصينية (المبسّطة)"_q },
		{ u"LuminaLangJa"_q, u"اليابانية"_q },
		{ u"LuminaLangKo"_q, u"الكورية"_q },
		{ u"LuminaLangEs"_q, u"الإسبانية"_q },
		{ u"LuminaLangFr"_q, u"الفرنسية"_q },
		{ u"LuminaLangDe"_q, u"الألمانية"_q },
		{ u"LuminaLangRu"_q, u"الروسية"_q },
		{ u"LuminaLangPtBr"_q, u"البرتغالية (البرازيل)"_q },
		{ u"LuminaLangPtPt"_q, u"البرتغالية (البرتغال)"_q },
		{ u"LuminaLangIt"_q, u"الإيطالية"_q },
		{ u"LuminaLangAr"_q, u"العربية"_q },
		{ u"LuminaLangHi"_q, u"الهندية"_q },
		{ u"LuminaLangId"_q, u"الإندونيسية"_q },
		{ u"LuminaLangTh"_q, u"التايلاندية"_q },
		{ u"LuminaLangVi"_q, u"الفيتنامية"_q },
		{ u"LuminaLangTr"_q, u"التركية"_q },
		{ u"LuminaLangPl"_q, u"البولندية"_q },
		{ u"LuminaLangUk"_q, u"الأوكرانية"_q },
		{ u"LuminaLangNl"_q, u"الهولندية"_q },
		{ u"LuminaLangAf"_q, u"الأفريكانية"_q },
		{ u"LuminaLangSq"_q, u"الألبانية"_q },
		{ u"LuminaLangAm"_q, u"الأمهرية"_q },
		{ u"LuminaLangHy"_q, u"الأرمنية"_q },
		{ u"LuminaLangAz"_q, u"الأذربيجانية"_q },
		{ u"LuminaLangEu"_q, u"الباسكية"_q },
		{ u"LuminaLangBe"_q, u"البيلاروسية"_q },
		{ u"LuminaLangBn"_q, u"البنغالية"_q },
		{ u"LuminaLangBs"_q, u"البوسنية"_q },
		{ u"LuminaLangBg"_q, u"البلغارية"_q },
		{ u"LuminaLangMy"_q, u"البورمية"_q },
		{ u"LuminaLangCa"_q, u"الكتالونية"_q },
		{ u"LuminaLangHr"_q, u"الكرواتية"_q },
		{ u"LuminaLangCs"_q, u"التشيكية"_q },
		{ u"LuminaLangDa"_q, u"الدنماركية"_q },
		{ u"LuminaLangEt"_q, u"الإستونية"_q },
		{ u"LuminaLangTl"_q, u"الفلبينية"_q },
		{ u"LuminaLangFi"_q, u"الفنلندية"_q },
		{ u"LuminaLangGl"_q, u"الجاليكية"_q },
		{ u"LuminaLangKa"_q, u"الجورجية"_q },
		{ u"LuminaLangEl"_q, u"اليونانية"_q },
		{ u"LuminaLangGu"_q, u"الغوجاراتية"_q },
		{ u"LuminaLangHe"_q, u"العبرية"_q },
		{ u"LuminaLangHu"_q, u"المجرية"_q },
		{ u"LuminaLangIs"_q, u"الآيسلندية"_q },
		{ u"LuminaLangGa"_q, u"الأيرلندية"_q },
		{ u"LuminaLangJv"_q, u"الجاوية"_q },
		{ u"LuminaLangKn"_q, u"الكانادية"_q },
		{ u"LuminaLangKk"_q, u"الكازاخية"_q },
		{ u"LuminaLangKm"_q, u"الخميرية"_q },
		{ u"LuminaLangKu"_q, u"الكردية"_q },
		{ u"LuminaLangKy"_q, u"القيرغيزية"_q },
		{ u"LuminaLangLo"_q, u"اللاوية"_q },
		{ u"LuminaLangLv"_q, u"اللاتفية"_q },
		{ u"LuminaLangLt"_q, u"الليتوانية"_q },
		{ u"LuminaLangMk"_q, u"المقدونية"_q },
		{ u"LuminaLangMs"_q, u"الملايوية"_q },
		{ u"LuminaLangMl"_q, u"المالايالامية"_q },
		{ u"LuminaLangMr"_q, u"المهاراتية"_q },
		{ u"LuminaLangMn"_q, u"المنغولية"_q },
		{ u"LuminaLangNe"_q, u"النيبالية"_q },
		{ u"LuminaLangNo"_q, u"النرويجية"_q },
		{ u"LuminaLangPs"_q, u"البشتوية"_q },
		{ u"LuminaLangFa"_q, u"الفارسية"_q },
		{ u"LuminaLangPa"_q, u"البنجابية"_q },
		{ u"LuminaLangRo"_q, u"الرومانية"_q },
		{ u"LuminaLangSr"_q, u"الصربية"_q },
		{ u"LuminaLangSi"_q, u"السنهالية"_q },
		{ u"LuminaLangSk"_q, u"السلوفاكية"_q },
		{ u"LuminaLangSl"_q, u"السلوفينية"_q },
		{ u"LuminaLangSo"_q, u"الصومالية"_q },
		{ u"LuminaLangSw"_q, u"السواحيلية"_q },
		{ u"LuminaLangSv"_q, u"السويدية"_q },
		{ u"LuminaLangTg"_q, u"الطاجيكية"_q },
		{ u"LuminaLangTa"_q, u"التاميلية"_q },
		{ u"LuminaLangTe"_q, u"التيلوغوية"_q },
		{ u"LuminaLangUr"_q, u"الأردية"_q },
		{ u"LuminaLangUz"_q, u"الأوزبكية"_q },
		{ u"LuminaLangCy"_q, u"الويلزية"_q },
		{ u"LuminaLangYi"_q, u"اليديشية"_q },
		{ u"LuminaLangZu"_q, u"الزولوية"_q },
	};
}

[[maybe_unused]] const auto kRegistered = RegisterLocaleTable("ar", Build);

} // namespace
} // namespace Lumina
