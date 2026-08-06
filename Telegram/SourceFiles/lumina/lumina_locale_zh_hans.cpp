/*
This file is part of LuminaGram,
a fork of Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lumina/lumina_locale.h"

namespace Lumina {
namespace {

// Simplified Chinese (mainland usage: 消息 / 发送 / 设置 / 视频 / 文件 /
// 密钥). Wording is shared with Android's LuminaLocale.java wherever the two
// platforms show the same English; where the desktop text was reworded, only
// the terminology is carried over and the desktop English is translated.
//
// Row labels quoted inside the divider paragraphs use the corner-free quotes
// that Simplified Chinese sets, and the phrase between them is copied from the
// row it names, so the two always match.
//
// Continuation lines repeat the u prefix on purpose: every piece of a
// concatenated literal then carries the same encoding, which keeps the
// multi-byte characters unambiguous for every compiler this fork builds with.
[[nodiscard]] LocaleTable Build() {
	return {
		// Sections of the LuminaGram settings page.
		{ u"LuminaGramTitle"_q, u"LuminaGram"_q },
		{ u"LuminaTranslateTitle"_q, u"翻译"_q },
		{ u"LuminaPrivacyTitle"_q, u"隐私"_q },
		{ u"LuminaSecurityTitle"_q, u"安全"_q },
		{ u"LuminaChatSettings"_q, u"聊天"_q },
		{ u"LuminaGramChatList"_q, u"聊天列表"_q },
		{ u"LuminaAppearanceTitle"_q, u"外观"_q },
		{ u"LuminaToolsTitle"_q, u"工具"_q },
		{ u"LuminaGramStoredLocallyInfo"_q, u"LuminaGram 的选项只保存在这台"
			u"设备上，绝不会同步到 Telegram。"_q },

		// Sub-pages that have no rows yet.
		{ u"LuminaAppearancePlaceholder"_q, u"消息、贴纸和数字格式的选项将会"
			u"显示在这里。"_q },
		{ u"LuminaChatListPlaceholder"_q, u"聊天列表的布局与角标选项将会显示"
			u"在这里。"_q },
		{ u"LuminaPrivacyPlaceholder"_q, u"链接、剪贴板与外发媒体的隐私选项"
			u"将会显示在这里。"_q },
		{ u"LuminaSecurityPlaceholder"_q, u"伪装保险箱、紧急清除与胁迫解锁的"
			u"选项将会显示在这里。"_q },
		{ u"LuminaToolsPlaceholder"_q, u"书签、回复模板与本地备份将会显示在"
			u"这里。"_q },

		// Chats sub-page.
		{ u"LuminaMessageActions"_q, u"消息操作"_q },
		{ u"LuminaAllowSaveRestricted"_q, u"允许从受限聊天保存／复制"_q },
		{ u"LuminaAllowSaveRestrictedInfo"_q, u"“允许从受限聊天保存／复制”"
			u"只影响这台设备上的本地操作。有些聊天限制保存是有原因的，请"
			u"谨慎使用。"_q },

		// Translation sub-page.
		{ u"LuminaTranslateEnable"_q, u"启用 LuminaGram 翻译"_q },
		{ u"LuminaTranslateEnableInfo"_q, u"使用你自己的翻译引擎，而不是 "
			u"Telegram 的 Premium 服务。默认的引擎不需要账号，也不需要 API "
			u"密钥。关闭时，LuminaGram 会让翻译保持 Telegram Desktop 原本的"
			u"样子。"_q },
		{ u"LuminaTranslateSendHeader"_q, u"发送"_q },
		{ u"LuminaTranslateBeforeSend"_q, u"发送前翻译"_q },
		{ u"LuminaTranslateSendLang"_q, u"发送语言"_q },
		{ u"LuminaTranslateSendLangAuto"_q, u"对方的语言"_q },
		{ u"LuminaTranslateBeforeSendConfirm"_q, u"发送前确认"_q },
		{ u"LuminaTranslateSendInfo"_q, u"你发出的消息会翻译成上面选择的"
			u"语言，原文也会与译文一起保留。选择“对方的语言”时，LuminaGram "
			u"会在每个聊天第一次询问要用哪种语言，之后就记住。开启“发送前"
			u"确认”时，会先把译文与原文并排显示，让你选择要发送哪一个；"
			u"关闭时译文会直接发送出去。"_q },
		{ u"LuminaTranslateReceiveHeader"_q, u"接收"_q },
		{ u"LuminaDualLanguageDisplay"_q, u"同时显示原文与译文"_q },
		{ u"LuminaTranslateReadLang"_q, u"阅读语言"_q },
		{ u"LuminaTranslateReadLangFollow"_q, u"界面语言"_q },
		{ u"LuminaTranslateModeHeader"_q, u"翻译收到的消息"_q },
		{ u"LuminaTranslateModeAll"_q, u"所有聊天"_q },
		{ u"LuminaTranslateModeManual"_q, u"只在我开启的聊天"_q },
		{ u"LuminaTranslateReceiveInfo"_q, u"收到的消息会以原本的大小保留"
			u"原文，译文显示在下方。“所有聊天”会为每一条消息向你的翻译服务"
			u"发送一次请求；如果密钥是按用量计费的，请保持“只在我开启的"
			u"聊天”。"_q },
		{ u"LuminaTranslateScopeHeader"_q, u"适用范围"_q },
		{ u"LuminaTranslateScopePrivate"_q, u"私聊"_q },
		{ u"LuminaTranslateScopeGroup"_q, u"群组与频道"_q },
		{ u"LuminaTranslateScopeInfo"_q, u"不在适用范围内的聊天，两个方向都"
			u"不会处理：你阅读时不会翻译，你在那里发送的消息也会以原文发出。"
			u"你仍然可以手动翻译任何一条消息。"_q },
		{ u"LuminaTranslateProviderHeader"_q, u"服务"_q },
		{ u"LuminaTranslateProvider"_q, u"翻译服务"_q },
		// The other provider names are the services' own brands and are not
		// translated; this one names a kind of endpoint.
		{ u"LuminaTranslateProviderLlm"_q, u"LLM（兼容 OpenAI）"_q },
		{ u"LuminaTranslateApiKey"_q, u"API 密钥"_q },
		{ u"LuminaTranslateApiKeyNotSet"_q, u"未设置"_q },
		{ u"LuminaTranslateBaseUrl"_q, u"基础 URL"_q },
		{ u"LuminaTranslateModel"_q, u"模型"_q },
		{ u"LuminaTranslateSystemPrompt"_q, u"系统提示词"_q },
		{ u"LuminaTranslatePromptDefault"_q, u"默认"_q },
		{ u"LuminaTranslatePromptCustom"_q, u"自定义"_q },
		{ u"LuminaTranslateFallbackTelegram"_q,
			u"这个服务失败时改用 Telegram"_q },
		{ u"LuminaTranslateProviderSecurityInfo"_q, u"密钥只保存在这台设备"
			u"上，存放在与其他设置分开的文件里，绝不会发送给 Telegram。你"
			u"翻译的所有内容都会发送到这里选择的服务，请挑一个你信任的。"_q },

		// Translate before sending: the boxes and the send menu.
		{ u"LuminaTranslateOriginalLabel"_q, u"原文"_q },
		{ u"LuminaSendTranslation"_q, u"发送译文"_q },
		{ u"LuminaSendOriginal"_q, u"发送原文"_q },
		{ u"LuminaTrSendPickerTitle"_q, u"翻译语言"_q },
		{ u"LuminaTrSendConfirmMessage"_q, u"这个聊天看起来是用{1}写的。要把"
			u"你在这里发送的消息翻译成{1}吗？LuminaGram 会记住这个聊天的"
			u"选择。"_q },
		{ u"LuminaTrSendConfirmTranslate"_q, u"翻译"_q },
		{ u"LuminaTrSendAsTyped"_q, u"按原文发送"_q },
		{ u"LuminaTrSendChooseOther"_q, u"选择语言"_q },

		// The translate-before-send preview bar above the composer.
		{ u"LuminaTranslatePreviewTranslating"_q, u"翻译中…"_q },
		{ u"LuminaTranslatePreviewFailed"_q, u"无法翻译"_q },

		// Names of the languages LuminaGram can translate into, in the order
		// the pickers show them.
		{ u"LuminaLangEn"_q, u"英语"_q },
		{ u"LuminaLangZhTw"_q, u"繁体中文"_q },
		{ u"LuminaLangZhCn"_q, u"简体中文"_q },
		{ u"LuminaLangJa"_q, u"日语"_q },
		{ u"LuminaLangKo"_q, u"韩语"_q },
		{ u"LuminaLangEs"_q, u"西班牙语"_q },
		{ u"LuminaLangFr"_q, u"法语"_q },
		{ u"LuminaLangDe"_q, u"德语"_q },
		{ u"LuminaLangRu"_q, u"俄语"_q },
		{ u"LuminaLangPtBr"_q, u"葡萄牙语（巴西）"_q },
		{ u"LuminaLangPtPt"_q, u"葡萄牙语（葡萄牙）"_q },
		{ u"LuminaLangIt"_q, u"意大利语"_q },
		{ u"LuminaLangAr"_q, u"阿拉伯语"_q },
		{ u"LuminaLangHi"_q, u"印地语"_q },
		{ u"LuminaLangId"_q, u"印度尼西亚语"_q },
		{ u"LuminaLangTh"_q, u"泰语"_q },
		{ u"LuminaLangVi"_q, u"越南语"_q },
		{ u"LuminaLangTr"_q, u"土耳其语"_q },
		{ u"LuminaLangPl"_q, u"波兰语"_q },
		{ u"LuminaLangUk"_q, u"乌克兰语"_q },
		{ u"LuminaLangNl"_q, u"荷兰语"_q },
		{ u"LuminaLangAf"_q, u"南非荷兰语"_q },
		{ u"LuminaLangSq"_q, u"阿尔巴尼亚语"_q },
		{ u"LuminaLangAm"_q, u"阿姆哈拉语"_q },
		{ u"LuminaLangHy"_q, u"亚美尼亚语"_q },
		{ u"LuminaLangAz"_q, u"阿塞拜疆语"_q },
		{ u"LuminaLangEu"_q, u"巴斯克语"_q },
		{ u"LuminaLangBe"_q, u"白俄罗斯语"_q },
		{ u"LuminaLangBn"_q, u"孟加拉语"_q },
		{ u"LuminaLangBs"_q, u"波斯尼亚语"_q },
		{ u"LuminaLangBg"_q, u"保加利亚语"_q },
		{ u"LuminaLangMy"_q, u"缅甸语"_q },
		{ u"LuminaLangCa"_q, u"加泰罗尼亚语"_q },
		{ u"LuminaLangHr"_q, u"克罗地亚语"_q },
		{ u"LuminaLangCs"_q, u"捷克语"_q },
		{ u"LuminaLangDa"_q, u"丹麦语"_q },
		{ u"LuminaLangEt"_q, u"爱沙尼亚语"_q },
		{ u"LuminaLangTl"_q, u"菲律宾语"_q },
		{ u"LuminaLangFi"_q, u"芬兰语"_q },
		{ u"LuminaLangGl"_q, u"加利西亚语"_q },
		{ u"LuminaLangKa"_q, u"格鲁吉亚语"_q },
		{ u"LuminaLangEl"_q, u"希腊语"_q },
		{ u"LuminaLangGu"_q, u"古吉拉特语"_q },
		{ u"LuminaLangHe"_q, u"希伯来语"_q },
		{ u"LuminaLangHu"_q, u"匈牙利语"_q },
		{ u"LuminaLangIs"_q, u"冰岛语"_q },
		{ u"LuminaLangGa"_q, u"爱尔兰语"_q },
		{ u"LuminaLangJv"_q, u"爪哇语"_q },
		{ u"LuminaLangKn"_q, u"卡纳达语"_q },
		{ u"LuminaLangKk"_q, u"哈萨克语"_q },
		{ u"LuminaLangKm"_q, u"高棉语"_q },
		{ u"LuminaLangKu"_q, u"库尔德语"_q },
		{ u"LuminaLangKy"_q, u"柯尔克孜语"_q },
		{ u"LuminaLangLo"_q, u"老挝语"_q },
		{ u"LuminaLangLv"_q, u"拉脱维亚语"_q },
		{ u"LuminaLangLt"_q, u"立陶宛语"_q },
		{ u"LuminaLangMk"_q, u"马其顿语"_q },
		{ u"LuminaLangMs"_q, u"马来语"_q },
		{ u"LuminaLangMl"_q, u"马拉雅拉姆语"_q },
		{ u"LuminaLangMr"_q, u"马拉地语"_q },
		{ u"LuminaLangMn"_q, u"蒙古语"_q },
		{ u"LuminaLangNe"_q, u"尼泊尔语"_q },
		{ u"LuminaLangNo"_q, u"挪威语"_q },
		{ u"LuminaLangPs"_q, u"普什图语"_q },
		{ u"LuminaLangFa"_q, u"波斯语"_q },
		{ u"LuminaLangPa"_q, u"旁遮普语"_q },
		{ u"LuminaLangRo"_q, u"罗马尼亚语"_q },
		{ u"LuminaLangSr"_q, u"塞尔维亚语"_q },
		{ u"LuminaLangSi"_q, u"僧伽罗语"_q },
		{ u"LuminaLangSk"_q, u"斯洛伐克语"_q },
		{ u"LuminaLangSl"_q, u"斯洛文尼亚语"_q },
		{ u"LuminaLangSo"_q, u"索马里语"_q },
		{ u"LuminaLangSw"_q, u"斯瓦希里语"_q },
		{ u"LuminaLangSv"_q, u"瑞典语"_q },
		{ u"LuminaLangTg"_q, u"塔吉克语"_q },
		{ u"LuminaLangTa"_q, u"泰米尔语"_q },
		{ u"LuminaLangTe"_q, u"泰卢固语"_q },
		{ u"LuminaLangUr"_q, u"乌尔都语"_q },
		{ u"LuminaLangUz"_q, u"乌兹别克语"_q },
		{ u"LuminaLangCy"_q, u"威尔士语"_q },
		{ u"LuminaLangYi"_q, u"意第绪语"_q },
		{ u"LuminaLangZu"_q, u"祖鲁语"_q },
	};
}

[[maybe_unused]] const auto kRegistered = RegisterLocaleTable(
	"zh-hans",
	Build);

} // namespace
} // namespace Lumina
