#pragma once

#include "module/text/word_instream.h"

class CharInStream;

class TxtWordStream : public WordInStream
{
public:
    TxtWordStream(CharInStream & charInStream)
        : m_charInStream(charInStream)
    {

    }

    virtual ~TxtWordStream() {}

    virtual bool HasNext() override;

    virtual QString Next() override;

private:
    CharInStream & m_charInStream;
};
