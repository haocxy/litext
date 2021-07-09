#pragma once

#include <string>
#include "core/fs.h"


namespace doc::CharsetDetectUtil
{

// 检测文件的字符集
// path 文件路径
// offset 跳过文件开始的offset个字节，默认为0表示不跳过任何字节
// len 检测从offset开始的len个字节，默认为0表示检测到文件结尾
std::string detectCharsetOfFile(const fs::path &path, size_t offset = 0, size_t len = 0);

}
