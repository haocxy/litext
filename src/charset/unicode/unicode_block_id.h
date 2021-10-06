#pragma once

namespace charset::unicode
{

// Unicode编码区块ID
// 这组枚举单纯地用于区分区块,不要给枚举项指定任何具体值
//
// 完整的Unicode编码区块有很多,随着软件开发慢慢加入可以支持的区块
enum class BlockId {

    // 无效值,表示程序内部的初始值或错误情况
    Unknown,

    // 常说的 ASCII 码 0 - 127
    Basic_Latin,

    // 中日韩符号和标点
    // (诸如句号或双引号之类的常用中日韩标点在这部分)
    CJK_Symbols_And_Punctuation,

    // 中日韩统一表意字符
    // (最常用的中日韩字符大部分在这部分)
    CJK_Unified_Ideographs,
};



}
