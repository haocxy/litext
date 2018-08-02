#pragma once

// 换行符
namespace linesep
{
    // 换行符类型
    enum LineSepType
    {
        kNo = 0, // 没有换行
        kLF = 1, // \n
        kCR = 2, // \r
        kCRLF = 3, // \r\n
    };
}
