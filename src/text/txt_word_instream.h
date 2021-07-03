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
    QChar PopNextChar();
    void PushBackChar(QChar c)
    {
        m_buff.push_front(c);
    }

private:
    CharInStream & m_charInStream;
    std::deque<QChar> m_buff;
};
