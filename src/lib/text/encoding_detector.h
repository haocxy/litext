#pragma once

#include <istream>
#include "supported_encoding.h"

// 编码检测，通过读取若干个字节，判断这些字节的字符编码
class EncodingDetector
{
public:
    EncodingDetector(std::istream & bytes);
    ~EncodingDetector();

    void detect();

    int encoding() const { return m_encoding; }

private:
    std::istream & m_bytes;
    int m_encoding = supported_encoding::kANSI;
};
