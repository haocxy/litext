#pragma once

#include "module/common/common_define.h"

class CharInStream2
{
public:
    virtual ~CharInStream2() {}

    virtual UChar Next() = 0;
};
