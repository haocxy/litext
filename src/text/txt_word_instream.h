#pragma once

#include <deque>
#include "text/word_instream.h"

class CharInStream;

class TxtWordStream : public WordInStream
{
public:
    explicit TxtWordStream(CharInStream & charInStream)
        : m_charInStream(charInStream)
    {

    }

    virtual ~TxtWordStream() {}

    virtual QString Next() override;
private:
    UChar PopNextChar();
    void PushBackChar(UChar c)
    {
        m_buff.push_front(c);
    }

private:
    CharInStream & m_charInStream;
    std::deque<UChar> m_buff;
};
