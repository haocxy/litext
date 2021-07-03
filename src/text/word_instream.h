#pragma once

#include <QString>


class WordInStream
{
public:
    virtual ~WordInStream() {}

    virtual QString Next() = 0;
};
