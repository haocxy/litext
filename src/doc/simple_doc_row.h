#pragma once

#include "doc_row.h"

class SimpleRow : public Row
{
public:

    virtual ~SimpleRow() {}

    virtual CharN charCnt() const override
    {
        return static_cast<CharN>(m_content.size());
    }

    virtual QChar charAt(CharN i) const override
    {
        return m_content[i];
    }

    virtual RowEnd rowEnd() const override
    {
        return m_rowEnd;
    }

    void setContent(const QString &content)
    {
        m_content = content;
    }
    
    void setContent(QString &&content)
    {
        m_content = std::move(content);
    }

    void Add(QChar c)
    {
        m_content.push_back(c);
    }

    void setRowEnd(RowEnd rowEnd)
    {
        m_rowEnd = rowEnd;
    }

private:
    QString m_content;
    RowEnd m_rowEnd = RowEnd::NO;
};
