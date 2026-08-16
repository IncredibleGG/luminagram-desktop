/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_locale.h"

namespace Lumina {
namespace {

// Traditional Chinese (Taiwan usage: 訊息 / 傳送 / 設定 / 影片 / 檔案 /
// 應用程式). Wording is shared with Android's LuminaLocale.java wherever the
// two platforms show the same English; where the desktop text was reworded,
// only the terminology is carried over and the desktop English is translated.
//
// Continuation lines repeat the u prefix on purpose: every piece of a
// concatenated literal then carries the same encoding, which keeps the
// multi-byte characters unambiguous for every compiler this fork builds with.
[[nodiscard]] LocaleTable Build() {
	return {
		// Sections of the LuminaGram settings page.
		{ u"LuminaGramTitle"_q, u"LuminaGram"_q },
		{ u"LuminaTranslateTitle"_q, u"翻譯"_q },
		{ u"LuminaPrivacyTitle"_q, u"隱私"_q },
		{ u"LuminaSecurityTitle"_q, u"安全"_q },
		{ u"LuminaChatSettings"_q, u"聊天"_q },
		{ u"LuminaGramChatList"_q, u"聊天列表"_q },
		{ u"LuminaAppearanceTitle"_q, u"外觀"_q },
		{ u"LuminaToolsTitle"_q, u"工具"_q },
		{ u"LuminaGramStoredLocallyInfo"_q, u"LuminaGram 的選項只會儲存在這台"
			u"裝置上，不會同步到 Telegram。"_q },

		// Sub-pages that have no rows yet.
		{ u"LuminaAppearancePlaceholder"_q, u"訊息、貼圖與數字格式的選項將會"
			u"顯示在這裡。"_q },
		{ u"LuminaChatListPlaceholder"_q, u"聊天列表的版面與角標選項將會顯示"
			u"在這裡。"_q },
		{ u"LuminaPrivacyPlaceholder"_q, u"連結、剪貼簿與傳出媒體的隱私選項"
			u"將會顯示在這裡。"_q },
		{ u"LuminaSecurityPlaceholder"_q, u"偽裝保險庫、緊急清除與脅迫解鎖的"
			u"選項將會顯示在這裡。"_q },
		{ u"LuminaToolsPlaceholder"_q, u"書籤、回覆範本與本機備份將會顯示在"
			u"這裡。"_q },

		// Chats sub-page.
		{ u"LuminaMessageActions"_q, u"訊息操作"_q },
		{ u"LuminaAllowSaveRestricted"_q, u"允許從受限聊天儲存／複製"_q },
		{ u"LuminaAllowSaveRestrictedInfo"_q, u"「允許從受限聊天儲存／複製」"
			u"只會影響這台裝置上的本機操作。有些聊天限制儲存是有原因的，請"
			u"謹慎使用。"_q },

		// Translation sub-page.
		{ u"LuminaTranslateEnable"_q, u"啟用 LuminaGram 翻譯"_q },
		{ u"LuminaTranslateEnableInfo"_q, u"使用你自己的翻譯引擎，而不是 "
			u"Telegram 的 Premium 服務。預設的引擎不需要帳號，也不需要 API "
			u"金鑰。關閉時，LuminaGram 會讓翻譯維持 Telegram Desktop 原本的"
			u"樣子。"_q },
		{ u"LuminaTranslateSendHeader"_q, u"傳送"_q },
		{ u"LuminaTranslateBeforeSend"_q, u"傳送前翻譯"_q },
		{ u"LuminaTranslateSendLang"_q, u"送出語言"_q },
		{ u"LuminaTranslateSendLangAuto"_q, u"對方的語言"_q },
		{ u"LuminaTranslateBeforeSendConfirm"_q, u"傳送前確認"_q },
		{ u"LuminaTranslateSendInfo"_q, u"這個開關只是讓這項功能可用，本身"
			u"不會翻譯任何訊息；必須在各個聊天分別開啟後，該聊天送出的訊息"
			u"才會翻譯。方法是在聊天中長按傳送鍵，開啟「傳送前翻譯」。你傳出"
			u"的訊息會翻譯成上面選擇的語言，原文也會與譯文一起保留。選擇"
			u"「對方的語言」時，LuminaGram 會在每個聊天第一次詢問要用哪個"
			u"語言，之後就記住。開啟「傳送前確認」時，會先把譯文與原文並排"
			u"顯示，讓你選擇要傳送哪一個；關閉時譯文會直接傳送出去。"_q },
		{ u"LuminaTranslateReceiveHeader"_q, u"接收"_q },
		{ u"LuminaDualLanguageDisplay"_q, u"同時顯示原文與譯文"_q },
		{ u"LuminaFoldOriginalLongMessages"_q,
			u"長訊息自動摺疊原文"_q },
		{ u"LuminaFoldOriginalLongMessagesInfo"_q,
			u"在雙語長訊息中，原文會摺疊成一行，以免洗版。點一下即可展開。譯文一律完整顯示。"_q },
		{ u"LuminaTranslateGroupSkipMyLanguages"_q,
			u"群組中略過我看得懂的語言"_q },
		{ u"LuminaExpandOriginal"_q, u"展開原文"_q },
		{ u"LuminaTranslateReadLang"_q, u"閱讀語言"_q },
		{ u"LuminaTranslateReadLangFollow"_q, u"介面語言"_q },
		{ u"LuminaTranslateReceiveInfo"_q, u"收到的訊息會以原本的大小保留"
			u"原文，譯文顯示在下方。"_q },
		{ u"LuminaTranslateProviderHeader"_q, u"服務"_q },
		{ u"LuminaTranslateProvider"_q, u"翻譯服務"_q },
		{ u"LuminaTranslateProviderLlm"_q, u"LLM（相容 OpenAI）"_q },
		{ u"LuminaTranslateApiKey"_q, u"API 金鑰"_q },
		{ u"LuminaTranslateApiKeyNotSet"_q, u"未設定"_q },
		{ u"LuminaTranslateBaseUrl"_q, u"基礎 URL"_q },
		{ u"LuminaTranslateModel"_q, u"模型"_q },
		{ u"LuminaTranslateSystemPrompt"_q, u"系統提示詞"_q },
		{ u"LuminaTranslatePromptDefault"_q, u"預設"_q },
		{ u"LuminaTranslatePromptCustom"_q, u"自訂"_q },
		{ u"LuminaTranslateFallbackTelegram"_q,
			u"這個服務失敗時改用 Telegram"_q },
		{ u"LuminaTranslateTest"_q, u"測試翻譯"_q },
		{ u"LuminaTranslateTestRunning"_q, u"測試中…"_q },
		{ u"LuminaTranslateTestSuccess"_q, u"翻譯可以正常運作。"_q },
		{ u"LuminaTranslateTestFailed"_q, u"測試失敗"_q },
		{ u"LuminaTranslateNoKey"_q, u"需要 API 金鑰"_q },
		{ u"LuminaTranslateTestKeyRejected"_q, u"無法連線到這個服務，或是"
			u"服務拒絕了這組 API 金鑰。請先檢查金鑰，再檢查基礎 URL 與你的"
			u"網路連線。"_q },
		{ u"LuminaTranslateTestNetwork"_q, u"無法連線到這個服務。請檢查你的"
			u"網路連線與代理伺服器設定，然後再試一次。"_q },
		{ u"LuminaTranslateTestNoChange"_q, u"服務把原文原封不動地退了回來，等於沒有翻譯。請檢查金鑰、模型與系統提示詞。"_q },
		{ u"LuminaTranslateTestQuota"_q, u"服務拒絕了這次請求：請求太過"
			u"頻繁，或這組金鑰的額度已經用完。請稍後再試。"_q },
		{ u"LuminaTranslateTestQuotaKeyed"_q, u"服務拒絕了這次請求：服務"
			u"拒絕了這組 API 金鑰，或這組金鑰的額度已經用完，也可能是請求"
			u"太過頻繁。請先檢查金鑰，然後稍後再試。"_q },
		{ u"LuminaTranslateTestBadResponse"_q, u"服務回覆了 LuminaGram 無法"
			u"解讀的內容，或根本沒有內容。請檢查基礎 URL 與模型。"_q },
		{ u"LuminaTranslateTestUnavailable"_q, u"這個服務無法在這裡執行。"
			u"請在上面改選其他服務。"_q },
		{ u"LuminaTranslateProviderSecurityInfo"_q, u"金鑰只會保存在這台裝置"
			u"上，存放在與其他設定分開的檔案裡，絕不會傳送給 Telegram。你"
			u"翻譯的所有內容都會送到這裡選擇的服務，請挑一個你信任的。"_q },

		// 「更多」：解釋一下與不翻譯詞清單。
		{ u"LuminaTranslateMoreHeader"_q, u"更多"_q },
		{ u"LuminaGlossaryManage"_q, u"不翻譯詞清單"_q },
		{ u"LuminaGlossaryNone"_q, u"無"_q },
		{ u"LuminaTranslateMoreInfo"_q, u"「解釋一下」會在右鍵選單裡加入一項，把一則訊息交給你自己的 LLM，說明它到底是什麼意思——語氣、俚語，以及你可以怎麼回覆。不翻譯詞清單裡的詞，在翻譯訊息時會原樣保留。"_q },
		// 不翻譯詞清單的編輯框。
		{ u"LuminaGlossaryTitle"_q, u"不翻譯詞清單"_q },
		{ u"LuminaGlossaryHeader"_q, u"詞條"_q },
		{ u"LuminaGlossaryAdd"_q, u"新增詞條"_q },
		{ u"LuminaGlossaryEdit"_q, u"編輯詞條"_q },
		{ u"LuminaGlossaryTermPlaceholder"_q, u"詞條"_q },
		{ u"LuminaGlossaryEmpty"_q, u"清單裡的詞在翻譯訊息時會原樣保留——人名、品牌、@使用者名稱、程式碼。連結和 @提及無論是否在清單裡，都會一律保留。"_q },
		{ u"LuminaGlossaryListInfo"_q, u"比對不分大小寫。連結和 @提及無論是否在清單裡，都會一律保留。"_q },

		// Translate before sending: the boxes and the send menu.
		{ u"LuminaTranslateOriginalLabel"_q, u"原文"_q },
		{ u"LuminaSendTranslation"_q, u"傳送譯文"_q },
		{ u"LuminaSendOriginal"_q, u"傳送原文"_q },
		{ u"LuminaTrSendPickerTitle"_q, u"翻譯語言"_q },
		{ u"LuminaTrSendConfirmMessage"_q, u"這個聊天看起來是用{1}寫的。要把"
			u"你在這裡傳送的訊息翻譯成{1}嗎？LuminaGram 會記住這個聊天的"
			u"選擇。"_q },
		{ u"LuminaTrSendConfirmTranslate"_q, u"翻譯"_q },
		{ u"LuminaTrSendAsTyped"_q, u"照原文傳送"_q },
		{ u"LuminaTrSendChooseOther"_q, u"選擇語言"_q },

		// The translate-before-send preview bar above the composer.
		{ u"LuminaTranslatePreviewTranslating"_q, u"翻譯中…"_q },
		{ u"LuminaTranslatePreviewFailed"_q, u"無法翻譯"_q },

		// The per-chat translate button in the chat top bar. Both texts name
		// what pressing the button will do, not what the chat is doing now.
		{ u"LuminaTranslateChatToggle"_q, u"翻譯這個聊天"_q },
		{ u"LuminaTranslateChatShowOriginal"_q, u"顯示原文"_q },

		// Pressing that button: the two rows that name this conversation's
		// language pair. Each row names its side first - 「對方」 for the
		// other person, 「己方」 for you - because a direction word never
		// said whose messages were meant. Each side has its own off string
		// instead of feeding 「不翻譯」 into {1}: 「翻譯成」 has to be
		// followed by a language, so the off state cannot reuse that row.
		{ u"LuminaChatLangThem"_q, u"對方，翻譯成{1}"_q },
		{ u"LuminaChatLangThemOff"_q, u"對方，不翻譯"_q },
		{ u"LuminaChatLangMe"_q, u"己方，翻譯成{1}"_q },
		{ u"LuminaChatLangMeOff"_q, u"己方，不翻譯"_q },
		{ u"LuminaChatLangThemTitle"_q, u"對方的訊息，翻譯成"_q },
		{ u"LuminaChatLangMeTitle"_q, u"己方的訊息，翻譯成"_q },
		{ u"LuminaChatLangNone"_q, u"不翻譯"_q },

		// The chat's tone (register): the third row of the translate menu
		// and the box it opens. The instruction the engine is actually sent
		// is written in English and is not one of these strings.
		{ u"LuminaChatRegister"_q, u"這個對話的語氣：{1}"_q },
		{ u"LuminaChatRegisterOff"_q, u"這個對話的語氣：未設定"_q },
		{ u"LuminaChatRegisterTitle"_q, u"這個對話的語氣"_q },
		{ u"LuminaChatRegisterNone"_q, u"未設定"_q },
		{ u"LuminaChatRegisterNoneInfo"_q, u"照常翻譯，不加任何語氣指示。"_q },
		{ u"LuminaChatRegisterClient"_q, u"客戶"_q },
		{ u"LuminaChatRegisterClientInfo"_q, u"禮貌、專業的商務用語；在有敬語的語言中使用敬語。"_q },
		{ u"LuminaChatRegisterColleague"_q, u"同事"_q },
		{ u"LuminaChatRegisterColleagueInfo"_q, u"日常職場用語，客氣但不生硬。"_q },
		{ u"LuminaChatRegisterFriend"_q, u"朋友"_q },
		{ u"LuminaChatRegisterFriendInfo"_q, u"輕鬆、口語化，就像跟朋友說話。"_q },
		{ u"LuminaChatRegisterFamily"_q, u"家人"_q },
		{ u"LuminaChatRegisterFamilyInfo"_q, u"溫暖親近，像在家裡說話的語氣。"_q },
		{ u"LuminaChatRegisterElder"_q, u"長輩"_q },
		{ u"LuminaChatRegisterElderInfo"_q, u"尊敬有禮，但仍然親切，不像公事公辦。"_q },
		{ u"LuminaChatRegisterRomance"_q, u"曖昧對象"_q },
		{ u"LuminaChatRegisterRomanceInfo"_q, u"溫柔俏皮，帶一點撩人的分寸。"_q },
		{ u"LuminaChatRegisterCustom"_q, u"自訂…"_q },
		{ u"LuminaChatRegisterCustomInfo"_q, u"用你自己的話描述這段關係。"_q },
		{ u"LuminaChatRegisterCustomTitle"_q, u"描述這段關係"_q },
		{ u"LuminaChatRegisterCustomHint"_q, u"例如：我的指導教授，尊敬但別太生硬"_q },
		{ u"LuminaChatRegisterUnsupported"_q, u"目前的翻譯引擎不支援語氣"_q },
		{ u"LuminaChatRegisterDeepL"_q, u"DeepL 只有正式／非正式，且限部分語言"_q },

		// Telegram's own AI editor, which carries a Translate tab of its own.
		{ u"LuminaAiEditorHeader"_q, u"Telegram AI 編輯器"_q },
		{ u"LuminaAiEditorKeep"_q, u"保留 Telegram 的 AI 編輯器"_q },
		{ u"LuminaAiEditorInfo"_q, u"Telegram 在訊息輸入框裡有自己的 AI "
			u"編輯器，其中的「翻譯」分頁與 LuminaGram 的翻譯重疊。開啟 "
			u"LuminaGram 翻譯時，它的按鈕與鍵盤快捷鍵都不會出現，所以你眼前"
			u"永遠只有一個翻譯工具。開啟這個選項，就能讓 Telegram 的編輯器"
			u"仍然保持可用。關閉 LuminaGram 翻譯時，Telegram 的編輯器一直"
			u"都在，這個選項不會有任何作用。"_q },

		// Names of the languages LuminaGram can translate into, in the order
		// the pickers show them.
		{ u"LuminaLangEn"_q, u"英文"_q },
		{ u"LuminaLangZhTw"_q, u"繁體中文"_q },
		{ u"LuminaLangZhCn"_q, u"簡體中文"_q },
		{ u"LuminaLangJa"_q, u"日文"_q },
		{ u"LuminaLangKo"_q, u"韓文"_q },
		{ u"LuminaLangEs"_q, u"西班牙文"_q },
		{ u"LuminaLangFr"_q, u"法文"_q },
		{ u"LuminaLangDe"_q, u"德文"_q },
		{ u"LuminaLangRu"_q, u"俄文"_q },
		{ u"LuminaLangPtBr"_q, u"葡萄牙文（巴西）"_q },
		{ u"LuminaLangPtPt"_q, u"葡萄牙文（葡萄牙）"_q },
		{ u"LuminaLangIt"_q, u"義大利文"_q },
		{ u"LuminaLangAr"_q, u"阿拉伯文"_q },
		{ u"LuminaLangHi"_q, u"印地文"_q },
		{ u"LuminaLangId"_q, u"印尼文"_q },
		{ u"LuminaLangTh"_q, u"泰文"_q },
		{ u"LuminaLangVi"_q, u"越南文"_q },
		{ u"LuminaLangTr"_q, u"土耳其文"_q },
		{ u"LuminaLangPl"_q, u"波蘭文"_q },
		{ u"LuminaLangUk"_q, u"烏克蘭文"_q },
		{ u"LuminaLangNl"_q, u"荷蘭文"_q },
		{ u"LuminaLangAf"_q, u"南非荷蘭文"_q },
		{ u"LuminaLangSq"_q, u"阿爾巴尼亞文"_q },
		{ u"LuminaLangAm"_q, u"阿姆哈拉文"_q },
		{ u"LuminaLangHy"_q, u"亞美尼亞文"_q },
		{ u"LuminaLangAz"_q, u"亞塞拜然文"_q },
		{ u"LuminaLangEu"_q, u"巴斯克文"_q },
		{ u"LuminaLangBe"_q, u"白俄羅斯文"_q },
		{ u"LuminaLangBn"_q, u"孟加拉文"_q },
		{ u"LuminaLangBs"_q, u"波士尼亞文"_q },
		{ u"LuminaLangBg"_q, u"保加利亞文"_q },
		{ u"LuminaLangMy"_q, u"緬甸文"_q },
		{ u"LuminaLangCa"_q, u"加泰隆尼亞文"_q },
		{ u"LuminaLangHr"_q, u"克羅埃西亞文"_q },
		{ u"LuminaLangCs"_q, u"捷克文"_q },
		{ u"LuminaLangDa"_q, u"丹麥文"_q },
		{ u"LuminaLangEt"_q, u"愛沙尼亞文"_q },
		{ u"LuminaLangTl"_q, u"菲律賓文"_q },
		{ u"LuminaLangFi"_q, u"芬蘭文"_q },
		{ u"LuminaLangGl"_q, u"加利西亞文"_q },
		{ u"LuminaLangKa"_q, u"喬治亞文"_q },
		{ u"LuminaLangEl"_q, u"希臘文"_q },
		{ u"LuminaLangGu"_q, u"古吉拉特文"_q },
		{ u"LuminaLangHe"_q, u"希伯來文"_q },
		{ u"LuminaLangHu"_q, u"匈牙利文"_q },
		{ u"LuminaLangIs"_q, u"冰島文"_q },
		{ u"LuminaLangGa"_q, u"愛爾蘭文"_q },
		{ u"LuminaLangJv"_q, u"爪哇文"_q },
		{ u"LuminaLangKn"_q, u"坎那達文"_q },
		{ u"LuminaLangKk"_q, u"哈薩克文"_q },
		{ u"LuminaLangKm"_q, u"高棉文"_q },
		{ u"LuminaLangKu"_q, u"庫德文"_q },
		{ u"LuminaLangKy"_q, u"吉爾吉斯文"_q },
		{ u"LuminaLangLo"_q, u"寮文"_q },
		{ u"LuminaLangLv"_q, u"拉脫維亞文"_q },
		{ u"LuminaLangLt"_q, u"立陶宛文"_q },
		{ u"LuminaLangMk"_q, u"馬其頓文"_q },
		{ u"LuminaLangMs"_q, u"馬來文"_q },
		{ u"LuminaLangMl"_q, u"馬拉雅拉姆文"_q },
		{ u"LuminaLangMr"_q, u"馬拉地文"_q },
		{ u"LuminaLangMn"_q, u"蒙古文"_q },
		{ u"LuminaLangNe"_q, u"尼泊爾文"_q },
		{ u"LuminaLangNo"_q, u"挪威文"_q },
		{ u"LuminaLangPs"_q, u"普什圖文"_q },
		{ u"LuminaLangFa"_q, u"波斯文"_q },
		{ u"LuminaLangPa"_q, u"旁遮普文"_q },
		{ u"LuminaLangRo"_q, u"羅馬尼亞文"_q },
		{ u"LuminaLangSr"_q, u"塞爾維亞文"_q },
		{ u"LuminaLangSi"_q, u"僧伽羅文"_q },
		{ u"LuminaLangSk"_q, u"斯洛伐克文"_q },
		{ u"LuminaLangSl"_q, u"斯洛維尼亞文"_q },
		{ u"LuminaLangSo"_q, u"索馬利文"_q },
		{ u"LuminaLangSw"_q, u"史瓦希里文"_q },
		{ u"LuminaLangSv"_q, u"瑞典文"_q },
		{ u"LuminaLangTg"_q, u"塔吉克文"_q },
		{ u"LuminaLangTa"_q, u"坦米爾文"_q },
		{ u"LuminaLangTe"_q, u"泰盧固文"_q },
		{ u"LuminaLangUr"_q, u"烏爾都文"_q },
		{ u"LuminaLangUz"_q, u"烏茲別克文"_q },
		{ u"LuminaLangCy"_q, u"威爾斯文"_q },
		{ u"LuminaLangYi"_q, u"意第緒文"_q },
		{ u"LuminaLangZu"_q, u"祖魯文"_q },

		{ u"LuminaAppearanceStickerSizeHeader"_q, u"貼圖大小"_q },
		{ u"LuminaBackupCryptoFailed"_q, u"這個系統無法使用加密功能。"_q },
		{ u"LuminaBackupDamaged"_q, u"備份檔案已損毀，無法還原。"_q },
		{ u"LuminaBackupExport"_q, u"匯出加密備份"_q },
		{ u"LuminaBackupExportDone"_q, u"備份已儲存。"_q },
		{ u"LuminaBackupExportFailed"_q, u"無法建立備份檔案。"_q },
		{ u"LuminaBackupExportInfo"_q, u"把 LuminaGram 存在這台裝置上的所有"
			u"資料——書籤、備註、回覆範本、文字取代與每一項設定——存成一個"
			u"檔案，並用你自己選的密碼加密。這個檔案裡也包含私密的內容："
			u"翻譯的 API 金鑰、保險庫與假當機的密碼，以及誘餌記事本的內容。"
			u"任何資料都不會傳送給 Telegram。請選一組夠長的密碼並妥善保管："
			u"沒有它就無法開啟這個檔案，也沒有任何方式可以救回。"_q },
		{ u"LuminaBackupExportPassphraseTitle"_q, u"設定密碼"_q },
		{ u"LuminaBackupFileFilter"_q, u"LuminaGram 備份 (*.lgbak)"_q },
		{ u"LuminaBackupImport"_q, u"匯入備份"_q },
		{ u"LuminaBackupImportFailed"_q, u"無法讀取備份檔案。"_q },
		{ u"LuminaBackupImportInfo"_q, u"選擇一個備份檔案並輸入它的密碼，"
			u"就能還原你的 LuminaGram 資料。備份裡有的設定會取代這台裝置上的"
			u"設定；備份裡沒有的則維持原狀。無法通過驗證的檔案會在寫入任何"
			u"內容之前就被拒絕，所以密碼輸入錯誤絕不會讓你停在還原到一半的"
			u"狀態。"_q },
		{ u"LuminaBackupImportSuccess"_q, u"備份已還原。請重新啟動 "
			u"LuminaGram 以套用所有變更。"_q },
		{ u"LuminaBackupInvalidFile"_q,
			u"這不是有效的 LuminaGram 備份檔案。"_q },
		{ u"LuminaBackupNewerFormat"_q,
			u"這個備份是由較新版本的 LuminaGram 建立的。"_q },
		{ u"LuminaBackupOpenCaption"_q, u"開啟 LuminaGram 備份"_q },
		{ u"LuminaBackupPassphraseHint"_q, u"密碼"_q },
		{ u"LuminaBackupPassphraseMismatch"_q, u"兩次輸入的密碼不一致。"_q },
		{ u"LuminaBackupPassphraseRepeatHint"_q, u"再次輸入密碼"_q },
		{ u"LuminaBackupPassphraseTitle"_q, u"輸入密碼"_q },
		{ u"LuminaBackupPassphraseTooShort"_q,
			u"請設定至少 4 個字元的密碼。"_q },
		{ u"LuminaBackupSaveCaption"_q, u"儲存 LuminaGram 備份"_q },
		{ u"LuminaBackupTitle"_q, u"加密備份"_q },
		{ u"LuminaBackupUnauthenticated"_q, u"這個備份使用舊的未保護格式，"
			u"無法檢查密碼是否正確，也無法檢查檔案是否遭到竄改。請用最新版的 "
			u"LuminaGram 重新製作一份備份。"_q },
		{ u"LuminaBackupWrongPassphrase"_q, u"密碼錯誤，或檔案已被變更。"_q },
		{ u"LuminaBookmark"_q, u"加入書籤"_q },
		{ u"LuminaBookmarkAdded"_q, u"已加入書籤"_q },
		{ u"LuminaBookmarkChatUnavailable"_q, u"聊天無法使用"_q },
		{ u"LuminaBookmarkDeleteTitle"_q, u"刪除書籤？"_q },
		{ u"LuminaBookmarkGone"_q,
			u"這個聊天在這台裝置上已經無法使用。"_q },
		{ u"LuminaBookmarkRemove"_q, u"移除書籤"_q },
		{ u"LuminaBookmarkRemoved"_q, u"已從書籤移除"_q },
		{ u"LuminaBookmarksAbout"_q, u"書籤只是指向訊息的標記，只保存在這台"
			u"裝置上，絕不會傳送給 Telegram。移除書籤不會動到訊息本身。"_q },
		{ u"LuminaBookmarksEmpty"_q, u"你還沒有收藏任何訊息。"_q },
		{ u"LuminaBookmarksFull"_q, u"書籤清單已滿。請先移除一個書籤，才能"
			u"再加入新的。"_q },
		{ u"LuminaBookmarksList"_q, u"已加入書籤的訊息"_q },
		{ u"LuminaBookmarksListAbout"_q, u"點擊書籤可以開啟該則訊息，按右鍵"
			u"則可以移除它。即使訊息已經被刪除，書籤仍會留在這份清單裡。"_q },
		{ u"LuminaBookmarksNone"_q, u"無"_q },
		{ u"LuminaBookmarksTitle"_q, u"書籤"_q },
		{ u"LuminaChatListDensityTitle"_q, u"聊天清單密度"_q },
		{ u"LuminaCompactListRows"_q, u"精簡列高"_q },
		{ u"LuminaCompactListRowsInfo"_q, u"縮短聊天清單每一列的高度，讓螢幕"
			u"上能顯示更多聊天。大頭貼與訊息預覽仍會顯示。"_q },
		{ u"LuminaChatListDotsTitle"_q, u"大頭貼圓點"_q },
		{ u"LuminaChatListOnlineDot"_q, u"線上圓點"_q },
		{ u"LuminaChatListOnlineDotInfo"_q, u"在一對一聊天中，若對方目前在"
			u"線上，就在他的大頭貼上顯示一個小綠點。"_q },
		{ u"LuminaChatListRecencyDot"_q, u"最近上線圓點"_q },
		{ u"LuminaChatListRecencyDotInfo"_q, u"依照對方最後上線的時間，為"
			u"一對一聊天的大頭貼圓點著色：目前在線上為綠色，一小時內為黃色，"
			u"一天內為橘色。更久以前或隱藏最後上線時間的則不顯示圓點。綠色"
			u"（在線上）的情況仍由「線上圓點」開關控制。"_q },
		{ u"LuminaChatListVisibilityAbout"_q, u"把資料夾列從聊天列表中移除"
			u"——包含旁邊的直向列與上方的橫向列。隱藏資料夾分頁時，你看到的"
			u"一律是全部聊天：原本所在的資料夾會被離開，資料夾快捷鍵與在"
			u"資料夾之間滑動也都不會有作用。隱藏限時動態只會移除聊天列表上方"
			u"的那一列；限時動態本身，以及其他所有開啟它們的方式都不受"
			u"影響。"_q },
		{ u"LuminaChatListVisibilityTitle"_q, u"資料夾與限時動態"_q },
		{ u"LuminaContactNote"_q, u"私密備註"_q },
		{ u"LuminaContactNoteAbout"_q, u"只有你看得到。它只會留在這台裝置"
			u"上，絕不會傳送給 Telegram，也不會同步到你的其他裝置。"_q },
		{ u"LuminaContactNoteEmpty"_q, u"點擊以新增私密備註"_q },
		{ u"LuminaContactNoteHint"_q, u"備註（僅你可見）"_q },
		{ u"LuminaContactNotesAbout"_q, u"在對方的個人檔案上加入一則私密備註"
			u"與一組標籤。兩者都只會留在這台裝置上，絕不會傳送給 Telegram，"
			u"也不會同步到你的其他裝置。Telegram 自己的聯絡人備註不受影響："
			u"私密備註只會出現在 Telegram 備註無法使用的地方——機器人，以及"
			u"不在你聯絡人中的人——標籤則到處都有。"_q },
		{ u"LuminaContactNotesClear"_q, u"刪除所有私密備註"_q },
		{ u"LuminaContactNotesClearText"_q, u"要刪除這台裝置上儲存的每一則"
			u"私密備註與標籤嗎？這項操作無法復原。"_q },
		{ u"LuminaContactNotesClearTitle"_q, u"刪除私密備註"_q },
		{ u"LuminaContactNotesNone"_q, u"無"_q },
		{ u"LuminaContactNotesTitle"_q, u"私密聯絡人備註"_q },
		{ u"LuminaContactNotesToggle"_q, u"在個人檔案上顯示私密備註與"
			u"標籤"_q },
		{ u"LuminaContactTags"_q, u"標籤"_q },
		{ u"LuminaContactTagsEmpty"_q, u"點擊以新增標籤"_q },
		{ u"LuminaContactTagsHint"_q, u"標籤，以逗號分隔"_q },
		{ u"LuminaDetailsDate"_q, u"日期"_q },
		{ u"LuminaDetailsForwardedFrom"_q, u"轉發自"_q },
		{ u"LuminaDetailsFrom"_q, u"來自"_q },
		{ u"LuminaDetailsMessageId"_q, u"訊息 ID"_q },
		{ u"LuminaDetailsOriginalDate"_q, u"原始日期"_q },
		{ u"LuminaExactNumbers"_q, u"顯示精確數字（不使用 1.2K 縮寫）"_q },
		{ u"LuminaExactNumbersInfo"_q, u"顯示完整計數，如 1,234,567，而非 "
			u"1.2M 之類的縮寫形式。已經畫在畫面上的計數會維持目前的形式，"
			u"直到重新啟動應用程式為止。"_q },
		{ u"LuminaForwardNoAuthor"_q, u"轉發時隱藏作者"_q },
		{ u"LuminaForwardNoAuthorTitle"_q, u"轉發時隱藏作者"_q },
		{ u"LuminaForwardNoCaption"_q, u"轉發時隱藏作者與說明文字"_q },
		{ u"LuminaForwardNoCaptionTitle"_q, u"轉發時隱藏作者與說明文字"_q },
		{ u"LuminaHideStories"_q, u"隱藏限時動態"_q },
		{ u"LuminaHideTabs"_q, u"隱藏資料夾分頁"_q },
		{ u"LuminaLinkSafetyDestination"_q, u"真實目標位址"_q },
		{ u"LuminaLinkSafetyInfo"_q, u"在開啟以下連結之前先詢問：把真實目標"
			u"位址藏在「@」符號前的文字後面、網域以 punycode 拼寫，或是透過"
			u"已知的短連結服務轉址。確認視窗會顯示真實的目標主機與完整網址。"
			u"以其他字母系統寫成的仿冒網域，Telegram Desktop 本身就會詢問。"
			u"所有檢查都不會連線——你開啟的任何網址都不會離開這台裝置。"_q },
		{ u"LuminaLinkSafetyRow"_q, u"連結安全檢查"_q },
		{ u"LuminaLinkSafetyTitle"_q, u"開啟外部連結？"_q },
		{ u"LuminaLinkSafetyWarnMismatch"_q, u"此連結透過「@」符號前的文字"
			u"隱藏了真實目標位址。"_q },
		{ u"LuminaLinkSafetyWarnPunycode"_q, u"此網址使用了編碼（punycode）"
			u"字元，可能在仿冒知名網站。"_q },
		{ u"LuminaLinkSafetyWarnShortener"_q, u"這是一個短連結——真實目標"
			u"位址在你開啟前始終隱藏。"_q },
		{ u"LuminaMediaAutoPauseBgVideo"_q, u"應用程式退到背景時暫停影片"_q },
		{ u"LuminaMediaAutoPauseBgVideoInfo"_q, u"當你離開 LuminaGram 時，"
			u"自動暫停正在播放的影片。把檢視器縮到最小也會暫停。你刻意放到"
			u"獨立視窗或子母畫面的影片會繼續播放，音樂與語音訊息在你離開時"
			u"也會繼續播放。"_q },
		{ u"LuminaMediaTitle"_q, u"媒體"_q },
		{ u"LuminaMessageActionsInfo"_q, u"這些項目只會出現在訊息的右鍵選單"
			u"裡。每一項都使用 Telegram 自己的轉發功能，所以限制轉發的聊天"
			u"依然會受到限制。「轉發時隱藏作者與說明文字」是照實際送出的結果"
			u"命名的：Telegram 無法在保留寄件者的同時移除說明文字，所以選擇"
			u"它會兩者一起移除。「詳細資訊」只會讀取這台裝置已經同步下來的"
			u"內容——不會發出任何請求，也不會儲存任何東西。"_q },
		{ u"LuminaMessageDetails"_q, u"詳細資訊"_q },
		{ u"LuminaMessageDisplayHeader"_q, u"訊息顯示"_q },
		{ u"LuminaOnboardingDualName"_q, u"兩種語言同時顯示"_q },
		{ u"LuminaOnboardingDualText"_q, u"讓原文和譯文一起留在畫面上，"
			u"你收到的訊息和你傳送的訊息都適用。"_q },
		{ u"LuminaOnboardingFooter"_q, u"到 LuminaGram 設定頁面開啟任何一個"
			u"區塊，就能設定這些功能。你可以從「工具」再次閱讀這張卡片。"_q },
		{ u"LuminaOnboardingGotIt"_q, u"知道了"_q },
		{ u"LuminaOnboardingHeader"_q, u"關於"_q },
		{ u"LuminaOnboardingIntro"_q, u"LuminaGram 在 Telegram Desktop 之上"
			u"加入了自己的工具。下面每一項選項都只會儲存在這台電腦上，"
			u"不會同步到 Telegram。"_q },
		{ u"LuminaOnboardingRow"_q, u"LuminaGram 加了什麼"_q },
		{ u"LuminaOnboardingRowAbout"_q, u"顯示你第一次開啟 LuminaGram 設定"
			u"時看到的歡迎卡片。它只是說明這裡有什麼，不會開啟任何功能。"_q },
		{ u"LuminaOnboardingSafetyName"_q, u"安全檢查"_q },
		{ u"LuminaOnboardingSafetyText"_q, u"在開啟網址與外觀不符的連結前"
			u"提出警告，在你貼上加密貨幣地址時攔下被掉包的地址，並移除你"
			u"傳送的相片中的位置資訊。"_q },
		{ u"LuminaOnboardingTitle"_q, u"歡迎使用 LuminaGram"_q },
		{ u"LuminaOnboardingTranslateText"_q, u"用你自己的翻譯服務翻譯收到的"
			u"訊息，也可以在傳送前先翻譯你輸入的內容。"_q },
		{ u"LuminaOnboardingVaultName"_q, u"偽裝保險庫"_q },
		{ u"LuminaOnboardingVaultText"_q, u"把 LuminaGram 藏在計算機或記事本"
			u"後面，只有輸入你的秘密密碼才會開啟真正的應用程式。"_q },
		{ u"LuminaProfileCardAbout"_q, u"一張關於你自己的本機名片——你會說"
			u"哪些語言、對什麼有興趣——可以複製後貼到聊天裡。它只會留在這台"
			u"裝置上。"_q },
		{ u"LuminaProfileCardBio"_q, u"簡介"_q },
		{ u"LuminaProfileCardBioHint"_q, u"簡單介紹一下你自己"_q },
		{ u"LuminaProfileCardCopied"_q, u"名片已複製"_q },
		{ u"LuminaProfileCardCopy"_q, u"複製到剪貼簿"_q },
		{ u"LuminaProfileCardCopyInfo"_q, u"把你的名片整理成純文字摘要。"
			u"想分享時，貼到任何聊天裡即可。"_q },
		{ u"LuminaProfileCardEdit"_q, u"我的名片"_q },
		{ u"LuminaProfileCardEmptyShare"_q, u"請先填寫你的名片"_q },
		{ u"LuminaProfileCardHeader"_q, u"我的名片"_q },
		{ u"LuminaProfileCardInfo"_q, u"這張名片只會儲存在這台裝置上，絕不會"
			u"傳送給 Telegram，也不會更改你的 Telegram 個人資料。"_q },
		{ u"LuminaProfileCardInterests"_q, u"興趣 / 標籤"_q },
		{ u"LuminaProfileCardInterestsHint"_q, u"例如：音樂、健行、"
			u"程式設計"_q },
		{ u"LuminaProfileCardLanguages"_q, u"我會說的語言"_q },
		{ u"LuminaProfileCardLanguagesHint"_q, u"例如：中文、English、"
			u"Español"_q },
		{ u"LuminaProfileCardNotSet"_q, u"未設定"_q },
		{ u"LuminaProfileCardTagline"_q, u"標語"_q },
		{ u"LuminaProfileCardTaglineHint"_q, u"一句話介紹你自己"_q },
		{ u"LuminaProfileCardTitle"_q, u"個人名片"_q },
		{ u"LuminaProfileChatCreated"_q, u"建立於"_q },
		{ u"LuminaProfileDcId"_q, u"資料中心"_q },
		{ u"LuminaProfileUserId"_q, u"用戶 ID"_q },
		{ u"LuminaProfileDcIdValue"_q, u"DC{1}"_q },
		{ u"LuminaProfileNameWarning"_q, u"可能冒充他人"_q },
		{ u"LuminaProfileNameWarningLabel"_q, u"名稱含視覺近似字元"_q },
		{ u"LuminaProfileInfoHeader"_q, u"個人檔案"_q },
		{ u"LuminaProfileInfoInfo"_q, u"個人檔案頁面上額外的幾行資訊，全部都"
			u"在這台裝置上算出來——不會向 Telegram 詢問任何東西。部分帳號的"
			u"註冊日期由 Telegram 自己提供；其餘的則是依帳號編號推估，並以"
			u"「~」標示。資料中心指的是存放大頭貼的那一個。建立日期是群組或"
			u"頻道被建立的時間。"_q },
		{ u"LuminaProfileRegistrationApprox"_q, u"~ {1}"_q },
		{ u"LuminaProfileRegistrationDate"_q, u"註冊日期"_q },
		{ u"LuminaProfileShowChatDate"_q, u"顯示建立日期"_q },
		{ u"LuminaProfileShowDcId"_q, u"顯示資料中心"_q },
		{ u"LuminaProfileShowRegistrationDate"_q, u"顯示註冊日期"_q },
		{ u"LuminaRecentLimitsInfo"_q, u"讓這台裝置不要比必要時更早丟掉最近"
			u"使用的貼圖與已儲存的 GIF，採用與 Android 版 LuminaGram 相同的"
			u"上限：最多 200 個最近使用的貼圖與 500 個已儲存的 GIF。實際存在"
			u"你帳號裡、並同步到其他裝置的數量仍然由 Telegram 的伺服器決定，"
			u"所以這並不會讓雲端清單變大——這台裝置下次同步時，伺服器上已經"
			u"沒有的項目在這裡也會跟著消失，通常只要幾秒。開啟這個選項時，"
			u"Telegram Desktop 不會再自行移除已儲存的 GIF，因此它關於已儲存 "
			u"GIF 上限的 Premium 提示也會保持安靜。除非你同時在「設定 › 進階 "
			u"› 實驗性設定」中開啟「Unlimited recent stickers」，貼圖面板仍然"
			u"只會顯示前 20 個最近使用的貼圖。"_q },
		{ u"LuminaRecentLimitsRow"_q, u"保留更多最近使用的貼圖與 GIF"_q },
		{ u"LuminaReplyTemplatesAbout"_q, u"保存在這台裝置上的短句，可以直接"
			u"放進訊息輸入框。在聊天中對表情符號按鈕按右鍵即可挑選。範本"
			u"絕不會傳送給 Telegram。"_q },
		{ u"LuminaReplyTemplatesAdd"_q, u"新增範本"_q },
		{ u"LuminaReplyTemplatesEdit"_q, u"編輯範本"_q },
		{ u"LuminaReplyTemplatesEmpty"_q, u"還沒有任何範本。先新增一則，"
			u"然後在聊天中對表情符號按鈕按右鍵就能插入。"_q },
		{ u"LuminaReplyTemplatesFull"_q, u"清單已滿——請先刪除一則範本，"
			u"才能再新增。"_q },
		{ u"LuminaReplyTemplatesInfo"_q, u"在聊天中對表情符號按鈕按右鍵即可"
			u"插入範本。在這裡對範本按右鍵可以上移或下移；開啟範本則可以編輯"
			u"或刪除。"_q },
		{ u"LuminaReplyTemplatesList"_q, u"範本"_q },
		{ u"LuminaReplyTemplatesManage"_q, u"管理範本…"_q },
		{ u"LuminaReplyTemplatesMoveDown"_q, u"下移"_q },
		{ u"LuminaReplyTemplatesMoveUp"_q, u"上移"_q },
		{ u"LuminaReplyTemplatesNone"_q, u"無"_q },
		{ u"LuminaReplyTemplatesPlaceholder"_q, u"範本內容"_q },
		{ u"LuminaReplyTemplatesShow"_q, u"在聊天中提供範本"_q },
		{ u"LuminaReplyTemplatesTitle"_q, u"回覆範本"_q },
		{ u"LuminaSaveSticker"_q, u"儲存貼圖"_q },
		{ u"LuminaSaveStickers"_q, u"儲存貼圖"_q },
		{ u"LuminaSaveStickersInfo"_q, u"在貼圖面板的右鍵選單中加入「儲存"
			u"貼圖」項目。貼圖會存到你其他下載檔案所在的位置；當群組限制"
			u"儲存時，該群組自己的貼圖包不會顯示這個項目。"_q },
		{ u"LuminaSaveToCloud"_q, u"儲存至「儲存的訊息」"_q },
		{ u"LuminaSaveToCloudTitle"_q, u"儲存至「儲存的訊息」"_q },
		{ u"LuminaSecurityPanicConfirmAck"_q, u"我了解這項操作無法復原"_q },
		{ u"LuminaSecurityPanicConfirmButton"_q, u"立即清除"_q },
		{ u"LuminaSecurityPanicConfirmText"_q, u"這台裝置上的每個帳號都會被"
			u"登出。本機的訊息資料庫、草稿與快取媒體都會被清除，連同 "
			u"LuminaGram 自己的設定、書籤、已儲存的譯文與 API 金鑰。\n\n"
			u"你的帳號不會被刪除。帳號會留在 Telegram 的伺服器上，你的訊息"
			u"也是——你可以隨時從任何地方重新登入。\n\n已經下載的檔案"
			u"不會被刪除。LuminaGram 不會動你的下載資料夾，因為那通常就是"
			u"你一般的「下載」資料夾，裡面還放著無關的檔案。那裡若有敏感"
			u"內容，請自行移動或刪除。\n\n這項操作無法復原。"_q },
		{ u"LuminaSecurityPanicConfirmTitle"_q, u"緊急清除？"_q },
		{ u"LuminaSecurityPanicHeader"_q, u"緊急清除"_q },
		{ u"LuminaSecurityPanicWipe"_q, u"緊急清除 (Kaboom)"_q },
		{ u"LuminaSecurityPanicWipeAbout"_q, u"登出這台裝置上的每個帳號，"
			u"並清除本機的訊息資料庫、草稿與快取媒體，連同 LuminaGram 自己的"
			u"設定、書籤、已儲存的譯文與 API 金鑰。你的帳號與訊息會留在 "
			u"Telegram 的伺服器上。已經下載的檔案會留在原處。這項操作無法"
			u"復原。"_q },
		{ u"LuminaSelectFromAuthor"_q, u"選擇該作者的所有訊息"_q },
		{ u"LuminaSelectFromAuthorAbout"_q, u"在群組的訊息選單中加入一個"
			u"項目，可以選取你點擊的那位寄件者的所有訊息。它只會處理這個視窗"
			u"已經載入的訊息——想包含更舊的訊息，請往前捲動之後再執行一次。"
			u"歸屬於聊天本身的訊息，例如頻道貼文與匿名管理員的貼文，不會有"
			u"這個項目。"_q },
		{ u"LuminaSelectFromAuthorLimit"_q,
			u"一次最多只能選取 {1} 則訊息。"_q },
		{ u"LuminaSelectFromAuthorTitle"_q, u"選擇該作者的所有訊息"_q },
		{ u"LuminaSelectionHeader"_q, u"選取"_q },
		{ u"LuminaSendOriginalCaption"_q, u"傳送原始說明文字"_q },
		{ u"LuminaShowBookmarks"_q, u"顯示書籤選單項目"_q },
		{ u"LuminaShowMessageDetails"_q, u"訊息詳細資訊"_q },
		{ u"LuminaShowMutedCount"_q, u"永遠顯示未讀計數"_q },
		{ u"LuminaShowMutedCountInfo"_q, u"將已靜音聊天的未讀角標以正常"
			u"強調色顯示，而非靜音灰色。"_q },
		{ u"LuminaStickerSavedTo"_q, u"貼圖已儲存到 {1}"_q },
		{ u"LuminaStickerSizeChoice"_q, u"{1}%"_q },
		{ u"LuminaStickerSizeChoiceDefault"_q, u"{1}%（預設）"_q },
		{ u"LuminaStickerSizeInfo"_q, u"貼圖在聊天中顯示的大小——你傳送的和"
			u"你收到的都適用。動態表情符號、骰子與禮物貼圖維持它們自己的"
			u"大小。Telegram Desktop 只會測量貼圖一次並記住結果，所以新的"
			u"大小要到下次啟動應用程式時才會生效。"_q },
		{ u"LuminaStickersHeader"_q, u"貼圖與 GIF"_q },
		{ u"LuminaTimeWithSeconds"_q, u"在訊息時間中顯示秒"_q },
		{ u"LuminaTimeWithSecondsAbout"_q, u"訊息下方的時間、複製文字中的"
			u"時間，以及螢幕閱讀器唸出的時間都會包含秒數。"_q },
		// The two items of the system tray menu; the second is also the only
		// item on the taskbar button's jump list on Windows. {1} is always
		// the literal application name, never translated and never inflected,
		// so each reads as a verb followed by a bare product name.
		{ u"LuminaTrayOpen"_q, u"開啟 {1}"_q },
		{ u"LuminaTrayQuit"_q, u"結束 {1}"_q },
		{ u"LuminaUndoSendBulletin"_q, u"正在傳送訊息…"_q },
		{ u"LuminaUndoSendTitle"_q, u"復原傳送"_q },
		{ u"LuminaUndoSendUndo"_q, u"復原"_q },
		{ u"LuminaUndoSendWindow"_q, u"復原傳送時間"_q },
		{ u"LuminaUndoSendWindowInfo"_q, u"純文字訊息在真正送出之前，會先在"
			u"「復原」按鈕後面保留 {1} 秒。這段期間你的文字會一直留在訊息"
			u"輸入框裡，只有在訊息真的送出後輸入框才會清空，所以按下復原只是"
			u"把文字留在原地——不會先拿走再放回來。再次傳送、開啟另一個聊天"
			u"或結束程式，都會立刻把保留中的訊息送出。媒體、語音、已編輯、"
			u"轉發與定時訊息永遠不會被保留，從論壇主題或留言串中傳送的訊息"
			u"也不會。"_q },
		// clipboard guard + scam watch
		{ u"LuminaPrivacyCryptoClipboardGuard"_q, u"加密貨幣地址貼上保護"_q },
		{ u"LuminaPrivacyCryptoClipboardGuardInfo"_q, u"把看起來像加密貨幣錢包"
			u"地址的內容貼進訊息前先問你一聲。剪貼簿劫持惡意軟體可能在你毫無察"
			u"覺的情況下，把你複製的地址換成騙徒的地址，而貼上正是你能發現它的"
			u"最後一刻。檢查在本機進行，不會把任何內容傳送出去。"_q },
		{ u"LuminaPrivacyScamKeywordWarning"_q, u"詐騙關鍵字警告"_q },
		{ u"LuminaPrivacyScamKeywordWarningInfo"_q, u"當不在你聯絡人中的人傳來"
			u"的訊息提到常見詐騙手法（匯款、禮品卡、加密貨幣「投資」、驗證費或"
			u"索取驗證碼）時，顯示一次性提醒，提醒你保持警覺。訊息永遠不會被攔"
			u"截或修改。檢查在你的裝置上離線執行。"_q },
		// otp guard
		{ u"LuminaOtpGuardTitle"_q, u"警告"_q },
		{ u"LuminaOtpGuardMessage"_q, u"你正要把 Telegram 登入碼傳給別人。對方拿到這組碼，就能登入你的帳號，看到你所有的訊息。Telegram 官方永遠不會跟你要這組碼。"_q },
		{ u"LuminaOtpGuardCancel"_q, u"取消傳送"_q },
		{ u"LuminaOtpGuardSendAnyway"_q, u"我知道風險，仍要傳送"_q },
		{ u"LuminaOtpGuard"_q, u"登入碼外洩防護"_q },
		{ u"LuminaOtpGuardInfo"_q, u"當 Telegram 剛寄給你登入碼、而你正要送出含有驗證碼的訊息時，先跳出警告。整個判斷都在你的裝置上完成，不會上傳任何內容，也不會更動你的訊息。"_q },
		{ u"LuminaPrivacyHomoglyphWarning"_q, u"冒名混淆警告"_q },
		{ u"LuminaPrivacyHomoglyphWarningInfo"_q, u"當一個名字混用了來自不同字母表的形近字元時發出警告——例如拉丁詞裡夾著西里爾字母「а」、全形字母，或用 0 冒充 O——這正是冒名者偽裝成熟悉名字的手法。不會攔截或更動任何內容；判斷都在你的裝置上完成，不會上傳任何名字。"_q },
		// link preview + voice confirm
		{ u"LuminaPrivacyDisableLinkPreview"_q, u"預設停用連結預覽"_q },
		{ u"LuminaLinkPreviewInfo"_q, u"你新寫的訊息會不帶連結預覽傳送，輸入框"
			u"也不會在你打字時要求 Telegram 去解析貼上的連結。已經附"
			u"上的預覽，以及你編輯的訊息，都會保持原樣。若想單獨為某則訊息附上"
			u"預覽，可以在設定 > 鍵盤快速鍵中為「切換連結預覽」指定一個按鍵，"
			u"撰寫時按下即可。"_q },
		{ u"LuminaLinkPreviewHeader"_q, u"連結預覽"_q },
		{ u"LuminaVoiceConfirmTitle"_q, u"語音與影片訊息"_q },
		{ u"LuminaVoiceConfirmRow"_q, u"傳送前確認"_q },
		{ u"LuminaVoiceConfirmInfo"_q, u"在錄好的語音或圓形影片訊息送出前先詢"
			u"問你。取消這個詢問會捨棄錄音，只有試聽預覽中例外——那裡會保留錄"
			u"音。定時傳送永遠不會詢問。"_q },
		// exif strip + text replace
		{ u"LuminaExifStripRow"_q, u"傳送前移除相片位置資訊"_q },
		{ u"LuminaExifStripInfo"_q, u"在上傳之前，抹除相機寫進 JPEG 相片裡的"
			u" GPS 座標——無論你是以一般的壓縮方式傳送相片，還是以檔案形式傳"
			u"送。只有位置標籤會被移除：方向、相機與日期標籤原封不動，所以以檔"
			u"案形式傳送的相片依然是正的。你磁碟上的那份檔案永遠不會被修改。這"
			u"不涵蓋影片，也不涵蓋由 XMP 或相機自家的製造商註記寫進相片的位"
			u"置，或是某些手機塞進同一個檔案裡的第二份影像。"_q },
		{ u"LuminaReplacerTitle"_q, u"文字取代"_q },
		{ u"LuminaReplacerHeader"_q, u"規則"_q },
		{ u"LuminaReplacerAdd"_q, u"新增規則"_q },
		{ u"LuminaReplacerEdit"_q, u"編輯規則"_q },
		{ u"LuminaReplacerFrom"_q, u"取代"_q },
		{ u"LuminaReplacerTo"_q, u"為"_q },
		{ u"LuminaReplacerEnable"_q, u"取代我傳送的訊息中的文字"_q },
		{ u"LuminaReplacerNone"_q, u"無"_q },
		{ u"LuminaReplacerNothing"_q, u"（空）"_q },
		{ u"LuminaReplacerEmpty"_q, u"還沒有任何規則。新增一則，就能開始取代你"
			u"傳送的訊息中的文字。"_q },
		{ u"LuminaReplacerFull"_q, u"規則會由上而下依序套用。清單已滿——請先刪"
			u"除一則規則，才能再新增。"_q },
		{ u"LuminaReplacerListInfo"_q, u"規則會由上而下依序套用，每一則處理的"
			u"都是上一則取代後的結果。"_q },
		{ u"LuminaReplacerAbout"_q, u"在你傳送的每一則訊息裡，把縮寫換成你真正"
			u"想說的話——「brb」會變成「be right back」。比對區分大小寫，而且只"
			u"以整個單字為單位。含有任何格式的訊息會原封不動地送出，連結、提"
			u"及、主題標籤與機器人指令也永遠不會被改寫。規則只留在這台裝置上，"
			u"絕不會傳送給 Telegram。"_q },
		// vault + fake crash
		{ u"LuminaVaultHeader"_q, u"偽裝保險箱"_q },
		{ u"LuminaVaultEnable"_q, u"保險箱開關"_q },
		{ u"LuminaVaultInfo"_q, u"這裡的一切都只保存在本機。保險箱會把 "
			u"LuminaGram 藏在一個不起眼的 App "
			u"後面，只有輸入秘密密碼才能回到真正的 App。"_q },
		{ u"LuminaVaultMode"_q, u"保險箱模式"_q },
		{ u"LuminaVaultModePasswordDoor"_q, u"密碼門"_q },
		{ u"LuminaVaultModeDecoyApp"_q, u"直接偽裝"_q },
		{ u"LuminaVaultModePasswordDoorInfo"_q, u"啟動 "
			u"LuminaGram 時會先要求輸入密碼。輸入秘"
			u"密密碼會進入真正的 App；輸入其他內容則會開啟"
			u"假 App，而且永遠不會提示密碼錯誤。"_q },
		{ u"LuminaVaultModeDecoyAppInfo"_q, u"啟動 LuminaGram "
			u"時會直接開啟假 App。在假 App 中輸入秘密"
			u"密碼即可進入真正的 App。"_q },
		{ u"LuminaVaultSkin"_q, u"假 App 樣式"_q },
		{ u"LuminaVaultSkinNotepad"_q, u"記事本"_q },
		{ u"LuminaVaultSkinCalculator"_q, u"計算機"_q },
		{ u"LuminaVaultSkinHintCalculator"_q, u"要從計算機解鎖，請輸入秘密密"
			u"碼，然後按等號鍵。"_q },
		{ u"LuminaVaultSkinHintNotepad"_q, u"要從記事本解鎖，請讓整篇筆記只"
			u"有秘密密碼，然後按兩下「記事本」標題列。"_q },
		{ u"LuminaVaultSecretCode"_q, u"秘密密碼"_q },
		{ u"LuminaVaultSecretCodeDialogTitle"_q, u"秘密密碼"_q },
		{ u"LuminaDisguiseDecoyCodeSet"_q, u"已設定"_q },
		{ u"LuminaDisguiseDecoyCodeNotSet"_q, u"未設定"_q },
		{ u"LuminaVaultNoCodeInfo"_q, u"在你設定秘密密碼之前，保險箱不會"
			u"生效。"_q },
		{ u"LuminaVaultCalculatorCodeInfo"_q, u"這個密碼無法用計算機鍵盤輸入，"
			u"因此會改用記事本假 App。如果你想用計算機，請"
			u"只使用數字、小數點和 + - * /。"_q },
		{ u"LuminaVaultDisclaimer"_q, u"這只擋得住別人不經意的一瞥，擋不"
			u"住真正懂行的檢查：程式在工作管理員裡仍然叫 "
			u"LuminaGram，任何能讀取本機檔案的人都能"
			u"讀到你的帳號。保險箱會在下次啟動 "
			u"LuminaGram 時出現，關閉假 App 就"
			u"等於結束 LuminaGram。萬一你忘了密碼，"
			u"請從 LuminaGram 資料夾中刪除 "
			u"tdata/luminagram.json 與 "
			u"tdata/luminagram_private.json："
			u"保險箱就會自動關閉，LuminaGram 也會正"
			u"常啟動。"_q },
		{ u"LuminaSecurityFakeCrashHeader"_q, u"假當機解鎖"_q },
		{ u"LuminaSecurityFakeCrashEnable"_q, u"啟用假當機解鎖"_q },
		{ u"LuminaSecurityFakeCrashCode"_q, u"假當機密碼"_q },
		{ u"LuminaSecurityFakeCrashCodeDialogTitle"_q, u"假當機密碼"_q },
		{ u"LuminaSecurityFakeCrashCodeHint"_q, u"密碼"_q },
		{ u"LuminaSecurityFakeCrashCodeSet"_q, u"已設定"_q },
		{ u"LuminaSecurityFakeCrashCodeNotSet"_q, u"未設定"_q },
		{ u"LuminaSecurityFakeCrashCodeSameAsPasscode"_q, u"此密碼不能與你的密碼"
			u"鎖相同。"_q },
		{ u"LuminaSecurityFakeCrashInfo"_q, u"開啟後，在鎖定畫面輸入假當機"
			u"密碼不會解鎖，而是立刻關閉 LuminaGram："
			u"不會有任何提示，畫面上也不會留下任何東西——在旁"
			u"人看來就是應用程式當掉了。這個密碼與你的密碼鎖互"
			u"相獨立，只保存在本機，絕不會傳送到任何地方。"_q },
		{ u"LuminaSecurityFakeCrashBiometricInfo"_q, u"在此功能開啟且已設定密"
			u"碼期間，使用 Touch ID、"
			u"Windows Hello 或系統密碼解鎖會被停"
			u"用：指紋解鎖根本不會詢問密碼鎖，也就會直接繞過這"
			u"個密碼。清除密碼或關閉此功能後，它們就會恢復。"_q },
		{ u"LuminaSecurityFakeCrashNoCodeInfo"_q, u"還沒有設定密碼，因此鎖定"
			u"畫面上不會有任何反應。"_q },
		{ u"LuminaSecurityFakeCrashNoPasscodeInfo"_q, u"LuminaGram "
			u"沒有設定密碼鎖，因此鎖定畫面永遠不會出現，也就永"
			u"遠不會用到這個密碼。請先在「隱私與安全性」中開啟"
			u"密碼鎖。"_q },
		{ u"LuminaSecurityFakeCrashCodeClashInfo"_q, u"這個密碼現在與你的密碼"
			u"鎖相同，因此永遠不會觸發——密碼鎖會先把應用程式"
			u"解鎖。請換一個密碼。"_q },
		// login guard (D-32)
		{ u"LuminaSessionGuardHeader"_q, u"登入守衛"_q },
		{ u"LuminaSessionGuard"_q, u"新登入警示"_q },
		{ u"LuminaSessionGuardInfo"_q, u"App 啟動時檢查你的已連結裝置，發現你沒確認過的登入就立刻提醒。掃描登入 QR code 等於把一個活躍的登入 session 交給對方，對方就能看到你全部的聊天紀錄。檢查最多每 30 分鐘一次，走 Telegram 官方 API，資料不會離開你的裝置。"_q },
		{ u"LuminaSessionGuardCheckNow"_q, u"立即檢查登入裝置"_q },
		{ u"LuminaSessionGuardNoNew"_q, u"沒有新的登入，所有已連結裝置都是你確認過的。"_q },
		{ u"LuminaSessionGuardCheckFailed"_q, u"無法檢查已連結裝置，請確認網路後再試一次。"_q },
		{ u"LuminaSessionGuardChecking"_q, u"檢查中…"_q },
		{ u"LuminaSessionGuardBusy"_q, u"已經有一次檢查正在進行，結果稍後就會出現。"_q },
		{ u"LuminaSessionAlertTitle"_q, u"偵測到新登入"_q },
		{ u"LuminaSessionAlertIntro"_q, u"有一台你沒確認過的裝置登入了你的帳號。如果不是你本人，對方現在就看得到你全部的聊天紀錄。"_q },
		{ u"LuminaSessionAlertDevice"_q, u"裝置"_q },
		{ u"LuminaSessionAlertApp"_q, u"App"_q },
		{ u"LuminaSessionAlertIp"_q, u"IP 位址"_q },
		{ u"LuminaSessionAlertLocation"_q, u"位置"_q },
		{ u"LuminaSessionAlertTime"_q, u"登入時間"_q },
		{ u"LuminaSessionAlertNotMe"_q, u"不是我 — 立即終止此登入"_q },
		{ u"LuminaSessionAlertItsMe"_q, u"是我"_q },
		{ u"LuminaSessionTerminated"_q, u"已終止該登入"_q },
		{ u"LuminaSessionTerminateFailed"_q, u"無法終止該登入。請到「設定 → 裝置」手動移除。"_q },
		{ u"LuminaSession2FAMessage"_q, u"該裝置已登出。建議開啟兩步驟驗證，這樣別人光拿到驗證碼也登不進來。"_q },
		{ u"LuminaSession2FAButton"_q, u"立即設定"_q },
		{ u"LuminaSessionLater"_q, u"稍後再說"_q },
		{ u"LuminaSessionQrConfirmTitle"_q, u"要授權新裝置登入嗎？"_q },
		{ u"LuminaSessionQrConfirmMessage"_q, u"你正在授權一個新裝置登入你的帳號。除非你人就在那台電腦前面，否則對方會看到你全部的訊息。絕對不要掃別人傳給你的登入 QR code。"_q },
		{ u"LuminaSessionQrConfirmContinue"_q, u"我就在那台裝置前，繼續"_q },

		// Stories, fully off.
		{ u"LuminaStoriesHeader"_q, u"限時動態"_q },
		{ u"LuminaStoriesFullyOff"_q, u"完全關閉限時動態"_q },
		{ u"LuminaStoriesFullyOffInfo"_q, u"把限時動態從整個 App "
			u"移除：聊天列表上方那一排、封存區裡的"
			u"限動列、所有頭像上的彩色圓圈、個人檔"
			u"案頁的限動區塊與「限時動態」分頁，以"
			u"及限動通知。不會更動伺服器上的任何設"
			u"定——你和別人的限時動態都還在，只是"
			u"這裡不再顯示。"_q },
		{ u"LuminaStoriesHidePostEntry"_q, u"同時隱藏發佈入口"_q },
		{ u"LuminaStoriesHidePostEntryInfo"_q, u"隱藏聊天列表與個人"
			u"檔案頁上用來發佈自己限時動態的相機"
			u"按鈕。"_q },

		// Voice to text (F-32), keys shared with Android's
		// LuminaLocale / LuminaVoiceToTextActivity.
		{ u"LuminaVoiceToTextTitle"_q, u"語音轉文字"_q },
		{ u"LuminaSttEnable"_q, u"啟用語音轉文字"_q },
		{ u"LuminaSttEngine"_q, u"引擎"_q },
		{ u"LuminaSttEngineWhisper"_q, u"OpenAI Whisper（自帶 key）"_q },
		{ u"LuminaSttEngineGoogle"_q, u"Google（自帶 key）"_q },
		{ u"LuminaSttKey"_q, u"API 金鑰"_q },
		{ u"LuminaSttBaseUrl"_q, u"介面位址"_q },
		{ u"LuminaSttModel"_q, u"模型"_q },
		{ u"LuminaSttNotReady"_q, u"語音轉文字已開啟，但下方選擇的引擎還沒有 API 金鑰，因此無法運作。請到「引擎」儲存金鑰，或關閉這個開關。"_q },
		{ u"LuminaSttInfoDesktop"_q, u"轉錄用的是你自己的 API key、花你自己的額度，"
			u"LuminaGram 不提供 key。語音檔會上傳到"
			u"你選的服務，因此只有在你願意把訊息交給該服務時才開啟"
			u"。"_q },
		{ u"LuminaSttVoskUnsupported"_q, u"離線引擎（Vosk）桌面版尚未支援：它需要在 Windows"
			u"、macOS、Linux 各自打包原生庫與語音模型。"
			u"在那之前，桌面版的轉錄只有雲端引擎。"_q },
		{ u"LuminaExplainMenuItem"_q, u"解釋一下"_q },
		{ u"LuminaExplainTitle"_q, u"解釋這則訊息"_q },
		{ u"LuminaExplainWorking"_q, u"解讀中…"_q },
		{ u"LuminaExplainError"_q, u"無法解釋這則訊息。請確認 LLM 金鑰與網路連線"
			u"後再試一次。"_q },
		{ u"LuminaExplainNoKey"_q, u"尚未設定 LLM 金鑰。請先到「設定、LuminaGram"
			u"、翻譯」填入 LLM 金鑰。"_q },
		{ u"LuminaExplainEnable"_q, u"「解釋一下」選單項目"_q },
		{ u"LuminaSttUiMenuItem"_q, u"轉文字"_q },
		{ u"LuminaSttUiPickTitle"_q, u"語音轉文字"_q },
		{ u"LuminaSttUiFetching"_q, u"正在下載語音訊息…"_q },
		{ u"LuminaSttUiTranscribing"_q, u"轉錄中…"_q },
		{ u"LuminaSttUiTranslation"_q, u"譯文"_q },
		{ u"LuminaSttUiError"_q, u"轉錄失敗"_q },
		{ u"LuminaSttUiNoText"_q, u"沒聽出文字"_q },
		{ u"LuminaSttUiNoKey"_q, u"尚未設定 API 金鑰。請到「設定、LuminaGram"
			u"、語音轉文字」填入。"_q },
		{ u"LuminaSttUiTooLarge"_q, u"這則語音訊息超過所選引擎的大小上限。"_q },
		{ u"LuminaSttUiRoundUnsupported"_q, u"Google Speech 無法讀取影片訊息。圓形影"
			u"片請改用 Whisper。"_q },
		{ u"LuminaSttAutoTranslate"_q, u"轉完自動翻譯"_q },
		{ u"LuminaSttAutoTranslateInfo"_q, u"語音轉出的文字如果不是你的閱讀語言，會自動接著翻譯，"
			u"先顯示轉錄原文，再顯示譯文。轉錄語言和閱讀語言相同就"
			u"直接跳過，不浪費額度；翻譯失敗時仍會保留轉錄原文。"_q },
	};
}

[[maybe_unused]] const auto kRegistered = RegisterLocaleTable(
	"zh-hant",
	Build);

} // namespace
} // namespace Lumina
