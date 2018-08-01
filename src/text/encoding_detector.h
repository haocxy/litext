#pragma once

#include <istream>
#include "supported_encoding.h"

// �����⣬ͨ����ȡ���ɸ��ֽڣ��ж���Щ�ֽڵ��ַ�����
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
