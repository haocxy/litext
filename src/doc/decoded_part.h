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

    uintmax_t fileSize() const {
        return fileSize_;
    }

    void setFileSize(uintmax_t fileSize) {
        fileSize_ = fileSize;
    }

    uintmax_t byteOffset() const {
        return byteOffset_;
    }

    void setByteOffset(uintmax_t byteOffset) {
        byteOffset_ = byteOffset;
    }

    uintmax_t partSize() const {
        return partSize_;
    }

    void setPartSize(uintmax_t partSize) {
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
    i64 fileSize_ = 0;
    i64 byteOffset_ = 0;
    i64 partSize_ = 0;
    bool isLast_ = false;
    std::shared_ptr<std::u32string> content_;
};


}
