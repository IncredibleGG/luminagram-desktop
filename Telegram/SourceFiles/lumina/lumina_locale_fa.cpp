/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_locale.h"

namespace Lumina {
namespace {

[[nodiscard]] LocaleTable Build() {
	return {
		// Sections of the LuminaGram settings page.
		{ u"LuminaGramTitle"_q, u"LuminaGram"_q },
		{ u"LuminaTranslateTitle"_q, u"ترجمه"_q },
		{ u"LuminaPrivacyTitle"_q, u"حریم خصوصی"_q },
		{ u"LuminaSecurityTitle"_q, u"امنیت"_q },
		{ u"LuminaChatSettings"_q, u"گفتگوها"_q },
		{ u"LuminaGramChatList"_q, u"فهرست گفتگوها"_q },
		{ u"LuminaAppearanceTitle"_q, u"ظاهر"_q },
		{ u"LuminaToolsTitle"_q, u"ابزارها"_q },
		{ u"LuminaGramStoredLocallyInfo"_q, u"گزینه‌های LuminaGram فقط "
			u"روی همین دستگاه ذخیره می‌شوند و هرگز با Telegram همگام‌سازی "
			u"نمی‌شوند."_q },

		// Sub-pages that have no rows yet.
		{ u"LuminaAppearancePlaceholder"_q, u"گزینه‌های قالب‌بندی پیام، "
			u"استیکر و اعداد اینجا نمایش داده می‌شوند."_q },
		{ u"LuminaChatListPlaceholder"_q, u"گزینه‌های چیدمان فهرست گفتگوها "
			u"و نشان‌ها اینجا نمایش داده می‌شوند."_q },
		{ u"LuminaPrivacyPlaceholder"_q, u"گزینه‌های حریم خصوصی پیوند، "
			u"کلیپ‌بورد و رسانه‌های ارسالی اینجا نمایش داده می‌شوند."_q },
		{ u"LuminaSecurityPlaceholder"_q, u"گزینه‌های گاوصندوق استتار، "
			u"پاک‌سازی اضطراری و باز کردن تحت اجبار اینجا نمایش داده "
			u"می‌شوند."_q },
		{ u"LuminaToolsPlaceholder"_q, u"نشانک‌ها، قالب‌های پاسخ و پشتیبان "
			u"محلی اینجا نمایش داده می‌شوند."_q },

		// Chats sub-page.
		{ u"LuminaMessageActions"_q, u"کنش‌های پیام"_q },
		{ u"LuminaAllowSaveRestricted"_q,
			u"اجازهٔ ذخیره / کپی از گفتگوهای محدودشده"_q },
		{ u"LuminaAllowSaveRestrictedInfo"_q, u"«اجازهٔ ذخیره / کپی از "
			u"گفتگوهای محدودشده» فقط بر کنش‌های محلی روی همین دستگاه اثر "
			u"می‌گذارد. برخی گفتگوها ذخیره‌سازی را به دلیلی محدود می‌کنند — "
			u"مسئولانه استفاده کنید."_q },

		// Translation sub-page.
		{ u"LuminaTranslateEnable"_q, u"فعال‌سازی ترجمهٔ LuminaGram"_q },
		{ u"LuminaTranslateEnableInfo"_q, u"به‌جای سرویس Premium خودِ "
			u"Telegram، با موتور ترجمهٔ خودتان ترجمه کنید. موتور پیش‌فرض نه "
			u"به حساب کاربری نیاز دارد و نه به کلید API. تا وقتی این گزینه "
			u"خاموش است، LuminaGram ترجمه را دقیقاً همان‌گونه که "
			u"Telegram Desktop عرضه می‌کند باقی می‌گذارد."_q },
		{ u"LuminaTranslateSendHeader"_q, u"ارسال"_q },
		{ u"LuminaTranslateBeforeSend"_q, u"ترجمه پیش از ارسال"_q },
		{ u"LuminaTranslateSendLang"_q, u"زبان ارسال"_q },
		{ u"LuminaTranslateSendLangAuto"_q, u"زبان مخاطب"_q },
		{ u"LuminaTranslateBeforeSendConfirm"_q, u"تأیید پیش از ارسال"_q },
		{ u"LuminaTranslateSendInfo"_q, u"پیام‌های ارسالی به زبان بالا "
			u"ترجمه می‌شوند و متن اصلی در کنار ترجمه نگه داشته می‌شود. با "
			u"«زبان مخاطب»، LuminaGram برای هر گفتگو یک‌بار می‌پرسد که آنجا "
			u"از چه زبانی استفاده شود و سپس آن را به خاطر می‌سپارد. «تأیید "
			u"پیش از ارسال» ابتدا ترجمه را کنار متن اصلی نشان می‌دهد تا "
			u"بتوانید هر کدام را بفرستید؛ اگر خاموش باشد، ترجمه بی‌درنگ "
			u"ارسال می‌شود."_q },
		{ u"LuminaTranslateModeHeader"_q, u"ترجمهٔ پیام‌های دریافتی"_q },
		{ u"LuminaTranslateModeAll"_q, u"در همهٔ گفتگوها"_q },
		{ u"LuminaTranslateModeManual"_q,
			u"فقط گفتگوهایی که خودم روشن می‌کنم"_q },
		{ u"LuminaTranslateModeInfo"_q, u"با گزینهٔ «فقط گفتگوهایی که خودم "
			u"روشن می‌کنم»، یک گفتگو را باز کنید و دکمهٔ «ترجمه» در بالای آن "
			u"را بزنید تا برای همان گفتگو روشن شود؛ بقیهٔ گفتگوها دست‌نخورده "
			u"می‌مانند. «در همهٔ گفتگوها» برای هر پیام یک درخواست به سرویس "
			u"ترجمهٔ شما می‌فرستد — اگر کلید شما سهمیه‌ای است، آن را روی "
			u"«فقط گفتگوهایی که خودم روشن می‌کنم» بگذارید."_q },
		{ u"LuminaTranslateReceiveHeader"_q, u"دریافت"_q },
		{ u"LuminaDualLanguageDisplay"_q,
			u"نمایش هم‌زمان متن اصلی و ترجمه"_q },
		{ u"LuminaTranslateReadLang"_q, u"زبان خواندن"_q },
		{ u"LuminaTranslateReadLangFollow"_q, u"زبان برنامه"_q },
		{ u"LuminaTranslateReceiveInfo"_q, u"پیام‌های دریافتی متن اصلی خود "
			u"را در اندازهٔ کامل نگه می‌دارند و ترجمه زیر آن نمایش داده "
			u"می‌شود."_q },
		{ u"LuminaTranslateScopeHeader"_q, u"دامنهٔ اعمال"_q },
		{ u"LuminaTranslateScopePrivate"_q, u"گفتگوهای خصوصی"_q },
		{ u"LuminaTranslateScopeGroup"_q, u"گروه‌ها و کانال‌ها"_q },
		{ u"LuminaTranslateScopeInfo"_q, u"گفتگوهای خارج از این دامنه در هر "
			u"دو جهت دست‌نخورده می‌مانند: هنگام خواندن ترجمه نمی‌شوند و "
			u"پیام‌هایی که آنجا می‌فرستید بدون ترجمه ارسال می‌شوند. همچنان "
			u"می‌توانید هر پیام را به‌صورت دستی ترجمه کنید."_q },
		{ u"LuminaTranslateProviderHeader"_q, u"سرویس"_q },
		{ u"LuminaTranslateProvider"_q, u"سرویس ترجمه"_q },
		{ u"LuminaTranslateProviderLlm"_q, u"LLM (سازگار با OpenAI)"_q },
		{ u"LuminaTranslateApiKey"_q, u"کلید API"_q },
		{ u"LuminaTranslateApiKeyNotSet"_q, u"تنظیم‌نشده"_q },
		{ u"LuminaTranslateBaseUrl"_q, u"URL پایه"_q },
		{ u"LuminaTranslateModel"_q, u"مدل"_q },
		{ u"LuminaTranslateSystemPrompt"_q, u"پرامپت سیستمی"_q },
		{ u"LuminaTranslatePromptDefault"_q, u"پیش‌فرض"_q },
		{ u"LuminaTranslatePromptCustom"_q, u"سفارشی"_q },
		{ u"LuminaTranslateFallbackTelegram"_q,
			u"بازگشت به Telegram در صورت خطای این سرویس"_q },
		{ u"LuminaTranslateTest"_q, u"آزمایش ترجمه"_q },
		{ u"LuminaTranslateTestRunning"_q, u"در حال آزمایش…"_q },
		{ u"LuminaTranslateTestSuccess"_q, u"ترجمه کار می‌کند."_q },
		{ u"LuminaTranslateTestFailed"_q, u"آزمایش ناموفق بود"_q },
		{ u"LuminaTranslateNoKey"_q, u"کلید API لازم است"_q },
		{ u"LuminaTranslateTestKeyRejected"_q, u"دسترسی به سرویس ممکن نشد، "
			u"یا سرویس این کلید API را نپذیرفت. اول کلید را بررسی کنید، سپس "
			u"نشانی پایه و اتصال خود را."_q },
		{ u"LuminaTranslateTestNetwork"_q, u"دسترسی به سرویس ممکن نشد. "
			u"اتصال اینترنت و تنظیمات پروکسی خود را بررسی کنید و دوباره "
			u"تلاش کنید."_q },
		{ u"LuminaTranslateTestQuota"_q, u"سرویس این درخواست را رد کرد: "
			u"درخواست‌ها بیش از حد زیاد است، یا سهمیهٔ این کلید تمام شده "
			u"است. بعداً دوباره تلاش کنید."_q },
		{ u"LuminaTranslateTestQuotaKeyed"_q, u"سرویس این درخواست را رد کرد: "
			u"این کلید API را نپذیرفت، یا سهمیهٔ کلید تمام شده است، یا "
			u"درخواست‌ها بیش از حد زیاد بوده است. اول کلید را بررسی کنید، "
			u"سپس بعداً دوباره تلاش کنید."_q },
		{ u"LuminaTranslateTestBadResponse"_q, u"سرویس چیزی فرستاد که "
			u"LuminaGram نتوانست بخواند، یا اصلاً چیزی نفرستاد. نشانی پایه و "
			u"مدل را بررسی کنید."_q },
		{ u"LuminaTranslateTestUnavailable"_q, u"این سرویس اینجا اجرا "
			u"نمی‌شود. در بالا سرویس دیگری انتخاب کنید."_q },
		{ u"LuminaTranslateProviderSecurityInfo"_q, u"کلیدها فقط روی همین "
			u"دستگاه و در فایلی جدا از بقیهٔ تنظیمات نگهداری می‌شوند و هرگز "
			u"به Telegram فرستاده نمی‌شوند. هر چیزی که ترجمه می‌کنید به "
			u"سرویس انتخاب‌شده در اینجا ارسال می‌شود، پس سرویسی را انتخاب "
			u"کنید که به آن اعتماد دارید."_q },

		// Translate before sending: the boxes and the send menu.
		{ u"LuminaTranslateOriginalLabel"_q, u"متن اصلی"_q },
		{ u"LuminaSendTranslation"_q, u"ارسال ترجمه"_q },
		{ u"LuminaSendOriginal"_q, u"ارسال متن اصلی"_q },
		{ u"LuminaTrSendPickerTitle"_q, u"زبان ترجمهٔ پیام‌ها"_q },
		{ u"LuminaTrSendConfirmMessage"_q, u"به نظر می‌رسد این گفتگو به "
			u"زبان {1} نوشته می‌شود. پیام‌هایی که اینجا می‌فرستید به {1} "
			u"ترجمه شوند؟ LuminaGram این انتخاب را برای این گفتگو به خاطر "
			u"می‌سپارد."_q },
		{ u"LuminaTrSendConfirmTranslate"_q, u"ترجمه"_q },
		{ u"LuminaTrSendAsTyped"_q, u"ارسال بدون ترجمه"_q },
		{ u"LuminaTrSendChooseOther"_q, u"انتخاب زبان"_q },

		// The translate-before-send preview bar above the composer.
		{ u"LuminaTranslatePreviewTranslating"_q, u"در حال ترجمه…"_q },
		{ u"LuminaTranslatePreviewFailed"_q, u"ترجمه در دسترس نیست"_q },

		// Names of the languages LuminaGram can translate into.
		{ u"LuminaLangEn"_q, u"انگلیسی"_q },
		{ u"LuminaLangZhTw"_q, u"چینی (سنتی)"_q },
		{ u"LuminaLangZhCn"_q, u"چینی (ساده‌شده)"_q },
		{ u"LuminaLangJa"_q, u"ژاپنی"_q },
		{ u"LuminaLangKo"_q, u"کره‌ای"_q },
		{ u"LuminaLangEs"_q, u"اسپانیایی"_q },
		{ u"LuminaLangFr"_q, u"فرانسوی"_q },
		{ u"LuminaLangDe"_q, u"آلمانی"_q },
		{ u"LuminaLangRu"_q, u"روسی"_q },
		{ u"LuminaLangPtBr"_q, u"پرتغالی (برزیل)"_q },
		{ u"LuminaLangPtPt"_q, u"پرتغالی (پرتغال)"_q },
		{ u"LuminaLangIt"_q, u"ایتالیایی"_q },
		{ u"LuminaLangAr"_q, u"عربی"_q },
		{ u"LuminaLangHi"_q, u"هندی"_q },
		{ u"LuminaLangId"_q, u"اندونزیایی"_q },
		{ u"LuminaLangTh"_q, u"تایلندی"_q },
		{ u"LuminaLangVi"_q, u"ویتنامی"_q },
		{ u"LuminaLangTr"_q, u"ترکی"_q },
		{ u"LuminaLangPl"_q, u"لهستانی"_q },
		{ u"LuminaLangUk"_q, u"اوکراینی"_q },
		{ u"LuminaLangNl"_q, u"هلندی"_q },
		{ u"LuminaLangAf"_q, u"آفریکانس"_q },
		{ u"LuminaLangSq"_q, u"آلبانیایی"_q },
		{ u"LuminaLangAm"_q, u"امهری"_q },
		{ u"LuminaLangHy"_q, u"ارمنی"_q },
		{ u"LuminaLangAz"_q, u"آذربایجانی"_q },
		{ u"LuminaLangEu"_q, u"باسکی"_q },
		{ u"LuminaLangBe"_q, u"بلاروسی"_q },
		{ u"LuminaLangBn"_q, u"بنگالی"_q },
		{ u"LuminaLangBs"_q, u"بوسنیایی"_q },
		{ u"LuminaLangBg"_q, u"بلغاری"_q },
		{ u"LuminaLangMy"_q, u"برمه‌ای"_q },
		{ u"LuminaLangCa"_q, u"کاتالانی"_q },
		{ u"LuminaLangHr"_q, u"کرواتی"_q },
		{ u"LuminaLangCs"_q, u"چکی"_q },
		{ u"LuminaLangDa"_q, u"دانمارکی"_q },
		{ u"LuminaLangEt"_q, u"استونیایی"_q },
		{ u"LuminaLangTl"_q, u"فیلیپینی"_q },
		{ u"LuminaLangFi"_q, u"فنلاندی"_q },
		{ u"LuminaLangGl"_q, u"گالیسیایی"_q },
		{ u"LuminaLangKa"_q, u"گرجی"_q },
		{ u"LuminaLangEl"_q, u"یونانی"_q },
		{ u"LuminaLangGu"_q, u"گجراتی"_q },
		{ u"LuminaLangHe"_q, u"عبری"_q },
		{ u"LuminaLangHu"_q, u"مجاری"_q },
		{ u"LuminaLangIs"_q, u"ایسلندی"_q },
		{ u"LuminaLangGa"_q, u"ایرلندی"_q },
		{ u"LuminaLangJv"_q, u"جاوه‌ای"_q },
		{ u"LuminaLangKn"_q, u"کانارا"_q },
		{ u"LuminaLangKk"_q, u"قزاقی"_q },
		{ u"LuminaLangKm"_q, u"خمری"_q },
		{ u"LuminaLangKu"_q, u"کردی"_q },
		{ u"LuminaLangKy"_q, u"قرقیزی"_q },
		{ u"LuminaLangLo"_q, u"لائوسی"_q },
		{ u"LuminaLangLv"_q, u"لتونیایی"_q },
		{ u"LuminaLangLt"_q, u"لیتوانیایی"_q },
		{ u"LuminaLangMk"_q, u"مقدونی"_q },
		{ u"LuminaLangMs"_q, u"مالایی"_q },
		{ u"LuminaLangMl"_q, u"مالایالامی"_q },
		{ u"LuminaLangMr"_q, u"مراتی"_q },
		{ u"LuminaLangMn"_q, u"مغولی"_q },
		{ u"LuminaLangNe"_q, u"نپالی"_q },
		{ u"LuminaLangNo"_q, u"نروژی"_q },
		{ u"LuminaLangPs"_q, u"پشتو"_q },
		{ u"LuminaLangFa"_q, u"فارسی"_q },
		{ u"LuminaLangPa"_q, u"پنجابی"_q },
		{ u"LuminaLangRo"_q, u"رومانیایی"_q },
		{ u"LuminaLangSr"_q, u"صربی"_q },
		{ u"LuminaLangSi"_q, u"سینهالی"_q },
		{ u"LuminaLangSk"_q, u"اسلواکی"_q },
		{ u"LuminaLangSl"_q, u"اسلوونیایی"_q },
		{ u"LuminaLangSo"_q, u"سومالیایی"_q },
		{ u"LuminaLangSw"_q, u"سواحیلی"_q },
		{ u"LuminaLangSv"_q, u"سوئدی"_q },
		{ u"LuminaLangTg"_q, u"تاجیکی"_q },
		{ u"LuminaLangTa"_q, u"تامیلی"_q },
		{ u"LuminaLangTe"_q, u"تلوگویی"_q },
		{ u"LuminaLangUr"_q, u"اردو"_q },
		{ u"LuminaLangUz"_q, u"ازبکی"_q },
		{ u"LuminaLangCy"_q, u"ولزی"_q },
		{ u"LuminaLangYi"_q, u"ییدیش"_q },
		{ u"LuminaLangZu"_q, u"زولویی"_q },
	};
}

[[maybe_unused]] const auto kRegistered = RegisterLocaleTable("fa", Build);

} // namespace
} // namespace Lumina
