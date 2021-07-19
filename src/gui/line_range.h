#pragma once

#include <cstddef>


namespace gui
{


class LineRange {
public:
    LineRange() {}

    LineRange(size_t offset, size_t length)
        : offset_(offset), length_(length) {}

    size_t offset() const {
        return offset_;
    }

    void setOffset(size_t offset) {
        offset_ = offset;
    }

    size_t length() const {
        return length_;
    }

    void setLength(size_t length) {
        length_ = length;
    }

private:
    size_t offset_ = 0;
    size_t length_ = 0;
};


}
