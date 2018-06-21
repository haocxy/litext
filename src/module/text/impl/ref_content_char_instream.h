#pragma once

#include <cassert>
#include <QString>

#include "module/text/char_instream.h"

class RefContentQCharInStream : public CharInStream
{
public:
    RefContentQCharInStream(const QString &content)
        : m_content(content)
    {}

    virtual ~RefContentQCharInStream() {}

    virtual bool HasNext() override
    {
        return m_index < m_content.size();
    }

    virtual QChar Next() override
    {
        assert(m_index < m_content.size());
        return m_content[m_index++];
    }

private:
    typedef QString::size_type Index;
    const QString &m_content;
    Index m_index = 0;
};
