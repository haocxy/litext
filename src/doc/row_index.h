#pragma once

#include "core/range.h"

#include "doc_define.h"


namespace doc
{

class RowIndex {
public:
    RowIndex() {}

    RowIndex(PartId partId, const Range<i64> &byteRange)
        : partId_(partId), byteRange_(byteRange) {}

    PartId partId() const {
        return partId_;
    }

    Range<i64> byteRange() const {
        return byteRange_;
    }

private:
    PartId partId_ = 0;
    Range<i64> byteRange_;
};

}
