#pragma once

#include "text/char_instream2.h"
#include "model/line.h"

class DocLineCharInStream : public CharInStream2
{
public:
    DocLineCharInStream(const Line & line) : m_line(line) {}
    virtual ~DocLineCharInStream() {}

    virtual UChar Next() override
    {
        if (m_index < m_line.charCnt())
        {
            return m_line.charAt(m_index++);
        }
        return 0;
    }

private:
    const Line & m_line;
    CharN m_index = 0;
};
