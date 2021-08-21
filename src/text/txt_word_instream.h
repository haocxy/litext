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

    virtual std::u32string Next() override;

private:
    char32_t PopNextChar();

    void PushBackChar(char32_t c)
    {
        m_buff.push_front(c);
    }

private:
    CharInStream & m_charInStream;
    std::deque<char32_t> m_buff;
};
