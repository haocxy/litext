#pragma once

#include "doc_define.h"
#include "row_end_enum.h"

class Row
{
public:
    virtual ~Row() {}

    virtual CharN charCnt() const = 0;

    virtual UChar charAt(CharN i) const = 0;

    virtual RowEnd rowEnd() const = 0;
};
