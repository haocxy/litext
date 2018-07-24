#pragma once

#include "common/common_define.h"

class WordInStream
{
public:
    virtual ~WordInStream() {}

    virtual UString Next() = 0;
};
