#pragma once

#pragma once

#include "doc_define.h"

class DocLine
{
public:
    virtual ~DocLine() {}

    virtual CharN charCnt() const = 0;

    virtual UChar charAt(CharN i) const = 0;
};
