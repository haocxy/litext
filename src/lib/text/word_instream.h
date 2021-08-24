#pragma once

#include <string>


class WordInStream
{
public:
    virtual ~WordInStream() {}

    virtual std::u32string Next() = 0;
};
