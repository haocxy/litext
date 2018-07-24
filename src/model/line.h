#pragma once

#pragma once

#include "model_define.h"

class Line
{
public:
    virtual ~Line() {}

    virtual CharN charCnt() const = 0;

    virtual UChar charAt(CharN i) const = 0;
};
