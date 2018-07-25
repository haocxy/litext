#pragma once

#include "text/char_instream.h"
#include "doc/doc_line.h"

class DocLineCharInStream : public CharInStream
{
public:
    DocLineCharInStream(const DocLine & line) : m_line(line) {}
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
    const DocLine & m_line;
    CharN m_index = 0;
};
