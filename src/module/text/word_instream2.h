#pragma once

#include "common/common_define.h"

class WordInStream2
{
public:
    virtual ~WordInStream2() {}

    virtual UString Next() = 0;
};
