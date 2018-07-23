#pragma once

#include "module/model/line.h"

class SimpleLine : public Line
{
public:

    virtual ~SimpleLine() {}

    virtual CharN charCnt() const override
    {
        return static_cast<UChar>(m_content.size());
    }

    virtual UChar charAt(CharN i) const override
    {
        return m_content[i];
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

private:
    UString m_content;
};
