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
// LuminaTranslateModeAll, LuminaTranslateModeManual and
// LuminaAllowSaveRestricted.
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
		{ u"LuminaTranslateModeHeader"_q, u"Traducir mensajes entrantes"_q },
		{ u"LuminaTranslateModeAll"_q, u"En todos los chats"_q },
		{ u"LuminaTranslateModeManual"_q, u"Solo los chats que yo active"_q },
		{ u"LuminaTranslateModeInfo"_q, u"Con «Solo los chats que yo active», "
			u"abre un chat y usa el botón «Traducir» de la parte superior "
			u"para activarlo ahí; los demás chats quedan intactos. «En todos "
			u"los chats» envía una petición por mensaje a tu servicio de "
			u"traducción: si tu clave tiene cuota limitada, déjalo en «Solo "
			u"los chats que yo active»."_q },
		{ u"LuminaTranslateReceiveHeader"_q, u"Recepción"_q },
		{ u"LuminaDualLanguageDisplay"_q,
			u"Mostrar el original y la traducción juntos"_q },
		{ u"LuminaTranslateReadLang"_q, u"Idioma de lectura"_q },
		{ u"LuminaTranslateReadLangFollow"_q, u"Idioma de la interfaz"_q },
		{ u"LuminaTranslateReceiveInfo"_q, u"Los mensajes entrantes conservan "
			u"su texto original a tamaño completo, con la traducción "
			u"debajo."_q },
		{ u"LuminaTranslateScopeHeader"_q, u"Alcance"_q },
		{ u"LuminaTranslateScopePrivate"_q, u"Chats privados"_q },
		{ u"LuminaTranslateScopeGroup"_q, u"Grupos y canales"_q },
		{ u"LuminaTranslateScopeInfo"_q, u"Los chats que quedan fuera del "
			u"alcance se dejan intactos en ambos sentidos: no se traducen "
			u"cuando los lees y los mensajes que envías allí salen sin "
			u"traducir. Aun así, siempre puedes traducir a mano cualquier "
			u"mensaje suelto."_q },
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
	};
}

[[maybe_unused]] const auto kRegistered = RegisterLocaleTable("es", Build);

} // namespace
} // namespace Lumina
