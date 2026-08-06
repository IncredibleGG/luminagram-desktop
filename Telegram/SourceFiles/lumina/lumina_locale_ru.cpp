/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_locale.h"

namespace Lumina {
namespace {

// Russian. The wording is Android's LuminaLocale.java wherever the two
// platforms show the same string, so the two can be compared key for key;
// where the desktop text says something the Android one does not - the longer
// divider paragraphs, the rows this page has and Android's does not - it is
// translated from the English table in lumina_locale.cpp.
//
// Quoted row labels inside the divider paragraphs are quoted with the Russian
// guillemets and must keep matching the rows they name.
[[nodiscard]] LocaleTable Build() {
	return {
		// Sections of the LuminaGram settings page.
		{ u"LuminaGramTitle"_q, u"LuminaGram"_q },
		{ u"LuminaTranslateTitle"_q, u"Перевод"_q },
		{ u"LuminaPrivacyTitle"_q, u"Конфиденциальность"_q },
		{ u"LuminaSecurityTitle"_q, u"Безопасность"_q },
		{ u"LuminaChatSettings"_q, u"Чаты"_q },
		{ u"LuminaGramChatList"_q, u"Список чатов"_q },
		{ u"LuminaAppearanceTitle"_q, u"Оформление"_q },
		{ u"LuminaToolsTitle"_q, u"Инструменты"_q },
		{ u"LuminaGramStoredLocallyInfo"_q, u"Настройки LuminaGram хранятся "
			u"только на этом устройстве и никогда не синхронизируются с "
			u"Telegram."_q },

		// Sub-pages that have no rows yet.
		{ u"LuminaAppearancePlaceholder"_q, u"Здесь появятся параметры "
			u"форматирования сообщений, стикеров и чисел."_q },
		{ u"LuminaChatListPlaceholder"_q, u"Здесь появятся параметры "
			u"внешнего вида списка чатов и значков."_q },
		{ u"LuminaPrivacyPlaceholder"_q, u"Здесь появятся параметры "
			u"конфиденциальности для ссылок, буфера обмена и исходящих "
			u"медиафайлов."_q },
		{ u"LuminaSecurityPlaceholder"_q, u"Здесь появятся параметры "
			u"скрытого хранилища, экстренной очистки и разблокировки под "
			u"принуждением."_q },
		{ u"LuminaToolsPlaceholder"_q, u"Здесь появятся закладки, шаблоны "
			u"ответов и локальные резервные копии."_q },

		// Chats sub-page.
		{ u"LuminaMessageActions"_q, u"Действия с сообщениями"_q },
		{ u"LuminaAllowSaveRestricted"_q,
			u"Разрешать сохранение / копирование в защищённых чатах"_q },
		{ u"LuminaAllowSaveRestrictedInfo"_q, u"«Разрешать сохранение / "
			u"копирование в защищённых чатах» влияет только на локальные "
			u"действия на этом устройстве. Некоторые чаты ограничивают "
			u"сохранение не просто так — пользуйтесь этим ответственно."_q },

		// Translation sub-page.
		{ u"LuminaTranslateEnable"_q, u"Включить перевод LuminaGram"_q },
		{ u"LuminaTranslateEnableInfo"_q, u"Переводите через собственный "
			u"сервис вместо Premium-перевода Telegram. Сервис по умолчанию не "
			u"требует ни аккаунта, ни ключа API. Пока этот параметр выключен, "
			u"перевод работает точно так же, как в обычном Telegram "
			u"Desktop."_q },
		{ u"LuminaTranslateSendHeader"_q, u"Отправка"_q },
		{ u"LuminaTranslateBeforeSend"_q, u"Переводить перед отправкой"_q },
		{ u"LuminaTranslateSendLang"_q, u"Язык отправки"_q },
		{ u"LuminaTranslateSendLangAuto"_q, u"Язык собеседника"_q },
		{ u"LuminaTranslateBeforeSendConfirm"_q,
			u"Подтверждать перед отправкой"_q },
		{ u"LuminaTranslateSendInfo"_q, u"Исходящие сообщения переводятся на "
			u"выбранный выше язык, а оригинал сохраняется рядом с переводом. "
			u"С вариантом «Язык собеседника» LuminaGram один раз спрашивает в "
			u"каждом чате, какой язык там использовать, и запоминает ответ. "
			u"«Подтверждать перед отправкой» сначала показывает перевод рядом "
			u"с оригиналом, чтобы вы могли отправить любой из них; если этот "
			u"параметр выключен, перевод уходит сразу."_q },
		{ u"LuminaTranslateReceiveHeader"_q, u"Получение"_q },
		{ u"LuminaDualLanguageDisplay"_q,
			u"Показывать оригинал и перевод вместе"_q },
		{ u"LuminaTranslateReadLang"_q, u"Язык чтения"_q },
		{ u"LuminaTranslateReadLangFollow"_q, u"Как язык приложения"_q },
		{ u"LuminaTranslateModeHeader"_q,
			u"Переводить входящие сообщения"_q },
		{ u"LuminaTranslateModeAll"_q, u"Во всех чатах"_q },
		{ u"LuminaTranslateModeManual"_q, u"Только выбранные мной чаты"_q },
		{ u"LuminaTranslateReceiveInfo"_q, u"Входящие сообщения сохраняют "
			u"исходный текст в полном размере, а перевод показывается под "
			u"ним. В режиме «Во всех чатах» на каждое сообщение уходит "
			u"отдельный запрос к вашему сервису перевода — с платным ключом "
			u"лучше оставить «Только выбранные мной чаты»."_q },
		{ u"LuminaTranslateScopeHeader"_q, u"Область применения"_q },
		{ u"LuminaTranslateScopePrivate"_q, u"Личные чаты"_q },
		{ u"LuminaTranslateScopeGroup"_q, u"Группы и каналы"_q },
		{ u"LuminaTranslateScopeInfo"_q, u"Чаты вне области применения не "
			u"затрагиваются в обе стороны: они не переводятся при чтении, а "
			u"сообщения, которые вы туда отправляете, уходят без перевода. "
			u"Любое отдельное сообщение по-прежнему можно перевести "
			u"вручную."_q },
		{ u"LuminaTranslateProviderHeader"_q, u"Сервис"_q },
		{ u"LuminaTranslateProvider"_q, u"Сервис перевода"_q },
		{ u"LuminaTranslateProviderLlm"_q, u"LLM (совместимый с OpenAI)"_q },
		{ u"LuminaTranslateApiKey"_q, u"Ключ API"_q },
		{ u"LuminaTranslateApiKeyNotSet"_q, u"Не задан"_q },
		{ u"LuminaTranslateBaseUrl"_q, u"Базовый URL"_q },
		{ u"LuminaTranslateModel"_q, u"Модель"_q },
		{ u"LuminaTranslateSystemPrompt"_q, u"Системный промпт"_q },
		{ u"LuminaTranslatePromptDefault"_q, u"По умолчанию"_q },
		{ u"LuminaTranslatePromptCustom"_q, u"Свой"_q },
		{ u"LuminaTranslateFallbackTelegram"_q,
			u"Использовать Telegram при сбое этого сервиса"_q },
		{ u"LuminaTranslateProviderSecurityInfo"_q, u"Ключи хранятся только "
			u"на этом устройстве, в отдельном файле от остальных настроек, и "
			u"никогда не отправляются в Telegram. Всё, что вы переводите, "
			u"уходит выбранному здесь сервису, поэтому выбирайте тот, "
			u"которому доверяете."_q },

		// Translate before sending: the boxes and the send menu.
		{ u"LuminaTranslateOriginalLabel"_q, u"Оригинал"_q },
		{ u"LuminaSendTranslation"_q, u"Отправить перевод"_q },
		{ u"LuminaSendOriginal"_q, u"Отправить оригинал"_q },
		{ u"LuminaTrSendPickerTitle"_q, u"Язык перевода"_q },
		// One "{1}" is enough here: Russian names the language once and then
		// refers back to it, so repeating it would read as a mistake.
		{ u"LuminaTrSendConfirmMessage"_q, u"Похоже, в этом чате пишут на "
			u"языке {1}. Переводить на него сообщения, которые вы здесь "
			u"отправляете? LuminaGram запомнит выбор для этого чата."_q },
		{ u"LuminaTrSendConfirmTranslate"_q, u"Переводить"_q },
		{ u"LuminaTrSendAsTyped"_q, u"Отправить как есть"_q },
		{ u"LuminaTrSendChooseOther"_q, u"Выбрать язык"_q },

		// The translate-before-send preview bar above the composer.
		{ u"LuminaTranslatePreviewTranslating"_q, u"Перевод…"_q },
		{ u"LuminaTranslatePreviewFailed"_q, u"Перевод недоступен"_q },

		// Names of the languages LuminaGram can translate into.
		{ u"LuminaLangEn"_q, u"Английский"_q },
		{ u"LuminaLangZhTw"_q, u"Китайский (традиционный)"_q },
		{ u"LuminaLangZhCn"_q, u"Китайский (упрощённый)"_q },
		{ u"LuminaLangJa"_q, u"Японский"_q },
		{ u"LuminaLangKo"_q, u"Корейский"_q },
		{ u"LuminaLangEs"_q, u"Испанский"_q },
		{ u"LuminaLangFr"_q, u"Французский"_q },
		{ u"LuminaLangDe"_q, u"Немецкий"_q },
		{ u"LuminaLangRu"_q, u"Русский"_q },
		{ u"LuminaLangPtBr"_q, u"Португальский (Бразилия)"_q },
		{ u"LuminaLangPtPt"_q, u"Португальский (Португалия)"_q },
		{ u"LuminaLangIt"_q, u"Итальянский"_q },
		{ u"LuminaLangAr"_q, u"Арабский"_q },
		{ u"LuminaLangHi"_q, u"Хинди"_q },
		{ u"LuminaLangId"_q, u"Индонезийский"_q },
		{ u"LuminaLangTh"_q, u"Тайский"_q },
		{ u"LuminaLangVi"_q, u"Вьетнамский"_q },
		{ u"LuminaLangTr"_q, u"Турецкий"_q },
		{ u"LuminaLangPl"_q, u"Польский"_q },
		{ u"LuminaLangUk"_q, u"Украинский"_q },
		{ u"LuminaLangNl"_q, u"Нидерландский"_q },
		{ u"LuminaLangAf"_q, u"Африкаанс"_q },
		{ u"LuminaLangSq"_q, u"Албанский"_q },
		{ u"LuminaLangAm"_q, u"Амхарский"_q },
		{ u"LuminaLangHy"_q, u"Армянский"_q },
		{ u"LuminaLangAz"_q, u"Азербайджанский"_q },
		{ u"LuminaLangEu"_q, u"Баскский"_q },
		{ u"LuminaLangBe"_q, u"Белорусский"_q },
		{ u"LuminaLangBn"_q, u"Бенгальский"_q },
		{ u"LuminaLangBs"_q, u"Боснийский"_q },
		{ u"LuminaLangBg"_q, u"Болгарский"_q },
		{ u"LuminaLangMy"_q, u"Бирманский"_q },
		{ u"LuminaLangCa"_q, u"Каталанский"_q },
		{ u"LuminaLangHr"_q, u"Хорватский"_q },
		{ u"LuminaLangCs"_q, u"Чешский"_q },
		{ u"LuminaLangDa"_q, u"Датский"_q },
		{ u"LuminaLangEt"_q, u"Эстонский"_q },
		{ u"LuminaLangTl"_q, u"Филиппинский"_q },
		{ u"LuminaLangFi"_q, u"Финский"_q },
		{ u"LuminaLangGl"_q, u"Галисийский"_q },
		{ u"LuminaLangKa"_q, u"Грузинский"_q },
		{ u"LuminaLangEl"_q, u"Греческий"_q },
		{ u"LuminaLangGu"_q, u"Гуджарати"_q },
		{ u"LuminaLangHe"_q, u"Иврит"_q },
		{ u"LuminaLangHu"_q, u"Венгерский"_q },
		{ u"LuminaLangIs"_q, u"Исландский"_q },
		{ u"LuminaLangGa"_q, u"Ирландский"_q },
		{ u"LuminaLangJv"_q, u"Яванский"_q },
		{ u"LuminaLangKn"_q, u"Каннада"_q },
		{ u"LuminaLangKk"_q, u"Казахский"_q },
		{ u"LuminaLangKm"_q, u"Кхмерский"_q },
		{ u"LuminaLangKu"_q, u"Курдский"_q },
		{ u"LuminaLangKy"_q, u"Киргизский"_q },
		{ u"LuminaLangLo"_q, u"Лаосский"_q },
		{ u"LuminaLangLv"_q, u"Латышский"_q },
		{ u"LuminaLangLt"_q, u"Литовский"_q },
		{ u"LuminaLangMk"_q, u"Македонский"_q },
		{ u"LuminaLangMs"_q, u"Малайский"_q },
		{ u"LuminaLangMl"_q, u"Малаялам"_q },
		{ u"LuminaLangMr"_q, u"Маратхи"_q },
		{ u"LuminaLangMn"_q, u"Монгольский"_q },
		{ u"LuminaLangNe"_q, u"Непальский"_q },
		{ u"LuminaLangNo"_q, u"Норвежский"_q },
		{ u"LuminaLangPs"_q, u"Пушту"_q },
		{ u"LuminaLangFa"_q, u"Персидский"_q },
		{ u"LuminaLangPa"_q, u"Панджаби"_q },
		{ u"LuminaLangRo"_q, u"Румынский"_q },
		{ u"LuminaLangSr"_q, u"Сербский"_q },
		{ u"LuminaLangSi"_q, u"Сингальский"_q },
		{ u"LuminaLangSk"_q, u"Словацкий"_q },
		{ u"LuminaLangSl"_q, u"Словенский"_q },
		{ u"LuminaLangSo"_q, u"Сомалийский"_q },
		{ u"LuminaLangSw"_q, u"Суахили"_q },
		{ u"LuminaLangSv"_q, u"Шведский"_q },
		{ u"LuminaLangTg"_q, u"Таджикский"_q },
		{ u"LuminaLangTa"_q, u"Тамильский"_q },
		{ u"LuminaLangTe"_q, u"Телугу"_q },
		{ u"LuminaLangUr"_q, u"Урду"_q },
		{ u"LuminaLangUz"_q, u"Узбекский"_q },
		{ u"LuminaLangCy"_q, u"Валлийский"_q },
		{ u"LuminaLangYi"_q, u"Идиш"_q },
		{ u"LuminaLangZu"_q, u"Зулу"_q },
	};
}

[[maybe_unused]] const auto kRegistered = RegisterLocaleTable(
	"ru",
	Build);

} // namespace
} // namespace Lumina
