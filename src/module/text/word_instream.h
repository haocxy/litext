#pragma once

#include <QString>

class WordInStream
{
public:
    virtual ~WordInStream() {}

    virtual bool HasNext() = 0;

    virtual QString Next() = 0;
};
