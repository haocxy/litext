#pragma once

#include <deque>
#include "text/word_instream2.h"

class CharInStream2;

class TxtWordStream2 : public WordInStream2
{
public:
    TxtWordStream2(CharInStream2 & charInStream)
        : m_charInStream(charInStream)
    {

    }

    virtual ~TxtWordStream2() {}

    virtual UString Next() override;
private:
    UChar PopNextChar();
    void PushBackChar(UChar c)
    {
        m_buff.push_front(c);
    }

private:
    CharInStream2 & m_charInStream;
    std::deque<UChar> m_buff;
};
