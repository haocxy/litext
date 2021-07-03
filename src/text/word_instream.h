#pragma once

#include <QString>

#include "common/common_define.h"

class WordInStream
{
public:
    virtual ~WordInStream() {}

    virtual QString Next() = 0;
};
