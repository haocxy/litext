#pragma once

#include "doc_define.h"


namespace doc
{

class DocPart {
public:
    DocPart() {}

    PartId id() const {
        return id_;
    }

    void setId(PartId id) {
        id_ = id;
    }

    const RowRange &rowRange() const {
        return rowRange_;
    }

    RowRange &rowRange() {
        return rowRange_;
    }

    void setRowRange(const RowRange &rowRange) {
        rowRange_ = rowRange;
    }

    const ByteRange &byteRange() const {
        return byteRange_;
    }

    ByteRange &byteRange() {
        return byteRange_;
    }

    void setByteRange(const ByteRange &byteRange) {
        byteRange_ = byteRange;
    }

private:
    PartId id_{ 0 };
    RowRange rowRange_;
    ByteRange byteRange_;
};

}
