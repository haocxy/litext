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


// 快速检测文件字符集
//
// 如果文件过大，检测整个文件是非常费时的，就不能检测整个文件，
// 但也不能仅检测文件的开始部分，因为某些文件的前面一部分都是ASCII编码，
// 内部使用多线程的方式同时检测文件的多个位置
//
// 检测的速度和文档大小无关，只取决于机器的处理能力
// 但检测的精确度取决于处理器数量，这是因为处理器越多就会选择越多的被检测部分
// 在大多数情况下即便处理器数量较少也具有足够的精确度
std::string quickDetectCharset(const fs::path &path);


}
