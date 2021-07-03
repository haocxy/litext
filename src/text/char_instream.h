#pragma once

#include "core/uchar.h"


class CharInStream
{
public:
    virtual ~CharInStream() {}

    virtual UChar Next() = 0;
};
