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
		{ u"LuminaTranslateSendInfo"_q, u"你傳出的訊息會翻譯成上面選擇的"
			u"語言，原文也會與譯文一起保留。選擇「對方的語言」時，LuminaGram "
			u"會在每個聊天第一次詢問要用哪個語言，之後就記住。開啟「傳送前"
			u"確認」時，會先把譯文與原文並排顯示，讓你選擇要傳送哪一個；"
			u"關閉時譯文會直接傳送出去。"_q },
		{ u"LuminaTranslateModeHeader"_q, u"翻譯收到的訊息"_q },
		{ u"LuminaTranslateModeAll"_q, u"所有聊天"_q },
		{ u"LuminaTranslateModeManual"_q, u"只在我開啟的聊天"_q },
		{ u"LuminaTranslateModeInfo"_q, u"選「只在我開啟的聊天」時，到某個"
			u"聊天頂部點「翻譯」按鈕即可為該聊天開啟，其他聊天不受影響。"
			u"「所有聊天」會為每一則訊息向你的翻譯服務送出一次請求；如果"
			u"金鑰是按用量計費的，請維持「只在我開啟的聊天」。"_q },
		{ u"LuminaTranslateReceiveHeader"_q, u"接收"_q },
		{ u"LuminaDualLanguageDisplay"_q, u"同時顯示原文與譯文"_q },
		{ u"LuminaTranslateReadLang"_q, u"閱讀語言"_q },
		{ u"LuminaTranslateReadLangFollow"_q, u"介面語言"_q },
		{ u"LuminaTranslateReceiveInfo"_q, u"收到的訊息會以原本的大小保留"
			u"原文，譯文顯示在下方。"_q },
		{ u"LuminaTranslateScopeHeader"_q, u"適用範圍"_q },
		{ u"LuminaTranslateScopePrivate"_q, u"私人聊天"_q },
		{ u"LuminaTranslateScopeGroup"_q, u"群組與頻道"_q },
		{ u"LuminaTranslateScopeInfo"_q, u"不在適用範圍內的聊天，兩個方向都"
			u"不會處理：你閱讀時不會翻譯，你在那裡傳送的訊息也會以原文送出。"
			u"你仍然可以手動翻譯任何一則訊息。"_q },
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
	};
}

[[maybe_unused]] const auto kRegistered = RegisterLocaleTable(
	"zh-hant",
	Build);

} // namespace
} // namespace Lumina
