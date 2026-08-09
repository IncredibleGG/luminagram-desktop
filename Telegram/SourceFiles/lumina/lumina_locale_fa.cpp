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
		{ u"LuminaTranslateReceiveHeader"_q, u"دریافت"_q },
		{ u"LuminaDualLanguageDisplay"_q,
			u"نمایش هم‌زمان متن اصلی و ترجمه"_q },
		{ u"LuminaTranslateReadLang"_q, u"زبان خواندن"_q },
		{ u"LuminaTranslateReadLangFollow"_q, u"زبان برنامه"_q },
		{ u"LuminaTranslateReceiveInfo"_q, u"پیام‌های دریافتی متن اصلی خود "
			u"را در اندازهٔ کامل نگه می‌دارند و ترجمه زیر آن نمایش داده "
			u"می‌شود."_q },
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

		// The per-chat translate button in the chat top bar.
		{ u"LuminaTranslateChatToggle"_q, u"ترجمهٔ این گفتگو"_q },
		{ u"LuminaTranslateChatShowOriginal"_q, u"نمایش متن اصلی"_q },
		// The two rows that button opens. Each names its side first - the
		// person on the other end, then the user - because "incoming" and
		// "outgoing" never said whose messages were meant. A row that is on
		// ends in the language it translates into; a row that is off is a
		// string of its own, so no row ever says "translated into" and then
		// names something that is not a language.
		{ u"LuminaChatLangThem"_q,
			u"پیام‌های طرف مقابل، ترجمه‌شده به {1}"_q },
		{ u"LuminaChatLangThemOff"_q, u"پیام‌های طرف مقابل، ترجمه‌نشده"_q },
		{ u"LuminaChatLangMe"_q, u"پیام‌های من، ترجمه‌شده به {1}"_q },
		{ u"LuminaChatLangMeOff"_q, u"پیام‌های من، ترجمه‌نشده"_q },
		{ u"LuminaChatLangThemTitle"_q, u"زبان ترجمهٔ پیام‌های طرف مقابل"_q },
		{ u"LuminaChatLangMeTitle"_q, u"زبان ترجمهٔ پیام‌های من"_q },
		{ u"LuminaChatLangNone"_q, u"ترجمه نمی‌شود"_q },

		// Telegram's own AI editor, which carries a Translate tab of its
		// own.
		{ u"LuminaAiEditorHeader"_q, u"ویرایشگر هوش مصنوعی Telegram"_q },
		{ u"LuminaAiEditorKeep"_q,
			u"نگه داشتن ویرایشگر هوش مصنوعی Telegram"_q },
		{ u"LuminaAiEditorInfo"_q, u"Telegram در کادر پیام ویرایشگر "
			u"هوش مصنوعی خودش را دارد که زبانهٔ «ترجمه» آن با ترجمهٔ "
			u"LuminaGram هم‌پوشانی دارد. تا وقتی ترجمهٔ LuminaGram روشن "
			u"است، دکمه و میان‌بر صفحه‌کلید آن پیشنهاد نمی‌شود، پس همیشه "
			u"فقط یک ابزار ترجمه پیش روی شماست. این گزینه را روشن کنید تا "
			u"ویرایشگر Telegram با این حال در دسترس بماند. وقتی ترجمهٔ "
			u"LuminaGram خاموش است، ویرایشگر Telegram همیشه سر جای خود است "
			u"و این گزینه هیچ چیزی را تغییر نمی‌دهد."_q },

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
		{ u"LuminaAppearanceStickerSizeHeader"_q, u"اندازهٔ استیکر"_q },
		{ u"LuminaBackupCryptoFailed"_q, u"رمزگذاری روی این سیستم در دسترس نیست."_q },
		{ u"LuminaBackupDamaged"_q, u"فایل پشتیبان آسیب دیده است و بازیابی نشد."_q },
		{ u"LuminaBackupExport"_q, u"خروجی گرفتن پشتیبان رمزگذاری‌شده"_q },
		{ u"LuminaBackupExportDone"_q, u"پشتیبان ذخیره شد."_q },
		{ u"LuminaBackupExportFailed"_q, u"ایجاد فایل پشتیبان ممکن نشد."_q },
		{ u"LuminaBackupExportInfo"_q, u"هر چیزی که LuminaGram روی این دستگاه نگه می‌دارد — نشانک‌ها، یادداشت‌ها، قالب‌های پاسخ، جایگزینی‌های متن و همهٔ تنظیمات — در یک فایل ذخیره می‌شود که با عبارت عبور انتخابی خودتان رمزگذاری شده است. این فایل مقادیر خصوصی را هم در بر دارد: کلیدهای API ترجمه، کدهای گاوصندوق و خرابی جعلی، و یادداشت فریبنده. چیزی به Telegram فرستاده نمی‌شود. عبارت عبوری بلند انتخاب کنید و آن را جایی امن نگه دارید — بدون آن فایل باز نمی‌شود و هیچ راهی برای بازیابی‌اش وجود ندارد."_q },
		{ u"LuminaBackupExportPassphraseTitle"_q, u"تعیین عبارت عبور"_q },
		{ u"LuminaBackupFileFilter"_q, u"پشتیبان LuminaGram (*.lgbak)"_q },
		{ u"LuminaBackupImport"_q, u"وارد کردن پشتیبان"_q },
		{ u"LuminaBackupImportFailed"_q, u"خواندن فایل پشتیبان ممکن نشد."_q },
		{ u"LuminaBackupImportInfo"_q, u"یک فایل پشتیبان انتخاب کنید و عبارت عبور آن را وارد کنید تا داده‌های LuminaGram بازیابی شود. تنظیماتی که پشتیبان در بر دارد جایگزین تنظیمات این دستگاه می‌شوند؛ هر چیزی که در آن نباشد دست‌نخورده می‌ماند. فایلی که نتوان آن را تأیید کرد، پیش از نوشتن هر چیزی رد می‌شود، پس یک عبارت عبور نادرست هرگز شما را با بازیابی نیمه‌کاره رها نمی‌کند."_q },
		{ u"LuminaBackupImportSuccess"_q, u"پشتیبان بازیابی شد. برای اعمال همهٔ تغییرات، LuminaGram را دوباره راه‌اندازی کنید."_q },
		{ u"LuminaBackupInvalidFile"_q, u"این یک فایل پشتیبان معتبر LuminaGram نیست."_q },
		{ u"LuminaBackupNewerFormat"_q, u"این پشتیبان با نسخهٔ جدیدتری از LuminaGram ساخته شده است."_q },
		{ u"LuminaBackupOpenCaption"_q, u"باز کردن پشتیبان LuminaGram"_q },
		{ u"LuminaBackupPassphraseHint"_q, u"عبارت عبور"_q },
		{ u"LuminaBackupPassphraseMismatch"_q, u"دو عبارت عبور یکسان نیستند."_q },
		{ u"LuminaBackupPassphraseRepeatHint"_q, u"تکرار عبارت عبور"_q },
		{ u"LuminaBackupPassphraseTitle"_q, u"عبارت عبور را وارد کنید"_q },
		{ u"LuminaBackupPassphraseTooShort"_q, u"عبارت عبوری با حداقل ۴ نویسه انتخاب کنید."_q },
		{ u"LuminaBackupSaveCaption"_q, u"ذخیرهٔ پشتیبان LuminaGram"_q },
		{ u"LuminaBackupTitle"_q, u"پشتیبان رمزگذاری‌شده"_q },
		{ u"LuminaBackupUnauthenticated"_q, u"این پشتیبان از قالب قدیمی و محافظت‌نشده استفاده می‌کند که نمی‌توان آن را از نظر عبارت عبور نادرست یا دستکاری بررسی کرد. با یک LuminaGram به‌روز، پشتیبان تازه‌ای بگیرید."_q },
		{ u"LuminaBackupWrongPassphrase"_q, u"عبارت عبور نادرست است، یا فایل تغییر کرده است."_q },
		{ u"LuminaBookmark"_q, u"افزودن نشانک"_q },
		{ u"LuminaBookmarkAdded"_q, u"به نشانک‌ها افزوده شد"_q },
		{ u"LuminaBookmarkChatUnavailable"_q, u"گفتگو در دسترس نیست"_q },
		{ u"LuminaBookmarkDeleteTitle"_q, u"حذف نشانک؟"_q },
		{ u"LuminaBookmarkGone"_q, u"این گفتگو دیگر روی این دستگاه در دسترس نیست."_q },
		{ u"LuminaBookmarkRemove"_q, u"حذف نشانک"_q },
		{ u"LuminaBookmarkRemoved"_q, u"از نشانک‌ها حذف شد"_q },
		{ u"LuminaBookmarksAbout"_q, u"نشانک‌ها اشاره‌گرهایی به پیام‌ها هستند که فقط روی همین دستگاه نگهداری می‌شوند و هرگز به Telegram فرستاده نمی‌شوند. حذف یک نشانک، خودِ پیام را دست‌نخورده باقی می‌گذارد."_q },
		{ u"LuminaBookmarksEmpty"_q, u"هنوز هیچ پیامی را نشانک نکرده‌اید."_q },
		{ u"LuminaBookmarksFull"_q, u"فهرست نشانک‌ها پر است. برای افزودن نشانک تازه، یکی را حذف کنید."_q },
		{ u"LuminaBookmarksList"_q, u"پیام‌های نشانک‌شده"_q },
		{ u"LuminaBookmarksListAbout"_q, u"برای باز کردن پیام روی یک نشانک کلیک کنید، یا برای حذف آن راست‌کلیک کنید. نشانک حتی پس از حذف پیامش در این فهرست باقی می‌ماند."_q },
		{ u"LuminaBookmarksNone"_q, u"هیچ"_q },
		{ u"LuminaBookmarksTitle"_q, u"نشانک‌ها"_q },
		{ u"LuminaChatListDensityTitle"_q, u"تراکم فهرست گفتگوها"_q },
		{ u"LuminaCompactListRows"_q, u"ردیف‌های فشرده"_q },
		{ u"LuminaCompactListRowsInfo"_q, u"با کوتاه‌کردن هر ردیف فهرست گفتگوها، گفتگوهای بیشتری در صفحه جا می‌شوند. تصویر نمایه و پیش‌نمایش پیام‌ها همچنان دیده می‌شوند."_q },
		{ u"LuminaChatListDotsTitle"_q, u"نقطه‌های تصویر نمایه"_q },
		{ u"LuminaChatListOnlineDot"_q, u"نقطهٔ آنلاین"_q },
		{ u"LuminaChatListOnlineDotInfo"_q, u"یک نقطهٔ سبز کوچک روی تصویر نمایهٔ گفتگوهای خصوصی که طرف مقابل هم‌اکنون آنلاین است نمایش می‌دهد."_q },
		{ u"LuminaChatListRecencyDot"_q, u"نقطهٔ آخرین بازدید"_q },
		{ u"LuminaChatListRecencyDotInfo"_q, u"نقطهٔ روی تصویر نمایهٔ گفتگوهای خصوصی را بر اساس اینکه طرف مقابل چند وقت پیش آنلاین بوده رنگ می‌کند: سبز اگر هم‌اکنون آنلاین باشد، زرد در یک ساعت گذشته و نارنجی در یک روز گذشته. برای زمان‌های قدیمی‌تر یا پنهان، نقطه‌ای نمایش داده نمی‌شود. حالت سبز (آنلاین) همچنان با کلید «نقطهٔ آنلاین» کنترل می‌شود."_q },
		{ u"LuminaChatListVisibilityAbout"_q, u"نوار پوشه‌ها را از فهرست گفتگوها بردارید — هم نوار عمودی کنار آن و هم نوار افقی بالای آن. تا وقتی زبانه‌های پوشه پنهان است، همیشه همهٔ گفتگوهای خود را می‌بینید: از هر پوشه‌ای که در آن بودید خارج می‌شوید، و میان‌برهای پوشه و کشیدن میان پوشه‌ها کاری نمی‌کنند. پنهان کردن استوری‌ها فقط ردیف بالای فهرست گفتگوها را برمی‌دارد؛ خودِ استوری‌ها و هر راه دیگر باز کردنشان بدون تغییر می‌ماند."_q },
		{ u"LuminaChatListVisibilityTitle"_q, u"پوشه‌ها و استوری‌ها"_q },
		{ u"LuminaContactNote"_q, u"یادداشت خصوصی"_q },
		{ u"LuminaContactNoteAbout"_q, u"فقط شما این را می‌بینید. روی همین دستگاه می‌ماند، هرگز به Telegram فرستاده نمی‌شود و با دستگاه‌های دیگر شما همگام‌سازی نمی‌شود."_q },
		{ u"LuminaContactNoteEmpty"_q, u"برای افزودن یادداشت خصوصی کلیک کنید"_q },
		{ u"LuminaContactNoteHint"_q, u"یادداشت (فقط شما می‌بینید)"_q },
		{ u"LuminaContactNotesAbout"_q, u"یک یادداشت خصوصی و فهرستی از برچسب‌ها به نمایهٔ هر شخص می‌افزاید. هر دو روی همین دستگاه می‌مانند، هرگز به Telegram فرستاده نمی‌شوند و با دستگاه‌های دیگر شما همگام‌سازی نمی‌شوند. یادداشت‌های مخاطبِ خودِ Telegram دست‌نخورده می‌مانند: یادداشت خصوصی جایی پیشنهاد می‌شود که آن‌ها در دسترس نیستند — روی ربات‌ها و روی کسانی که در مخاطبان شما نیستند — در حالی که برچسب‌ها همه‌جا پیشنهاد می‌شوند."_q },
		{ u"LuminaContactNotesClear"_q, u"حذف همهٔ یادداشت‌های خصوصی"_q },
		{ u"LuminaContactNotesClearText"_q, u"همهٔ یادداشت‌ها و برچسب‌های خصوصی ذخیره‌شده روی این دستگاه حذف شوند؟ این کار قابل بازگشت نیست."_q },
		{ u"LuminaContactNotesClearTitle"_q, u"حذف یادداشت‌های خصوصی"_q },
		{ u"LuminaContactNotesNone"_q, u"هیچ"_q },
		{ u"LuminaContactNotesTitle"_q, u"یادداشت‌های خصوصی مخاطبان"_q },
		{ u"LuminaContactNotesToggle"_q, u"یادداشت‌ها و برچسب‌های خصوصی در نمایه‌ها"_q },
		{ u"LuminaContactTags"_q, u"برچسب‌ها"_q },
		{ u"LuminaContactTagsEmpty"_q, u"برای افزودن برچسب کلیک کنید"_q },
		{ u"LuminaContactTagsHint"_q, u"برچسب‌ها، جدا شده با کاما"_q },
		{ u"LuminaDetailsDate"_q, u"تاریخ"_q },
		{ u"LuminaDetailsForwardedFrom"_q, u"هدایت‌شده از"_q },
		{ u"LuminaDetailsFrom"_q, u"از"_q },
		{ u"LuminaDetailsMessageId"_q, u"شناسهٔ پیام"_q },
		{ u"LuminaDetailsOriginalDate"_q, u"تاریخ اصلی"_q },
		{ u"LuminaExactNumbers"_q, u"نمایش اعداد دقیق (بدون خلاصه‌سازی 1.2K)"_q },
		{ u"LuminaExactNumbersInfo"_q, u"نمایش اعداد کامل مانند 1,234,567 به‌جای شکل‌های خلاصه مانند 1.2M. شمارنده‌هایی که همین حالا روی صفحه رسم شده‌اند تا راه‌اندازی مجدد برنامه شکل فعلی خود را نگه می‌دارند."_q },
		{ u"LuminaForwardNoAuthor"_q, u"هدایت بدون نویسنده"_q },
		{ u"LuminaForwardNoAuthorTitle"_q, u"هدایت بدون نویسنده"_q },
		{ u"LuminaForwardNoCaption"_q, u"هدایت بدون نویسنده و شرح"_q },
		{ u"LuminaForwardNoCaptionTitle"_q, u"هدایت بدون نویسنده و شرح"_q },
		{ u"LuminaHideStories"_q, u"پنهان کردن استوری‌ها"_q },
		{ u"LuminaHideTabs"_q, u"پنهان کردن زبانه‌های پوشه"_q },
		{ u"LuminaLinkSafetyDestination"_q, u"مقصد واقعی"_q },
		{ u"LuminaLinkSafetyInfo"_q, u"پیش از باز کردن پیوندی که مقصد واقعی خود را پشت متنِ پیش از نشانهٔ @ پنهان می‌کند، دامنه‌اش را با punycode می‌نویسد، یا از یک کوتاه‌کنندهٔ پیوند شناخته‌شده می‌گذرد، تأیید می‌گیرد. در پنجرهٔ تأیید، میزبان مقصد واقعی و نشانی کامل نشان داده می‌شود. Telegram Desktop خودش دربارهٔ دامنه‌های شبیه‌سازی‌شده که با الفبای دیگری نوشته شده‌اند هشدار می‌دهد. هیچ چیزی به‌صورت آنلاین بررسی نمی‌شود — هیچ نشانی‌ای که باز می‌کنید از این دستگاه بیرون نمی‌رود."_q },
		{ u"LuminaLinkSafetyRow"_q, u"بازرس ایمنی پیوندها"_q },
		{ u"LuminaLinkSafetyTitle"_q, u"پیوند بیرونی باز شود؟"_q },
		{ u"LuminaLinkSafetyWarnMismatch"_q, u"این پیوند مقصد واقعی خود را پشت متنِ پیش از نشانهٔ «@» پنهان می‌کند."_q },
		{ u"LuminaLinkSafetyWarnPunycode"_q, u"این نشانی از نویسه‌های کدگذاری‌شده (punycode) استفاده می‌کند که می‌تواند یک سایت شناخته‌شده را تقلید کند."_q },
		{ u"LuminaLinkSafetyWarnShortener"_q, u"این یک کوتاه‌کنندهٔ پیوند است — مقصد واقعی تا زمانی که آن را باز نکنید پنهان می‌ماند."_q },
		{ u"LuminaMediaAutoPauseBgVideo"_q, u"توقف ویدیو هنگام رفتن برنامه به پس‌زمینه"_q },
		{ u"LuminaMediaAutoPauseBgVideoInfo"_q, u"هنگام خروج از LuminaGram، ویدیوی در حال پخش به‌طور خودکار متوقف می‌شود. کوچک کردن نمایشگر هم آن را متوقف می‌کند. ویدیویی که خودتان عمداً در پنجره‌ای جدا یا در حالت تصویر در تصویر گذاشته‌اید به پخش ادامه می‌دهد، و موسیقی و پیام‌های صوتی هم در نبودِ شما به پخش ادامه می‌دهند."_q },
		{ u"LuminaMediaSaving"_q, u"رسانه"_q },
		{ u"LuminaMediaTitle"_q, u"رسانه"_q },
		{ u"LuminaMessageActionsInfo"_q, u"این گزینه‌ها فقط در منوی زمینهٔ پیام نمایش داده می‌شوند. هر کدام از هدایت خودِ Telegram استفاده می‌کنند، پس گفتگویی که هدایت را محدود کرده همچنان محدود می‌ماند. «هدایت بدون نویسنده و شرح» به نام کاری است که واقعاً انجام می‌شود: Telegram نمی‌تواند شرح را حذف کند و در همان حال فرستنده را نگه دارد، پس انتخاب آن هر دو را حذف می‌کند. «جزئیات» فقط چیزی را می‌خواند که این دستگاه پیش‌تر همگام‌سازی کرده است — نه چیزی درخواست می‌شود و نه چیزی ذخیره می‌شود."_q },
		{ u"LuminaMessageDetails"_q, u"جزئیات"_q },
		{ u"LuminaOnboardingDualName"_q, u"هر دو زبان با هم"_q },
		{ u"LuminaOnboardingDualText"_q, u"متن اصلی را کنار ترجمه‌اش روی صفحه نگه می‌دارد، هم برای پیام‌هایی که دریافت می‌کنید و هم برای آن‌هایی که می‌فرستید."_q },
		{ u"LuminaOnboardingFooter"_q, u"برای تنظیم هر کدام از این‌ها، بخش مربوطه را در صفحهٔ تنظیمات LuminaGram باز کنید. این کارت را می‌توانید دوباره از «ابزارها» بخوانید."_q },
		{ u"LuminaOnboardingGotIt"_q, u"متوجه شدم"_q },
		{ u"LuminaOnboardingHeader"_q, u"درباره"_q },
		{ u"LuminaOnboardingIntro"_q, u"LuminaGram ابزارهای خودش را روی Telegram Desktop می‌افزاید. هر گزینه‌ای که در پایین می‌بینید فقط روی همین رایانه ذخیره می‌شود و هرگز با Telegram همگام‌سازی نمی‌شود."_q },
		{ u"LuminaOnboardingRow"_q, u"آنچه LuminaGram می‌افزاید"_q },
		{ u"LuminaOnboardingRowAbout"_q, u"کارت خوش‌آمدی را نشان می‌دهد که نخستین بار هنگام باز کردن تنظیمات LuminaGram دیده می‌شود. فقط توضیح می‌دهد اینجا چه چیزهایی هست و هیچ گزینه‌ای را روشن نمی‌کند."_q },
		{ u"LuminaOnboardingSafetyName"_q, u"بررسی‌های ایمنی"_q },
		{ u"LuminaOnboardingSafetyText"_q, u"پیش از باز کردن پیوندی که نشانی‌اش آن چیزی نیست که به نظر می‌رسد هشدار می‌دهد، هنگام چسباندن نشانی رمزارز جابه‌جاشده را می‌گیرد، و موقعیت مکانی را از عکس‌هایی که می‌فرستید حذف می‌کند."_q },
		{ u"LuminaOnboardingTitle"_q, u"به LuminaGram خوش آمدید"_q },
		{ u"LuminaOnboardingTranslateText"_q, u"پیام‌های دریافتی را با سرویس ترجمهٔ خودتان ترجمه کنید، و آنچه را می‌نویسید پیش از ارسال ترجمه کنید."_q },
		{ u"LuminaOnboardingVaultName"_q, u"گاوصندوق استتار"_q },
		{ u"LuminaOnboardingVaultText"_q, u"LuminaGram را پشت یک ماشین‌حساب یا یک دفترچهٔ یادداشت پنهان می‌کند که فقط با کد مخفی شما برنامهٔ واقعی را باز می‌کند."_q },
		{ u"LuminaProfileCardAbout"_q, u"کارتی محلی دربارهٔ خودتان — اینکه چه زبان‌هایی صحبت می‌کنید و به چه چیزهایی علاقه دارید — که می‌توانید کپی کنید و در یک گفتگو بچسبانید. روی همین دستگاه می‌ماند."_q },
		{ u"LuminaProfileCardBio"_q, u"معرفی کوتاه"_q },
		{ u"LuminaProfileCardBioHint"_q, u"چند کلمه دربارهٔ خودتان"_q },
		{ u"LuminaProfileCardCopied"_q, u"کارت کپی شد"_q },
		{ u"LuminaProfileCardCopy"_q, u"کپی در کلیپ‌بورد"_q },
		{ u"LuminaProfileCardCopyInfo"_q, u"یک خلاصهٔ متنی ساده از کارت شما می‌سازد. هر وقت خواستید آن را هم‌رسانی کنید، در هر گفتگویی بچسبانید."_q },
		{ u"LuminaProfileCardEdit"_q, u"کارت من"_q },
		{ u"LuminaProfileCardEmptyShare"_q, u"ابتدا کارت خود را پر کنید"_q },
		{ u"LuminaProfileCardHeader"_q, u"کارت من"_q },
		{ u"LuminaProfileCardInfo"_q, u"این کارت فقط روی همین دستگاه ذخیره می‌شود و هرگز به Telegram فرستاده نمی‌شود. نمایهٔ Telegram شما را هم تغییر نمی‌دهد."_q },
		{ u"LuminaProfileCardInterests"_q, u"علاقه‌مندی‌ها / برچسب‌ها"_q },
		{ u"LuminaProfileCardInterestsHint"_q, u"مثال: موسیقی، کوه‌نوردی، برنامه‌نویسی"_q },
		{ u"LuminaProfileCardLanguages"_q, u"زبان‌هایی که صحبت می‌کنم"_q },
		{ u"LuminaProfileCardLanguagesHint"_q, u"مثال: فارسی، English، Español"_q },
		{ u"LuminaProfileCardNotSet"_q, u"تنظیم‌نشده"_q },
		{ u"LuminaProfileCardTagline"_q, u"شعار"_q },
		{ u"LuminaProfileCardTaglineHint"_q, u"یک جملهٔ کوتاه دربارهٔ خودتان"_q },
		{ u"LuminaProfileCardTitle"_q, u"کارت معرفی"_q },
		{ u"LuminaProfileChatCreated"_q, u"ساخته‌شده در"_q },
		{ u"LuminaProfileDcId"_q, u"مرکز داده"_q },
		{ u"LuminaProfileDcIdValue"_q, u"DC{1}"_q },
		{ u"LuminaProfileInfoHeader"_q, u"نمایه"_q },
		{ u"LuminaProfileInfoInfo"_q, u"خط‌های اضافی در صفحه‌های نمایه که همه روی همین دستگاه به دست می‌آیند — چیزی از Telegram پرسیده نمی‌شود. تاریخ ثبت‌نام برخی حساب‌ها را خودِ Telegram می‌دهد؛ برای بقیه از روی شمارهٔ حساب تخمین زده می‌شود و با «~» نشان داده می‌شود. مرکز داده همان مرکزی است که عکس نمایه را ذخیره می‌کند. تاریخ ساخت، زمان ساخته‌شدن یک گروه یا کانال است."_q },
		{ u"LuminaProfileRegistrationApprox"_q, u"~ {1}"_q },
		{ u"LuminaProfileRegistrationDate"_q, u"تاریخ ثبت‌نام"_q },
		{ u"LuminaProfileShowChatDate"_q, u"نمایش تاریخ ساخت"_q },
		{ u"LuminaProfileShowDcId"_q, u"نمایش مرکز داده"_q },
		{ u"LuminaProfileShowRegistrationDate"_q, u"نمایش تاریخ ثبت‌نام"_q },
		{ u"LuminaRecentLimitsInfo"_q, u"جلوی این دستگاه را می‌گیرد که استیکرهای اخیر و GIFهای ذخیره‌شده را زودتر از آنچه ناچار است دور بیندازد، با همان محدودیت‌های LuminaGram روی Android: تا ۲۰۰ استیکر اخیر و ۵۰۰ GIF ذخیره‌شده. اینکه واقعاً چند تا در حساب شما نگهداری و با دستگاه‌های دیگرتان همگام‌سازی شود همچنان با سرورهای Telegram است، پس این گزینه فهرست ابری بزرگ‌تری به شما نمی‌دهد — دفعهٔ بعد که این دستگاه همگام‌سازی شود، هر چیزی که سرور دیگر نگه نداشته باشد اینجا هم ناپدید می‌شود، معمولاً در عرض چند ثانیه. تا وقتی این گزینه روشن است، Telegram Desktop خودبه‌خود GIF ذخیره‌شده‌ای را حذف نمی‌کند، پس یادآوری Premium آن دربارهٔ محدودیت GIFهای ذخیره‌شده ساکت می‌ماند. پنل استیکر همچنان فقط ۲۰ استیکر اخیر نخست را نشان می‌دهد، مگر اینکه «Unlimited recent stickers» را هم در تنظیمات › پیشرفته › تنظیمات آزمایشی روشن کنید."_q },
		{ u"LuminaRecentLimitsRow"_q, u"نگهداری استیکرها و GIFهای اخیر بیشتر"_q },
		{ u"LuminaReplyTemplatesAbout"_q, u"تکه‌های کوتاه متنی که روی همین دستگاه نگه می‌دارید و در کادر پیام می‌گذارید. برای انتخاب یکی، در گفتگو روی دکمهٔ ایموجی راست‌کلیک کنید. قالب‌ها هرگز به Telegram فرستاده نمی‌شوند."_q },
		{ u"LuminaReplyTemplatesAdd"_q, u"افزودن قالب"_q },
		{ u"LuminaReplyTemplatesEdit"_q, u"ویرایش قالب"_q },
		{ u"LuminaReplyTemplatesEmpty"_q, u"هنوز قالبی ندارید. یکی بیفزایید، سپس در یک گفتگو روی دکمهٔ ایموجی راست‌کلیک کنید تا آن را درج کنید."_q },
		{ u"LuminaReplyTemplatesFull"_q, u"فهرست پر است — برای افزودن قالب تازه، یکی را حذف کنید."_q },
		{ u"LuminaReplyTemplatesInfo"_q, u"برای درج یک قالب، در گفتگو روی دکمهٔ ایموجی راست‌کلیک کنید. برای جابه‌جایی یک قالب به بالا یا پایین، اینجا روی آن راست‌کلیک کنید؛ برای ویرایش یا حذف، آن را باز کنید."_q },
		{ u"LuminaReplyTemplatesList"_q, u"قالب‌ها"_q },
		{ u"LuminaReplyTemplatesManage"_q, u"مدیریت قالب‌ها…"_q },
		{ u"LuminaReplyTemplatesMoveDown"_q, u"انتقال به پایین"_q },
		{ u"LuminaReplyTemplatesMoveUp"_q, u"انتقال به بالا"_q },
		{ u"LuminaReplyTemplatesNone"_q, u"هیچ"_q },
		{ u"LuminaReplyTemplatesPlaceholder"_q, u"متن قالب"_q },
		{ u"LuminaReplyTemplatesShow"_q, u"پیشنهاد قالب‌ها در گفتگوها"_q },
		{ u"LuminaReplyTemplatesTitle"_q, u"قالب‌های پاسخ"_q },
		{ u"LuminaSaveSticker"_q, u"ذخیرهٔ استیکر"_q },
		{ u"LuminaSaveStickers"_q, u"ذخیرهٔ استیکرها"_q },
		{ u"LuminaSaveStickersInfo"_q, u"یک ردیف «ذخیرهٔ استیکر» به منوی زمینهٔ پنل استیکر می‌افزاید. استیکرها همان‌جایی می‌روند که بقیهٔ دانلودهای شما می‌روند، و اگر گروهی ذخیره‌سازی را محدود کرده باشد، این ردیف برای بستهٔ استیکر خودِ آن گروه پنهان می‌ماند."_q },
		{ u"LuminaSaveToCloud"_q, u"ذخیره در پیام‌های ذخیره‌شده"_q },
		{ u"LuminaSaveToCloudTitle"_q, u"ذخیره در پیام‌های ذخیره‌شده"_q },
		{ u"LuminaSecurityPanicConfirmAck"_q, u"می‌دانم که این کار قابل بازگشت نیست"_q },
		{ u"LuminaSecurityPanicConfirmButton"_q, u"اکنون پاک کن"_q },
		{ u"LuminaSecurityPanicConfirmText"_q, u"از همهٔ حساب‌های این دستگاه خارج می‌شوید. پایگاه دادهٔ محلی پیام‌ها، پیش‌نویس‌ها و رسانه‌های حافظهٔ پنهان پاک می‌شوند، همراه با تنظیمات خودِ LuminaGram، نشانک‌ها، ترجمه‌های ذخیره‌شده و کلیدهای API.\n\nحساب‌های شما حذف نمی‌شوند. آن‌ها روی سرورهای Telegram می‌مانند و پیام‌هایتان هم همین‌طور — می‌توانید از هر جایی دوباره وارد شوید.\n\nفایل‌هایی که پیش‌تر دانلود شده‌اند حذف نمی‌شوند. LuminaGram به پوشهٔ دانلود شما دست نمی‌زند، چون معمولاً همان پوشهٔ Downloads معمولی شماست و فایل‌های بی‌ربط هم در آن هست. هر چیز حساسی را که آنجاست خودتان جابه‌جا یا حذف کنید.\n\nاین کار قابل بازگشت نیست."_q },
		{ u"LuminaSecurityPanicConfirmTitle"_q, u"پاک‌سازی اضطراری؟"_q },
		{ u"LuminaSecurityPanicHeader"_q, u"پاک‌سازی اضطراری"_q },
		{ u"LuminaSecurityPanicWipe"_q, u"پاک‌سازی اضطراری (Kaboom)"_q },
		{ u"LuminaSecurityPanicWipeAbout"_q, u"از همهٔ حساب‌های این دستگاه خارج می‌شود و پایگاه دادهٔ محلی پیام‌ها، پیش‌نویس‌ها و رسانه‌های حافظهٔ پنهان را پاک می‌کند، همراه با تنظیمات خودِ LuminaGram، نشانک‌ها، ترجمه‌های ذخیره‌شده و کلیدهای API. حساب‌ها و پیام‌های شما روی سرورهای Telegram باقی می‌مانند. فایل‌هایی که پیش‌تر دانلود کرده‌اید سر جای خود می‌مانند. این کار قابل بازگشت نیست."_q },
		{ u"LuminaSelectFromAuthor"_q, u"انتخاب همهٔ پیام‌های این نویسنده"_q },
		{ u"LuminaSelectFromAuthorAbout"_q, u"در گروه‌ها یک گزینه به منوی پیام می‌افزاید که همهٔ پیام‌های فرستنده‌ای را که رویش کلیک کرده‌اید انتخاب می‌کند. فقط پیام‌هایی را در بر می‌گیرد که این پنجره پیش‌تر بارگذاری کرده است — برای افزودن پیام‌های قدیمی‌تر، بیشتر به عقب بروید و دوباره همین کار را انجام دهید. پیام‌هایی که به خودِ گفتگو نسبت داده می‌شوند، مانند پست‌های کانال و پست‌های مدیران ناشناس، این گزینه را نمی‌گیرند."_q },
		{ u"LuminaSelectFromAuthorLimit"_q, u"در یک زمان فقط {1} پیام می‌توان انتخاب کرد."_q },
		{ u"LuminaSelectFromAuthorTitle"_q, u"انتخاب همهٔ پیام‌های این نویسنده"_q },
		{ u"LuminaSendOriginalCaption"_q, u"ارسال شرح اصلی"_q },
		{ u"LuminaShowBookmarks"_q, u"نمایش گزینهٔ نشانک در منو"_q },
		{ u"LuminaShowMessageDetails"_q, u"منوی جزئیات پیام"_q },
		{ u"LuminaShowMutedCount"_q, u"همیشه نمایش شمار نخوانده"_q },
		{ u"LuminaShowMutedCountInfo"_q, u"نشان نخواندهٔ گفتگوهای بی‌صدا را به‌جای خاکستری کم‌رنگ با رنگ تأکید عادی رسم می‌کند."_q },
		{ u"LuminaStickerSavedTo"_q, u"استیکر در {1} ذخیره شد"_q },
		{ u"LuminaStickerSizeChoice"_q, u"{1}%"_q },
		{ u"LuminaStickerSizeChoiceDefault"_q, u"{1}% (پیش‌فرض)"_q },
		{ u"LuminaStickerSizeInfo"_q, u"استیکرها در گفتگوها با چه اندازه‌ای رسم شوند — هم آن‌هایی که می‌فرستید و هم آن‌هایی که دریافت می‌کنید. ایموجی‌های متحرک، تاس و استیکرهای هدیه اندازهٔ خودشان را نگه می‌دارند. Telegram Desktop اندازهٔ یک استیکر را یک‌بار می‌سنجد و به خاطر می‌سپارد، پس اندازهٔ تازه از راه‌اندازی بعدی برنامه اعمال می‌شود."_q },
		{ u"LuminaTimeWithSeconds"_q, u"نمایش ثانیه در زمان پیام"_q },
		{ u"LuminaTimeWithSecondsAbout"_q, u"زمانِ زیر پیام، زمان در متن کپی‌شده و زمانی که صفحه‌خوان اعلام می‌کند، همگی ثانیه را در بر می‌گیرند."_q },

		// The two items in the system tray menu; the second is also the one
		// item on the taskbar button's jump list on Windows. {1} is the
		// application's name, always the literal LuminaGram, never
		// translated and never inflected - so both rows read as a verb
		// phrase ending in a bare product name.
		{ u"LuminaTrayOpen"_q, u"باز کردن {1}"_q },
		{ u"LuminaTrayQuit"_q, u"خروج از {1}"_q },

		{ u"LuminaUndoSendBulletin"_q, u"در حال ارسال پیام…"_q },
		{ u"LuminaUndoSendTitle"_q, u"لغو ارسال"_q },
		{ u"LuminaUndoSendUndo"_q, u"لغو"_q },
		{ u"LuminaUndoSendWindow"_q, u"پنجرهٔ لغو ارسال"_q },
		{ u"LuminaUndoSendWindowInfo"_q, u"یک پیام متنی ساده را پیش از ارسال، {1} ثانیه پشت دکمهٔ «لغو» نگه می‌دارد. متن شما در تمام این مدت در کادر پیام می‌ماند و کادر تنها وقتی پاک می‌شود که پیام واقعاً بیرون برود، پس «لغو» فقط آن را همان‌جا رها می‌کند — چیزی برداشته و برگردانده نمی‌شود. ارسال دوباره، باز کردن گفتگویی دیگر، یا بستن برنامه پیامِ نگه‌داشته‌شده را بی‌درنگ می‌فرستد. پیام‌های رسانه‌ای، صوتی، ویرایش‌شده، هدایت‌شده و زمان‌بندی‌شده هرگز نگه داشته نمی‌شوند، و همچنین پیام‌هایی که از یک موضوع انجمن یا رشتهٔ نظرات فرستاده می‌شوند."_q },
		// clipboard guard + scam watch
		{ u"LuminaPrivacyCryptoClipboardGuard"_q, u"محافظت هنگام چسباندن نشانی رمزارز"_q },
		{ u"LuminaPrivacyCryptoClipboardGuardInfo"_q, u"پیش از چسباندن چیزی که شبیه نشانی کیف‌پول رمزارز است در یک پیام، از تو می‌پرسد. بدافزارِ ربایندهٔ حافظهٔ موقت می‌تواند بی‌آنکه متوجه شوی نشانی کپی‌شده را با نشانی کلاهبردار جایگزین کند، و چسباندن آخرین لحظه‌ای است که می‌توانی آن را بگیری. بررسی روی همین دستگاه انجام می‌شود و چیزی به جایی فرستاده نمی‌شود."_q },
		{ u"LuminaPrivacyScamKeywordWarning"_q, u"هشدار واژگان کلاه‌برداری"_q },
		{ u"LuminaPrivacyScamKeywordWarningInfo"_q, u"وقتی پیامی از شخصی که در مخاطبان شما نیست به ترفندهای رایج کلاه‌برداری اشاره کند — انتقال پول، کارت هدیه، «سرمایه‌گذاری» رمزارز، هزینهٔ تأیید یا درخواست کد — یک یادآوری یک‌باره برای احتیاط نمایش داده می‌شود. پیام هرگز مسدود یا تغییر داده نمی‌شود. بررسی‌ها به‌صورت آفلاین روی دستگاه شما انجام می‌شود."_q },
		// link preview + voice confirm
		{ u"LuminaPrivacyDisableLinkPreview"_q, u"غیرفعال‌کردن پیش‌نمایش پیوند به‌صورت پیش‌فرض"_q },
		{ u"LuminaLinkPreviewInfo"_q, u"پیام‌هایی که از ابتدا می‌نویسید بدون پیش‌نمایش پیوند ارسال می‌شوند و کادر نوشتن هم هنگام تایپ هرگز از Telegram نمی‌خواهد پیوند چسبانده‌شده را باز کند. پیش‌نمایشی که از پیش پیوست شده و پیامی که ویرایش می‌کنید دست‌نخورده می‌مانند. برای پیوست‌کردن پیش‌نمایش به یک پیام خاص، در تنظیمات > میان‌برهای صفحه‌کلید کلیدی به «تغییر وضعیت پیش‌نمایش پیوند» بدهید و هنگام نوشتن آن را فشار دهید."_q },
		{ u"LuminaVoiceConfirmTitle"_q, u"پیام‌های صوتی و تصویری"_q },
		{ u"LuminaVoiceConfirmRow"_q, u"تأیید پیش از ارسال"_q },
		{ u"LuminaVoiceConfirmInfo"_q, u"پیش از ارسال پیام صوتی یا پیام تصویری گرد ضبط‌شده بپرس. انصراف از این پرسش ضبط را دور می‌اندازد، مگر در پیش‌نمایش شنیدن که ضبط را دست‌نخورده باقی می‌گذارد. دربارهٔ ارسال‌های زمان‌بندی‌شده هرگز پرسیده نمی‌شود."_q },
		// exif strip + text replace
		{ u"LuminaExifStripRow"_q, u"حذف موقعیت مکانی عکس پیش از ارسال"_q },
		{ u"LuminaExifStripInfo"_q, u"مختصات GPS را که دوربین درون عکس JPEG ذخیره کرده است، پیش از بارگذاری پاک می‌کند — چه عکس را به شکل فشردهٔ معمول بفرستید و چه آن را به‌صورت فایل. تنها برچسب‌های موقعیت حذف می‌شوند: برچسب‌های جهت، دوربین و تاریخ دقیقاً دست‌نخورده می‌مانند، پس عکسی که به‌صورت فایل فرستاده شود همچنان درست نمایش داده می‌شود. نسخهٔ خودتان از فایل روی دیسک هرگز تغییر نمی‌کند. این کار ویدیوها را در بر نمی‌گیرد، و نیز موقعیتی را که XMP یا یادداشت‌های سازندهٔ خود دوربین در عکس نوشته‌اند، یا نسخهٔ دومی از تصویر که برخی گوشی‌ها در همان فایل جا می‌دهند."_q },
		{ u"LuminaReplacerTitle"_q, u"جایگزین متن"_q },
		{ u"LuminaReplacerHeader"_q, u"قواعد"_q },
		{ u"LuminaReplacerAdd"_q, u"افزودن قاعده"_q },
		{ u"LuminaReplacerEdit"_q, u"ویرایش قاعده"_q },
		{ u"LuminaReplacerFrom"_q, u"جایگزینی"_q },
		{ u"LuminaReplacerTo"_q, u"با"_q },
		{ u"LuminaReplacerEnable"_q, u"جایگزینی متن در پیام‌هایی که می‌فرستم"_q },
		{ u"LuminaReplacerNone"_q, u"هیچ"_q },
		{ u"LuminaReplacerNothing"_q, u"(هیچ)"_q },
		{ u"LuminaReplacerEmpty"_q, u"هنوز قاعده‌ای ندارید. یکی بیفزایید تا جایگزینی متن در پیام‌هایی که می‌فرستید آغاز شود."_q },
		{ u"LuminaReplacerFull"_q, u"قواعد به ترتیب و از بالا اعمال می‌شوند. فهرست پر است — برای افزودن قاعدهٔ تازه، یکی را حذف کنید."_q },
		{ u"LuminaReplacerListInfo"_q, u"قواعد به ترتیب و از بالا اعمال می‌شوند و هر قاعده روی نتیجهٔ قاعدهٔ بالای خود کار می‌کند."_q },
		{ u"LuminaReplacerAbout"_q, u"در هر پیامی که می‌فرستید، کوته‌نوشت را با عبارتی که منظورتان بوده جایگزین می‌کند — «brb» به «be right back» تبدیل می‌شود. تطبیق به بزرگی و کوچکی حروف حساس است و تنها واژه‌های کامل را در بر می‌گیرد. پیامی که هرگونه قالب‌بندی داشته باشد دقیقاً همان‌گونه که نوشته‌اید فرستاده می‌شود، و پیوندها، اشاره‌ها، هشتگ‌ها و فرمان‌های ربات هرگز بازنویسی نمی‌شوند. قواعد روی همین دستگاه می‌مانند و هرگز به Telegram فرستاده نمی‌شوند."_q },
		// vault + fake crash
		{ u"LuminaVaultHeader"_q, u"گاوصندوق استتار"_q },
		{ u"LuminaVaultEnable"_q, u"فعال‌سازی گاوصندوق"_q },
		{ u"LuminaVaultInfo"_q, u"همه‌چیز اینجا فقط روی همین دستگاه می‌ماند. گاوصندوق LuminaGram را پشت یک برنامهٔ بی‌خطر پنهان می‌کند و تنها کد مخفی شما را به برنامهٔ واقعی بازمی‌گرداند."_q },
		{ u"LuminaVaultMode"_q, u"حالت گاوصندوق"_q },
		{ u"LuminaVaultModePasswordDoor"_q, u"درِ رمز"_q },
		{ u"LuminaVaultModeDecoyApp"_q, u"برنامهٔ فریبنده"_q },
		{ u"LuminaVaultModePasswordDoorInfo"_q, u"با اجرای LuminaGram یک رمز خواسته می‌شود. کد مخفی برنامهٔ واقعی را باز می‌کند؛ هر چیز دیگری برنامهٔ فریبنده را باز می‌کند و هرگز نمی‌گوید که اشتباه بوده است."_q },
		{ u"LuminaVaultModeDecoyAppInfo"_q, u"با اجرای LuminaGram مستقیماً برنامهٔ فریبنده باز می‌شود. کد مخفی خود را در آن وارد کنید تا به برنامهٔ واقعی برسید."_q },
		{ u"LuminaVaultSkin"_q, u"ظاهر برنامهٔ فریبنده"_q },
		{ u"LuminaVaultSkinNotepad"_q, u"یادداشت"_q },
		{ u"LuminaVaultSkinCalculator"_q, u"ماشین‌حساب"_q },
		{ u"LuminaVaultSkinHintCalculator"_q, u"برای باز کردن از ماشین‌حساب، کد مخفی را وارد کنید و کلید مساوی را بزنید."_q },
		{ u"LuminaVaultSkinHintNotepad"_q, u"برای باز کردن از یادداشت، کاری کنید که کل یادداشت همان کد مخفی باشد، سپس روی نوار عنوان «یادداشت» دوبار کلیک کنید."_q },
		{ u"LuminaVaultSecretCode"_q, u"کد مخفی"_q },
		{ u"LuminaVaultSecretCodeDialogTitle"_q, u"کد مخفی"_q },
		{ u"LuminaDisguiseDecoyCodeSet"_q, u"تنظیم‌شده"_q },
		{ u"LuminaDisguiseDecoyCodeNotSet"_q, u"تنظیم‌نشده"_q },
		{ u"LuminaVaultNoCodeInfo"_q, u"تا زمانی که کد مخفی تعیین نکنید، گاوصندوق خاموش می‌ماند."_q },
		{ u"LuminaVaultCalculatorCodeInfo"_q, u"این کد را نمی‌توان با صفحه‌کلید ماشین‌حساب تایپ کرد، بنابراین به‌جای آن یادداشت فریبنده نشان داده می‌شود. اگر ماشین‌حساب را می‌خواهید، فقط از ارقام، نقطه و + - * / استفاده کنید."_q },
		{ u"LuminaVaultDisclaimer"_q, u"این کار جلوی یک نگاه گذرا از پشت شانه‌تان را می‌گیرد، نه بازرسی یک فرد آگاه را: نام برنامه در مدیر وظیفه هنوز LuminaGram است و هر کسی که بتواند فایل‌های این دستگاه را بخواند می‌تواند به حساب شما دست یابد. گاوصندوق در اجرای بعدی LuminaGram ظاهر می‌شود و بستن برنامهٔ فریبنده LuminaGram را می‌بندد. اگر روزی کد را فراموش کردید، فایل‌های tdata/luminagram.json و tdata/luminagram_private.json را از پوشهٔ دادهٔ LuminaGram حذف کنید: آنگاه گاوصندوق خودش خاموش می‌شود و LuminaGram عادی اجرا می‌شود."_q },
		{ u"LuminaSecurityFakeCrashHeader"_q, u"بازگشایی با خرابی جعلی"_q },
		{ u"LuminaSecurityFakeCrashEnable"_q, u"فعال‌سازی بازگشایی با خرابی جعلی"_q },
		{ u"LuminaSecurityFakeCrashCode"_q, u"رمز خرابی جعلی"_q },
		{ u"LuminaSecurityFakeCrashCodeDialogTitle"_q, u"رمز خرابی جعلی"_q },
		{ u"LuminaSecurityFakeCrashCodeHint"_q, u"رمز"_q },
		{ u"LuminaSecurityFakeCrashCodeSet"_q, u"تنظیم‌شده"_q },
		{ u"LuminaSecurityFakeCrashCodeNotSet"_q, u"تنظیم‌نشده"_q },
		{ u"LuminaSecurityFakeCrashCodeSameAsPasscode"_q, u"این رمز نمی‌تواند با رمز عبور شما یکسان باشد."_q },
		{ u"LuminaSecurityFakeCrashInfo"_q, u"وقتی این گزینه روشن باشد، تایپ رمز خرابی جعلی در صفحهٔ قفل به‌جای باز کردن LuminaGram بی‌درنگ آن را می‌بندد، بدون هیچ پیامی و بدون آنکه چیزی روی صفحه بماند — برای هر کسی که تماشا می‌کند، برنامه خراب شده است. این رمز از رمز عبور شما جداست، فقط روی همین دستگاه نگهداری می‌شود و هرگز به جایی فرستاده نمی‌شود."_q },
		{ u"LuminaSecurityFakeCrashBiometricInfo"_q, u"تا زمانی که این گزینه روشن و رمزی تعیین شده باشد، باز کردن قفل با Touch ID، Windows Hello یا رمز سیستم خاموش می‌شود: اثر انگشت بدون آنکه هرگز رمز عبور را بپرسد برنامه را باز می‌کند و بنابراین از کنار این رمز می‌گذرد. با پاک کردن رمز یا خاموش کردن این گزینه، دوباره فعال می‌شوند."_q },
		{ u"LuminaSecurityFakeCrashNoCodeInfo"_q, u"هنوز رمزی تعیین نشده است، بنابراین در صفحهٔ قفل هیچ اتفاقی نمی‌افتد."_q },
		{ u"LuminaSecurityFakeCrashNoPasscodeInfo"_q, u"LuminaGram رمز عبور ندارد، بنابراین صفحهٔ قفل هرگز ظاهر نمی‌شود و این رمز هرگز خواسته نمی‌شود. ابتدا رمز عبور را در «حریم خصوصی و امنیت» روشن کنید."_q },
		{ u"LuminaSecurityFakeCrashCodeClashInfo"_q, u"این رمز اکنون با رمز عبور شما یکسان است، بنابراین هرگز عمل نمی‌کند — رمز عبور به‌جای آن برنامه را باز می‌کند. رمز دیگری انتخاب کنید."_q },

		// Voice to text (F-32), keys shared with Android's
		// LuminaLocale / LuminaVoiceToTextActivity.
		{ u"LuminaVoiceToTextTitle"_q, u"تبدیل گفتار به متن"_q },
		{ u"LuminaSttEnable"_q, u"فعال‌سازی تبدیل گفتار به متن"_q },
		{ u"LuminaSttEngine"_q, u"موتور"_q },
		{ u"LuminaSttEngineWhisper"_q, u"OpenAI Whisper (کلید خودتان)"_q },
		{ u"LuminaSttEngineGoogle"_q, u"Google (کلید خودتان)"_q },
		{ u"LuminaSttKey"_q, u"کلید API"_q },
		{ u"LuminaSttBaseUrl"_q, u"آدرس پایه"_q },
		{ u"LuminaSttModel"_q, u"مدل"_q },
		{ u"LuminaSttInfoDesktop"_q, u"رونویسی با کلید API و سهمیهٔ خود شما انجام می‌شود و "
			u"LuminaGram هیچ کلیدی ارائه نمی‌دهد. پیام صوتی به سرویسی "
			u"که در پایین انتخاب می‌کنید آپلود می‌شود؛ پس فقط برای "
			u"سرویسی روشن کنید که به آن اعتماد دارید."_q },
		{ u"LuminaSttVoskUnsupported"_q, u"موتور آفلاین (Vosk) هنوز در نسخهٔ دسکتاپ پشتیبانی "
			u"نمی‌شود: به یک کتابخانهٔ بومی و یک مدل گفتاری نیاز دارد "
			u"که باید جداگانه برای Windows، macOS و Linux بسته‌بندی "
			u"شود. تا آن زمان، رونویسی اینجا فقط ابری است."_q },
		{ u"LuminaSttUiMenuItem"_q, u"به متن"_q },
		{ u"LuminaSttUiPickTitle"_q, u"تبدیل گفتار به متن"_q },
		{ u"LuminaSttUiFetching"_q, u"در حال دانلود پیام صوتی…"_q },
		{ u"LuminaSttUiTranscribing"_q, u"در حال رونویسی…"_q },
		{ u"LuminaSttUiTranslation"_q, u"ترجمه"_q },
		{ u"LuminaSttUiError"_q, u"رونویسی ناموفق بود"_q },
		{ u"LuminaSttUiNoText"_q, u"گفتاری تشخیص داده نشد"_q },
		{ u"LuminaSttUiNoKey"_q, u"کلید API تنظیم نشده است. آن را از تنظیمات، LuminaGram، "
			u"تبدیل گفتار به متن اضافه کنید."_q },
		{ u"LuminaSttUiTooLarge"_q, u"این پیام صوتی برای موتور انتخاب‌شده بزرگ است."_q },
		{ u"LuminaSttUiRoundUnsupported"_q, u"Google Speech نمی‌تواند پیام‌های ویدئویی را بخواند. برای "
			u"ویدئوی دایره‌ای Whisper را انتخاب کنید."_q },
		{ u"LuminaSttAutoTranslate"_q, u"ترجمه پس از پیاده‌سازی"_q },
		{ u"LuminaSttAutoTranslateInfo"_q, u"اگر متن پیاده‌شدهٔ پیام صوتی به زبانی باشد که نمی‌خوانید، "
			u"LuminaGram بی‌درنگ آن را ترجمه می‌کند و ابتدا متن اصلی و "
			u"سپس ترجمه را نشان می‌دهد. اگر زبان متن همان زبان خواندن "
			u"شما باشد ترجمه انجام نمی‌شود و سهمیه هدر نمی‌رود؛ اگر "
			u"ترجمه شکست بخورد، متن اصلی باقی می‌ماند."_q },
	};
}

[[maybe_unused]] const auto kRegistered = RegisterLocaleTable("fa", Build);

} // namespace
} // namespace Lumina
