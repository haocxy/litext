#pragma once

#include <string>

#include "doc_row.h"


class SimpleRow : public Row
{
public:

    virtual ~SimpleRow() {}

    virtual CharN charCnt() const override
    {
        return static_cast<CharN>(m_content.size());
    }

    virtual char32_t charAt(CharN i) const override
    {
        return m_content[i];
    }

    virtual RowEnd rowEnd() const override
    {
        return m_rowEnd;
    }

    void setContent(const std::u32string &content)
    {
        m_content = content;
    }
    
    void setContent(std::u32string &&content)
    {
        m_content = std::move(content);
    }

    void setRowEnd(RowEnd rowEnd)
    {
        m_rowEnd = rowEnd;
    }

private:
    std::u32string m_content;
    RowEnd m_rowEnd = RowEnd::NO;
};
