#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "doc_define.h"


namespace doc
{

class DecodedPart {
public:
    DecodedPart()
        : content_(std::make_shared<std::u32string>()) {}

    i64 byteOffset() const {
        return byteOffset_;
    }

    void setByteOffset(i64 byteOffset) {
        byteOffset_ = byteOffset;
    }

    i64 partSize() const {
        return partSize_;
    }

    void setPartSize(i64 partSize) {
        partSize_ = partSize;
    }

    bool isLast() const {
        return isLast_;
    }

    void setIsLast(bool isLast) {
        isLast_ = isLast;
    }

    const std::u32string &utf32content() const {
        return *content_;
    }

    void setContent(std::u32string &&content) {
        *content_ = std::move(content);
    }

private:
    i64 byteOffset_ = 0;
    i64 partSize_ = 0;
    bool isLast_ = false;
    sptr<std::u32string> content_;
};


}
