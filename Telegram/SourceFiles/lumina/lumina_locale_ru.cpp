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
		{ u"LuminaTranslateReceiveInfo"_q, u"Входящие сообщения сохраняют "
			u"исходный текст в полном размере, а перевод показывается под "
			u"ним."_q },
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
		{ u"LuminaTranslateTest"_q, u"Проверить перевод"_q },
		{ u"LuminaTranslateTestRunning"_q, u"Проверка…"_q },
		{ u"LuminaTranslateTestSuccess"_q, u"Перевод работает."_q },
		{ u"LuminaTranslateTestFailed"_q, u"Проверка не удалась"_q },
		{ u"LuminaTranslateNoKey"_q, u"Требуется ключ API"_q },
		{ u"LuminaTranslateTestKeyRejected"_q, u"Сервис недоступен или "
			u"отклонил этот ключ API. Сначала проверьте ключ, затем базовый "
			u"URL и подключение."_q },
		{ u"LuminaTranslateTestNetwork"_q, u"Не удалось связаться с "
			u"сервисом. Проверьте подключение к интернету и настройки прокси, "
			u"затем попробуйте снова."_q },
		{ u"LuminaTranslateTestQuota"_q, u"Сервис отклонил запрос: слишком "
			u"много обращений или квота этого ключа исчерпана. Попробуйте "
			u"позже."_q },
		{ u"LuminaTranslateTestQuotaKeyed"_q, u"Сервис отклонил запрос: он "
			u"отклонил этот ключ API, квота ключа исчерпана или обращений "
			u"было слишком много. Сначала проверьте ключ, затем попробуйте "
			u"позже."_q },
		{ u"LuminaTranslateTestBadResponse"_q, u"Сервис ответил тем, что "
			u"LuminaGram не смог прочитать, или не ответил вовсе. Проверьте "
			u"базовый URL и модель."_q },
		{ u"LuminaTranslateTestUnavailable"_q, u"Этот сервис здесь не "
			u"работает. Выберите другой выше."_q },
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

		// The per-chat translate button in the chat top bar. Both texts name
		// what pressing the button will do, not what the chat is doing now.
		{ u"LuminaTranslateChatToggle"_q, u"Перевести этот чат"_q },
		{ u"LuminaTranslateChatShowOriginal"_q, u"Показать оригинал"_q },

		// Pressing that button: the one place that states this conversation's
		// language pair, and the shortcut to the two rows on the translation
		// settings page that hold it. Each row names its side first -
		// "сообщений собеседника" and "моих сообщений" rather than an
		// incoming/outgoing pair - because the complaint these rows answer is
		// that the app never said whose messages were meant, and a direction
		// word does not say it. Off is a row of its own on each side, so the
		// value slot never has to hold a negation.
		//
		// The language is the whole value of each row, and it follows a colon
		// rather than a preposition: the "{1}" arrives in the nominative, and
		// on the "me" row it can be «Язык собеседника», which carries a "язык"
		// of its own. After "на язык" both of those break; after a colon
		// nothing has to agree with anything.
		{ u"LuminaChatLangThem"_q, u"Перевод сообщений собеседника: {1}"_q },
		{ u"LuminaChatLangThemOff"_q,
			u"Перевод сообщений собеседника: Выключен"_q },
		{ u"LuminaChatLangMe"_q, u"Перевод моих сообщений: {1}"_q },
		{ u"LuminaChatLangMeOff"_q, u"Перевод моих сообщений: Выключен"_q },
		{ u"LuminaChatLangThemTitle"_q, u"Перевод сообщений собеседника"_q },
		{ u"LuminaChatLangMeTitle"_q, u"Перевод моих сообщений"_q },
		{ u"LuminaChatLangNone"_q, u"Выключен"_q },

		// The tray menu and the taskbar button's jump list, the one menu that
		// named Telegram rather than this app: upstream builds both from
		// lng_open_from_tray / lng_quit_from_tray. The "{1}" is the app name,
		// always the literal "LuminaGram", and it never declines - the
		// genitive after "из" leaves a Latin name exactly as it is, which is
		// how Telegram itself words this item in Russian.
		{ u"LuminaTrayOpen"_q, u"Открыть {1}"_q },
		{ u"LuminaTrayQuit"_q, u"Выйти из {1}"_q },

		// Telegram's own AI editor, which carries a Translate tab of its own.
		// "Telegram" and "LuminaGram" are product names and stay as they are;
		// the tab is named here by the Russian name Telegram gives it.
		{ u"LuminaAiEditorHeader"_q, u"ИИ-редактор Telegram"_q },
		{ u"LuminaAiEditorKeep"_q, u"Оставить ИИ-редактор Telegram"_q },
		{ u"LuminaAiEditorInfo"_q, u"У Telegram в поле ввода есть "
			u"собственный ИИ-редактор, а в нём — вкладка «Перевод», которая "
			u"пересекается с переводом LuminaGram. Пока перевод LuminaGram "
			u"включён, кнопка этого редактора и его сочетание клавиш не "
			u"предлагаются, поэтому перед вами всегда только один инструмент "
			u"перевода. Включите этот параметр, чтобы редактор Telegram всё "
			u"равно оставался доступен. Когда перевод LuminaGram выключен, "
			u"редактор Telegram доступен всегда и этот параметр ничего не "
			u"меняет."_q },

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

		// Appearance: sticker size.
		{ u"LuminaAppearanceStickerSizeHeader"_q, u"Размер стикеров"_q },
		{ u"LuminaStickerSizeChoice"_q, u"{1}%"_q },
		{ u"LuminaStickerSizeChoiceDefault"_q, u"{1}% (по умолчанию)"_q },
		{ u"LuminaStickerSizeInfo"_q, u"Насколько крупно стикеры "
			u"рисуются в чатах — и отправленные, и полученные. "
			u"Анимированные эмодзи, кубики и подарочные стикеры "
			u"сохраняют свои собственные размеры. Telegram Desktop "
			u"измеряет стикер один раз и запоминает результат, поэтому "
			u"новый размер применяется при следующем запуске приложения."_q },

		// Encrypted local backup.
		{ u"LuminaBackupTitle"_q, u"Зашифрованная резервная копия"_q },
		{ u"LuminaBackupExport"_q, u"Экспортировать зашифрованную копию"_q },
		{ u"LuminaBackupExportInfo"_q, u"Сохраните всё, что LuminaGram "
			u"хранит на этом устройстве — закладки, заметки, шаблоны "
			u"ответов, замены текста и все настройки — в один файл, "
			u"зашифрованный выбранной вами парольной фразой. В файл "
			u"попадают и приватные значения: ключи API для перевода, "
			u"коды сейфа и ложного сбоя, заметка-обманка. Ничего не "
			u"отправляется в Telegram. Выберите длинную парольную фразу "
			u"и храните её в надёжном месте — без неё файл нельзя "
			u"открыть, и восстановить её невозможно."_q },
		{ u"LuminaBackupImport"_q, u"Импортировать копию"_q },
		{ u"LuminaBackupImportInfo"_q, u"Выберите файл резервной копии и "
			u"введите его парольную фразу, чтобы восстановить данные "
			u"LuminaGram. Настройки, которые есть в копии, заменяют "
			u"настройки на этом устройстве; то, чего в ней нет, остаётся "
			u"без изменений. Файл, который не удалось проверить, "
			u"отклоняется до того, как что-либо будет записано, поэтому "
			u"неверная парольная фраза никогда не оставит восстановление "
			u"наполовину."_q },
		{ u"LuminaBackupExportPassphraseTitle"_q,
			u"Задайте парольную фразу"_q },
		{ u"LuminaBackupPassphraseTitle"_q, u"Введите парольную фразу"_q },
		{ u"LuminaBackupPassphraseHint"_q, u"Парольная фраза"_q },
		{ u"LuminaBackupPassphraseRepeatHint"_q,
			u"Повторите парольную фразу"_q },
		{ u"LuminaBackupPassphraseMismatch"_q,
			u"Парольные фразы не совпадают."_q },
		{ u"LuminaBackupPassphraseTooShort"_q,
			u"Выберите парольную фразу не короче 4 символов."_q },
		{ u"LuminaBackupSaveCaption"_q,
			u"Сохранить резервную копию LuminaGram"_q },
		{ u"LuminaBackupOpenCaption"_q,
			u"Открыть резервную копию LuminaGram"_q },
		{ u"LuminaBackupFileFilter"_q,
			u"Резервная копия LuminaGram (*.lgbak)"_q },
		{ u"LuminaBackupExportDone"_q, u"Резервная копия сохранена."_q },
		{ u"LuminaBackupExportFailed"_q,
			u"Не удалось создать файл резервной копии."_q },
		{ u"LuminaBackupImportSuccess"_q, u"Копия восстановлена. "
			u"Перезапустите LuminaGram, чтобы применить все изменения."_q },
		{ u"LuminaBackupImportFailed"_q,
			u"Не удалось прочитать файл резервной копии."_q },
		{ u"LuminaBackupInvalidFile"_q,
			u"Это недействительный файл резервной копии LuminaGram."_q },
		{ u"LuminaBackupDamaged"_q, u"Файл резервной копии повреждён, "
			u"восстановить его не удалось."_q },
		{ u"LuminaBackupWrongPassphrase"_q,
			u"Неверная парольная фраза или файл был изменён."_q },
		{ u"LuminaBackupNewerFormat"_q, u"Эта резервная копия создана "
			u"более новой версией LuminaGram."_q },
		{ u"LuminaBackupUnauthenticated"_q, u"Эта резервная копия "
			u"сделана в старом незащищённом формате: её нельзя проверить "
			u"ни на неверную парольную фразу, ни на изменения. Создайте "
			u"новую копию в актуальной версии LuminaGram."_q },
		{ u"LuminaBackupCryptoFailed"_q,
			u"Шифрование недоступно в этой системе."_q },

		// Bookmarks.
		{ u"LuminaBookmarksTitle"_q, u"Закладки"_q },
		{ u"LuminaBookmarksAbout"_q, u"Закладки — это указатели на "
			u"сообщения; они хранятся только на этом устройстве и "
			u"никогда не отправляются в Telegram. Удаление закладки не "
			u"затрагивает само сообщение."_q },
		{ u"LuminaShowBookmarks"_q, u"Показывать пункт меню «В закладки»"_q },
		{ u"LuminaBookmarksList"_q, u"Сообщения в закладках"_q },
		{ u"LuminaBookmarksListAbout"_q, u"Нажмите на закладку, чтобы "
			u"открыть сообщение, или нажмите правой кнопкой, чтобы "
			u"удалить её. Закладка остаётся в этом списке, даже если "
			u"само сообщение удалено."_q },
		{ u"LuminaBookmarksNone"_q, u"Нет"_q },
		{ u"LuminaBookmarksEmpty"_q, u"У вас пока нет закладок."_q },
		{ u"LuminaBookmarksFull"_q, u"Список закладок заполнен. Удалите "
			u"закладку, чтобы добавить новую."_q },
		{ u"LuminaBookmark"_q, u"В закладки"_q },
		{ u"LuminaBookmarkAdded"_q, u"Добавлено в закладки"_q },
		{ u"LuminaBookmarkRemove"_q, u"Убрать из закладок"_q },
		{ u"LuminaBookmarkRemoved"_q, u"Удалено из закладок"_q },
		{ u"LuminaBookmarkDeleteTitle"_q, u"Удалить закладку?"_q },
		{ u"LuminaBookmarkChatUnavailable"_q, u"Чат недоступен"_q },
		{ u"LuminaBookmarkGone"_q,
			u"Этот чат больше недоступен на этом устройстве."_q },

		// Chat list: folders, stories, badges and avatar dots.
		{ u"LuminaChatListVisibilityTitle"_q, u"Папки и истории"_q },
		{ u"LuminaHideTabs"_q, u"Скрыть вкладки папок"_q },
		{ u"LuminaHideStories"_q, u"Скрыть истории"_q },
		{ u"LuminaChatListVisibilityAbout"_q, u"Убирает панель папок из "
			u"списка чатов — и вертикальную панель сбоку, и "
			u"горизонтальную сверху. Пока вкладки папок скрыты, вы "
			u"всегда видите все свои чаты: папка, в которой вы "
			u"находились, покидается, а сочетания клавиш для папок и "
			u"переключение между папками ничего не делают. Скрытие "
			u"историй убирает только строку над списком чатов; сами "
			u"истории и все остальные способы их открыть не меняются."_q },
		{ u"LuminaShowMutedCount"_q,
			u"Всегда показывать счётчик непрочитанных"_q },
		{ u"LuminaShowMutedCountInfo"_q, u"Рисует счётчик непрочитанных "
			u"у чатов без звука обычным акцентным цветом вместо "
			u"приглушённого серого."_q },
		{ u"LuminaChatListDensityTitle"_q, u"Плотность списка чатов"_q },
		{ u"LuminaCompactListRows"_q, u"Компактные строки"_q },
		{ u"LuminaCompactListRowsInfo"_q, u"Уменьшает высоту каждой строки "
			u"списка чатов, чтобы на экране помещалось больше чатов. "
			u"Аватары и превью сообщений остаются видимыми."_q },
		{ u"LuminaChatListDotsTitle"_q, u"Точки на аватарах"_q },
		{ u"LuminaChatListOnlineDot"_q, u"Точка «в сети»"_q },
		{ u"LuminaChatListOnlineDotInfo"_q, u"Показывает маленькую "
			u"зелёную точку на аватаре личных чатов, собеседник которых "
			u"сейчас в сети."_q },
		{ u"LuminaChatListRecencyDot"_q, u"Точка недавнего визита"_q },
		{ u"LuminaChatListRecencyDotInfo"_q, u"Окрашивает точку на "
			u"аватаре личных чатов в зависимости от того, как недавно "
			u"собеседник был в сети: зелёная — сейчас в сети, жёлтая — в "
			u"течение часа, оранжевая — в течение суток. Для более "
			u"давнего или скрытого времени визита точка не показывается. "
			u"Зелёную (в сети) точку по-прежнему включает переключатель "
			u"«Точка в сети»."_q },

		// Private contact notes and tags.
		{ u"LuminaContactNotesTitle"_q, u"Личные заметки о контактах"_q },
		{ u"LuminaContactNotesToggle"_q,
			u"Личные заметки и теги в профилях"_q },
		{ u"LuminaContactNotesAbout"_q, u"Добавляет в профиль человека "
			u"личную заметку и список тегов. И то, и другое остаётся на "
			u"этом устройстве, никогда не отправляется в Telegram и не "
			u"синхронизируется с другими вашими устройствами. "
			u"Собственные заметки о контактах в Telegram не "
			u"затрагиваются: личная заметка предлагается там, где их нет "
			u"— у ботов и у людей не из ваших контактов, — а теги "
			u"предлагаются везде."_q },
		{ u"LuminaContactNotesNone"_q, u"Нет"_q },
		{ u"LuminaContactNotesClear"_q, u"Удалить все личные заметки"_q },
		{ u"LuminaContactNotesClearTitle"_q, u"Удалить личные заметки"_q },
		{ u"LuminaContactNotesClearText"_q, u"Удалить все личные заметки "
			u"и теги, сохранённые на этом устройстве? Это действие "
			u"нельзя отменить."_q },
		{ u"LuminaContactNote"_q, u"Личная заметка"_q },
		{ u"LuminaContactNoteAbout"_q, u"Это видите только вы. Заметка "
			u"остаётся на этом устройстве, никогда не отправляется в "
			u"Telegram и не синхронизируется с другими вашими "
			u"устройствами."_q },
		{ u"LuminaContactNoteEmpty"_q,
			u"Нажмите, чтобы добавить личную заметку"_q },
		{ u"LuminaContactNoteHint"_q, u"Заметка (видна только вам)"_q },
		{ u"LuminaContactTags"_q, u"Теги"_q },
		{ u"LuminaContactTagsEmpty"_q, u"Нажмите, чтобы добавить теги"_q },
		{ u"LuminaContactTagsHint"_q, u"Теги через запятую"_q },

		// Message actions: forwarding, details, saving, select from author.
		{ u"LuminaMessageActionsInfo"_q, u"Эти пункты появляются только "
			u"в контекстном меню сообщения. Каждый из них использует "
			u"пересылку самого Telegram, поэтому чат с запретом "
			u"пересылки остаётся защищённым. «Пересылать без автора и "
			u"подписей» назван по тому, что действительно уходит: "
			u"Telegram не умеет убирать подписи, сохраняя отправителя, "
			u"поэтому этот вариант убирает и то, и другое. «Подробности» "
			u"читают только то, что это устройство уже синхронизировало, "
			u"— ничего не запрашивается и ничего не сохраняется."_q },
		{ u"LuminaForwardNoAuthor"_q, u"Пересылать без автора"_q },
		{ u"LuminaForwardNoAuthorTitle"_q, u"Пересылать без автора"_q },
		{ u"LuminaForwardNoCaption"_q,
			u"Пересылать без автора и подписей"_q },
		{ u"LuminaForwardNoCaptionTitle"_q,
			u"Пересылать без автора и подписей"_q },
		{ u"LuminaSendOriginalCaption"_q, u"Отправить исходную подпись"_q },
		{ u"LuminaSaveToCloud"_q, u"Сохранить в «Избранное»"_q },
		{ u"LuminaSaveToCloudTitle"_q, u"Сохранить в «Избранное»"_q },
		{ u"LuminaShowMessageDetails"_q, u"Подробности сообщения"_q },
		{ u"LuminaMessageDetails"_q, u"Подробности"_q },
		{ u"LuminaDetailsFrom"_q, u"От"_q },
		{ u"LuminaDetailsDate"_q, u"Дата"_q },
		{ u"LuminaDetailsMessageId"_q, u"ID сообщения"_q },
		{ u"LuminaDetailsForwardedFrom"_q, u"Переслано от"_q },
		{ u"LuminaDetailsOriginalDate"_q, u"Исходная дата"_q },
		{ u"LuminaSelectFromAuthor"_q, u"Выбрать все сообщения автора"_q },
		{ u"LuminaSelectFromAuthorTitle"_q,
			u"Выбрать все сообщения автора"_q },
		{ u"LuminaSelectFromAuthorAbout"_q, u"Добавляет в меню сообщения "
			u"в группах пункт, который выделяет все сообщения нажатого "
			u"отправителя. Он охватывает только сообщения, уже "
			u"загруженные в этом окне, — прокрутите дальше назад и "
			u"повторите, чтобы захватить более старые. У сообщений, "
			u"приписанных самому чату — постов канала и постов анонимных "
			u"администраторов, — этого пункта нет."_q },
		{ u"LuminaSelectFromAuthorLimit"_q,
			u"За один раз можно выделить не более {1} сообщений."_q },

		// Numbers and message time.
		{ u"LuminaExactNumbers"_q,
			u"Показывать точные числа (без округления 1.2K)"_q },
		{ u"LuminaExactNumbersInfo"_q, u"Показывать полные значения, "
			u"например 1 234 567, вместо сокращённых форм вроде 1.2M. "
			u"Счётчики, уже нарисованные на экране, сохраняют текущий "
			u"вид до перезапуска приложения."_q },
		{ u"LuminaTimeWithSeconds"_q,
			u"Показывать секунды во времени сообщения"_q },
		{ u"LuminaTimeWithSecondsAbout"_q, u"Время под сообщением, время "
			u"в скопированном тексте и время, которое произносит "
			u"программа чтения с экрана, — везде включаются секунды."_q },

		// Link safety inspector.
		{ u"LuminaLinkSafetyRow"_q, u"Проверка безопасности ссылок"_q },
		{ u"LuminaLinkSafetyInfo"_q, u"Спрашивать перед открытием "
			u"ссылки, которая скрывает настоящий адрес за текстом перед "
			u"знаком «@», записывает домен в punycode или ведёт через "
			u"известный сокращатель ссылок. В подтверждении показываются "
			u"настоящий домен назначения и полный адрес. Telegram "
			u"Desktop и сам предупреждает о похожих доменах, записанных "
			u"буквами других алфавитов. Ничего не проверяется в "
			u"интернете — ни один открываемый вами адрес не покидает это "
			u"устройство."_q },
		{ u"LuminaLinkSafetyTitle"_q, u"Открыть внешнюю ссылку?"_q },
		{ u"LuminaLinkSafetyDestination"_q, u"Настоящий адрес"_q },
		{ u"LuminaLinkSafetyWarnMismatch"_q, u"Эта ссылка скрывает "
			u"настоящий адрес за текстом перед знаком «@»."_q },
		{ u"LuminaLinkSafetyWarnPunycode"_q, u"В этом адресе "
			u"используются закодированные символы (punycode), способные "
			u"имитировать известный сайт."_q },
		{ u"LuminaLinkSafetyWarnShortener"_q, u"Это сокращатель ссылок — "
			u"настоящий адрес скрыт, пока вы не откроете его."_q },

		// Media: background pause and sticker saving.
		{ u"LuminaMediaTitle"_q, u"Медиа"_q },
		{ u"LuminaMediaAutoPauseBgVideo"_q,
			u"Ставить видео на паузу при сворачивании приложения"_q },
		{ u"LuminaMediaAutoPauseBgVideoInfo"_q, u"Автоматически ставить "
			u"воспроизводимое видео на паузу, когда вы покидаете "
			u"LuminaGram. Сворачивание просмотрщика тоже ставит его на "
			u"паузу. Видео, которое вы намеренно вынесли в отдельное "
			u"окно или в режим «картинка в картинке», продолжает играть, "
			u"и музыка с голосовыми сообщениями тоже продолжает играть, "
			u"пока вас нет."_q },
		{ u"LuminaMediaSaving"_q, u"Медиа"_q },
		{ u"LuminaSaveStickers"_q, u"Сохранять стикеры"_q },
		{ u"LuminaSaveStickersInfo"_q, u"Добавляет пункт «Сохранить "
			u"стикер» в контекстное меню панели стикеров. Стикеры "
			u"сохраняются туда же, куда и остальные загрузки, а для "
			u"собственного набора стикеров группы этот пункт скрывается, "
			u"если группа ограничивает сохранение."_q },
		{ u"LuminaSaveSticker"_q, u"Сохранить стикер"_q },
		{ u"LuminaStickerSavedTo"_q, u"Стикер сохранён в {1}"_q },

		// Recent stickers and saved GIFs. "Unlimited recent stickers" is an
		// experimental setting of Telegram Desktop and is never translated
		// there, so it has to stay English here for it to be findable.
		{ u"LuminaRecentLimitsRow"_q,
			u"Хранить больше недавних стикеров и GIF"_q },
		{ u"LuminaRecentLimitsInfo"_q, u"Не даёт этому устройству "
			u"выбрасывать недавние стикеры и сохранённые GIF раньше, чем "
			u"необходимо, с теми же пределами, что и у LuminaGram на "
			u"Android: до 200 недавних стикеров и 500 сохранённых GIF. "
			u"Сколько их на самом деле хранится в вашем аккаунте и "
			u"синхронизируется с другими вашими устройствами, "
			u"по-прежнему решают серверы Telegram, поэтому облачный "
			u"список от этого больше не станет — при следующей "
			u"синхронизации всё, чего сервер больше не хранит, исчезнет "
			u"и здесь, обычно за считаные секунды. Пока параметр "
			u"включён, Telegram Desktop перестаёт сам удалять "
			u"сохранённые GIF, поэтому его напоминание Premium о лимите "
			u"сохранённых GIF не появляется. В панели стикеров "
			u"по-прежнему видны только первые 20 недавних стикеров, если "
			u"вы дополнительно не включите «Unlimited recent stickers» в "
			u"разделе «Настройки › Продвинутые настройки › "
			u"Экспериментальные настройки»."_q },

		// The first-run card.
		{ u"LuminaOnboardingHeader"_q, u"О программе"_q },
		{ u"LuminaOnboardingRow"_q, u"Что добавляет LuminaGram"_q },
		{ u"LuminaOnboardingRowAbout"_q, u"Показывает приветственную "
			u"карточку, которая появляется при первом открытии настроек "
			u"LuminaGram. Она только рассказывает, что здесь есть, и "
			u"ничего не включает."_q },
		{ u"LuminaOnboardingTitle"_q, u"Добро пожаловать в LuminaGram"_q },
		{ u"LuminaOnboardingIntro"_q, u"LuminaGram добавляет собственные "
			u"инструменты поверх Telegram Desktop. Все параметры ниже "
			u"хранятся только на этом компьютере и никогда не "
			u"синхронизируются с Telegram."_q },
		{ u"LuminaOnboardingTranslateText"_q, u"Переводите входящие "
			u"сообщения через собственный сервис перевода и переводите "
			u"то, что печатаете, перед отправкой."_q },
		{ u"LuminaOnboardingDualName"_q, u"Оба языка сразу"_q },
		{ u"LuminaOnboardingDualText"_q, u"Оригинальный текст остаётся "
			u"на экране рядом с переводом — и для полученных сообщений, "
			u"и для отправленных."_q },
		{ u"LuminaOnboardingVaultName"_q, u"Сейф маскировки"_q },
		{ u"LuminaOnboardingVaultText"_q, u"Спрячьте LuminaGram за "
			u"калькулятором или заметками, которые открывают настоящее "
			u"приложение только по вашему секретному коду."_q },
		{ u"LuminaOnboardingSafetyName"_q, u"Проверки безопасности"_q },
		{ u"LuminaOnboardingSafetyText"_q, u"Предупреждает перед "
			u"открытием ссылки, адрес которой не тот, чем кажется, "
			u"замечает подменённый крипто-адрес при вставке и удаляет "
			u"геоданные из отправляемых фотографий."_q },
		{ u"LuminaOnboardingFooter"_q, u"Откройте нужный раздел на "
			u"странице настроек LuminaGram, чтобы всё это настроить. Эту "
			u"карточку можно открыть снова из раздела «Инструменты»."_q },
		{ u"LuminaOnboardingGotIt"_q, u"Понятно"_q },

		// The local profile card.
		{ u"LuminaProfileCardTitle"_q, u"Визитка"_q },
		{ u"LuminaProfileCardAbout"_q, u"Локальная визитка о вас — на "
			u"каких языках вы говорите, чем увлекаетесь, — которую можно "
			u"скопировать и вставить в чат. Она остаётся на этом "
			u"устройстве."_q },
		{ u"LuminaProfileCardEdit"_q, u"Моя визитка"_q },
		{ u"LuminaProfileCardHeader"_q, u"Моя визитка"_q },
		{ u"LuminaProfileCardInfo"_q, u"Эта визитка хранится только на "
			u"этом устройстве и никогда не отправляется в Telegram. Она "
			u"не меняет ваш профиль Telegram."_q },
		{ u"LuminaProfileCardNotSet"_q, u"Не задано"_q },
		{ u"LuminaProfileCardTagline"_q, u"Слоган"_q },
		{ u"LuminaProfileCardTaglineHint"_q, u"Короткая фраза о себе"_q },
		{ u"LuminaProfileCardBio"_q, u"О себе"_q },
		{ u"LuminaProfileCardBioHint"_q, u"Несколько слов о себе"_q },
		{ u"LuminaProfileCardLanguages"_q, u"Языки, на которых я говорю"_q },
		{ u"LuminaProfileCardLanguagesHint"_q,
			u"например: Русский, English, Español"_q },
		{ u"LuminaProfileCardInterests"_q, u"Интересы / теги"_q },
		{ u"LuminaProfileCardInterestsHint"_q,
			u"например: музыка, походы, программирование"_q },
		{ u"LuminaProfileCardCopy"_q, u"Скопировать в буфер обмена"_q },
		{ u"LuminaProfileCardCopyInfo"_q, u"Создаёт текстовую сводку "
			u"вашей визитки. Вставьте её в любой чат, когда захотите "
			u"поделиться."_q },
		{ u"LuminaProfileCardCopied"_q, u"Визитка скопирована"_q },
		{ u"LuminaProfileCardEmptyShare"_q, u"Сначала заполните визитку"_q },

		// Extra rows on profile pages.
		{ u"LuminaProfileInfoHeader"_q, u"Профиль"_q },
		{ u"LuminaProfileInfoInfo"_q, u"Дополнительные строки на "
			u"страницах профилей, все вычисляются на этом устройстве — у "
			u"Telegram ничего не запрашивается. Для некоторых аккаунтов "
			u"дату регистрации сообщает сам Telegram; для остальных она "
			u"оценивается по номеру аккаунта и показывается со знаком "
			u"«~». Дата-центр — это тот, где хранится фотография "
			u"профиля. Дата создания — это когда была создана группа или "
			u"канал."_q },
		{ u"LuminaProfileShowRegistrationDate"_q,
			u"Показывать дату регистрации"_q },
		{ u"LuminaProfileRegistrationDate"_q, u"Дата регистрации"_q },
		{ u"LuminaProfileRegistrationApprox"_q, u"~ {1}"_q },
		{ u"LuminaProfileShowDcId"_q, u"Показывать дата-центр"_q },
		{ u"LuminaProfileDcId"_q, u"Дата-центр"_q },
		{ u"LuminaProfileDcIdValue"_q, u"DC{1}"_q },
		{ u"LuminaProfileShowChatDate"_q, u"Показывать дату создания"_q },
		{ u"LuminaProfileChatCreated"_q, u"Создан"_q },

		// Reply templates.
		{ u"LuminaReplyTemplatesTitle"_q, u"Шаблоны ответов"_q },
		{ u"LuminaReplyTemplatesAbout"_q, u"Короткие фрагменты текста, "
			u"которые хранятся на этом устройстве и вставляются в поле "
			u"сообщения. Нажмите правой кнопкой на кнопку эмодзи в чате, "
			u"чтобы выбрать шаблон. Шаблоны никогда не отправляются в "
			u"Telegram."_q },
		{ u"LuminaReplyTemplatesShow"_q, u"Предлагать шаблоны в чатах"_q },
		{ u"LuminaReplyTemplatesManage"_q, u"Управление шаблонами…"_q },
		{ u"LuminaReplyTemplatesList"_q, u"Шаблоны"_q },
		{ u"LuminaReplyTemplatesInfo"_q, u"Нажмите правой кнопкой на "
			u"кнопку эмодзи в чате, чтобы вставить шаблон. Нажмите "
			u"правой кнопкой на шаблон здесь, чтобы переместить его "
			u"вверх или вниз; откройте шаблон, чтобы изменить или "
			u"удалить его."_q },
		{ u"LuminaReplyTemplatesNone"_q, u"Нет"_q },
		{ u"LuminaReplyTemplatesEmpty"_q, u"Шаблонов пока нет. Добавьте "
			u"шаблон, затем нажмите правой кнопкой на кнопку эмодзи в "
			u"чате, чтобы вставить его."_q },
		{ u"LuminaReplyTemplatesFull"_q,
			u"Список заполнен — удалите шаблон, чтобы добавить новый."_q },
		{ u"LuminaReplyTemplatesAdd"_q, u"Добавить шаблон"_q },
		{ u"LuminaReplyTemplatesEdit"_q, u"Изменить шаблон"_q },
		{ u"LuminaReplyTemplatesPlaceholder"_q, u"Текст шаблона"_q },
		{ u"LuminaReplyTemplatesMoveUp"_q, u"Переместить вверх"_q },
		{ u"LuminaReplyTemplatesMoveDown"_q, u"Переместить вниз"_q },

		// Panic wipe. The "\n" pairs are spelled exactly as the English
		// table spells them, so both languages lay the text out the same.
		{ u"LuminaSecurityPanicHeader"_q, u"Экстренная очистка"_q },
		{ u"LuminaSecurityPanicWipe"_q, u"Экстренная очистка (Kaboom)"_q },
		{ u"LuminaSecurityPanicWipeAbout"_q, u"Выходит из всех аккаунтов "
			u"на этом устройстве и стирает локальную базу сообщений, "
			u"черновики и кэш медиафайлов вместе с собственными "
			u"настройками LuminaGram, закладками, сохранёнными "
			u"переводами и ключами API. Ваши аккаунты и ваши сообщения "
			u"остаются на серверах Telegram. Уже загруженные файлы "
			u"остаются на месте. Это действие нельзя отменить."_q },
		{ u"LuminaSecurityPanicConfirmTitle"_q, u"Экстренная очистка?"_q },
		{ u"LuminaSecurityPanicConfirmText"_q, u"Все аккаунты на этом "
			u"устройстве будут выведены из системы. Локальная база "
			u"сообщений, черновики и кэш медиафайлов будут стёрты вместе "
			u"с собственными настройками LuminaGram, закладками, "
			u"сохранёнными переводами и ключами API.\n\nВаши аккаунты "
			u"не удаляются. Они остаются на серверах Telegram, как и "
			u"ваши сообщения, — вы сможете войти снова откуда "
			u"угодно.\n\nУже загруженные файлы НЕ удаляются. "
			u"LuminaGram не трогает вашу папку загрузок, потому что "
			u"обычно это ваша системная папка «Загрузки», в которой лежат "
			u"посторонние файлы. Всё важное оттуда перенесите или "
			u"удалите сами.\n\nЭто действие нельзя отменить."_q },
		{ u"LuminaSecurityPanicConfirmAck"_q,
			u"Я понимаю, что это нельзя отменить"_q },
		{ u"LuminaSecurityPanicConfirmButton"_q, u"Стереть сейчас"_q },

		// Undo send. The one "{1}" is the length of the window in seconds,
		// so it is followed by the invariant "сек." rather than a form that
		// would only agree with one number.
		{ u"LuminaUndoSendTitle"_q, u"Отмена отправки"_q },
		{ u"LuminaUndoSendWindow"_q, u"Окно отмены отправки"_q },
		{ u"LuminaUndoSendWindowInfo"_q, u"Задерживает обычное текстовое "
			u"сообщение на {1} сек. за кнопкой «Отмена», прежде чем оно "
			u"уйдёт. Ваш текст всё это время остаётся в поле ввода, и "
			u"поле очищается только когда сообщение действительно "
			u"отправлено, поэтому «Отмена» просто оставляет текст на "
			u"месте — ничего не убирается и не возвращается обратно. "
			u"Повторная отправка, переход в другой чат или выход из "
			u"приложения отправляют задержанное сообщение сразу. Медиа, "
			u"голосовые, изменённые, пересланные и отложенные сообщения "
			u"никогда не задерживаются, как и сообщения из темы форума "
			u"или ветки комментариев."_q },
		{ u"LuminaUndoSendBulletin"_q, u"Отправка сообщения…"_q },
		{ u"LuminaUndoSendUndo"_q, u"Отмена"_q },
		// clipboard guard + scam watch
		{ u"LuminaPrivacyCryptoClipboardGuard"_q, u"Защита при вставке "
			u"крипто-адреса"_q },
		{ u"LuminaPrivacyCryptoClipboardGuardInfo"_q, u"Спрашивать перед "
			u"вставкой в сообщение текста, похожего на адрес криптокошелька. "
			u"Вредоносные программы, перехватывающие буфер обмена, могут "
			u"незаметно подменить скопированный адрес на адрес мошенника, а "
			u"вставка — последний момент, когда это можно заметить. Проверка "
			u"выполняется на этом устройстве, и никуда ничего не "
			u"отправляется."_q },
		{ u"LuminaPrivacyScamKeywordWarning"_q, u"Предупреждение о "
			u"мошенничестве"_q },
		{ u"LuminaPrivacyScamKeywordWarningInfo"_q, u"Когда в сообщении от "
			u"человека не из ваших контактов упоминаются типичные уловки "
			u"мошенников — переводы денег, подарочные карты, «инвестиции» в "
			u"криптовалюту, плата за верификацию или просьба прислать код — "
			u"показывается однократное напоминание быть осторожнее. Сообщение "
			u"никогда не блокируется и не изменяется. Проверка выполняется "
			u"офлайн на вашем устройстве."_q },
	};
}

[[maybe_unused]] const auto kRegistered = RegisterLocaleTable(
	"ru",
	Build);

} // namespace
} // namespace Lumina
