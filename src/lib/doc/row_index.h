#pragma once

#include "core/range.h"

#include "doc_define.h"


namespace doc
{

class RowIndex {
public:
    RowIndex() {}

    RowIndex(PartId partId, RowN rowOff)
        : partId_(partId), rowOff_(rowOff) {}

    PartId partId() const {
        return partId_;
    }

    RowN rowOff() const {
        return rowOff_;
    }

private:
    PartId partId_ = 0;
    RowN rowOff_ = 0;
};

}
