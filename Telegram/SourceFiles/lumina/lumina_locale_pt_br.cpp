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
		{ u"LuminaTranslateReceiveInfo"_q, u"As mensagens recebidas mantêm o "
			u"texto original em tamanho normal, com a tradução exibida "
			u"abaixo."_q },
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
		{ u"LuminaTranslateTest"_q, u"Testar tradução"_q },
		{ u"LuminaTranslateTestRunning"_q, u"Testando…"_q },
		{ u"LuminaTranslateTestSuccess"_q, u"A tradução funciona."_q },
		{ u"LuminaTranslateTestFailed"_q, u"Falha no teste"_q },
		{ u"LuminaTranslateNoKey"_q, u"Chave de API necessária"_q },
		{ u"LuminaTranslateTestKeyRejected"_q, u"Não foi possível alcançar o "
			u"serviço, ou ele recusou esta chave de API. Verifique primeiro "
			u"a chave e depois a URL base e sua conexão."_q },
		{ u"LuminaTranslateTestNetwork"_q, u"Não foi possível alcançar o "
			u"serviço. Verifique sua conexão com a internet e o proxy, se "
			u"houver, e tente de novo."_q },
		{ u"LuminaTranslateTestQuota"_q, u"O serviço recusou a requisição: "
			u"requisições demais, ou a cota desta chave acabou. Tente mais "
			u"tarde."_q },
		{ u"LuminaTranslateTestQuotaKeyed"_q, u"O serviço recusou a "
			u"requisição: ele rejeitou esta chave de API, a cota da chave "
			u"acabou, ou houve requisições demais. Confira primeiro a chave "
			u"e depois tente mais tarde."_q },
		{ u"LuminaTranslateTestBadResponse"_q, u"O serviço respondeu com "
			u"algo que o LuminaGram não conseguiu ler, ou não respondeu "
			u"nada. Verifique a URL base e o modelo."_q },
		{ u"LuminaTranslateTestUnavailable"_q, u"Este serviço não pode "
			u"funcionar aqui. Escolha outro acima."_q },
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

		// The per-chat translate button in the chat top bar.
		{ u"LuminaTranslateChatToggle"_q, u"Traduzir esta conversa"_q },
		{ u"LuminaTranslateChatShowOriginal"_q, u"Mostrar original"_q },

		// The two-row menu that button opens. Each row names its side -
		// the other person, or me - because a direction word never said
		// whose messages were meant.
		{ u"LuminaChatLangThem"_q,
			u"Mensagens da outra pessoa, traduzidas para {1}"_q },
		{ u"LuminaChatLangThemOff"_q,
			u"Mensagens da outra pessoa, não traduzidas"_q },
		{ u"LuminaChatLangMe"_q, u"Minhas mensagens, traduzidas para {1}"_q },
		{ u"LuminaChatLangMeOff"_q, u"Minhas mensagens, não traduzidas"_q },
		{ u"LuminaChatLangThemTitle"_q,
			u"Traduzir as mensagens da outra pessoa para"_q },
		{ u"LuminaChatLangMeTitle"_q, u"Traduzir minhas mensagens para"_q },
		{ u"LuminaChatLangNone"_q, u"Sem tradução"_q },

		// The chat's tone (register): the third row of the translate menu
		// and the box it opens. The instruction the engine is actually sent
		// is written in English and is not one of these strings.
		{ u"LuminaChatRegister"_q, u"Tom desta conversa: {1}"_q },
		{ u"LuminaChatRegisterOff"_q, u"Tom desta conversa: não definido"_q },
		{ u"LuminaChatRegisterTitle"_q, u"Tom desta conversa"_q },
		{ u"LuminaChatRegisterNone"_q, u"Não definido"_q },
		{ u"LuminaChatRegisterNoneInfo"_q, u"Traduzir normalmente, sem "
			u"nenhuma instrução de tom."_q },
		{ u"LuminaChatRegisterClient"_q, u"Cliente"_q },
		{ u"LuminaChatRegisterClientInfo"_q, u"Linguagem comercial educada e "
			u"profissional; formas formais onde o idioma as tiver."_q },
		{ u"LuminaChatRegisterColleague"_q, u"Colega de trabalho"_q },
		{ u"LuminaChatRegisterColleagueInfo"_q, u"Linguagem de trabalho do "
			u"dia a dia: cordial, mas sem formalidade excessiva."_q },
		{ u"LuminaChatRegisterFriend"_q, u"Amigo"_q },
		{ u"LuminaChatRegisterFriendInfo"_q, u"Linguagem informal e "
			u"coloquial, como se fala com um amigo."_q },
		{ u"LuminaChatRegisterFamily"_q, u"Família"_q },
		{ u"LuminaChatRegisterFamilyInfo"_q, u"Linguagem calorosa e familiar, "
			u"do jeito que se fala em casa."_q },
		{ u"LuminaChatRegisterElder"_q, u"Pessoa mais velha"_q },
		{ u"LuminaChatRegisterElderInfo"_q, u"Respeitoso e deferente, mas "
			u"caloroso, não corporativo."_q },
		{ u"LuminaChatRegisterRomance"_q, u"Alguém especial"_q },
		{ u"LuminaChatRegisterRomanceInfo"_q, u"Caloroso e brincalhão, com um "
			u"toque de flerte."_q },
		{ u"LuminaChatRegisterCustom"_q, u"Personalizado…"_q },
		{ u"LuminaChatRegisterCustomInfo"_q, u"Descreva a relação com suas "
			u"próprias palavras."_q },
		{ u"LuminaChatRegisterCustomTitle"_q, u"Descreva esta relação"_q },
		{ u"LuminaChatRegisterCustomHint"_q, u"ex.: meu orientador — "
			u"respeitoso, mas sem rigidez"_q },
		{ u"LuminaChatRegisterUnsupported"_q, u"O mecanismo de tradução atual "
			u"ignora o tom"_q },
		{ u"LuminaChatRegisterDeepL"_q, u"DeepL: apenas formal/informal e só "
			u"em alguns idiomas"_q },

		// Telegram's own AI editor, whose Translate tab overlaps ours.
		{ u"LuminaAiEditorHeader"_q, u"Editor de IA do Telegram"_q },
		{ u"LuminaAiEditorKeep"_q, u"Manter o editor de IA do Telegram"_q },
		{ u"LuminaAiEditorInfo"_q, u"O Telegram tem um editor de IA próprio "
			u"no campo de mensagem, com uma aba «Traduzir» que se sobrepõe à "
			u"tradução do LuminaGram. Enquanto a tradução do LuminaGram "
			u"estiver ativada, o botão e o atalho de teclado dele não são "
			u"oferecidos, para que exista sempre uma única ferramenta de "
			u"tradução à sua frente. Ative isto para manter o editor do "
			u"Telegram disponível mesmo assim. Com a tradução do LuminaGram "
			u"desativada, o editor do Telegram está sempre disponível e esta "
			u"opção não muda nada."_q },

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
		{ u"LuminaAppearanceStickerSizeHeader"_q,
			u"Tamanho das figurinhas"_q },
		{ u"LuminaBackupCryptoFailed"_q,
			u"A criptografia não está disponível neste sistema."_q },
		{ u"LuminaBackupDamaged"_q, u"O arquivo de backup está danificado e "
			u"não pôde ser restaurado."_q },
		{ u"LuminaBackupExport"_q, u"Exportar backup criptografado"_q },
		{ u"LuminaBackupExportDone"_q, u"Backup salvo."_q },
		{ u"LuminaBackupExportFailed"_q,
			u"Não foi possível criar o arquivo de backup."_q },
		{ u"LuminaBackupExportInfo"_q, u"Salve tudo o que o LuminaGram guarda "
			u"neste dispositivo — marcadores, notas, modelos de resposta, "
			u"substituições de texto e todas as configurações — em um único "
			u"arquivo, criptografado com uma frase secreta que você escolher. "
			u"O arquivo também leva os valores privados: as chaves de API de "
			u"tradução, os códigos do cofre e da falha falsa, a nota do bloco "
			u"de notas falso. Nada é enviado ao Telegram. Escolha uma frase "
			u"secreta longa e guarde-a em um lugar seguro — sem ela o arquivo "
			u"não pode ser aberto, e não há como recuperá-la."_q },
		{ u"LuminaBackupExportPassphraseTitle"_q,
			u"Defina uma frase secreta"_q },
		{ u"LuminaBackupFileFilter"_q, u"Backup do LuminaGram (*.lgbak)"_q },
		{ u"LuminaBackupImport"_q, u"Importar backup"_q },
		{ u"LuminaBackupImportFailed"_q,
			u"Não foi possível ler o arquivo de backup."_q },
		{ u"LuminaBackupImportInfo"_q, u"Escolha um arquivo de backup e "
			u"digite a frase secreta dele para restaurar seus dados do "
			u"LuminaGram. As configurações que o backup contém substituem as "
			u"deste dispositivo; o que ele não contém fica intacto. Um "
			u"arquivo que não pode ser verificado é recusado antes de "
			u"qualquer gravação, então uma frase secreta errada nunca deixa a "
			u"restauração pela metade."_q },
		{ u"LuminaBackupImportSuccess"_q,
			u"Backup restaurado. Reinicie o LuminaGram para aplicar tudo."_q },
		{ u"LuminaBackupInvalidFile"_q,
			u"Este não é um arquivo de backup válido do LuminaGram."_q },
		{ u"LuminaBackupNewerFormat"_q, u"Este backup foi feito por uma "
			u"versão mais recente do LuminaGram."_q },
		{ u"LuminaBackupOpenCaption"_q, u"Abrir backup do LuminaGram"_q },
		{ u"LuminaBackupPassphraseHint"_q, u"Frase secreta"_q },
		{ u"LuminaBackupPassphraseMismatch"_q,
			u"As duas frases secretas são diferentes."_q },
		{ u"LuminaBackupPassphraseRepeatHint"_q, u"Repita a frase secreta"_q },
		{ u"LuminaBackupPassphraseTitle"_q, u"Digite a frase secreta"_q },
		{ u"LuminaBackupPassphraseTooShort"_q,
			u"Escolha uma frase secreta com pelo menos 4 caracteres."_q },
		{ u"LuminaBackupSaveCaption"_q, u"Salvar backup do LuminaGram"_q },
		{ u"LuminaBackupTitle"_q, u"Backup criptografado"_q },
		{ u"LuminaBackupUnauthenticated"_q, u"Este backup usa o formato "
			u"antigo sem proteção, que não permite verificar uma frase "
			u"secreta errada nem adulteração. Faça um backup novo em um "
			u"LuminaGram atualizado."_q },
		{ u"LuminaBackupWrongPassphrase"_q,
			u"Frase secreta incorreta, ou o arquivo foi alterado."_q },
		{ u"LuminaBookmark"_q, u"Adicionar marcador"_q },
		{ u"LuminaBookmarkAdded"_q, u"Adicionado aos marcadores"_q },
		{ u"LuminaBookmarkChatUnavailable"_q, u"Conversa indisponível"_q },
		{ u"LuminaBookmarkDeleteTitle"_q, u"Excluir marcador?"_q },
		{ u"LuminaBookmarkGone"_q,
			u"Esta conversa não está mais disponível neste dispositivo."_q },
		{ u"LuminaBookmarkRemove"_q, u"Remover marcador"_q },
		{ u"LuminaBookmarkRemoved"_q, u"Removido dos marcadores"_q },
		{ u"LuminaBookmarksAbout"_q, u"Os marcadores são ponteiros para "
			u"mensagens, guardados somente neste dispositivo e nunca enviados "
			u"ao Telegram. Remover um marcador não altera a mensagem em "
			u"si."_q },
		{ u"LuminaBookmarksEmpty"_q,
			u"Você ainda não salvou nenhuma mensagem nos marcadores."_q },
		{ u"LuminaBookmarksFull"_q, u"A lista de marcadores está cheia. "
			u"Remova um marcador para adicionar outro."_q },
		{ u"LuminaBookmarksList"_q, u"Mensagens marcadas"_q },
		{ u"LuminaBookmarksListAbout"_q, u"Clique em um marcador para abrir a "
			u"mensagem, ou clique nele com o botão direito para removê-lo. Um "
			u"marcador continua nesta lista mesmo depois que a mensagem é "
			u"excluída."_q },
		{ u"LuminaBookmarksNone"_q, u"Nenhum"_q },
		{ u"LuminaBookmarksTitle"_q, u"Marcadores"_q },
		{ u"LuminaChatListDensityTitle"_q, u"Densidade da lista de conversas"_q },
		{ u"LuminaCompactListRows"_q, u"Linhas compactas"_q },
		{ u"LuminaCompactListRowsInfo"_q, u"Mostra mais conversas na tela "
			u"encurtando cada linha da lista de conversas. Os avatares e as "
			u"prévias das mensagens continuam visíveis."_q },
		{ u"LuminaChatListDotsTitle"_q, u"Pontos no avatar"_q },
		{ u"LuminaChatListOnlineDot"_q, u"Ponto on-line"_q },
		{ u"LuminaChatListOnlineDotInfo"_q, u"Mostra um pequeno ponto verde "
			u"no avatar das conversas privadas cujo contato está on-line no "
			u"momento."_q },
		{ u"LuminaChatListRecencyDot"_q, u"Ponto de visto por último"_q },
		{ u"LuminaChatListRecencyDotInfo"_q, u"Colore o ponto do avatar nas "
			u"conversas privadas de acordo com há quanto tempo o contato foi "
			u"visto por último: verde quando está on-line agora, amarelo em "
			u"até uma hora e laranja em até um dia. Nenhum ponto para visto "
			u"por último mais antigo ou oculto. O botão do ponto on-line "
			u"ainda controla o caso verde (on-line)."_q },
		{ u"LuminaChatListVisibilityAbout"_q, u"Tira a barra de pastas da "
			u"lista de conversas — tanto a barra vertical ao lado dela quanto "
			u"a horizontal acima dela. Enquanto as abas de pastas estiverem "
			u"ocultas, você sempre vê todas as suas conversas: a pasta em que "
			u"você estava é deixada, e os atalhos de pastas e o gesto de "
			u"deslizar entre elas não fazem nada. Ocultar stories remove "
			u"apenas a linha acima da lista de conversas; os stories em si, e "
			u"todas as outras formas de abri-los, continuam iguais."_q },
		{ u"LuminaChatListVisibilityTitle"_q, u"Pastas e stories"_q },
		{ u"LuminaContactNote"_q, u"Nota privada"_q },
		{ u"LuminaContactNoteAbout"_q, u"Só você pode ver isto. Fica neste "
			u"dispositivo, nunca é enviado ao Telegram e não é sincronizado "
			u"com seus outros dispositivos."_q },
		{ u"LuminaContactNoteEmpty"_q,
			u"Clique para adicionar uma nota privada"_q },
		{ u"LuminaContactNoteHint"_q, u"Nota (só você pode ver)"_q },
		{ u"LuminaContactNotesAbout"_q, u"Adiciona uma nota privada e uma "
			u"lista de tags ao perfil de uma pessoa. As duas ficam neste "
			u"dispositivo, nunca são enviadas ao Telegram e não são "
			u"sincronizadas com seus outros dispositivos. As notas de contato "
			u"do próprio Telegram ficam intactas: a nota privada é oferecida "
			u"onde elas não estão disponíveis — em bots e em pessoas que não "
			u"estão nos seus contatos — enquanto as tags são oferecidas em "
			u"todos os perfis."_q },
		{ u"LuminaContactNotesClear"_q, u"Excluir todas as notas privadas"_q },
		{ u"LuminaContactNotesClearText"_q, u"Excluir todas as notas privadas "
			u"e tags guardadas neste dispositivo? Isso não pode ser "
			u"desfeito."_q },
		{ u"LuminaContactNotesClearTitle"_q, u"Excluir notas privadas"_q },
		{ u"LuminaContactNotesNone"_q, u"Nenhuma"_q },
		{ u"LuminaContactNotesTitle"_q, u"Notas privadas de contatos"_q },
		{ u"LuminaContactNotesToggle"_q,
			u"Notas privadas e tags nos perfis"_q },
		{ u"LuminaContactTags"_q, u"Tags"_q },
		{ u"LuminaContactTagsEmpty"_q, u"Clique para adicionar tags"_q },
		{ u"LuminaContactTagsHint"_q, u"Tags separadas por vírgula"_q },
		{ u"LuminaDetailsDate"_q, u"Data"_q },
		{ u"LuminaDetailsForwardedFrom"_q, u"Encaminhado de"_q },
		{ u"LuminaDetailsFrom"_q, u"De"_q },
		{ u"LuminaDetailsMessageId"_q, u"ID da mensagem"_q },
		{ u"LuminaDetailsOriginalDate"_q, u"Data original"_q },
		{ u"LuminaExactNumbers"_q,
			u"Mostrar números exatos (sem arredondamento 1.2K)"_q },
		{ u"LuminaExactNumbersInfo"_q, u"Mostrar contagens completas como "
			u"1.234.567 em vez de formas abreviadas como 1.2M. Os contadores "
			u"já desenhados na tela mantêm a forma atual até o app ser "
			u"reiniciado."_q },
		{ u"LuminaForwardNoAuthor"_q, u"Encaminhar sem autor"_q },
		{ u"LuminaForwardNoAuthorTitle"_q, u"Encaminhar sem autor"_q },
		{ u"LuminaForwardNoCaption"_q,
			u"Encaminhar sem autor e sem legendas"_q },
		{ u"LuminaForwardNoCaptionTitle"_q,
			u"Encaminhar sem autor e sem legendas"_q },
		{ u"LuminaHideStories"_q, u"Ocultar stories"_q },
		{ u"LuminaHideTabs"_q, u"Ocultar abas de pastas"_q },
		{ u"LuminaLinkSafetyDestination"_q, u"Destino real"_q },
		{ u"LuminaLinkSafetyInfo"_q, u"Pergunta antes de abrir um link que "
			u"esconde o destino real atrás do texto antes de um sinal @, que "
			u"escreve o domínio em punycode, ou que passa por um encurtador "
			u"de links conhecido. A confirmação mostra o host de destino real "
			u"e o endereço completo. O Telegram Desktop já avisa por conta "
			u"própria sobre domínios parecidos escritos em outros alfabetos. "
			u"Nada é verificado on-line — nenhum endereço que você abre sai "
			u"deste dispositivo."_q },
		{ u"LuminaLinkSafetyRow"_q, u"Inspetor de segurança de links"_q },
		{ u"LuminaLinkSafetyTitle"_q, u"Abrir link externo?"_q },
		{ u"LuminaLinkSafetyWarnMismatch"_q, u"Este link esconde o destino "
			u"real atrás do texto antes do sinal «@»."_q },
		{ u"LuminaLinkSafetyWarnPunycode"_q, u"Este endereço usa caracteres "
			u"codificados (punycode) que podem imitar um site conhecido."_q },
		{ u"LuminaLinkSafetyWarnShortener"_q, u"É um encurtador de links — o "
			u"destino real fica oculto até você abri-lo."_q },
		{ u"LuminaMediaAutoPauseBgVideo"_q,
			u"Pausar o vídeo quando o app vai para segundo plano"_q },
		{ u"LuminaMediaAutoPauseBgVideoInfo"_q, u"Pausa automaticamente o "
			u"vídeo em reprodução quando você sai do LuminaGram. Minimizar o "
			u"visualizador também o pausa. Um vídeo que você colocou de "
			u"propósito em uma janela própria, ou em picture-in-picture, "
			u"continua tocando, e as músicas e as mensagens de voz continuam "
			u"tocando enquanto você está fora."_q },
		{ u"LuminaMediaSaving"_q, u"Mídia"_q },
		{ u"LuminaMediaTitle"_q, u"Mídia"_q },
		{ u"LuminaMessageActionsInfo"_q, u"Estas entradas aparecem somente no "
			u"menu de contexto de uma mensagem. Cada uma usa o encaminhamento "
			u"do próprio Telegram, então uma conversa que restringe o "
			u"encaminhamento continua restrita. «Encaminhar sem autor e sem "
			u"legendas» tem esse nome pelo que realmente sai: o Telegram não "
			u"consegue remover as legendas mantendo o remetente, então "
			u"escolher essa opção remove os dois. «Detalhes» lê apenas o que "
			u"este dispositivo já sincronizou — nada é solicitado e nada é "
			u"guardado."_q },
		{ u"LuminaMessageDetails"_q, u"Detalhes"_q },
		{ u"LuminaOnboardingDualName"_q, u"Os dois idiomas ao mesmo tempo"_q },
		{ u"LuminaOnboardingDualText"_q, u"Mantenha o texto original na tela "
			u"ao lado da tradução, nas mensagens que você recebe e nas que "
			u"você envia."_q },
		{ u"LuminaOnboardingFooter"_q, u"Abra uma seção na página de "
			u"configurações do LuminaGram para configurar qualquer um desses "
			u"recursos. Você pode ler este cartão de novo em Ferramentas."_q },
		{ u"LuminaOnboardingGotIt"_q, u"Entendi"_q },
		{ u"LuminaOnboardingHeader"_q, u"Sobre"_q },
		{ u"LuminaOnboardingIntro"_q, u"O LuminaGram adiciona as próprias "
			u"ferramentas sobre o Telegram Desktop. Todas as opções abaixo "
			u"ficam salvas somente neste computador e nunca são sincronizadas "
			u"com o Telegram."_q },
		{ u"LuminaOnboardingRow"_q, u"O que o LuminaGram adiciona"_q },
		{ u"LuminaOnboardingRowAbout"_q, u"Mostra o cartão de boas-vindas que "
			u"aparece na primeira vez que você abre as configurações do "
			u"LuminaGram. Ele apenas explica o que existe aqui e não ativa "
			u"nada."_q },
		{ u"LuminaOnboardingSafetyName"_q, u"Verificações de segurança"_q },
		{ u"LuminaOnboardingSafetyText"_q, u"Avisa antes de abrir um link "
			u"cujo endereço não é o que parece, detecta um endereço de cripto "
			u"trocado quando você cola um, e remove a localização das fotos "
			u"que você envia."_q },
		{ u"LuminaOnboardingTitle"_q, u"Boas-vindas ao LuminaGram"_q },
		{ u"LuminaOnboardingTranslateText"_q, u"Traduza as mensagens "
			u"recebidas com o seu próprio serviço de tradução, e traduza o "
			u"que você digita antes de ser enviado."_q },
		{ u"LuminaOnboardingVaultName"_q, u"Cofre de disfarce"_q },
		{ u"LuminaOnboardingVaultText"_q, u"Esconda o LuminaGram atrás de uma "
			u"calculadora ou de um bloco de notas que abre o app real somente "
			u"com o seu código secreto."_q },
		{ u"LuminaProfileCardAbout"_q, u"Um cartão local sobre você — os "
			u"idiomas que você fala, os seus interesses — que você pode "
			u"copiar e colar em uma conversa. Ele fica neste dispositivo."_q },
		{ u"LuminaProfileCardBio"_q, u"Biografia curta"_q },
		{ u"LuminaProfileCardBioHint"_q, u"Algumas palavras sobre você"_q },
		{ u"LuminaProfileCardCopied"_q, u"Cartão copiado"_q },
		{ u"LuminaProfileCardCopy"_q,
			u"Copiar para a área de transferência"_q },
		{ u"LuminaProfileCardCopyInfo"_q, u"Gera um resumo em texto do seu "
			u"cartão. Cole-o em qualquer conversa quando quiser "
			u"compartilhá-lo."_q },
		{ u"LuminaProfileCardEdit"_q, u"Meu cartão"_q },
		{ u"LuminaProfileCardEmptyShare"_q,
			u"Preencha seu cartão primeiro"_q },
		{ u"LuminaProfileCardHeader"_q, u"Meu cartão"_q },
		{ u"LuminaProfileCardInfo"_q, u"Este cartão fica salvo apenas neste "
			u"dispositivo e nunca é enviado ao Telegram. Ele não altera o seu "
			u"perfil do Telegram."_q },
		{ u"LuminaProfileCardInterests"_q, u"Interesses / tags"_q },
		{ u"LuminaProfileCardInterestsHint"_q,
			u"ex.: música, trilhas, programação"_q },
		{ u"LuminaProfileCardLanguages"_q, u"Idiomas que falo"_q },
		{ u"LuminaProfileCardLanguagesHint"_q,
			u"ex.: Português, English, Español"_q },
		{ u"LuminaProfileCardNotSet"_q, u"Não definido"_q },
		{ u"LuminaProfileCardTagline"_q, u"Lema"_q },
		{ u"LuminaProfileCardTaglineHint"_q, u"Uma frase curta sobre você"_q },
		{ u"LuminaProfileCardTitle"_q, u"Cartão de perfil"_q },
		{ u"LuminaProfileChatCreated"_q, u"Criado"_q },
		{ u"LuminaProfileDcId"_q, u"Datacenter"_q },
		{ u"LuminaProfileDcIdValue"_q, u"DC{1}"_q },
		{ u"LuminaProfileInfoHeader"_q, u"Perfil"_q },
		{ u"LuminaProfileInfoInfo"_q, u"Linhas extras nas páginas de perfil, "
			u"todas calculadas neste dispositivo — nada é pedido ao Telegram. "
			u"O próprio Telegram fornece a data de registro de algumas "
			u"contas; para as demais, ela é estimada a partir do número da "
			u"conta e exibida com um «~». O datacenter é o que armazena a "
			u"foto do perfil. A data de criação é quando um grupo ou canal "
			u"foi criado."_q },
		{ u"LuminaProfileRegistrationApprox"_q, u"~ {1}"_q },
		{ u"LuminaProfileRegistrationDate"_q, u"Data de registro"_q },
		{ u"LuminaProfileShowChatDate"_q, u"Mostrar data de criação"_q },
		{ u"LuminaProfileShowDcId"_q, u"Mostrar datacenter"_q },
		{ u"LuminaProfileShowRegistrationDate"_q,
			u"Mostrar data de registro"_q },
		{ u"LuminaRecentLimitsInfo"_q, u"Impede que este dispositivo descarte "
			u"figurinhas recentes e GIFs salvos antes do necessário, usando "
			u"os mesmos limites do LuminaGram para Android: até 200 "
			u"figurinhas recentes e 500 GIFs salvos. Os servidores do "
			u"Telegram continuam decidindo quantos ficam realmente guardados "
			u"na sua conta e sincronizados com seus outros dispositivos, "
			u"então isto não deixa a lista da nuvem maior — na próxima vez "
			u"que este dispositivo sincronizar, tudo o que o servidor não "
			u"guarda mais some daqui também, normalmente em segundos. "
			u"Enquanto isto estiver ativado, o Telegram Desktop deixa de "
			u"remover um GIF salvo por conta própria, então o aviso Premium "
			u"sobre o limite de GIFs salvos fica quieto. O painel de "
			u"figurinhas ainda mostra apenas as 20 primeiras figurinhas "
			u"recentes, a menos que você também ative «Figurinhas recentes "
			u"ilimitadas» em Configurações › Avançado › Configurações "
			u"experimentais."_q },
		{ u"LuminaRecentLimitsRow"_q,
			u"Guardar mais figurinhas recentes e GIFs"_q },
		{ u"LuminaReplyTemplatesAbout"_q, u"Trechos curtos de texto que você "
			u"guarda neste dispositivo e insere no campo de mensagem. Clique "
			u"com o botão direito no botão de emoji em uma conversa para "
			u"escolher um. Os modelos nunca são enviados ao Telegram."_q },
		{ u"LuminaReplyTemplatesAdd"_q, u"Adicionar modelo"_q },
		{ u"LuminaReplyTemplatesEdit"_q, u"Editar modelo"_q },
		{ u"LuminaReplyTemplatesEmpty"_q, u"Nenhum modelo ainda. Adicione um "
			u"e depois clique com o botão direito no botão de emoji em uma "
			u"conversa para inseri-lo."_q },
		{ u"LuminaReplyTemplatesFull"_q,
			u"A lista está cheia — exclua um modelo para adicionar outro."_q },
		{ u"LuminaReplyTemplatesInfo"_q, u"Clique com o botão direito no "
			u"botão de emoji em uma conversa para inserir um modelo. Clique "
			u"com o botão direito em um modelo aqui para movê-lo para cima ou "
			u"para baixo; abra um para editá-lo ou excluí-lo."_q },
		{ u"LuminaReplyTemplatesList"_q, u"Modelos"_q },
		{ u"LuminaReplyTemplatesManage"_q, u"Gerenciar modelos…"_q },
		{ u"LuminaReplyTemplatesMoveDown"_q, u"Mover para baixo"_q },
		{ u"LuminaReplyTemplatesMoveUp"_q, u"Mover para cima"_q },
		{ u"LuminaReplyTemplatesNone"_q, u"Nenhum"_q },
		{ u"LuminaReplyTemplatesPlaceholder"_q, u"Texto do modelo"_q },
		{ u"LuminaReplyTemplatesShow"_q, u"Oferecer modelos nas conversas"_q },
		{ u"LuminaReplyTemplatesTitle"_q, u"Modelos de resposta"_q },
		{ u"LuminaSaveSticker"_q, u"Salvar figurinha"_q },
		{ u"LuminaSaveStickers"_q, u"Salvar figurinhas"_q },
		{ u"LuminaSaveStickersInfo"_q, u"Adiciona uma linha «Salvar "
			u"figurinha» ao menu de contexto do painel de figurinhas. As "
			u"figurinhas vão para onde vão os seus outros downloads, e a "
			u"linha fica oculta no pacote de figurinhas próprio de um grupo "
			u"quando esse grupo restringe o salvamento."_q },
		{ u"LuminaSaveToCloud"_q, u"Salvar em Mensagens Salvas"_q },
		{ u"LuminaSaveToCloudTitle"_q, u"Salvar em Mensagens Salvas"_q },
		{ u"LuminaSecurityPanicConfirmAck"_q,
			u"Eu entendo que isso não pode ser desfeito"_q },
		{ u"LuminaSecurityPanicConfirmButton"_q, u"Apagar agora"_q },
		{ u"LuminaSecurityPanicConfirmText"_q, u"Todas as contas deste "
			u"dispositivo serão desconectadas. O banco de dados local de "
			u"mensagens, os rascunhos e a mídia em cache serão apagados, "
			u"junto com as configurações do próprio LuminaGram, os "
			u"marcadores, as traduções salvas e as chaves de API.\n\nSuas "
			u"contas não são excluídas. Elas continuam nos servidores do "
			u"Telegram, e as suas mensagens também — você pode entrar de novo "
			u"de qualquer lugar.\n\nOs arquivos que já foram baixados NÃO "
			u"são excluídos. O LuminaGram não mexe na sua pasta de downloads, "
			u"porque normalmente ela é a sua pasta Downloads comum e guarda "
			u"arquivos sem relação com o app. Mova ou exclua você mesmo o que "
			u"for sensível ali.\n\nIsso não pode ser desfeito."_q },
		{ u"LuminaSecurityPanicConfirmTitle"_q, u"Limpeza de emergência?"_q },
		{ u"LuminaSecurityPanicHeader"_q, u"Limpeza de emergência"_q },
		{ u"LuminaSecurityPanicWipe"_q, u"Limpeza de emergência (Kaboom)"_q },
		{ u"LuminaSecurityPanicWipeAbout"_q, u"Encerra a sessão de todas as "
			u"contas deste dispositivo e apaga o banco de dados local de "
			u"mensagens, os rascunhos e a mídia em cache, junto com as "
			u"configurações do próprio LuminaGram, os marcadores, as "
			u"traduções salvas e as chaves de API. Suas contas e suas "
			u"mensagens continuam nos servidores do Telegram. Os arquivos que "
			u"você já baixou ficam onde estão. Isso não pode ser "
			u"desfeito."_q },
		{ u"LuminaSelectFromAuthor"_q, u"Selecionar tudo do autor"_q },
		{ u"LuminaSelectFromAuthorAbout"_q, u"Adiciona no menu de mensagens "
			u"dos grupos uma entrada que seleciona todas as mensagens do "
			u"remetente em que você clicou. Ela alcança apenas as mensagens "
			u"que esta janela já carregou — role mais para trás e repita para "
			u"incluir as mais antigas. Mensagens atribuídas à própria "
			u"conversa, como publicações de canal e publicações de "
			u"administradores anônimos, não recebem a entrada."_q },
		{ u"LuminaSelectFromAuthorLimit"_q,
			u"Só é possível selecionar {1} mensagens de uma vez."_q },
		{ u"LuminaSelectFromAuthorTitle"_q, u"Selecionar tudo do autor"_q },
		{ u"LuminaSendOriginalCaption"_q, u"Enviar legenda original"_q },
		{ u"LuminaShowBookmarks"_q, u"Mostrar item de marcador no menu"_q },
		{ u"LuminaShowMessageDetails"_q, u"Menu de detalhes da mensagem"_q },
		{ u"LuminaShowMutedCount"_q,
			u"Sempre mostrar contador de não lidas"_q },
		{ u"LuminaShowMutedCountInfo"_q, u"Desenha o selo de não lidas das "
			u"conversas silenciadas na cor de destaque normal em vez do cinza "
			u"silenciado."_q },
		{ u"LuminaStickerSavedTo"_q, u"Figurinha salva em {1}"_q },
		{ u"LuminaStickerSizeChoice"_q, u"{1}%"_q },
		{ u"LuminaStickerSizeChoiceDefault"_q, u"{1}% (padrão)"_q },
		{ u"LuminaStickerSizeInfo"_q, u"O tamanho com que as figurinhas são "
			u"desenhadas nas conversas — as que você envia e as que você "
			u"recebe. Emojis animados, dados e figurinhas de presente mantêm "
			u"os próprios tamanhos. O Telegram Desktop mede uma figurinha uma "
			u"vez e guarda essa medida, então um tamanho novo passa a valer "
			u"na próxima vez que o app iniciar."_q },
		{ u"LuminaTimeWithSeconds"_q,
			u"Mostrar segundos na hora da mensagem"_q },
		{ u"LuminaTimeWithSecondsAbout"_q, u"A hora abaixo de uma mensagem, a "
			u"hora no texto copiado e a hora que um leitor de tela anuncia "
			u"passam a incluir os segundos."_q },
		// The tray menu and the taskbar button's jump list, where {1} is
		// always the bare product name.
		{ u"LuminaTrayOpen"_q, u"Abrir o {1}"_q },
		{ u"LuminaTrayQuit"_q, u"Sair do {1}"_q },
		{ u"LuminaUndoSendBulletin"_q, u"Enviando mensagem…"_q },
		{ u"LuminaUndoSendTitle"_q, u"Desfazer envio"_q },
		{ u"LuminaUndoSendUndo"_q, u"Desfazer"_q },
		{ u"LuminaUndoSendWindow"_q, u"Janela para desfazer o envio"_q },
		{ u"LuminaUndoSendWindowInfo"_q, u"Retém uma mensagem de texto "
			u"simples por {1} segundos atrás de um botão Desfazer antes de "
			u"enviá-la. Seu texto continua na caixa de mensagem esse tempo "
			u"todo, e a caixa só é limpa quando a mensagem realmente sai, "
			u"então Desfazer simplesmente deixa o texto onde está — nada é "
			u"retirado e devolvido. Enviar de novo, abrir outra conversa ou "
			u"fechar o app envia na hora a mensagem retida. Mensagens de "
			u"mídia, de voz, editadas, encaminhadas e agendadas nunca são "
			u"retidas, e o mesmo vale para mensagens enviadas de um tópico de "
			u"fórum ou de uma thread de comentários."_q },
		// clipboard guard + scam watch
		{ u"LuminaPrivacyCryptoClipboardGuard"_q, u"Proteção ao colar endereço "
			u"de cripto"_q },
		{ u"LuminaPrivacyCryptoClipboardGuardInfo"_q, u"Pergunta antes de "
			u"colar em uma mensagem algo que parece um endereço de carteira de "
			u"criptomoedas. Malware que sequestra a área de transferência pode "
			u"trocar o endereço copiado pelo de um golpista sem você perceber, "
			u"e colar é o último momento em que dá para notar. A verificação é "
			u"feita neste aparelho e nada é enviado para lugar nenhum."_q },
		{ u"LuminaPrivacyScamKeywordWarning"_q, u"Aviso de palavras de "
			u"golpe"_q },
		{ u"LuminaPrivacyScamKeywordWarningInfo"_q, u"Quando uma mensagem de "
			u"alguém que não está nos seus contatos menciona táticas comuns de "
			u"golpe — transferências de dinheiro, cartões-presente, "
			u"“investimentos” em cripto, taxas de verificação ou pedir códigos "
			u"— é exibido um lembrete único para você ter cuidado. A mensagem "
			u"nunca é bloqueada nem alterada. As verificações são feitas "
			u"off-line no seu dispositivo."_q },
		// otp guard
		{ u"LuminaOtpGuardTitle"_q, u"Aviso"_q },
		{ u"LuminaOtpGuardMessage"_q, u"Você está prestes a enviar o seu código de login do Telegram para outra pessoa. Quem receber esse código pode entrar na sua conta e ler todas as suas mensagens. O Telegram nunca pede esse código."_q },
		{ u"LuminaOtpGuardCancel"_q, u"Não enviar"_q },
		{ u"LuminaOtpGuardSendAnyway"_q, u"Entendo o risco, enviar mesmo assim"_q },
		{ u"LuminaOtpGuard"_q, u"Proteção do código de login"_q },
		{ u"LuminaOtpGuardInfo"_q, u"Avisa antes de você enviar uma mensagem com um código de login, se o Telegram acabou de enviar um para você. A verificação acontece inteiramente no seu aparelho: nada é enviado para fora e sua mensagem nunca é alterada."_q },
		// link preview + voice confirm
		{ u"LuminaPrivacyDisableLinkPreview"_q, u"Desativar a prévia de links "
			u"por padrão"_q },
		{ u"LuminaLinkPreviewInfo"_q, u"As mensagens que você escreve do zero "
			u"são enviadas sem prévia de link, e o campo de digitação nunca "
			u"pede ao Telegram que resolva um link colado enquanto você "
			u"digita. Uma prévia já anexada, e uma mensagem que você edita, "
			u"ficam como estão. Para anexar mesmo assim uma prévia a uma "
			u"mensagem específica, atribua uma tecla a «Alternar prévia do "
			u"link» em Configurações > Atalhos de teclado e pressione-a "
			u"enquanto escreve."_q },
		{ u"LuminaVoiceConfirmTitle"_q, u"Mensagens de voz e vídeo"_q },
		{ u"LuminaVoiceConfirmRow"_q, u"Confirmar antes de enviar"_q },
		{ u"LuminaVoiceConfirmInfo"_q, u"Perguntar antes de enviar uma "
			u"mensagem de voz ou um vídeo redondo já gravado. Desistir dessa "
			u"pergunta descarta a gravação, exceto na prévia de audição, onde "
			u"a gravação é mantida. Envios agendados nunca são perguntados."_q },
		// exif strip + text replace
		{ u"LuminaExifStripRow"_q, u"Remover a localização da foto antes de "
			u"enviar"_q },
		{ u"LuminaExifStripInfo"_q, u"Apaga as coordenadas GPS que a câmera "
			u"gravou dentro de uma foto JPEG antes de ela ser enviada — tanto "
			u"quando você envia a foto do jeito comprimido comum quanto "
			u"quando a envia como arquivo. Só as etiquetas de localização "
			u"saem: as de orientação, câmera e data ficam exatamente como "
			u"estavam, então uma foto enviada como arquivo continua "
			u"aparecendo na posição certa. Sua própria cópia do arquivo no "
			u"disco nunca é modificada. Isso não cobre vídeos, a localização "
			u"escrita em uma foto pelo XMP ou pelas notas do próprio "
			u"fabricante da câmera, nem a segunda cópia da imagem que alguns "
			u"celulares guardam dentro do mesmo arquivo."_q },
		{ u"LuminaReplacerTitle"_q, u"Substituição de texto"_q },
		{ u"LuminaReplacerHeader"_q, u"Regras"_q },
		{ u"LuminaReplacerAdd"_q, u"Adicionar regra"_q },
		{ u"LuminaReplacerEdit"_q, u"Editar regra"_q },
		{ u"LuminaReplacerFrom"_q, u"Substituir"_q },
		{ u"LuminaReplacerTo"_q, u"Por"_q },
		{ u"LuminaReplacerEnable"_q, u"Substituir texto nas mensagens que eu "
			u"envio"_q },
		{ u"LuminaReplacerNone"_q, u"Nenhuma"_q },
		{ u"LuminaReplacerNothing"_q, u"(nada)"_q },
		{ u"LuminaReplacerEmpty"_q, u"Nenhuma regra ainda. Adicione uma para "
			u"começar a substituir texto nas mensagens que você envia."_q },
		{ u"LuminaReplacerFull"_q, u"As regras são aplicadas em ordem, de "
			u"cima para baixo. A lista está cheia — exclua uma regra para "
			u"adicionar outra."_q },
		{ u"LuminaReplacerListInfo"_q, u"As regras são aplicadas em ordem, de "
			u"cima para baixo, e cada uma atua sobre o que a anterior "
			u"produziu."_q },
		{ u"LuminaReplacerAbout"_q, u"Troca uma abreviação pela frase que "
			u"você queria dizer em cada mensagem que envia: «brb» vira «be "
			u"right back». A correspondência diferencia maiúsculas e só vale "
			u"para palavras inteiras. Uma mensagem que tenha qualquer "
			u"formatação é enviada exatamente como foi digitada, e links, "
			u"menções, hashtags e comandos de bots nunca são reescritos. As "
			u"regras ficam neste dispositivo e nunca são enviadas ao "
			u"Telegram."_q },
		// vault + fake crash
		{ u"LuminaVaultHeader"_q, u"Cofre de disfarce"_q },
		{ u"LuminaVaultEnable"_q, u"Ativar cofre"_q },
		{ u"LuminaVaultInfo"_q, u"Tudo aqui fica somente neste dispositivo. O cofre "
			u"esconde o LuminaGram atrás de um app de aparência "
			u"inofensiva, e só o código secreto leva você de volta ao "
			u"app real."_q },
		{ u"LuminaVaultMode"_q, u"Modo do cofre"_q },
		{ u"LuminaVaultModePasswordDoor"_q, u"Porta com senha"_q },
		{ u"LuminaVaultModeDecoyApp"_q, u"App falso"_q },
		{ u"LuminaVaultModePasswordDoorInfo"_q, u"Ao iniciar o LuminaGram, uma senha é "
			u"pedida. O código secreto abre o app real; qualquer outra "
			u"coisa abre o app falso, e nunca avisa que estava errada."_q },
		{ u"LuminaVaultModeDecoyAppInfo"_q, u"Ao iniciar o LuminaGram, o app falso abre "
			u"direto. Digite nele seu código secreto para chegar ao app "
			u"real."_q },
		{ u"LuminaVaultSkin"_q, u"Estilo do app falso"_q },
		{ u"LuminaVaultSkinNotepad"_q, u"Notas"_q },
		{ u"LuminaVaultSkinCalculator"_q, u"Calculadora"_q },
		{ u"LuminaVaultSkinHintCalculator"_q, u"Para desbloquear pela calculadora, digite "
			u"o código secreto e pressione a tecla de igual."_q },
		{ u"LuminaVaultSkinHintNotepad"_q, u"Para desbloquear pelas notas, faça o código "
			u"secreto ser a nota inteira e depois dê um duplo clique na "
			u"barra de título de Notas."_q },
		{ u"LuminaVaultSecretCode"_q, u"Código secreto"_q },
		{ u"LuminaVaultSecretCodeDialogTitle"_q, u"Código secreto"_q },
		{ u"LuminaDisguiseDecoyCodeSet"_q, u"Definido"_q },
		{ u"LuminaDisguiseDecoyCodeNotSet"_q, u"Não definido"_q },
		{ u"LuminaVaultNoCodeInfo"_q, u"O cofre fica desligado até você definir um código "
			u"secreto."_q },
		{ u"LuminaVaultCalculatorCodeInfo"_q, u"Este código não pode ser digitado no "
			u"teclado da calculadora, então a nota falsa é mostrada no "
			u"lugar dela. Use apenas dígitos, um ponto e + - * / se "
			u"quiser a calculadora."_q },
		{ u"LuminaVaultDisclaimer"_q, u"Isto engana um olhar por cima do ombro, não uma "
			u"inspeção bem-informada: o programa continua se chamando "
			u"LuminaGram no gerenciador de tarefas, e quem conseguir ler "
			u"os arquivos deste dispositivo consegue chegar à sua conta. "
			u"O cofre aparece na próxima vez que o LuminaGram iniciar, e "
			u"fechar o app falso encerra o LuminaGram. Se algum dia você "
			u"esquecer o código, exclua tdata/luminagram.json e "
			u"tdata/luminagram_private.json da pasta de dados do "
			u"LuminaGram: o cofre então se desliga sozinho e o "
			u"LuminaGram inicia normalmente."_q },
		{ u"LuminaSecurityFakeCrashHeader"_q, u"Desbloqueio com falha falsa"_q },
		{ u"LuminaSecurityFakeCrashEnable"_q, u"Ativar desbloqueio com falha falsa"_q },
		{ u"LuminaSecurityFakeCrashCode"_q, u"Código de falha falsa"_q },
		{ u"LuminaSecurityFakeCrashCodeDialogTitle"_q, u"Código de falha falsa"_q },
		{ u"LuminaSecurityFakeCrashCodeHint"_q, u"Código"_q },
		{ u"LuminaSecurityFakeCrashCodeSet"_q, u"Definido"_q },
		{ u"LuminaSecurityFakeCrashCodeNotSet"_q, u"Não definido"_q },
		{ u"LuminaSecurityFakeCrashCodeSameAsPasscode"_q, u"Este código não pode ser igual "
			u"à sua senha de bloqueio."_q },
		{ u"LuminaSecurityFakeCrashInfo"_q, u"Quando isto está ativado, digitar o código "
			u"de falha falsa na tela de bloqueio fecha o LuminaGram na "
			u"hora em vez de desbloqueá-lo, sem nenhuma mensagem e sem "
			u"deixar nada na tela — para quem estiver olhando, o app "
			u"travou. O código é separado da sua senha de bloqueio, fica "
			u"somente neste dispositivo e nunca é enviado a lugar algum."_q },
		{ u"LuminaSecurityFakeCrashBiometricInfo"_q, u"Enquanto isto estiver ativado e "
			u"houver um código definido, o desbloqueio com Touch ID, "
			u"Windows Hello ou a senha do sistema fica desligado: a "
			u"digital abre o app sem nunca pedir a senha de bloqueio, "
			u"então passaria direto por este código. Apagar o código, ou "
			u"desativar isto, traz tudo de volta."_q },
		{ u"LuminaSecurityFakeCrashNoCodeInfo"_q, u"Nenhum código foi definido ainda, "
			u"então nada vai acontecer na tela de bloqueio."_q },
		{ u"LuminaSecurityFakeCrashNoPasscodeInfo"_q, u"O LuminaGram não tem senha de "
			u"bloqueio, então a tela de bloqueio nunca aparece e este "
			u"código nunca é pedido. Ative primeiro a senha de bloqueio "
			u"em Privacidade e segurança."_q },
		{ u"LuminaSecurityFakeCrashCodeClashInfo"_q, u"Este código agora é igual à sua "
			u"senha de bloqueio, então ele nunca vai funcionar — a senha "
			u"desbloqueia o app no lugar dele. Escolha um código "
			u"diferente."_q },
		// login guard (D-32)
		{ u"LuminaSessionGuardHeader"_q, u"Proteção de login"_q },
		{ u"LuminaSessionGuard"_q, u"Alertas de novos logins"_q },
		{ u"LuminaSessionGuardInfo"_q, u"Verifica seus dispositivos conectados ao abrir o app e avisa sobre qualquer sessão que você não aprovou. Escanear um QR code de login entrega à outra pessoa uma sessão ativa que lê todas as suas conversas. A verificação acontece no máximo a cada 30 minutos, usa a API oficial do Telegram e nada sai do seu aparelho."_q },
		{ u"LuminaSessionGuardCheckNow"_q, u"Verificar dispositivos conectados agora"_q },
		{ u"LuminaSessionGuardNoNew"_q, u"Nenhum login novo. Todos os dispositivos conectados já foram aprovados por você."_q },
		{ u"LuminaSessionGuardCheckFailed"_q, u"Não foi possível verificar os dispositivos conectados. Confira sua conexão e tente de novo."_q },
		{ u"LuminaSessionAlertTitle"_q, u"Novo login detectado"_q },
		{ u"LuminaSessionAlertIntro"_q, u"Um dispositivo que você não aprovou entrou na sua conta. Se não foi você, ele pode ler todas as suas conversas agora mesmo."_q },
		{ u"LuminaSessionAlertDevice"_q, u"Dispositivo"_q },
		{ u"LuminaSessionAlertApp"_q, u"App"_q },
		{ u"LuminaSessionAlertIp"_q, u"Endereço IP"_q },
		{ u"LuminaSessionAlertLocation"_q, u"Local"_q },
		{ u"LuminaSessionAlertTime"_q, u"Entrou em"_q },
		{ u"LuminaSessionAlertNotMe"_q, u"Não fui eu — encerrar agora"_q },
		{ u"LuminaSessionAlertItsMe"_q, u"Fui eu"_q },
		{ u"LuminaSessionTerminated"_q, u"Sessão encerrada"_q },
		{ u"LuminaSessionTerminateFailed"_q, u"Não foi possível encerrar essa sessão. Abra Configurações → Dispositivos e remova por lá."_q },
		{ u"LuminaSession2FAMessage"_q, u"Esse dispositivo foi desconectado. Ative a verificação em duas etapas para que ninguém entre só com um código."_q },
		{ u"LuminaSession2FAButton"_q, u"Configurar agora"_q },
		{ u"LuminaSessionLater"_q, u"Depois"_q },
		{ u"LuminaSessionQrConfirmTitle"_q, u"Autorizar um novo dispositivo?"_q },
		{ u"LuminaSessionQrConfirmMessage"_q, u"Você está prestes a autorizar um novo dispositivo a entrar na sua conta. A não ser que você mesmo esteja na frente daquele computador, quem o controla vai conseguir ler todas as suas mensagens. Nunca escaneie um QR code de login que alguém enviou para você."_q },
		{ u"LuminaSessionQrConfirmContinue"_q, u"Estou nesse dispositivo — continuar"_q },

		// Stories, fully off.
		{ u"LuminaStoriesHeader"_q, u"Stories"_q },
		{ u"LuminaStoriesFullyOff"_q, u"Desativar os stories completamente"_q },
		{ u"LuminaStoriesFullyOffInfo"_q, u"Remove os stories do aplicativo "
			u"inteiro: a faixa acima da lista de conversas e a do arquivo, os "
			u"anéis coloridos nas fotos de perfil, o bloco de stories e a aba "
			u"Stories nos perfis, e as notificações de stories. Nada muda nos "
			u"servidores do Telegram — os seus stories e os dos outros "
			u"continuam lá, apenas não são mais exibidos aqui."_q },
		{ u"LuminaStoriesHidePostEntry"_q, u"Ocultar também o botão de publicar"_q },
		{ u"LuminaStoriesHidePostEntryInfo"_q, u"Oculta o botão de câmera usado "
			u"para publicar o seu próprio story, na lista de conversas e nos "
			u"perfis."_q },

		// Voice to text (F-32), keys shared with Android's
		// LuminaLocale / LuminaVoiceToTextActivity.
		{ u"LuminaVoiceToTextTitle"_q, u"Voz para texto"_q },
		{ u"LuminaSttEnable"_q, u"Ativar voz para texto"_q },
		{ u"LuminaSttEngine"_q, u"Mecanismo"_q },
		{ u"LuminaSttEngineWhisper"_q, u"OpenAI Whisper (sua própria chave)"_q },
		{ u"LuminaSttEngineGoogle"_q, u"Google (sua própria chave)"_q },
		{ u"LuminaSttKey"_q, u"Chave de API"_q },
		{ u"LuminaSttBaseUrl"_q, u"URL base"_q },
		{ u"LuminaSttModel"_q, u"Modelo"_q },
		{ u"LuminaSttInfoDesktop"_q, u"A transcrição usa sua própria chave de API e sua própria "
			u"cota; o LuminaGram nunca fornece uma chave. A mensagem de "
			u"voz é enviada ao serviço escolhido abaixo, então ative "
			u"isso apenas para um serviço ao qual você aceite enviar "
			u"suas mensagens."_q },
		{ u"LuminaSttVoskUnsupported"_q, u"O mecanismo offline (Vosk) ainda não é compatível no "
			u"desktop: ele precisa de uma biblioteca nativa e de um "
			u"modelo de voz empacotados separadamente para Windows, "
			u"macOS e Linux. Até lá, a transcrição no desktop é somente "
			u"na nuvem."_q },
		{ u"LuminaSttUiMenuItem"_q, u"Transcrever"_q },
		{ u"LuminaSttUiPickTitle"_q, u"Voz para texto"_q },
		{ u"LuminaSttUiFetching"_q, u"Baixando a mensagem de voz…"_q },
		{ u"LuminaSttUiTranscribing"_q, u"Transcrevendo…"_q },
		{ u"LuminaSttUiTranslation"_q, u"Tradução"_q },
		{ u"LuminaSttUiError"_q, u"Falha na transcrição"_q },
		{ u"LuminaSttUiNoText"_q, u"Nenhuma fala detectada"_q },
		{ u"LuminaSttUiNoKey"_q, u"Nenhuma chave de API definida. Adicione uma em "
			u"Configurações, LuminaGram, Voz para texto."_q },
		{ u"LuminaSttUiTooLarge"_q, u"Esta mensagem de voz é grande demais para o mecanismo "
			u"selecionado."_q },
		{ u"LuminaSttUiRoundUnsupported"_q, u"O Google Speech não lê mensagens de vídeo. Escolha o "
			u"Whisper para vídeos redondos."_q },
		{ u"LuminaSttAutoTranslate"_q, u"Traduzir depois de transcrever"_q },
		{ u"LuminaSttAutoTranslateInfo"_q, u"Quando uma mensagem de voz é transcrita em um idioma que "
			u"você não lê, o LuminaGram traduz na hora e mostra "
			u"primeiro a transcrição e depois a tradução. Se a "
			u"transcrição já estiver no seu idioma de leitura, nada é "
			u"traduzido e nenhuma cota é gasta, e se a tradução falhar "
			u"a transcrição continua na tela."_q },
	};
}

[[maybe_unused]] const auto kRegistered = RegisterLocaleTable(
	"pt-br",
	Build);

} // namespace
} // namespace Lumina
