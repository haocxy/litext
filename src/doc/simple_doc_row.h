#pragma once

#include "core/ustring.h"

#include "doc_row.h"


class SimpleRow : public Row
{
public:

    virtual ~SimpleRow() {}

    virtual CharN charCnt() const override
    {
        return static_cast<CharN>(m_content.size());
    }

    virtual UChar charAt(CharN i) const override
    {
        return m_content[i];
    }

    virtual RowEnd rowEnd() const override
    {
        return m_rowEnd;
    }

    void setContent(const UString &content)
    {
        m_content = content;
    }
    
    void setContent(UString &&content)
    {
        m_content = std::move(content);
    }

    void Add(UChar c)
    {
        m_content.push_back(c);
    }

    void setRowEnd(RowEnd rowEnd)
    {
        m_rowEnd = rowEnd;
    }

private:
    UString m_content;
    RowEnd m_rowEnd = RowEnd::NO;
};
