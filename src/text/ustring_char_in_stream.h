#pragma once

#include <string>

#include "char_instream.h"


namespace gui
{

class UStringCharInStream : public CharInStream {
public:
    UStringCharInStream(const std::u32string_view &str)
        : str_(str) {}

    virtual ~UStringCharInStream() {}

    virtual char32_t Next() override {
        if (offset_ < str_.size()) {
            return str_[offset_++];
        } else {
            return 0;
        }
    }

private:
    const std::u32string_view &str_;
    size_t offset_ = 0;
};

}
