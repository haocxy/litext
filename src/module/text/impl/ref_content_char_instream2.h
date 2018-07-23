#pragma once

#include "module/text/char_instream2.h"

class RefContentCharInStream2 : public CharInStream2
{
public:
    RefContentCharInStream2(const UString & content) : m_content(content) {}
    virtual ~RefContentCharInStream2() {}

    virtual UChar Next() override
    {
        if (m_index < m_content.size())
        {
            return m_content[m_index++];
        }
        return 0;
    }
private:
    const UString & m_content;
    UString::size_type m_index = 0;
};
