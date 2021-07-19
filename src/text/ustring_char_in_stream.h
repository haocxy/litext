#pragma once

#include "core/ustring.h"

#include "char_instream.h"


namespace gui
{

class UStringCharInStream : public CharInStream {
public:
    UStringCharInStream(const UStringView &str)
        : str_(str) {}

    virtual ~UStringCharInStream() {}

    virtual UChar Next() override {
        if (offset_ < str_.size()) {
            return str_[offset_++];
        } else {
            return 0;
        }
    }

private:
    const UStringView &str_;
    size_t offset_ = 0;
};

}
