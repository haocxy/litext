#pragma once

#include "text/char_instream.h"
#include "doc/doc_row.h"


class DocLineCharInStream : public CharInStream
{
public:
    explicit DocLineCharInStream(const Row & line) : m_line(line) {}
    virtual ~DocLineCharInStream() {}

    virtual char32_t Next() override
    {
        if (m_index < m_line.charCnt())
        {
            return m_line.charAt(m_index++);
        }
        return 0;
    }

private:
    const Row & m_line;
    CharN m_index = 0;
};
