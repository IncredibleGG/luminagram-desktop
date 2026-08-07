/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_locale.h"

namespace Lumina {
namespace {

// Spanish. Where LuminaGram says the same thing on both platforms the wording
// is Android's LuminaLocale.java, so the two can be compared key for key.
//
// The divider paragraphs quote the rows they talk about; Spanish quotes those
// with angle quotes, and the text inside them has to keep matching the row it
// names - LuminaTranslateSendLangAuto, LuminaTranslateBeforeSendConfirm,
// LuminaAllowSaveRestricted, LuminaForwardNoCaptionTitle and
// LuminaMessageDetails (both quoted by LuminaMessageActionsInfo),
// LuminaSaveSticker (quoted by LuminaSaveStickersInfo), LuminaUndoSendUndo
// (quoted by LuminaUndoSendWindowInfo) and LuminaChatListOnlineDot (named by
// LuminaChatListRecencyDotInfo).
//
// Every literal carries the u prefix, continuation lines included: these
// strings are not ASCII, and concatenating a u"" literal with an unprefixed
// one leaves the re-encoding of the unprefixed half up to the compiler.
[[nodiscard]] LocaleTable Build() {
	return {
		// Sections of the LuminaGram settings page.
		{ u"LuminaGramTitle"_q, u"LuminaGram"_q },
		{ u"LuminaTranslateTitle"_q, u"Traducción"_q },
		{ u"LuminaPrivacyTitle"_q, u"Privacidad"_q },
		{ u"LuminaSecurityTitle"_q, u"Seguridad"_q },
		{ u"LuminaChatSettings"_q, u"Chats"_q },
		{ u"LuminaGramChatList"_q, u"Lista de chats"_q },
		{ u"LuminaAppearanceTitle"_q, u"Apariencia"_q },
		{ u"LuminaToolsTitle"_q, u"Herramientas"_q },
		{ u"LuminaGramStoredLocallyInfo"_q, u"Las opciones de LuminaGram se "
			u"guardan solo en este dispositivo y nunca se sincronizan con "
			u"Telegram."_q },

		// Sub-pages that have no rows yet.
		{ u"LuminaAppearancePlaceholder"_q, u"Aquí aparecerán las opciones de "
			u"formato de mensajes, stickers y números."_q },
		{ u"LuminaChatListPlaceholder"_q, u"Aquí aparecerán las opciones de "
			u"diseño e insignias de la lista de chats."_q },
		{ u"LuminaPrivacyPlaceholder"_q, u"Aquí aparecerán las opciones de "
			u"privacidad de enlaces, portapapeles y multimedia saliente."_q },
		{ u"LuminaSecurityPlaceholder"_q, u"Aquí aparecerán las opciones de "
			u"caja fuerte de disfraz, borrado de emergencia y desbloqueo bajo "
			u"coacción."_q },
		{ u"LuminaToolsPlaceholder"_q, u"Aquí aparecerán los marcadores, las "
			u"plantillas de respuesta y la copia de seguridad local."_q },

		// Chats sub-page.
		{ u"LuminaMessageActions"_q, u"Acciones de mensajes"_q },
		{ u"LuminaAllowSaveRestricted"_q,
			u"Permitir guardar / copiar en chats restringidos"_q },
		{ u"LuminaAllowSaveRestrictedInfo"_q, u"«Permitir guardar / copiar en "
			u"chats restringidos» solo afecta a las acciones locales en este "
			u"dispositivo. Algunos chats restringen el guardado por algún "
			u"motivo: úsalo con responsabilidad."_q },

		// Translation sub-page.
		{ u"LuminaTranslateEnable"_q,
			u"Activar la traducción de LuminaGram"_q },
		{ u"LuminaTranslateEnableInfo"_q, u"Traduce con tu propio motor en "
			u"lugar del servicio Premium de Telegram. El motor predeterminado "
			u"no necesita cuenta ni clave API. Mientras esto esté "
			u"desactivado, LuminaGram deja la traducción exactamente como "
			u"viene en Telegram Desktop."_q },
		{ u"LuminaTranslateSendHeader"_q, u"Envío"_q },
		{ u"LuminaTranslateBeforeSend"_q, u"Traducir antes de enviar"_q },
		{ u"LuminaTranslateSendLang"_q, u"Idioma de envío"_q },
		{ u"LuminaTranslateSendLangAuto"_q, u"Idioma del destinatario"_q },
		{ u"LuminaTranslateBeforeSendConfirm"_q,
			u"Confirmar antes de enviar"_q },
		{ u"LuminaTranslateSendInfo"_q, u"Los mensajes que envías se traducen "
			u"al idioma de arriba y el original se conserva junto a la "
			u"traducción. Con «Idioma del destinatario», LuminaGram pregunta "
			u"una vez en cada chat qué idioma usar allí y luego lo recuerda. "
			u"«Confirmar antes de enviar» muestra primero la traducción junto "
			u"al original, para que puedas enviar cualquiera de los dos; si "
			u"está desactivado, la traducción se envía directamente."_q },
		{ u"LuminaTranslateReceiveHeader"_q, u"Recepción"_q },
		{ u"LuminaDualLanguageDisplay"_q,
			u"Mostrar el original y la traducción juntos"_q },
		{ u"LuminaTranslateReadLang"_q, u"Idioma de lectura"_q },
		{ u"LuminaTranslateReadLangFollow"_q, u"Idioma de la interfaz"_q },
		{ u"LuminaTranslateReceiveInfo"_q, u"Los mensajes entrantes conservan "
			u"su texto original a tamaño completo, con la traducción "
			u"debajo."_q },
		{ u"LuminaTranslateProviderHeader"_q, u"Servicio"_q },
		{ u"LuminaTranslateProvider"_q, u"Servicio de traducción"_q },
		// The other provider names are the services' own brands and are not
		// translated; this one names a kind of endpoint.
		{ u"LuminaTranslateProviderLlm"_q, u"LLM (compatible con OpenAI)"_q },
		{ u"LuminaTranslateApiKey"_q, u"Clave API"_q },
		{ u"LuminaTranslateApiKeyNotSet"_q, u"Sin definir"_q },
		{ u"LuminaTranslateBaseUrl"_q, u"URL base"_q },
		{ u"LuminaTranslateModel"_q, u"Modelo"_q },
		{ u"LuminaTranslateSystemPrompt"_q, u"Instrucción del sistema"_q },
		{ u"LuminaTranslatePromptDefault"_q, u"Predeterminada"_q },
		{ u"LuminaTranslatePromptCustom"_q, u"Personalizada"_q },
		{ u"LuminaTranslateFallbackTelegram"_q,
			u"Recurrir a Telegram cuando este servicio falle"_q },
		{ u"LuminaTranslateTest"_q, u"Probar traducción"_q },
		{ u"LuminaTranslateTestRunning"_q, u"Probando…"_q },
		{ u"LuminaTranslateTestSuccess"_q, u"La traducción funciona."_q },
		{ u"LuminaTranslateTestFailed"_q, u"La prueba falló"_q },
		{ u"LuminaTranslateNoKey"_q, u"Se requiere una clave API"_q },
		{ u"LuminaTranslateTestKeyRejected"_q, u"No se pudo contactar con el "
			u"servicio, o rechazó esta clave API. Revisa primero la clave y "
			u"después la URL base y tu conexión."_q },
		{ u"LuminaTranslateTestNetwork"_q, u"No se pudo contactar con el "
			u"servicio. Revisa tu conexión a internet y el proxy, si lo "
			u"usas, e inténtalo de nuevo."_q },
		{ u"LuminaTranslateTestQuota"_q, u"El servicio rechazó la petición: "
			u"demasiadas peticiones, o la cuota de esta clave se agotó. "
			u"Inténtalo más tarde."_q },
		{ u"LuminaTranslateTestQuotaKeyed"_q, u"El servicio rechazó la "
			u"petición: rechazó esta clave API, la cuota de la clave se "
			u"agotó, o hubo demasiadas peticiones. Revisa primero la clave y "
			u"vuelve a intentarlo más tarde."_q },
		{ u"LuminaTranslateTestBadResponse"_q, u"El servicio respondió con "
			u"algo que LuminaGram no pudo leer, o no respondió nada. Revisa "
			u"la URL base y el modelo."_q },
		{ u"LuminaTranslateTestUnavailable"_q, u"Este servicio no puede "
			u"funcionar aquí. Elige otro más arriba."_q },
		{ u"LuminaTranslateProviderSecurityInfo"_q, u"Las claves se guardan "
			u"solo en este dispositivo, en un archivo aparte del resto de los "
			u"ajustes, y nunca se envían a Telegram. Todo lo que traduzcas se "
			u"envía al servicio seleccionado aquí, así que elige uno en el "
			u"que confíes."_q },

		// Translate before sending: the boxes and the send menu.
		{ u"LuminaTranslateOriginalLabel"_q, u"Original"_q },
		{ u"LuminaSendTranslation"_q, u"Enviar traducción"_q },
		{ u"LuminaSendOriginal"_q, u"Enviar original"_q },
		{ u"LuminaTrSendPickerTitle"_q, u"Idioma de traducción"_q },
		{ u"LuminaTrSendConfirmMessage"_q, u"Parece que este chat está "
			u"escrito en {1}. ¿Traducir a {1} los mensajes que envíes aquí? "
			u"LuminaGram lo recordará para este chat."_q },
		{ u"LuminaTrSendConfirmTranslate"_q, u"Traducir"_q },
		{ u"LuminaTrSendAsTyped"_q, u"Enviar sin traducir"_q },
		{ u"LuminaTrSendChooseOther"_q, u"Elegir otro idioma"_q },

		// The translate-before-send preview bar above the composer.
		{ u"LuminaTranslatePreviewTranslating"_q, u"Traduciendo…"_q },
		{ u"LuminaTranslatePreviewFailed"_q, u"Traducción no disponible"_q },

		// The per-chat translate button in the chat top bar. Both texts name
		// what pressing the button will do, not what the chat is doing now,
		// because that is what a screen reader reads out before the press.
		{ u"LuminaTranslateChatToggle"_q, u"Traducir este chat"_q },
		{ u"LuminaTranslateChatShowOriginal"_q, u"Mostrar original"_q },

		// Pressing that button: the one place that states this conversation's
		// language pair. Each row names its side first - «La otra persona»
		// and «Yo», never a direction word - because the complaint these rows
		// answer is that «entrantes» and «salientes» never said whose
		// messages were meant. Off is a row of its own per side, worded like
		// LuminaChatLangNone - the entry inside the language list that turns
		// that half off - so the row and the list say the same thing.
		//
		// {1} is the whole value of the row, and holding it after a colon is
		// what keeps every value grammatical: a participle after «La otra
		// persona,» would agree with the person rather than the messages,
		// and «Idioma del destinatario», which the outgoing half puts here
		// when the send language is asked per chat, would need the article
		// «al» that a bare {1} cannot carry.
		{ u"LuminaChatLangThem"_q, u"La otra persona, traducción: {1}"_q },
		{ u"LuminaChatLangThemOff"_q, u"La otra persona, sin traducir"_q },
		{ u"LuminaChatLangMe"_q, u"Yo, traducción: {1}"_q },
		{ u"LuminaChatLangMeOff"_q, u"Yo, sin traducir"_q },
		{ u"LuminaChatLangThemTitle"_q, u"Traducción de los mensajes de la "
			u"otra persona"_q },
		{ u"LuminaChatLangMeTitle"_q, u"Traducción de mis mensajes"_q },
		{ u"LuminaChatLangNone"_q, u"Sin traducir"_q },

		// Telegram's own AI editor, which carries a Translate tab of its
		// own. Its rows live on the Chats sub-page, but everything the
		// divider text says is about the overlap with our translation, so
		// they sit with the translation rows, as they do in English.
		{ u"LuminaAiEditorHeader"_q, u"Editor con IA de Telegram"_q },
		{ u"LuminaAiEditorKeep"_q,
			u"Mantener el editor con IA de Telegram"_q },
		{ u"LuminaAiEditorInfo"_q, u"Telegram tiene su propio editor con IA "
			u"en el campo del mensaje, con una pestaña Traducir que se "
			u"solapa con la traducción de LuminaGram. Mientras la traducción "
			u"de LuminaGram está activada, no se ofrecen ni el botón ni el "
			u"atajo de teclado de ese editor, así que nunca tienes delante "
			u"más de una herramienta de traducción. Activa esto para "
			u"mantener disponible el editor de Telegram de todos modos. Con "
			u"la traducción de LuminaGram desactivada, el editor de Telegram "
			u"está siempre ahí y esta opción no cambia nada."_q },

		// Names of the languages LuminaGram can translate into, in the order
		// the pickers show them. Spanish writes language names in lower case
		// inside a sentence, but these are list entries, so they are
		// capitalised the way every language list on the system is.
		{ u"LuminaLangEn"_q, u"Inglés"_q },
		{ u"LuminaLangZhTw"_q, u"Chino (tradicional)"_q },
		{ u"LuminaLangZhCn"_q, u"Chino (simplificado)"_q },
		{ u"LuminaLangJa"_q, u"Japonés"_q },
		{ u"LuminaLangKo"_q, u"Coreano"_q },
		{ u"LuminaLangEs"_q, u"Español"_q },
		{ u"LuminaLangFr"_q, u"Francés"_q },
		{ u"LuminaLangDe"_q, u"Alemán"_q },
		{ u"LuminaLangRu"_q, u"Ruso"_q },
		{ u"LuminaLangPtBr"_q, u"Portugués (Brasil)"_q },
		{ u"LuminaLangPtPt"_q, u"Portugués (Portugal)"_q },
		{ u"LuminaLangIt"_q, u"Italiano"_q },
		{ u"LuminaLangAr"_q, u"Árabe"_q },
		{ u"LuminaLangHi"_q, u"Hindi"_q },
		{ u"LuminaLangId"_q, u"Indonesio"_q },
		{ u"LuminaLangTh"_q, u"Tailandés"_q },
		{ u"LuminaLangVi"_q, u"Vietnamita"_q },
		{ u"LuminaLangTr"_q, u"Turco"_q },
		{ u"LuminaLangPl"_q, u"Polaco"_q },
		{ u"LuminaLangUk"_q, u"Ucraniano"_q },
		{ u"LuminaLangNl"_q, u"Neerlandés"_q },
		{ u"LuminaLangAf"_q, u"Afrikáans"_q },
		{ u"LuminaLangSq"_q, u"Albanés"_q },
		{ u"LuminaLangAm"_q, u"Amárico"_q },
		{ u"LuminaLangHy"_q, u"Armenio"_q },
		{ u"LuminaLangAz"_q, u"Azerbaiyano"_q },
		{ u"LuminaLangEu"_q, u"Euskera"_q },
		{ u"LuminaLangBe"_q, u"Bielorruso"_q },
		{ u"LuminaLangBn"_q, u"Bengalí"_q },
		{ u"LuminaLangBs"_q, u"Bosnio"_q },
		{ u"LuminaLangBg"_q, u"Búlgaro"_q },
		{ u"LuminaLangMy"_q, u"Birmano"_q },
		{ u"LuminaLangCa"_q, u"Catalán"_q },
		{ u"LuminaLangHr"_q, u"Croata"_q },
		{ u"LuminaLangCs"_q, u"Checo"_q },
		{ u"LuminaLangDa"_q, u"Danés"_q },
		{ u"LuminaLangEt"_q, u"Estonio"_q },
		{ u"LuminaLangTl"_q, u"Filipino"_q },
		{ u"LuminaLangFi"_q, u"Finés"_q },
		{ u"LuminaLangGl"_q, u"Gallego"_q },
		{ u"LuminaLangKa"_q, u"Georgiano"_q },
		{ u"LuminaLangEl"_q, u"Griego"_q },
		{ u"LuminaLangGu"_q, u"Guyaratí"_q },
		{ u"LuminaLangHe"_q, u"Hebreo"_q },
		{ u"LuminaLangHu"_q, u"Húngaro"_q },
		{ u"LuminaLangIs"_q, u"Islandés"_q },
		{ u"LuminaLangGa"_q, u"Irlandés"_q },
		{ u"LuminaLangJv"_q, u"Javanés"_q },
		{ u"LuminaLangKn"_q, u"Canarés"_q },
		{ u"LuminaLangKk"_q, u"Kazajo"_q },
		{ u"LuminaLangKm"_q, u"Jemer"_q },
		{ u"LuminaLangKu"_q, u"Kurdo"_q },
		{ u"LuminaLangKy"_q, u"Kirguís"_q },
		{ u"LuminaLangLo"_q, u"Lao"_q },
		{ u"LuminaLangLv"_q, u"Letón"_q },
		{ u"LuminaLangLt"_q, u"Lituano"_q },
		{ u"LuminaLangMk"_q, u"Macedonio"_q },
		{ u"LuminaLangMs"_q, u"Malayo"_q },
		{ u"LuminaLangMl"_q, u"Malayalam"_q },
		{ u"LuminaLangMr"_q, u"Maratí"_q },
		{ u"LuminaLangMn"_q, u"Mongol"_q },
		{ u"LuminaLangNe"_q, u"Nepalí"_q },
		{ u"LuminaLangNo"_q, u"Noruego"_q },
		{ u"LuminaLangPs"_q, u"Pastún"_q },
		{ u"LuminaLangFa"_q, u"Persa"_q },
		{ u"LuminaLangPa"_q, u"Panyabí"_q },
		{ u"LuminaLangRo"_q, u"Rumano"_q },
		{ u"LuminaLangSr"_q, u"Serbio"_q },
		{ u"LuminaLangSi"_q, u"Cingalés"_q },
		{ u"LuminaLangSk"_q, u"Eslovaco"_q },
		{ u"LuminaLangSl"_q, u"Esloveno"_q },
		{ u"LuminaLangSo"_q, u"Somalí"_q },
		{ u"LuminaLangSw"_q, u"Suajili"_q },
		{ u"LuminaLangSv"_q, u"Sueco"_q },
		{ u"LuminaLangTg"_q, u"Tayiko"_q },
		{ u"LuminaLangTa"_q, u"Tamil"_q },
		{ u"LuminaLangTe"_q, u"Telugu"_q },
		{ u"LuminaLangUr"_q, u"Urdu"_q },
		{ u"LuminaLangUz"_q, u"Uzbeko"_q },
		{ u"LuminaLangCy"_q, u"Galés"_q },
		{ u"LuminaLangYi"_q, u"Yidis"_q },
		{ u"LuminaLangZu"_q, u"Zulú"_q },
		{ u"LuminaAppearanceStickerSizeHeader"_q, u"Tamaño de stickers"_q },
		{ u"LuminaBackupCryptoFailed"_q, u"El cifrado no está disponible en "
			u"este sistema."_q },
		{ u"LuminaBackupDamaged"_q, u"El archivo de copia está dañado y no se "
			u"pudo restaurar."_q },
		{ u"LuminaBackupExport"_q, u"Exportar copia cifrada"_q },
		{ u"LuminaBackupExportDone"_q, u"Copia guardada."_q },
		{ u"LuminaBackupExportFailed"_q, u"No se pudo crear el archivo de "
			u"copia."_q },
		{ u"LuminaBackupExportInfo"_q, u"Guarda todo lo que LuminaGram "
			u"conserva en este dispositivo —marcadores, notas, plantillas de "
			u"respuesta, reemplazos de texto y todos los ajustes— en un solo "
			u"archivo, cifrado con una frase de contraseña que tú elijas. El "
			u"archivo también incluye los valores privados: las claves API de "
			u"traducción, los códigos de la caja fuerte y del fallo falso, y "
			u"la nota señuelo. No se envía nada a Telegram. Elige una frase "
			u"de contraseña larga y guárdala en un lugar seguro: sin ella el "
			u"archivo no se puede abrir y no hay forma de recuperarlo."_q },
		{ u"LuminaBackupExportPassphraseTitle"_q, u"Establece una frase de "
			u"contraseña"_q },
		{ u"LuminaBackupFileFilter"_q, u"Copia de LuminaGram (*.lgbak)"_q },
		{ u"LuminaBackupImport"_q, u"Importar copia"_q },
		{ u"LuminaBackupImportFailed"_q, u"No se pudo leer el archivo de "
			u"copia."_q },
		{ u"LuminaBackupImportInfo"_q, u"Elige un archivo de copia e "
			u"introduce su frase de contraseña para restaurar tus datos de "
			u"LuminaGram. Los ajustes que incluya la copia reemplazan a los "
			u"de este dispositivo; lo que no incluya se deja intacto. Un "
			u"archivo que no se pueda verificar se rechaza antes de escribir "
			u"nada, así que una frase de contraseña incorrecta nunca puede "
			u"dejarte a medio restaurar."_q },
		{ u"LuminaBackupImportSuccess"_q, u"Copia restaurada. Reinicia "
			u"LuminaGram para aplicar todo."_q },
		{ u"LuminaBackupInvalidFile"_q, u"Este no es un archivo de copia de "
			u"LuminaGram válido."_q },
		{ u"LuminaBackupNewerFormat"_q, u"Esta copia se creó con una versión "
			u"más reciente de LuminaGram."_q },
		{ u"LuminaBackupOpenCaption"_q, u"Abrir copia de LuminaGram"_q },
		{ u"LuminaBackupPassphraseHint"_q, u"Frase de contraseña"_q },
		{ u"LuminaBackupPassphraseMismatch"_q, u"Las dos frases de contraseña "
			u"no coinciden."_q },
		{ u"LuminaBackupPassphraseRepeatHint"_q, u"Repite la frase de "
			u"contraseña"_q },
		{ u"LuminaBackupPassphraseTitle"_q, u"Introduce la frase de "
			u"contraseña"_q },
		{ u"LuminaBackupPassphraseTooShort"_q, u"Elige una frase de "
			u"contraseña de al menos 4 caracteres."_q },
		{ u"LuminaBackupSaveCaption"_q, u"Guardar copia de LuminaGram"_q },
		{ u"LuminaBackupTitle"_q, u"Copia de seguridad cifrada"_q },
		{ u"LuminaBackupUnauthenticated"_q, u"Esta copia usa el formato "
			u"antiguo sin protección, que no permite comprobar si la frase de "
			u"contraseña es incorrecta ni si el archivo ha sido manipulado. "
			u"Crea una copia nueva desde una versión actualizada de "
			u"LuminaGram."_q },
		{ u"LuminaBackupWrongPassphrase"_q, u"Frase de contraseña incorrecta, "
			u"o el archivo ha sido modificado."_q },
		{ u"LuminaBookmark"_q, u"Añadir marcador"_q },
		{ u"LuminaBookmarkAdded"_q, u"Añadido a marcadores"_q },
		{ u"LuminaBookmarkChatUnavailable"_q, u"Chat no disponible"_q },
		{ u"LuminaBookmarkDeleteTitle"_q, u"¿Eliminar marcador?"_q },
		{ u"LuminaBookmarkGone"_q, u"Este chat ya no está disponible en este "
			u"dispositivo."_q },
		{ u"LuminaBookmarkRemove"_q, u"Quitar marcador"_q },
		{ u"LuminaBookmarkRemoved"_q, u"Quitado de marcadores"_q },
		{ u"LuminaBookmarksAbout"_q, u"Los marcadores son punteros a "
			u"mensajes, se guardan solo en este dispositivo y nunca se envían "
			u"a Telegram. Quitar un marcador no altera el mensaje en sí."_q },
		{ u"LuminaBookmarksEmpty"_q, u"Aún no has guardado ningún mensaje en "
			u"marcadores."_q },
		{ u"LuminaBookmarksFull"_q, u"La lista de marcadores está llena. "
			u"Quita un marcador para añadir otro."_q },
		{ u"LuminaBookmarksList"_q, u"Mensajes en marcadores"_q },
		{ u"LuminaBookmarksListAbout"_q, u"Haz clic en un marcador para abrir "
			u"el mensaje, o haz clic con el botón derecho para quitarlo. Un "
			u"marcador permanece en esta lista aunque su mensaje se "
			u"elimine."_q },
		{ u"LuminaBookmarksNone"_q, u"Ninguno"_q },
		{ u"LuminaBookmarksTitle"_q, u"Marcadores"_q },
		{ u"LuminaChatListDensityTitle"_q, u"Densidad de la lista de chats"_q },
		{ u"LuminaCompactListRows"_q, u"Filas compactas"_q },
		{ u"LuminaCompactListRowsInfo"_q, u"Muestra más chats en la pantalla "
			u"acortando cada fila de la lista de chats. Los avatares y las "
			u"vistas previas de los mensajes siguen visibles."_q },
		{ u"LuminaChatListDotsTitle"_q, u"Puntos en el avatar"_q },
		{ u"LuminaChatListOnlineDot"_q, u"Punto de conexión"_q },
		{ u"LuminaChatListOnlineDotInfo"_q, u"Muestra un pequeño punto verde "
			u"en el avatar de los chats privados cuyo contacto está conectado "
			u"en este momento."_q },
		{ u"LuminaChatListRecencyDot"_q, u"Punto de última conexión"_q },
		{ u"LuminaChatListRecencyDotInfo"_q, u"Colorea el punto del avatar en "
			u"los chats privados según lo reciente que fue la última conexión "
			u"del contacto: verde si está conectado ahora, amarillo dentro de "
			u"una hora y naranja dentro de un día. No se muestra punto para "
			u"conexiones más antiguas u ocultas. El interruptor del punto de "
			u"conexión sigue controlando el caso verde (conectado)."_q },
		{ u"LuminaChatListVisibilityAbout"_q, u"Quita la barra de carpetas de "
			u"la lista de chats: tanto la barra vertical que hay a su lado "
			u"como la horizontal que hay encima. Mientras las pestañas de "
			u"carpetas están ocultas siempre ves todos tus chats: se sale de "
			u"la carpeta en la que estuvieras, y los atajos de carpeta y el "
			u"deslizamiento entre carpetas no hacen nada. Ocultar las "
			u"historias solo quita la fila que hay encima de la lista de "
			u"chats; las historias en sí, y todas las demás formas de "
			u"abrirlas, no cambian."_q },
		{ u"LuminaChatListVisibilityTitle"_q, u"Carpetas e historias"_q },
		{ u"LuminaContactNote"_q, u"Nota privada"_q },
		{ u"LuminaContactNoteAbout"_q, u"Solo tú puedes verla. Se queda en "
			u"este dispositivo, nunca se envía a Telegram y no se sincroniza "
			u"con tus otros dispositivos."_q },
		{ u"LuminaContactNoteEmpty"_q, u"Haz clic para añadir una nota "
			u"privada"_q },
		{ u"LuminaContactNoteHint"_q, u"Nota (solo tú puedes verla)"_q },
		{ u"LuminaContactNotesAbout"_q, u"Añade una nota privada y una lista "
			u"de etiquetas al perfil de una persona. Ambas se quedan en este "
			u"dispositivo, nunca se envían a Telegram y no se sincronizan con "
			u"tus otros dispositivos. Las notas de contacto propias de "
			u"Telegram se dejan intactas: la nota privada se ofrece donde "
			u"aquellas no están disponibles —en los bots y en las personas "
			u"que no están en tus contactos—, mientras que las etiquetas se "
			u"ofrecen en todas partes."_q },
		{ u"LuminaContactNotesClear"_q, u"Eliminar todas las notas "
			u"privadas"_q },
		{ u"LuminaContactNotesClearText"_q, u"¿Eliminar todas las notas "
			u"privadas y etiquetas guardadas en este dispositivo? Esto no se "
			u"puede deshacer."_q },
		{ u"LuminaContactNotesClearTitle"_q, u"Eliminar notas privadas"_q },
		{ u"LuminaContactNotesNone"_q, u"Ninguna"_q },
		{ u"LuminaContactNotesTitle"_q, u"Notas privadas de contactos"_q },
		{ u"LuminaContactNotesToggle"_q, u"Notas privadas y etiquetas en los "
			u"perfiles"_q },
		{ u"LuminaContactTags"_q, u"Etiquetas"_q },
		{ u"LuminaContactTagsEmpty"_q, u"Haz clic para añadir etiquetas"_q },
		{ u"LuminaContactTagsHint"_q, u"Etiquetas separadas por comas"_q },
		{ u"LuminaDetailsDate"_q, u"Fecha"_q },
		{ u"LuminaDetailsForwardedFrom"_q, u"Reenviado de"_q },
		{ u"LuminaDetailsFrom"_q, u"De"_q },
		{ u"LuminaDetailsMessageId"_q, u"ID del mensaje"_q },
		{ u"LuminaDetailsOriginalDate"_q, u"Fecha original"_q },
		{ u"LuminaExactNumbers"_q, u"Mostrar números exactos (sin redondeo "
			u"1.2K)"_q },
		{ u"LuminaExactNumbersInfo"_q, u"Mostrar cifras completas como "
			u"1.234.567 en lugar de formas abreviadas como 1.2M. Los "
			u"contadores que ya están dibujados en pantalla conservan su "
			u"forma actual hasta que se reinicie la aplicación."_q },
		{ u"LuminaForwardNoAuthor"_q, u"Reenviar sin autor"_q },
		{ u"LuminaForwardNoAuthorTitle"_q, u"Reenviar sin autor"_q },
		{ u"LuminaForwardNoCaption"_q, u"Reenviar sin autor ni "
			u"descripción"_q },
		{ u"LuminaForwardNoCaptionTitle"_q, u"Reenviar sin autor ni "
			u"descripción"_q },
		{ u"LuminaHideStories"_q, u"Ocultar historias"_q },
		{ u"LuminaHideTabs"_q, u"Ocultar pestañas de carpetas"_q },
		{ u"LuminaLinkSafetyDestination"_q, u"Destino real"_q },
		{ u"LuminaLinkSafetyInfo"_q, u"Pregunta antes de abrir un enlace que "
			u"oculte su destino real tras el texto que precede a un signo @, "
			u"que escriba su dominio en punycode o que pase por un acortador "
			u"de enlaces conocido. La confirmación muestra el host de destino "
			u"real y la dirección completa. Telegram Desktop ya avisa por su "
			u"cuenta de los dominios parecidos escritos en otros alfabetos. "
			u"No se comprueba nada en línea: ninguna dirección que abras sale "
			u"de este dispositivo."_q },
		{ u"LuminaLinkSafetyRow"_q, u"Inspector de seguridad de enlaces"_q },
		{ u"LuminaLinkSafetyTitle"_q, u"¿Abrir enlace externo?"_q },
		{ u"LuminaLinkSafetyWarnMismatch"_q, u"Este enlace oculta su destino "
			u"real tras el texto que precede al signo «@»."_q },
		{ u"LuminaLinkSafetyWarnPunycode"_q, u"Esta dirección usa caracteres "
			u"codificados (punycode) que pueden imitar un sitio conocido."_q },
		{ u"LuminaLinkSafetyWarnShortener"_q, u"Es un acortador de enlaces: "
			u"el destino real permanece oculto hasta que lo abras."_q },
		{ u"LuminaMediaAutoPauseBgVideo"_q, u"Pausar el video cuando la app "
			u"pasa a segundo plano"_q },
		{ u"LuminaMediaAutoPauseBgVideoInfo"_q, u"Pausa automáticamente el "
			u"video en reproducción cuando sales de LuminaGram. Minimizar el "
			u"visor también lo pausa. Un video que hayas puesto a propósito "
			u"en una ventana propia, o en imagen sobre imagen, sigue "
			u"reproduciéndose, y la música y los mensajes de voz siguen "
			u"sonando mientras estás fuera."_q },
		{ u"LuminaMediaSaving"_q, u"Multimedia"_q },
		{ u"LuminaMediaTitle"_q, u"Multimedia"_q },
		{ u"LuminaMessageActionsInfo"_q, u"Estas entradas solo aparecen en el "
			u"menú contextual de un mensaje. Todas usan el reenvío propio de "
			u"Telegram, así que un chat que restringe el reenvío sigue "
			u"restringido. «Reenviar sin autor ni descripción» se llama así "
			u"por lo que realmente sale: Telegram no puede quitar la "
			u"descripción y conservar al remitente, así que al elegirlo se "
			u"quitan las dos cosas. «Detalles» solo lee lo que este "
			u"dispositivo ya ha sincronizado: no se pide nada y no se guarda "
			u"nada."_q },
		{ u"LuminaMessageDetails"_q, u"Detalles"_q },
		{ u"LuminaOnboardingDualName"_q, u"Los dos idiomas a la vez"_q },
		{ u"LuminaOnboardingDualText"_q, u"Mantén el texto original en "
			u"pantalla junto a su traducción, tanto en los mensajes que "
			u"recibes como en los que envías."_q },
		{ u"LuminaOnboardingFooter"_q, u"Abre una sección de la página de "
			u"ajustes de LuminaGram para configurar cualquiera de estas "
			u"cosas. Puedes volver a leer esta tarjeta desde "
			u"Herramientas."_q },
		{ u"LuminaOnboardingGotIt"_q, u"Entendido"_q },
		{ u"LuminaOnboardingHeader"_q, u"Acerca de"_q },
		{ u"LuminaOnboardingIntro"_q, u"LuminaGram añade sus propias "
			u"herramientas sobre Telegram Desktop. Todas las opciones de "
			u"abajo se guardan solo en este ordenador y nunca se sincronizan "
			u"con Telegram."_q },
		{ u"LuminaOnboardingRow"_q, u"Qué añade LuminaGram"_q },
		{ u"LuminaOnboardingRowAbout"_q, u"Muestra la tarjeta de bienvenida "
			u"que aparece la primera vez que abres los ajustes de LuminaGram. "
			u"Solo explica lo que hay aquí y no activa nada."_q },
		{ u"LuminaOnboardingSafetyName"_q, u"Comprobaciones de seguridad"_q },
		{ u"LuminaOnboardingSafetyText"_q, u"Avisa antes de abrir un enlace "
			u"cuya dirección no es lo que parece, detecta una dirección de "
			u"criptomonedas cambiada cuando la pegas y quita la ubicación de "
			u"las fotos que envías."_q },
		{ u"LuminaOnboardingTitle"_q, u"Te damos la bienvenida a "
			u"LuminaGram"_q },
		{ u"LuminaOnboardingTranslateText"_q, u"Traduce los mensajes "
			u"entrantes con tu propio servicio de traducción, y traduce lo "
			u"que escribes antes de enviarlo."_q },
		{ u"LuminaOnboardingVaultName"_q, u"Caja fuerte de disfraz"_q },
		{ u"LuminaOnboardingVaultText"_q, u"Oculta LuminaGram tras una "
			u"calculadora o un bloc de notas que solo abre la app real con tu "
			u"código secreto."_q },
		{ u"LuminaProfileCardAbout"_q, u"Una tarjeta local sobre ti —qué "
			u"idiomas hablas, qué te interesa— que puedes copiar y pegar en "
			u"un chat. Se queda en este dispositivo."_q },
		{ u"LuminaProfileCardBio"_q, u"Biografía breve"_q },
		{ u"LuminaProfileCardBioHint"_q, u"Unas palabras sobre ti"_q },
		{ u"LuminaProfileCardCopied"_q, u"Tarjeta copiada"_q },
		{ u"LuminaProfileCardCopy"_q, u"Copiar al portapapeles"_q },
		{ u"LuminaProfileCardCopyInfo"_q, u"Crea un resumen en texto de tu "
			u"tarjeta. Pégalo en cualquier chat cuando quieras "
			u"compartirlo."_q },
		{ u"LuminaProfileCardEdit"_q, u"Mi tarjeta"_q },
		{ u"LuminaProfileCardEmptyShare"_q, u"Primero completa tu tarjeta"_q },
		{ u"LuminaProfileCardHeader"_q, u"Mi tarjeta"_q },
		{ u"LuminaProfileCardInfo"_q, u"Esta tarjeta se guarda solo en este "
			u"dispositivo y nunca se envía a Telegram. No cambia tu perfil de "
			u"Telegram."_q },
		{ u"LuminaProfileCardInterests"_q, u"Intereses / etiquetas"_q },
		{ u"LuminaProfileCardInterestsHint"_q, u"p. ej. música, senderismo, "
			u"programación"_q },
		{ u"LuminaProfileCardLanguages"_q, u"Idiomas que hablo"_q },
		{ u"LuminaProfileCardLanguagesHint"_q, u"p. ej. Español, English, "
			u"中文"_q },
		{ u"LuminaProfileCardNotSet"_q, u"Sin definir"_q },
		{ u"LuminaProfileCardTagline"_q, u"Lema"_q },
		{ u"LuminaProfileCardTaglineHint"_q, u"Una frase corta sobre ti"_q },
		{ u"LuminaProfileCardTitle"_q, u"Tarjeta de perfil"_q },
		{ u"LuminaProfileChatCreated"_q, u"Creado"_q },
		{ u"LuminaProfileDcId"_q, u"Centro de datos"_q },
		{ u"LuminaProfileDcIdValue"_q, u"DC{1}"_q },
		{ u"LuminaProfileInfoHeader"_q, u"Perfil"_q },
		{ u"LuminaProfileInfoInfo"_q, u"Líneas adicionales en las páginas de "
			u"perfil, todas calculadas en este dispositivo: no se le pide "
			u"nada a Telegram. Telegram proporciona la fecha de registro de "
			u"algunas cuentas; para el resto se estima a partir del número de "
			u"cuenta y se muestra con un «~». El centro de datos es el que "
			u"almacena la foto de perfil. La fecha de creación es cuando se "
			u"creó un grupo o un canal."_q },
		{ u"LuminaProfileRegistrationApprox"_q, u"~ {1}"_q },
		{ u"LuminaProfileRegistrationDate"_q, u"Fecha de registro"_q },
		{ u"LuminaProfileShowChatDate"_q, u"Mostrar fecha de creación"_q },
		{ u"LuminaProfileShowDcId"_q, u"Mostrar centro de datos"_q },
		{ u"LuminaProfileShowRegistrationDate"_q, u"Mostrar fecha de "
			u"registro"_q },
		{ u"LuminaRecentLimitsInfo"_q, u"Evita que este dispositivo descarte "
			u"los stickers recientes y los GIFs guardados antes de lo "
			u"necesario, con los mismos límites que LuminaGram en Android: "
			u"hasta 200 stickers recientes y 500 GIFs guardados. Los "
			u"servidores de Telegram siguen decidiendo cuántos se guardan "
			u"realmente en tu cuenta y se sincronizan con tus otros "
			u"dispositivos, así que esto no puede darte una lista más grande "
			u"en la nube: la próxima vez que este dispositivo se sincronice, "
			u"todo lo que el servidor ya no conserve desaparecerá también de "
			u"aquí, normalmente en unos segundos. Mientras esto está "
			u"activado, Telegram Desktop deja de quitar un GIF guardado por "
			u"su cuenta, así que su recordatorio Premium sobre el límite de "
			u"GIFs guardados no aparece. El panel de stickers sigue mostrando "
			u"solo los 20 primeros stickers recientes salvo que actives "
			u"también los stickers recientes ilimitados en Ajustes › Avanzado "
			u"› Ajustes experimentales."_q },
		{ u"LuminaRecentLimitsRow"_q, u"Conservar más stickers recientes y "
			u"GIFs"_q },
		{ u"LuminaReplyTemplatesAbout"_q, u"Fragmentos cortos de texto que "
			u"guardas en este dispositivo y colocas en el campo del mensaje. "
			u"Haz clic con el botón derecho en el botón de emojis de un chat "
			u"para elegir uno. Las plantillas nunca se envían a Telegram."_q },
		{ u"LuminaReplyTemplatesAdd"_q, u"Añadir plantilla"_q },
		{ u"LuminaReplyTemplatesEdit"_q, u"Editar plantilla"_q },
		{ u"LuminaReplyTemplatesEmpty"_q, u"Todavía no hay plantillas. Añade "
			u"una y luego haz clic con el botón derecho en el botón de emojis "
			u"de un chat para insertarla."_q },
		{ u"LuminaReplyTemplatesFull"_q, u"La lista está llena: elimina una "
			u"plantilla para añadir otra."_q },
		{ u"LuminaReplyTemplatesInfo"_q, u"Haz clic con el botón derecho en "
			u"el botón de emojis de un chat para insertar una plantilla. Haz "
			u"clic con el botón derecho en una plantilla de esta lista para "
			u"moverla arriba o abajo; ábrela para editarla o eliminarla."_q },
		{ u"LuminaReplyTemplatesList"_q, u"Plantillas"_q },
		{ u"LuminaReplyTemplatesManage"_q, u"Gestionar plantillas…"_q },
		{ u"LuminaReplyTemplatesMoveDown"_q, u"Mover abajo"_q },
		{ u"LuminaReplyTemplatesMoveUp"_q, u"Mover arriba"_q },
		{ u"LuminaReplyTemplatesNone"_q, u"Ninguna"_q },
		{ u"LuminaReplyTemplatesPlaceholder"_q, u"Texto de la plantilla"_q },
		{ u"LuminaReplyTemplatesShow"_q, u"Ofrecer plantillas en los "
			u"chats"_q },
		{ u"LuminaReplyTemplatesTitle"_q, u"Plantillas de respuesta"_q },
		{ u"LuminaSaveSticker"_q, u"Guardar sticker"_q },
		{ u"LuminaSaveStickers"_q, u"Guardar stickers"_q },
		{ u"LuminaSaveStickersInfo"_q, u"Añade una fila «Guardar sticker» al "
			u"menú contextual del panel de stickers. Los stickers van a donde "
			u"vayan tus demás descargas, y la fila se oculta para el paquete "
			u"de stickers propio de un grupo cuando ese grupo restringe el "
			u"guardado."_q },
		{ u"LuminaSaveToCloud"_q, u"Guardar en Mensajes Guardados"_q },
		{ u"LuminaSaveToCloudTitle"_q, u"Guardar en Mensajes Guardados"_q },
		{ u"LuminaSecurityPanicConfirmAck"_q, u"Entiendo que esto no se puede "
			u"deshacer"_q },
		{ u"LuminaSecurityPanicConfirmButton"_q, u"Borrar ahora"_q },
		{ u"LuminaSecurityPanicConfirmText"_q, u"Se cerrará la sesión de "
			u"todas las cuentas de este dispositivo. Se borrarán la base de "
			u"datos local de mensajes, los borradores y los archivos "
			u"multimedia en caché, junto con los ajustes propios de "
			u"LuminaGram, los marcadores, las traducciones guardadas y las "
			u"claves API.\n\nTus cuentas no se eliminan. Permanecen en los "
			u"servidores de Telegram, y tus mensajes también: puedes volver a "
			u"iniciar sesión desde cualquier sitio.\n\nLos archivos que ya "
			u"se habían descargado NO se eliminan. LuminaGram no toca tu "
			u"carpeta de descargas, porque normalmente es tu carpeta de "
			u"Descargas habitual y contiene archivos que no tienen nada que "
			u"ver. Mueve o elimina tú mismo lo que sea sensible.\n\nEsto no "
			u"se puede deshacer."_q },
		{ u"LuminaSecurityPanicConfirmTitle"_q, u"¿Borrado de emergencia?"_q },
		{ u"LuminaSecurityPanicHeader"_q, u"Borrado de emergencia"_q },
		{ u"LuminaSecurityPanicWipe"_q, u"Borrado de emergencia (Kaboom)"_q },
		{ u"LuminaSecurityPanicWipeAbout"_q, u"Cierra la sesión de todas las "
			u"cuentas de este dispositivo y borra la base de datos local de "
			u"mensajes, los borradores y los archivos multimedia en caché, "
			u"junto con los ajustes propios de LuminaGram, los marcadores, "
			u"las traducciones guardadas y las claves API. Tus cuentas y tus "
			u"mensajes permanecen en los servidores de Telegram. Los archivos "
			u"que ya descargaste se quedan donde están. Esto no se puede "
			u"deshacer."_q },
		{ u"LuminaSelectFromAuthor"_q, u"Seleccionar todo del autor"_q },
		{ u"LuminaSelectFromAuthorAbout"_q, u"Añade en los grupos una entrada "
			u"del menú del mensaje que selecciona todos los mensajes del "
			u"remitente en el que hiciste clic. Solo alcanza los mensajes que "
			u"esta ventana ya ha cargado: desplázate más hacia atrás y "
			u"repítelo para incluir los más antiguos. Los mensajes atribuidos "
			u"al propio chat, como las publicaciones de canal y las de "
			u"administradores anónimos, no tienen esta entrada."_q },
		{ u"LuminaSelectFromAuthorLimit"_q, u"Solo se pueden seleccionar {1} "
			u"mensajes a la vez."_q },
		{ u"LuminaSelectFromAuthorTitle"_q, u"Seleccionar todo del autor"_q },
		{ u"LuminaSendOriginalCaption"_q, u"Enviar descripción original"_q },
		{ u"LuminaShowBookmarks"_q, u"Mostrar opción de marcador en el "
			u"menú"_q },
		{ u"LuminaShowMessageDetails"_q, u"Detalles del mensaje"_q },
		{ u"LuminaShowMutedCount"_q, u"Mostrar siempre el contador de no "
			u"leídos"_q },
		{ u"LuminaShowMutedCountInfo"_q, u"Dibuja la insignia de no leídos de "
			u"los chats silenciados con el color de acento normal en lugar "
			u"del gris silenciado."_q },
		{ u"LuminaStickerSavedTo"_q, u"Sticker guardado en {1}"_q },
		{ u"LuminaStickerSizeChoice"_q, u"{1}%"_q },
		{ u"LuminaStickerSizeChoiceDefault"_q, u"{1}% (predeterminado)"_q },
		{ u"LuminaStickerSizeInfo"_q, u"El tamaño con que se dibujan los "
			u"stickers en los chats, tanto los que envías como los que "
			u"recibes. Los emojis animados, los dados y los stickers de "
			u"regalo conservan sus propios tamaños. Telegram Desktop mide un "
			u"sticker una vez y lo recuerda, así que un tamaño nuevo se "
			u"aplica la próxima vez que se inicie la aplicación."_q },
		{ u"LuminaTimeWithSeconds"_q, u"Mostrar segundos en la hora del "
			u"mensaje"_q },
		{ u"LuminaTimeWithSecondsAbout"_q, u"La hora que aparece bajo un "
			u"mensaje, la hora del texto copiado y la hora que anuncia un "
			u"lector de pantalla incluyen los segundos."_q },

		// The two items in the system tray menu; LuminaTrayQuit is also the
		// whole jump list on the Windows taskbar button. {1} is the
		// application's name, always the literal LuminaGram: never
		// translated and never inflected, so both rows take it bare, and
		// the preposition «Salir de» carries what Spanish needs here.
		{ u"LuminaTrayOpen"_q, u"Abrir {1}"_q },
		{ u"LuminaTrayQuit"_q, u"Salir de {1}"_q },

		{ u"LuminaUndoSendBulletin"_q, u"Enviando mensaje…"_q },
		{ u"LuminaUndoSendTitle"_q, u"Deshacer el envío"_q },
		{ u"LuminaUndoSendUndo"_q, u"Deshacer"_q },
		{ u"LuminaUndoSendWindow"_q, u"Ventana para deshacer el envío"_q },
		{ u"LuminaUndoSendWindowInfo"_q, u"Retiene un mensaje de texto simple "
			u"durante {1} segundos tras un botón «Deshacer» antes de "
			u"enviarlo. Tu texto se queda en el cuadro del mensaje todo ese "
			u"tiempo y el cuadro solo se vacía cuando el mensaje sale de "
			u"verdad, así que «Deshacer» simplemente lo deja donde está: no "
			u"se quita nada para volver a ponerlo. Volver a enviar, abrir "
			u"otro chat o salir de la aplicación envía el mensaje retenido de "
			u"inmediato. Los mensajes multimedia, de voz, editados, "
			u"reenviados y programados nunca se retienen, y tampoco los "
			u"enviados desde un tema de foro o un hilo de comentarios."_q },
	};
}

[[maybe_unused]] const auto kRegistered = RegisterLocaleTable("es", Build);

} // namespace
} // namespace Lumina
