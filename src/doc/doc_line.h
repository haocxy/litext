#pragma once

#include "doc_define.h"
#include "line_end_enum.h"

class Line
{
public:
    virtual ~Line() {}

    virtual CharN charCnt() const = 0;

    virtual UChar charAt(CharN i) const = 0;

    virtual LineEnd lineEnd() const = 0;
};
