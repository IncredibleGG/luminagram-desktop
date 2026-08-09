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
		{ u"LuminaTranslateReceiveInfo"_q, u"收到的消息会以原本的大小保留"
			u"原文，译文显示在下方。"_q },
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
		{ u"LuminaTranslateTest"_q, u"测试翻译"_q },
		{ u"LuminaTranslateTestRunning"_q, u"测试中…"_q },
		{ u"LuminaTranslateTestSuccess"_q, u"翻译可以正常工作。"_q },
		{ u"LuminaTranslateTestFailed"_q, u"测试失败"_q },
		{ u"LuminaTranslateNoKey"_q, u"需要 API 密钥"_q },
		{ u"LuminaTranslateTestKeyRejected"_q, u"无法连接到这个服务，或者"
			u"服务拒绝了这个 API 密钥。请先检查密钥，再检查基础 URL 与你的"
			u"网络连接。"_q },
		{ u"LuminaTranslateTestNetwork"_q, u"无法连接到这个服务。请检查你的"
			u"网络连接与代理设置，然后再试一次。"_q },
		{ u"LuminaTranslateTestQuota"_q, u"服务拒绝了这次请求：请求太过"
			u"频繁，或这个密钥的额度已经用完。请稍后再试。"_q },
		{ u"LuminaTranslateTestQuotaKeyed"_q, u"服务拒绝了这次请求：服务"
			u"拒绝了这个 API 密钥，或这个密钥的额度已经用完，也可能是请求"
			u"太过频繁。请先检查密钥，然后稍后再试。"_q },
		{ u"LuminaTranslateTestBadResponse"_q, u"服务返回了 LuminaGram 无法"
			u"解读的内容，或者根本没有内容。请检查基础 URL 与模型。"_q },
		{ u"LuminaTranslateTestUnavailable"_q, u"这个服务无法在这里运行。"
			u"请在上面改选其他服务。"_q },
		{ u"LuminaTranslateProviderSecurityInfo"_q, u"密钥只保存在这台设备"
			u"上，存放在与其他设置分开的文件里，绝不会发送给 Telegram。你"
			u"翻译的所有内容都会发送到这里选择的服务，请挑一个你信任的。"_q },

		// Telegram's own AI editor in the composer, whose Translate tab does
		// what LuminaGram translation already does. The two product names
		// stay untranslated.
		{ u"LuminaAiEditorHeader"_q, u"Telegram AI 编辑器"_q },
		{ u"LuminaAiEditorKeep"_q, u"保留 Telegram 的 AI 编辑器"_q },
		{ u"LuminaAiEditorInfo"_q, u"Telegram 在输入框里自带一个 AI 编辑器，"
			u"其中的“翻译”标签页与 LuminaGram 的翻译重叠。开启 LuminaGram "
			u"翻译时，它的按钮和键盘快捷键都不会提供，这样你面前始终只有"
			u"一个翻译工具。开启此项可以让 Telegram 的编辑器照常可用。关闭 "
			u"LuminaGram 翻译时，Telegram 的编辑器始终都在，这个设置不会有"
			u"任何影响。"_q },

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

		// The per-chat translate button in the chat top bar. Both texts name
		// what pressing the button will do, not what the chat is doing now.
		{ u"LuminaTranslateChatToggle"_q, u"翻译此聊天"_q },
		{ u"LuminaTranslateChatShowOriginal"_q, u"显示原文"_q },

		// Right-clicking that button: the per-chat language menu. Its two
		// rows are the two sides of the conversation, named as the other
		// person and as oneself rather than by direction, because a
		// direction word alone left the reader guessing whose messages were
		// meant. The Title strings head the language list each row opens,
		// the Off strings are how a row reads once that half is off, and
		// the None entry is the list item inside the list that turns it off.
		{ u"LuminaChatLangThem"_q, u"对方，翻译成{1}"_q },
		{ u"LuminaChatLangMe"_q, u"己方，翻译成{1}"_q },
		{ u"LuminaChatLangThemOff"_q, u"对方，不翻译"_q },
		{ u"LuminaChatLangMeOff"_q, u"己方，不翻译"_q },
		{ u"LuminaChatLangThemTitle"_q, u"对方的消息，翻译成"_q },
		{ u"LuminaChatLangMeTitle"_q, u"己方的消息，翻译成"_q },
		{ u"LuminaChatLangNone"_q, u"不翻译"_q },

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
		{ u"LuminaAppearanceStickerSizeHeader"_q, u"贴纸大小"_q },
		{ u"LuminaBackupCryptoFailed"_q, u"这个系统无法使用加密功能。"_q },
		{ u"LuminaBackupDamaged"_q, u"备份文件已损坏，无法恢复。"_q },
		{ u"LuminaBackupExport"_q, u"导出加密备份"_q },
		{ u"LuminaBackupExportDone"_q, u"备份已保存。"_q },
		{ u"LuminaBackupExportFailed"_q, u"无法创建备份文件。"_q },
		{ u"LuminaBackupExportInfo"_q, u"把 LuminaGram 保存在这台设备上的"
			u"所有内容——书签、备注、回复模板、文字替换以及全部设置——"
			u"存入一个文件，并用你设定的口令加密。这个文件也包含私密的值："
			u"翻译 API 密钥、保险箱与假崩溃密码，以及诱饵记事本里的笔记。"
			u"任何内容都不会发送给 Telegram。请设置一个较长的口令并妥善"
			u"保管：没有它就打不开这个文件，也没有任何办法找回。"_q },
		{ u"LuminaBackupExportPassphraseTitle"_q, u"设置口令"_q },
		{ u"LuminaBackupFileFilter"_q, u"LuminaGram 备份 (*.lgbak)"_q },
		{ u"LuminaBackupImport"_q, u"导入备份"_q },
		{ u"LuminaBackupImportFailed"_q, u"无法读取备份文件。"_q },
		{ u"LuminaBackupImportInfo"_q, u"选择一个备份文件并输入它的口令，"
			u"即可恢复你的 LuminaGram 数据。备份中带有的设置会覆盖这台设备"
			u"上的设置；备份中没有的内容保持不变。无法通过校验的文件会在"
			u"写入任何内容之前被拒绝，所以口令输错绝不会让你只恢复一半。"_q },
		{ u"LuminaBackupImportSuccess"_q, u"备份已恢复。请重启 LuminaGram 以"
			u"应用全部更改。"_q },
		{ u"LuminaBackupInvalidFile"_q, u"这不是有效的 LuminaGram 备份"
			u"文件。"_q },
		{ u"LuminaBackupNewerFormat"_q, u"这个备份是由更新版本的 LuminaGram "
			u"创建的。"_q },
		{ u"LuminaBackupOpenCaption"_q, u"打开 LuminaGram 备份"_q },
		{ u"LuminaBackupPassphraseHint"_q, u"口令"_q },
		{ u"LuminaBackupPassphraseMismatch"_q, u"两次输入的口令不一致。"_q },
		{ u"LuminaBackupPassphraseRepeatHint"_q, u"再次输入口令"_q },
		{ u"LuminaBackupPassphraseTitle"_q, u"输入口令"_q },
		{ u"LuminaBackupPassphraseTooShort"_q, u"请设置至少 4 个字符的"
			u"口令。"_q },
		{ u"LuminaBackupSaveCaption"_q, u"保存 LuminaGram 备份"_q },
		{ u"LuminaBackupTitle"_q, u"加密备份"_q },
		{ u"LuminaBackupUnauthenticated"_q, u"这个备份使用的是旧的无保护"
			u"格式，无法检查口令是否正确，也无法检查文件是否被改动过。请用"
			u"最新版本的 LuminaGram 重新制作备份。"_q },
		{ u"LuminaBackupWrongPassphrase"_q, u"口令错误，或文件已被改动。"_q },
		{ u"LuminaBookmark"_q, u"添加书签"_q },
		{ u"LuminaBookmarkAdded"_q, u"已添加到书签"_q },
		{ u"LuminaBookmarkChatUnavailable"_q, u"聊天不可用"_q },
		{ u"LuminaBookmarkDeleteTitle"_q, u"删除书签？"_q },
		{ u"LuminaBookmarkGone"_q, u"这个聊天在这台设备上已不可用。"_q },
		{ u"LuminaBookmarkRemove"_q, u"移除书签"_q },
		{ u"LuminaBookmarkRemoved"_q, u"已从书签移除"_q },
		{ u"LuminaBookmarksAbout"_q, u"书签只是指向消息的记号，只保存在这台"
			u"设备上，绝不会发送给 Telegram。移除书签不会影响消息本身。"_q },
		{ u"LuminaBookmarksEmpty"_q, u"你还没有收藏任何消息。"_q },
		{ u"LuminaBookmarksFull"_q, u"书签列表已满。请先移除一个书签，才能"
			u"再添加。"_q },
		{ u"LuminaBookmarksList"_q, u"已加书签的消息"_q },
		{ u"LuminaBookmarksListAbout"_q, u"点击书签即可打开对应的消息，右键"
			u"点击可以移除它。即使消息被删除，书签也会留在这个列表里。"_q },
		{ u"LuminaBookmarksNone"_q, u"无"_q },
		{ u"LuminaBookmarksTitle"_q, u"书签"_q },
		{ u"LuminaChatListDensityTitle"_q, u"聊天列表密度"_q },
		{ u"LuminaCompactListRows"_q, u"精简行高"_q },
		{ u"LuminaCompactListRowsInfo"_q, u"缩短聊天列表每一行的高度，让屏幕"
			u"上能显示更多聊天。头像与消息预览仍会显示。"_q },
		{ u"LuminaChatListDotsTitle"_q, u"头像圆点"_q },
		{ u"LuminaChatListOnlineDot"_q, u"在线圆点"_q },
		{ u"LuminaChatListOnlineDotInfo"_q, u"在单聊头像上显示一个小绿点，"
			u"表示对方当前在线。"_q },
		{ u"LuminaChatListRecencyDot"_q, u"最近上线圆点"_q },
		{ u"LuminaChatListRecencyDotInfo"_q, u"根据对方最后上线的时间为单聊"
			u"头像圆点着色：当前在线为绿色，一小时内为黄色，一天内为橙色。"
			u"更久或隐藏的最后上线时间则不显示圆点。绿色（在线）圆点仍由"
			u"“在线圆点”开关控制。"_q },
		{ u"LuminaChatListVisibilityAbout"_q, u"把文件夹栏从聊天列表中移除"
			u"——旁边的竖栏和上方的横栏都会移除。隐藏文件夹标签时，你始终"
			u"看到全部聊天：原本所在的文件夹会被退出，文件夹快捷键与在文件夹"
			u"之间的滑动也不再起作用。隐藏动态只会移除聊天列表上方的那一行；"
			u"动态本身以及其他打开动态的方式都不受影响。"_q },
		{ u"LuminaChatListVisibilityTitle"_q, u"文件夹与动态"_q },
		{ u"LuminaContactNote"_q, u"私密备注"_q },
		{ u"LuminaContactNoteAbout"_q, u"只有你能看到这些内容。它保存在这台"
			u"设备上，绝不会发送给 Telegram，也不会同步到你的其他设备。"_q },
		{ u"LuminaContactNoteEmpty"_q, u"点击以添加私密备注"_q },
		{ u"LuminaContactNoteHint"_q, u"备注（仅你可见）"_q },
		{ u"LuminaContactNotesAbout"_q, u"在联系人资料页上添加私密备注和一组"
			u"标签。两者都保存在这台设备上，绝不会发送给 Telegram，也不会"
			u"同步到你的其他设备。Telegram 自己的联系人备注不受影响：只有在"
			u"无法使用它的地方——机器人，以及不在你联系人中的人——才会提供"
			u"私密备注，而标签在任何地方都会提供。"_q },
		{ u"LuminaContactNotesClear"_q, u"删除所有私密备注"_q },
		{ u"LuminaContactNotesClearText"_q, u"删除这台设备上保存的所有私密"
			u"备注和标签吗？此操作无法撤销。"_q },
		{ u"LuminaContactNotesClearTitle"_q, u"删除私密备注"_q },
		{ u"LuminaContactNotesNone"_q, u"无"_q },
		{ u"LuminaContactNotesTitle"_q, u"私密联系人备注"_q },
		{ u"LuminaContactNotesToggle"_q, u"在资料页显示私密备注与标签"_q },
		{ u"LuminaContactTags"_q, u"标签"_q },
		{ u"LuminaContactTagsEmpty"_q, u"点击以添加标签"_q },
		{ u"LuminaContactTagsHint"_q, u"标签，以逗号分隔"_q },
		{ u"LuminaDetailsDate"_q, u"日期"_q },
		{ u"LuminaDetailsForwardedFrom"_q, u"转发自"_q },
		{ u"LuminaDetailsFrom"_q, u"来自"_q },
		{ u"LuminaDetailsMessageId"_q, u"消息 ID"_q },
		{ u"LuminaDetailsOriginalDate"_q, u"原始日期"_q },
		{ u"LuminaExactNumbers"_q, u"显示精确数字（不使用 1.2K 缩写）"_q },
		{ u"LuminaExactNumbersInfo"_q, u"显示完整计数，如 1,234,567，而非 "
			u"1.2M 之类的缩写形式。已经绘制在屏幕上的计数会保持当前的"
			u"形式，直到应用重启。"_q },
		{ u"LuminaForwardNoAuthor"_q, u"转发时隐藏作者"_q },
		{ u"LuminaForwardNoAuthorTitle"_q, u"转发时隐藏作者"_q },
		{ u"LuminaForwardNoCaption"_q, u"转发时隐藏作者与说明文字"_q },
		{ u"LuminaForwardNoCaptionTitle"_q, u"转发时隐藏作者与说明文字"_q },
		{ u"LuminaHideStories"_q, u"隐藏动态"_q },
		{ u"LuminaHideTabs"_q, u"隐藏文件夹标签"_q },
		{ u"LuminaLinkSafetyDestination"_q, u"真实目标地址"_q },
		{ u"LuminaLinkSafetyInfo"_q, u"在打开这几类链接前先询问：用“@”符号"
			u"前的文字隐藏真实目标地址的链接、域名用 punycode 拼写的链接，"
			u"以及通过已知短链接服务跳转的链接。确认框会显示真实的目标主机"
			u"和完整网址。对于用其他字母系统书写的仿冒域名，Telegram Desktop "
			u"本身已经会询问。所有检查都在本机进行——你打开的网址不会离开"
			u"这台设备。"_q },
		{ u"LuminaLinkSafetyRow"_q, u"链接安全检查"_q },
		{ u"LuminaLinkSafetyTitle"_q, u"打开外部链接？"_q },
		{ u"LuminaLinkSafetyWarnMismatch"_q, u"此链接通过“@”符号前的文字"
			u"隐藏了真实目标地址。"_q },
		{ u"LuminaLinkSafetyWarnPunycode"_q, u"此网址使用了编码（punycode）"
			u"字符，可能在仿冒知名网站。"_q },
		{ u"LuminaLinkSafetyWarnShortener"_q, u"这是一个短链接——真实目标"
			u"地址在你打开前始终隐藏。"_q },
		{ u"LuminaMediaAutoPauseBgVideo"_q, u"应用退到后台时暂停视频"_q },
		{ u"LuminaMediaAutoPauseBgVideoInfo"_q, u"当你离开 LuminaGram 时，"
			u"自动暂停正在播放的视频。最小化播放器同样会暂停。你特意放到"
			u"独立窗口或画中画里的视频会继续播放，音乐和语音消息在你离开时"
			u"也会继续播放。"_q },
		{ u"LuminaMediaSaving"_q, u"媒体"_q },
		{ u"LuminaMediaTitle"_q, u"媒体"_q },
		{ u"LuminaMessageActionsInfo"_q, u"这些条目只出现在消息的右键菜单里。"
			u"每一项都使用 Telegram 自己的转发功能，所以限制转发的聊天仍然"
			u"受限。“转发时隐藏作者与说明文字”是按实际发出的内容命名的："
			u"Telegram 无法在保留发送者的同时去掉说明文字，所以选择它会两者"
			u"都去掉。“详情”只读取这台设备已经同步的内容——不会发出任何"
			u"请求，也不会保存任何内容。"_q },
		{ u"LuminaMessageDetails"_q, u"详情"_q },
		{ u"LuminaOnboardingDualName"_q, u"两种语言同时显示"_q },
		{ u"LuminaOnboardingDualText"_q, u"让原文和译文一起留在屏幕上，收到的"
			u"消息和你发送的消息都可以。"_q },
		{ u"LuminaOnboardingFooter"_q, u"在 LuminaGram 设置页里打开相应的分区"
			u"即可设置这些功能。你可以随时从“工具”里再次查看这张卡片。"_q },
		{ u"LuminaOnboardingGotIt"_q, u"知道了"_q },
		{ u"LuminaOnboardingHeader"_q, u"关于"_q },
		{ u"LuminaOnboardingIntro"_q, u"LuminaGram 在 Telegram Desktop 之上"
			u"增加了自己的工具。下面的每个选项都只保存在这台电脑上，绝不会"
			u"同步到 Telegram。"_q },
		{ u"LuminaOnboardingRow"_q, u"LuminaGram 增加了什么"_q },
		{ u"LuminaOnboardingRowAbout"_q, u"显示第一次打开 LuminaGram 设置时"
			u"出现的欢迎卡片。它只介绍这里有些什么，不会开启任何功能。"_q },
		{ u"LuminaOnboardingSafetyName"_q, u"安全检查"_q },
		{ u"LuminaOnboardingSafetyText"_q, u"在打开地址与外表不符的链接前发出"
			u"警告，在你粘贴加密货币地址时发现被掉包的地址，并移除你发送的"
			u"照片中的位置信息。"_q },
		{ u"LuminaOnboardingTitle"_q, u"欢迎使用 LuminaGram"_q },
		{ u"LuminaOnboardingTranslateText"_q, u"用你自己的翻译服务翻译收到的"
			u"消息，也可以在发送前翻译你输入的内容。"_q },
		{ u"LuminaOnboardingVaultName"_q, u"伪装保险箱"_q },
		{ u"LuminaOnboardingVaultText"_q, u"把 LuminaGram 藏在计算器或记事本"
			u"后面，只有输入你的秘密密码才会打开真正的应用。"_q },
		{ u"LuminaProfileCardAbout"_q, u"一张关于你自己的本地名片——你会说"
			u"哪些语言、对什么感兴趣——可以复制后粘贴到聊天里。它只保存在"
			u"这台设备上。"_q },
		{ u"LuminaProfileCardBio"_q, u"简介"_q },
		{ u"LuminaProfileCardBioHint"_q, u"简单介绍一下你自己"_q },
		{ u"LuminaProfileCardCopied"_q, u"名片已复制"_q },
		{ u"LuminaProfileCardCopy"_q, u"复制到剪贴板"_q },
		{ u"LuminaProfileCardCopyInfo"_q, u"将名片生成为纯文本摘要。想要分享"
			u"时，把它粘贴到任意聊天中即可。"_q },
		{ u"LuminaProfileCardEdit"_q, u"我的名片"_q },
		{ u"LuminaProfileCardEmptyShare"_q, u"请先填写你的名片"_q },
		{ u"LuminaProfileCardHeader"_q, u"我的名片"_q },
		{ u"LuminaProfileCardInfo"_q, u"此名片仅保存在本设备上，绝不会发送给 "
			u"Telegram，也不会更改你的 Telegram 个人资料。"_q },
		{ u"LuminaProfileCardInterests"_q, u"兴趣／标签"_q },
		{ u"LuminaProfileCardInterestsHint"_q, u"例如：音乐、徒步、编程"_q },
		{ u"LuminaProfileCardLanguages"_q, u"我会说的语言"_q },
		{ u"LuminaProfileCardLanguagesHint"_q,
			u"例如：中文、English、Español"_q },
		{ u"LuminaProfileCardNotSet"_q, u"未设置"_q },
		{ u"LuminaProfileCardTagline"_q, u"标语"_q },
		{ u"LuminaProfileCardTaglineHint"_q, u"一句话介绍你自己"_q },
		{ u"LuminaProfileCardTitle"_q, u"个人名片"_q },
		{ u"LuminaProfileChatCreated"_q, u"创建于"_q },
		{ u"LuminaProfileDcId"_q, u"数据中心"_q },
		{ u"LuminaProfileDcIdValue"_q, u"DC{1}"_q },
		{ u"LuminaProfileInfoHeader"_q, u"个人资料"_q },
		{ u"LuminaProfileInfoInfo"_q, u"在资料页上增加几行信息，全部在这台"
			u"设备上算出——不会向 Telegram 索取任何内容。部分账号的注册日期"
			u"由 Telegram 自己提供；其余的则根据账号编号估算，并以“~”标出。"
			u"数据中心指的是存放头像的那一个。创建日期是群组或频道建立的"
			u"时间。"_q },
		{ u"LuminaProfileRegistrationApprox"_q, u"~ {1}"_q },
		{ u"LuminaProfileRegistrationDate"_q, u"注册日期"_q },
		{ u"LuminaProfileShowChatDate"_q, u"显示创建日期"_q },
		{ u"LuminaProfileShowDcId"_q, u"显示数据中心"_q },
		{ u"LuminaProfileShowRegistrationDate"_q, u"显示注册日期"_q },
		{ u"LuminaRecentLimitsInfo"_q, u"让这台设备不要过早丢弃最近使用的"
			u"贴纸和收藏的 GIF，采用与 Android 版 LuminaGram 相同的上限："
			u"最多 200 个最近贴纸和 500 个收藏的 GIF。你的账号里实际保存"
			u"多少、同步到其他设备多少，仍由 Telegram 的服务器决定，所以"
			u"这并不能让云端的列表变大——这台设备下次同步时，服务器上已经"
			u"没有的内容在这里也会消失，通常只要几秒。开启时，Telegram "
			u"Desktop 不再自行删除收藏的 GIF，因此它关于收藏 GIF 上限的 "
			u"Premium 提示也不会再出现。除非你同时在“设置”›“高级”›"
			u"“实验性设置”里开启“无限最近贴纸”，贴纸面板仍然只显示前 "
			u"20 个最近使用的贴纸。"_q },
		{ u"LuminaRecentLimitsRow"_q, u"保留更多最近贴纸与 GIF"_q },
		{ u"LuminaReplyTemplatesAbout"_q, u"保存在这台设备上、可以插入到"
			u"输入框的短文本。在聊天中右键点击表情按钮即可选择一条。模板"
			u"绝不会发送给 Telegram。"_q },
		{ u"LuminaReplyTemplatesAdd"_q, u"添加模板"_q },
		{ u"LuminaReplyTemplatesEdit"_q, u"编辑模板"_q },
		{ u"LuminaReplyTemplatesEmpty"_q, u"还没有模板。先添加一条，然后在"
			u"聊天中右键点击表情按钮即可插入。"_q },
		{ u"LuminaReplyTemplatesFull"_q, u"列表已满——请先删除一条模板，"
			u"才能再添加。"_q },
		{ u"LuminaReplyTemplatesInfo"_q, u"在聊天中右键点击表情按钮即可插入"
			u"模板。在这里右键点击某条模板可以上移或下移；打开它可以编辑"
			u"或删除。"_q },
		{ u"LuminaReplyTemplatesList"_q, u"模板"_q },
		{ u"LuminaReplyTemplatesManage"_q, u"管理模板…"_q },
		{ u"LuminaReplyTemplatesMoveDown"_q, u"下移"_q },
		{ u"LuminaReplyTemplatesMoveUp"_q, u"上移"_q },
		{ u"LuminaReplyTemplatesNone"_q, u"无"_q },
		{ u"LuminaReplyTemplatesPlaceholder"_q, u"模板内容"_q },
		{ u"LuminaReplyTemplatesShow"_q, u"在聊天中提供模板"_q },
		{ u"LuminaReplyTemplatesTitle"_q, u"回复模板"_q },
		{ u"LuminaSaveSticker"_q, u"保存贴纸"_q },
		{ u"LuminaSaveStickers"_q, u"保存贴纸"_q },
		{ u"LuminaSaveStickersInfo"_q, u"在贴纸面板的右键菜单中添加“保存"
			u"贴纸”一项。贴纸会保存到你其他下载文件所在的位置；当群组限制"
			u"保存时，该群组自己的贴纸包不会显示这一项。"_q },
		{ u"LuminaSaveToCloud"_q, u"保存到收藏夹"_q },
		{ u"LuminaSaveToCloudTitle"_q, u"保存到收藏夹"_q },
		{ u"LuminaSecurityPanicConfirmAck"_q, u"我明白此操作无法撤销"_q },
		{ u"LuminaSecurityPanicConfirmButton"_q, u"立即清除"_q },
		{ u"LuminaSecurityPanicConfirmText"_q, u"这台设备上的所有账号都会被"
			u"登出。本地消息数据库、草稿和缓存的媒体都会被清除，LuminaGram "
			u"自己的设置、书签、已保存的译文和 API 密钥也一并清除。\n\n你的"
			u"账号不会被删除。它们和你的消息都留在 Telegram 的服务器上——"
			u"你可以随时在任何地方重新登录。\n\n已经下载的文件不会被删除。"
			u"LuminaGram 不会碰你的下载文件夹，因为那通常就是你普通的下载"
			u"文件夹，里面还放着无关的文件。请自己移动或删除其中的敏感"
			u"内容。\n\n此操作无法撤销。"_q },
		{ u"LuminaSecurityPanicConfirmTitle"_q, u"紧急清除？"_q },
		{ u"LuminaSecurityPanicHeader"_q, u"紧急清除"_q },
		{ u"LuminaSecurityPanicWipe"_q, u"紧急清除 (Kaboom)"_q },
		{ u"LuminaSecurityPanicWipeAbout"_q, u"登出这台设备上的所有账号，并"
			u"清除本地消息数据库、草稿和缓存的媒体，以及 LuminaGram 自己的"
			u"设置、书签、已保存的译文和 API 密钥。你的账号和消息仍留在 "
			u"Telegram 的服务器上。已经下载的文件会留在原处。此操作无法"
			u"撤销。"_q },
		{ u"LuminaSelectFromAuthor"_q, u"选择该作者的全部消息"_q },
		{ u"LuminaSelectFromAuthorAbout"_q, u"在群组的消息菜单中添加一项，"
			u"用来选中你点击的发送者发出的所有消息。它只能选到这个窗口已经"
			u"加载的消息——向上滚动加载更早的消息后再执行一次，才能把它们"
			u"也包含进来。归属于聊天本身的消息，例如频道帖子和匿名管理员的"
			u"帖子，不会出现这一项。"_q },
		{ u"LuminaSelectFromAuthorLimit"_q,
			u"一次最多只能选择 {1} 条消息。"_q },
		{ u"LuminaSelectFromAuthorTitle"_q, u"选择该作者的全部消息"_q },
		{ u"LuminaSendOriginalCaption"_q, u"发送原始说明文字"_q },
		{ u"LuminaShowBookmarks"_q, u"显示书签菜单项"_q },
		{ u"LuminaShowMessageDetails"_q, u"消息详情菜单"_q },
		{ u"LuminaShowMutedCount"_q, u"始终显示未读计数"_q },
		{ u"LuminaShowMutedCountInfo"_q, u"将已静音聊天的未读角标以正常"
			u"强调色显示，而非静音灰色。"_q },
		{ u"LuminaStickerSavedTo"_q, u"贴纸已保存到 {1}"_q },
		{ u"LuminaStickerSizeChoice"_q, u"{1}%"_q },
		{ u"LuminaStickerSizeChoiceDefault"_q, u"{1}%（默认）"_q },
		{ u"LuminaStickerSizeInfo"_q, u"贴纸在聊天中绘制的大小——你发送的和"
			u"你收到的都包括在内。动画表情、骰子和礼物贴纸保持各自原有的"
			u"大小。Telegram Desktop 只会测量一次贴纸并把结果记下来，所以"
			u"新的大小要到下次启动应用时才生效。"_q },
		{ u"LuminaTimeWithSeconds"_q, u"在消息时间中显示秒"_q },
		{ u"LuminaTimeWithSecondsAbout"_q, u"消息下方的时间、复制文本中的"
			u"时间，以及屏幕阅读器朗读的时间都会包含秒。"_q },
		// The two items of the system tray menu; the second is also the one
		// item on the taskbar jump list on Windows. {1} is the application's
		// own name, never translated and never inflected, so both read as a
		// verb followed by a bare product name.
		{ u"LuminaTrayOpen"_q, u"打开 {1}"_q },
		{ u"LuminaTrayQuit"_q, u"退出 {1}"_q },
		{ u"LuminaUndoSendBulletin"_q, u"正在发送消息…"_q },
		{ u"LuminaUndoSendTitle"_q, u"撤销发送"_q },
		{ u"LuminaUndoSendUndo"_q, u"撤销"_q },
		{ u"LuminaUndoSendWindow"_q, u"撤销发送窗口"_q },
		{ u"LuminaUndoSendWindowInfo"_q, u"纯文字消息在真正发出之前，会先在"
			u"“撤销”按钮后面暂缓 {1} 秒。这段时间里你的文字一直留在输入框"
			u"中，只有消息真正发出后输入框才会清空，所以撤销只是让文字留在"
			u"原处——不会先拿走再放回。再次发送、打开另一个聊天或退出应用，"
			u"都会立刻把暂缓的消息发出去。媒体、语音、已编辑、转发和定时"
			u"消息永远不会被暂缓，从论坛话题或评论串中发送的消息也不会。"_q },
		// clipboard guard + scam watch
		{ u"LuminaPrivacyCryptoClipboardGuard"_q, u"加密货币地址粘贴保护"_q },
		{ u"LuminaPrivacyCryptoClipboardGuardInfo"_q, u"把看起来像加密货币钱包"
			u"地址的内容粘贴进消息前先问你一声。剪贴板劫持恶意软件可能在你毫无"
			u"察觉的情况下，把你复制的地址换成骗子的地址，而粘贴正是你能发现它"
			u"的最后一刻。检查在本机进行，不会把任何内容发送出去。"_q },
		{ u"LuminaPrivacyScamKeywordWarning"_q, u"诈骗关键词警告"_q },
		{ u"LuminaPrivacyScamKeywordWarningInfo"_q, u"当不在你联系人中的人发来"
			u"的消息提到常见诈骗手段（汇款、礼品卡、加密货币“投资”、验证费或索"
			u"要验证码）时，显示一次性提醒，提醒你保持警惕。消息永远不会被拦截"
			u"或修改。检查在你的设备上离线运行。"_q },
		// otp guard
		{ u"LuminaOtpGuardTitle"_q, u"警告"_q },
		{ u"LuminaOtpGuardMessage"_q, u"你正要把 Telegram 登录码发给别人。对方拿到这组码，就能登录你的账号，看到你所有的消息。Telegram 官方永远不会跟你要这组码。"_q },
		{ u"LuminaOtpGuardCancel"_q, u"取消发送"_q },
		{ u"LuminaOtpGuardSendAnyway"_q, u"我知道风险，仍要发送"_q },
		{ u"LuminaOtpGuard"_q, u"登录码泄露防护"_q },
		{ u"LuminaOtpGuardInfo"_q, u"当 Telegram 刚给你发过登录码、而你正要发出含有验证码的消息时，先弹出警告。整个判断都在你的设备上完成，不会上传任何内容，也不会改动你的消息。"_q },
		// link preview + voice confirm
		{ u"LuminaPrivacyDisableLinkPreview"_q, u"默认禁用链接预览"_q },
		{ u"LuminaLinkPreviewInfo"_q, u"你新写的消息会不带链接预览发送，输入框"
			u"也不会在你打字时让 Telegram 去解析粘贴的链接。已经附上"
			u"的预览，以及你编辑的消息，都保持原样。若想单独给某条消息附上预览，"
			u"可以在设置 > 键盘快捷键中给“切换链接预览”指定一个按键，撰写"
			u"时按下即可。"_q },
		{ u"LuminaVoiceConfirmTitle"_q, u"语音和视频消息"_q },
		{ u"LuminaVoiceConfirmRow"_q, u"发送前确认"_q },
		{ u"LuminaVoiceConfirmInfo"_q, u"在录好的语音或圆形视频消息发出前先询"
			u"问你。取消这个询问会丢弃录音，只有试听预览中例外——那里会保留录"
			u"音。定时发送永远不会询问。"_q },
		// exif strip + text replace
		{ u"LuminaExifStripRow"_q, u"发送前移除照片位置信息"_q },
		{ u"LuminaExifStripInfo"_q, u"在上传之前，抹掉相机写进 JPEG 照片里的"
			u" GPS 坐标——无论你是以普通的压缩方式发送照片，还是以文件形式发"
			u"送。只有位置标签会被移除：方向、相机和日期标签完全保留原样，所以"
			u"以文件形式发送的照片依然是正的。你磁盘上的那份文件永远不会被修"
			u"改。这不涵盖视频，也不涵盖由 XMP 或相机自家的制造商注释写进照片"
			u"的位置，或是某些手机塞进同一个文件里的第二份图像。"_q },
		{ u"LuminaReplacerTitle"_q, u"文字替换"_q },
		{ u"LuminaReplacerHeader"_q, u"规则"_q },
		{ u"LuminaReplacerAdd"_q, u"添加规则"_q },
		{ u"LuminaReplacerEdit"_q, u"编辑规则"_q },
		{ u"LuminaReplacerFrom"_q, u"替换"_q },
		{ u"LuminaReplacerTo"_q, u"为"_q },
		{ u"LuminaReplacerEnable"_q, u"替换我发送的消息中的文字"_q },
		{ u"LuminaReplacerNone"_q, u"无"_q },
		{ u"LuminaReplacerNothing"_q, u"（空）"_q },
		{ u"LuminaReplacerEmpty"_q, u"还没有任何规则。添加一条，就能开始替换你"
			u"发送的消息中的文字。"_q },
		{ u"LuminaReplacerFull"_q, u"规则按从上到下的顺序套用。列表已满——请先"
			u"删除一条规则，才能再添加。"_q },
		{ u"LuminaReplacerListInfo"_q, u"规则按从上到下的顺序套用，每一条处理"
			u"的都是上一条替换后的结果。"_q },
		{ u"LuminaReplacerAbout"_q, u"在你发送的每一条消息里，把简写换成你真正"
			u"想说的话——“brb”会变成“be right back”。匹配区分大小写，而且只按整"
			u"词匹配。带有任何格式的消息会原样发出，链接、提及、话题标签和机器"
			u"人命令也永远不会被改写。规则只保存在这台设备上，绝不会发送给"
			u" Telegram。"_q },
		// vault + fake crash
		{ u"LuminaVaultHeader"_q, u"伪装保险箱"_q },
		{ u"LuminaVaultEnable"_q, u"保险箱开关"_q },
		{ u"LuminaVaultInfo"_q, u"这里的一切都只保存在本机。保险箱把 "
			u"LuminaGram 藏在一个不起眼的 App "
			u"后面，只有输入秘密密码才能回到真正的 App。"_q },
		{ u"LuminaVaultMode"_q, u"保险箱模式"_q },
		{ u"LuminaVaultModePasswordDoor"_q, u"密码门"_q },
		{ u"LuminaVaultModeDecoyApp"_q, u"直接伪装"_q },
		{ u"LuminaVaultModePasswordDoorInfo"_q, u"启动 "
			u"LuminaGram 时会先要求输入密码。输入秘"
			u"密密码进入真正的 App；输入其他内容则打开假 "
			u"App，而且永远不会提示密码错误。"_q },
		{ u"LuminaVaultModeDecoyAppInfo"_q, u"启动 LuminaGram "
			u"时直接打开假 App。在假 App 中输入秘密密"
			u"码即可进入真正的 App。"_q },
		{ u"LuminaVaultSkin"_q, u"假 App 样式"_q },
		{ u"LuminaVaultSkinNotepad"_q, u"记事本"_q },
		{ u"LuminaVaultSkinCalculator"_q, u"计算器"_q },
		{ u"LuminaVaultSkinHintCalculator"_q, u"要从计算器解锁，请输入秘密密"
			u"码，然后按等号键。"_q },
		{ u"LuminaVaultSkinHintNotepad"_q, u"要从记事本解锁，请让整篇便笺只"
			u"有秘密密码，然后双击“记事本”标题栏。"_q },
		{ u"LuminaVaultSecretCode"_q, u"秘密密码"_q },
		{ u"LuminaVaultSecretCodeDialogTitle"_q, u"秘密密码"_q },
		{ u"LuminaDisguiseDecoyCodeSet"_q, u"已设置"_q },
		{ u"LuminaDisguiseDecoyCodeNotSet"_q, u"未设置"_q },
		{ u"LuminaVaultNoCodeInfo"_q, u"在你设置秘密密码之前，保险箱不会"
			u"生效。"_q },
		{ u"LuminaVaultCalculatorCodeInfo"_q, u"这个密码无法在计算器键盘上输"
			u"入，因此会改用记事本假 App。如果你想用计算器，"
			u"请只使用数字、小数点和 + - * /。"_q },
		{ u"LuminaVaultDisclaimer"_q, u"这只挡得住别人不经意的一瞥，挡不"
			u"住真正懂行的检查：程序在任务管理器里仍然叫 "
			u"LuminaGram，任何能读取本机文件的人都能"
			u"读到你的账号。保险箱会在下次启动 "
			u"LuminaGram 时出现，关闭假 App 就"
			u"等于退出 LuminaGram。万一你忘了密码，"
			u"请从 LuminaGram 数据文件夹中删除 "
			u"tdata/luminagram.json 和 "
			u"tdata/luminagram_private.json："
			u"保险箱就会自动关闭，LuminaGram 也会正"
			u"常启动。"_q },
		{ u"LuminaSecurityFakeCrashHeader"_q, u"假崩溃解锁"_q },
		{ u"LuminaSecurityFakeCrashEnable"_q, u"启用假崩溃解锁"_q },
		{ u"LuminaSecurityFakeCrashCode"_q, u"假崩溃密码"_q },
		{ u"LuminaSecurityFakeCrashCodeDialogTitle"_q, u"假崩溃密码"_q },
		{ u"LuminaSecurityFakeCrashCodeHint"_q, u"密码"_q },
		{ u"LuminaSecurityFakeCrashCodeSet"_q, u"已设置"_q },
		{ u"LuminaSecurityFakeCrashCodeNotSet"_q, u"未设置"_q },
		{ u"LuminaSecurityFakeCrashCodeSameAsPasscode"_q, u"此密码不能与你的密码"
			u"锁相同。"_q },
		{ u"LuminaSecurityFakeCrashInfo"_q, u"开启后，在锁屏界面输入假崩溃"
			u"密码不会解锁，而是立刻关闭 LuminaGram："
			u"没有任何提示，屏幕上也不留下任何东西——在旁人看"
			u"来，就是应用崩溃了。这个密码与你的密码锁相互独立，"
			u"只保存在本机，绝不会发送到任何地方。"_q },
		{ u"LuminaSecurityFakeCrashBiometricInfo"_q, u"在此功能开启且已设置密"
			u"码期间，使用 Touch ID、"
			u"Windows Hello 或系统密码解锁会被停"
			u"用：指纹解锁根本不会询问密码锁，也就会直接绕过这"
			u"个密码。清除密码或关闭此功能后，它们会恢复。"_q },
		{ u"LuminaSecurityFakeCrashNoCodeInfo"_q, u"还没有设置密码，因此锁屏"
			u"时不会有任何反应。"_q },
		{ u"LuminaSecurityFakeCrashNoPasscodeInfo"_q, u"LuminaGram "
			u"没有设置密码锁，因此锁屏界面永远不会出现，也就永"
			u"远不会用到这个密码。请先在“隐私与安全”中开启密"
			u"码锁。"_q },
		{ u"LuminaSecurityFakeCrashCodeClashInfo"_q, u"这个密码现在与你的密码"
			u"锁相同，因此永远不会触发——密码锁会先解锁应用。"
			u"请换一个密码。"_q },
		// login guard (D-32)
		{ u"LuminaSessionGuardHeader"_q, u"登录守卫"_q },
		{ u"LuminaSessionGuard"_q, u"新登录提醒"_q },
		{ u"LuminaSessionGuardInfo"_q, u"应用启动时检查你的已连接设备，发现你没确认过的登录就立刻提醒。扫描登录二维码等于把一个活跃会话交给对方，对方能看到你全部的聊天记录。检查最多每 30 分钟一次，走 Telegram 官方接口，数据不会离开你的设备。"_q },
		{ u"LuminaSessionGuardCheckNow"_q, u"立即检查登录设备"_q },
		{ u"LuminaSessionGuardNoNew"_q, u"没有新登录，所有已连接设备都是你确认过的。"_q },
		{ u"LuminaSessionGuardCheckFailed"_q, u"无法检查已连接设备，请检查网络后重试。"_q },
		{ u"LuminaSessionAlertTitle"_q, u"发现新登录"_q },
		{ u"LuminaSessionAlertIntro"_q, u"有一台你没确认过的设备已登录你的账号。如果不是你本人操作，对方现在就能看到你全部的聊天记录。"_q },
		{ u"LuminaSessionAlertDevice"_q, u"设备"_q },
		{ u"LuminaSessionAlertApp"_q, u"应用"_q },
		{ u"LuminaSessionAlertIp"_q, u"IP 地址"_q },
		{ u"LuminaSessionAlertLocation"_q, u"位置"_q },
		{ u"LuminaSessionAlertTime"_q, u"登录时间"_q },
		{ u"LuminaSessionAlertNotMe"_q, u"不是我 — 立即终止此登录"_q },
		{ u"LuminaSessionAlertItsMe"_q, u"是我"_q },
		{ u"LuminaSessionTerminated"_q, u"已终止该登录"_q },
		{ u"LuminaSessionTerminateFailed"_q, u"无法终止该登录。请到「设置 → 设备」手动移除。"_q },
		{ u"LuminaSession2FAMessage"_q, u"该设备已被登出。建议开启两步验证，这样别人光有验证码也登不进来。"_q },
		{ u"LuminaSession2FAButton"_q, u"立即设置"_q },
		{ u"LuminaSessionLater"_q, u"以后再说"_q },
		{ u"LuminaSessionQrConfirmTitle"_q, u"要授权新设备登录吗？"_q },
		{ u"LuminaSessionQrConfirmMessage"_q, u"你正在授权一台新设备登录你的账号。除非你人就在那台电脑前面，否则对方会看到你全部的消息。不要扫别人发给你的登录二维码。"_q },
		{ u"LuminaSessionQrConfirmContinue"_q, u"我就在那台设备前，继续"_q },
	};
}

[[maybe_unused]] const auto kRegistered = RegisterLocaleTable(
	"zh-hans",
	Build);

} // namespace
} // namespace Lumina
