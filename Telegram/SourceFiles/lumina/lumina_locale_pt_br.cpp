/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_locale.h"

namespace Lumina {
namespace {

// Brazilian Portuguese. Wording is taken from LuminaGram for Android
// (LuminaLocale.java, the "pt-br" table) wherever the two platforms show the
// same thing, so the same option reads the same on both. Quotation marks are
// the guillemets that table already uses.
//
// A key this table does not carry falls back to the English text in
// lumina_locale.cpp on its own.
[[nodiscard]] LocaleTable Build() {
	return {
		// Sections of the LuminaGram settings page.
		{ u"LuminaGramTitle"_q, u"LuminaGram"_q },
		{ u"LuminaTranslateTitle"_q, u"Tradução"_q },
		{ u"LuminaPrivacyTitle"_q, u"Privacidade"_q },
		{ u"LuminaSecurityTitle"_q, u"Segurança"_q },
		{ u"LuminaChatSettings"_q, u"Conversas"_q },
		{ u"LuminaGramChatList"_q, u"Lista de conversas"_q },
		{ u"LuminaAppearanceTitle"_q, u"Aparência"_q },
		{ u"LuminaToolsTitle"_q, u"Ferramentas"_q },
		{ u"LuminaGramStoredLocallyInfo"_q, u"As opções do LuminaGram ficam "
			u"salvas somente neste dispositivo e nunca são sincronizadas com "
			u"o Telegram."_q },

		// Sub-pages that have no rows yet.
		{ u"LuminaAppearancePlaceholder"_q, u"As opções de formatação de "
			u"mensagens, figurinhas e números vão aparecer aqui."_q },
		{ u"LuminaChatListPlaceholder"_q, u"As opções de layout e de selos "
			u"da lista de conversas vão aparecer aqui."_q },
		{ u"LuminaPrivacyPlaceholder"_q, u"As opções de privacidade de "
			u"links, área de transferência e mídia enviada vão aparecer "
			u"aqui."_q },
		{ u"LuminaSecurityPlaceholder"_q, u"As opções de cofre de disfarce, "
			u"limpeza de emergência e desbloqueio sob coação vão aparecer "
			u"aqui."_q },
		{ u"LuminaToolsPlaceholder"_q, u"Marcadores, modelos de resposta e "
			u"backup local vão aparecer aqui."_q },

		// Chats sub-page.
		{ u"LuminaMessageActions"_q, u"Ações de mensagens"_q },
		{ u"LuminaAllowSaveRestricted"_q,
			u"Permitir salvar / copiar em conversas restritas"_q },
		{ u"LuminaAllowSaveRestrictedInfo"_q, u"«Permitir salvar / copiar em "
			u"conversas restritas» afeta apenas ações locais neste "
			u"dispositivo. Algumas conversas restringem o salvamento por um "
			u"motivo — use com responsabilidade."_q },

		// Translation sub-page.
		{ u"LuminaTranslateEnable"_q, u"Ativar a tradução do LuminaGram"_q },
		{ u"LuminaTranslateEnableInfo"_q, u"Traduza com o seu próprio "
			u"mecanismo em vez do serviço Premium do Telegram. O mecanismo "
			u"padrão não precisa de conta nem de chave de API. Enquanto isto "
			u"estiver desativado, o LuminaGram deixa a tradução exatamente "
			u"como o Telegram Desktop a entrega."_q },
		{ u"LuminaTranslateSendHeader"_q, u"Envio"_q },
		{ u"LuminaTranslateBeforeSend"_q, u"Traduzir antes de enviar"_q },
		{ u"LuminaTranslateSendLang"_q, u"Idioma de envio"_q },
		{ u"LuminaTranslateSendLangAuto"_q, u"Idioma do destinatário"_q },
		{ u"LuminaTranslateBeforeSendConfirm"_q,
			u"Confirmar antes de enviar"_q },
		{ u"LuminaTranslateSendInfo"_q, u"As mensagens enviadas são "
			u"traduzidas para o idioma acima, e o original é guardado junto "
			u"com a tradução. Com «Idioma do destinatário», o LuminaGram "
			u"pergunta uma vez por conversa qual idioma usar ali e depois "
			u"lembra da escolha. «Confirmar antes de enviar» mostra a "
			u"tradução ao lado do original primeiro, para você enviar "
			u"qualquer uma das duas; com a opção desativada, a tradução é "
			u"enviada na hora."_q },
		{ u"LuminaTranslateReceiveHeader"_q, u"Recebimento"_q },
		{ u"LuminaDualLanguageDisplay"_q,
			u"Mostrar o original e a tradução juntos"_q },
		{ u"LuminaTranslateReadLang"_q, u"Idioma de leitura"_q },
		{ u"LuminaTranslateReadLangFollow"_q, u"Seguir idioma do app"_q },
		{ u"LuminaTranslateModeHeader"_q, u"Traduzir mensagens recebidas"_q },
		{ u"LuminaTranslateModeAll"_q, u"Em todas as conversas"_q },
		{ u"LuminaTranslateModeManual"_q,
			u"Somente as conversas que eu ativar"_q },
		{ u"LuminaTranslateReceiveInfo"_q, u"As mensagens recebidas mantêm o "
			u"texto original em tamanho normal, com a tradução exibida "
			u"abaixo. «Em todas as conversas» envia uma requisição por "
			u"mensagem ao seu serviço de tradução — com uma chave paga por "
			u"uso, deixe em «Somente as conversas que eu ativar»."_q },
		{ u"LuminaTranslateScopeHeader"_q, u"Onde se aplica"_q },
		{ u"LuminaTranslateScopePrivate"_q, u"Conversas privadas"_q },
		{ u"LuminaTranslateScopeGroup"_q, u"Grupos e canais"_q },
		{ u"LuminaTranslateScopeInfo"_q, u"As conversas que ficam de fora "
			u"não são afetadas nos dois sentidos: não são traduzidas quando "
			u"você as lê, e as mensagens que você envia nelas saem sem "
			u"tradução. Você ainda pode traduzir qualquer mensagem "
			u"individual à mão."_q },
		{ u"LuminaTranslateProviderHeader"_q, u"Serviço"_q },
		{ u"LuminaTranslateProvider"_q, u"Serviço de tradução"_q },
		{ u"LuminaTranslateProviderLlm"_q, u"LLM (compatível com OpenAI)"_q },
		{ u"LuminaTranslateApiKey"_q, u"Chave de API"_q },
		{ u"LuminaTranslateApiKeyNotSet"_q, u"Não definida"_q },
		{ u"LuminaTranslateBaseUrl"_q, u"URL base"_q },
		{ u"LuminaTranslateModel"_q, u"Modelo"_q },
		{ u"LuminaTranslateSystemPrompt"_q, u"Prompt do sistema"_q },
		{ u"LuminaTranslatePromptDefault"_q, u"Padrão"_q },
		{ u"LuminaTranslatePromptCustom"_q, u"Personalizado"_q },
		{ u"LuminaTranslateFallbackTelegram"_q,
			u"Usar o Telegram quando este serviço falhar"_q },
		{ u"LuminaTranslateProviderSecurityInfo"_q, u"As chaves ficam "
			u"somente neste dispositivo, em um arquivo separado do resto das "
			u"configurações, e nunca são enviadas ao Telegram. Tudo o que "
			u"você traduz é enviado ao serviço selecionado aqui, então "
			u"escolha um em que você confie."_q },

		// Translate before sending: the boxes and the send menu.
		{ u"LuminaTranslateOriginalLabel"_q, u"Original"_q },
		{ u"LuminaSendTranslation"_q, u"Enviar tradução"_q },
		{ u"LuminaSendOriginal"_q, u"Enviar original"_q },
		{ u"LuminaTrSendPickerTitle"_q, u"Idioma da tradução"_q },
		{ u"LuminaTrSendConfirmMessage"_q, u"Parece que esta conversa é "
			u"escrita em {1}. Traduzir para {1} as mensagens que você enviar "
			u"aqui? O LuminaGram vai lembrar disso nesta conversa."_q },
		{ u"LuminaTrSendConfirmTranslate"_q, u"Traduzir"_q },
		{ u"LuminaTrSendAsTyped"_q, u"Enviar como digitado"_q },
		{ u"LuminaTrSendChooseOther"_q, u"Escolher idioma"_q },

		// The translate-before-send preview bar above the composer.
		{ u"LuminaTranslatePreviewTranslating"_q, u"Traduzindo…"_q },
		{ u"LuminaTranslatePreviewFailed"_q, u"Tradução indisponível"_q },

		// Names of the languages LuminaGram can translate into.
		{ u"LuminaLangEn"_q, u"Inglês"_q },
		{ u"LuminaLangZhTw"_q, u"Chinês (tradicional)"_q },
		{ u"LuminaLangZhCn"_q, u"Chinês (simplificado)"_q },
		{ u"LuminaLangJa"_q, u"Japonês"_q },
		{ u"LuminaLangKo"_q, u"Coreano"_q },
		{ u"LuminaLangEs"_q, u"Espanhol"_q },
		{ u"LuminaLangFr"_q, u"Francês"_q },
		{ u"LuminaLangDe"_q, u"Alemão"_q },
		{ u"LuminaLangRu"_q, u"Russo"_q },
		{ u"LuminaLangPtBr"_q, u"Português (Brasil)"_q },
		{ u"LuminaLangPtPt"_q, u"Português (Portugal)"_q },
		{ u"LuminaLangIt"_q, u"Italiano"_q },
		{ u"LuminaLangAr"_q, u"Árabe"_q },
		{ u"LuminaLangHi"_q, u"Híndi"_q },
		{ u"LuminaLangId"_q, u"Indonésio"_q },
		{ u"LuminaLangTh"_q, u"Tailandês"_q },
		{ u"LuminaLangVi"_q, u"Vietnamita"_q },
		{ u"LuminaLangTr"_q, u"Turco"_q },
		{ u"LuminaLangPl"_q, u"Polonês"_q },
		{ u"LuminaLangUk"_q, u"Ucraniano"_q },
		{ u"LuminaLangNl"_q, u"Holandês"_q },
		{ u"LuminaLangAf"_q, u"Africâner"_q },
		{ u"LuminaLangSq"_q, u"Albanês"_q },
		{ u"LuminaLangAm"_q, u"Amárico"_q },
		{ u"LuminaLangHy"_q, u"Armênio"_q },
		{ u"LuminaLangAz"_q, u"Azerbaijano"_q },
		{ u"LuminaLangEu"_q, u"Basco"_q },
		{ u"LuminaLangBe"_q, u"Bielorrusso"_q },
		{ u"LuminaLangBn"_q, u"Bengali"_q },
		{ u"LuminaLangBs"_q, u"Bósnio"_q },
		{ u"LuminaLangBg"_q, u"Búlgaro"_q },
		{ u"LuminaLangMy"_q, u"Birmanês"_q },
		{ u"LuminaLangCa"_q, u"Catalão"_q },
		{ u"LuminaLangHr"_q, u"Croata"_q },
		{ u"LuminaLangCs"_q, u"Tcheco"_q },
		{ u"LuminaLangDa"_q, u"Dinamarquês"_q },
		{ u"LuminaLangEt"_q, u"Estoniano"_q },
		{ u"LuminaLangTl"_q, u"Filipino"_q },
		{ u"LuminaLangFi"_q, u"Finlandês"_q },
		{ u"LuminaLangGl"_q, u"Galego"_q },
		{ u"LuminaLangKa"_q, u"Georgiano"_q },
		{ u"LuminaLangEl"_q, u"Grego"_q },
		{ u"LuminaLangGu"_q, u"Guzerate"_q },
		{ u"LuminaLangHe"_q, u"Hebraico"_q },
		{ u"LuminaLangHu"_q, u"Húngaro"_q },
		{ u"LuminaLangIs"_q, u"Islandês"_q },
		{ u"LuminaLangGa"_q, u"Irlandês"_q },
		{ u"LuminaLangJv"_q, u"Javanês"_q },
		{ u"LuminaLangKn"_q, u"Canarês"_q },
		{ u"LuminaLangKk"_q, u"Cazaque"_q },
		{ u"LuminaLangKm"_q, u"Khmer"_q },
		{ u"LuminaLangKu"_q, u"Curdo"_q },
		{ u"LuminaLangKy"_q, u"Quirguiz"_q },
		{ u"LuminaLangLo"_q, u"Laosiano"_q },
		{ u"LuminaLangLv"_q, u"Letão"_q },
		{ u"LuminaLangLt"_q, u"Lituano"_q },
		{ u"LuminaLangMk"_q, u"Macedônio"_q },
		{ u"LuminaLangMs"_q, u"Malaio"_q },
		{ u"LuminaLangMl"_q, u"Malaiala"_q },
		{ u"LuminaLangMr"_q, u"Marati"_q },
		{ u"LuminaLangMn"_q, u"Mongol"_q },
		{ u"LuminaLangNe"_q, u"Nepalês"_q },
		{ u"LuminaLangNo"_q, u"Norueguês"_q },
		{ u"LuminaLangPs"_q, u"Pashto"_q },
		{ u"LuminaLangFa"_q, u"Persa"_q },
		{ u"LuminaLangPa"_q, u"Punjabi"_q },
		{ u"LuminaLangRo"_q, u"Romeno"_q },
		{ u"LuminaLangSr"_q, u"Sérvio"_q },
		{ u"LuminaLangSi"_q, u"Cingalês"_q },
		{ u"LuminaLangSk"_q, u"Eslovaco"_q },
		{ u"LuminaLangSl"_q, u"Esloveno"_q },
		{ u"LuminaLangSo"_q, u"Somali"_q },
		{ u"LuminaLangSw"_q, u"Suaíli"_q },
		{ u"LuminaLangSv"_q, u"Sueco"_q },
		{ u"LuminaLangTg"_q, u"Tadjique"_q },
		{ u"LuminaLangTa"_q, u"Tâmil"_q },
		{ u"LuminaLangTe"_q, u"Télugo"_q },
		{ u"LuminaLangUr"_q, u"Urdu"_q },
		{ u"LuminaLangUz"_q, u"Uzbeque"_q },
		{ u"LuminaLangCy"_q, u"Galês"_q },
		{ u"LuminaLangYi"_q, u"Iídiche"_q },
		{ u"LuminaLangZu"_q, u"Zulu"_q },
	};
}

[[maybe_unused]] const auto kRegistered = RegisterLocaleTable(
	"pt-br",
	Build);

} // namespace
} // namespace Lumina
