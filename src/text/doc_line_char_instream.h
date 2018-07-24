#pragma once

#include "text/char_instream.h"
#include "model/line.h"

class DocLineCharInStream : public CharInStream
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
