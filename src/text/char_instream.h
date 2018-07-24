#pragma once

#include "common/common_define.h"

class CharInStream
{
public:
    virtual ~CharInStream() {}

    virtual UChar Next() = 0;
};
