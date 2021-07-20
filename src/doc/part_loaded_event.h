#pragma once

#include <cstdint>
#include <memory>

#include "core/membuff.h"
#include "doc_define.h"


namespace doc
{

// 文档加载的比例，各属性单位都是字节
class PartLoadedEvent {
public:
    PartLoadedEvent()
        : utf16content_(std::make_shared<MemBuff>()) {}

    int64_t partId() const {
        return partId_;
    }

    void setPartId(int64_t id) {
        partId_ = id;
    }

    uintmax_t fileSize() const {
        return fileSize_;
    }

    void setFileSize(uintmax_t fileSize) {
        fileSize_ = fileSize;
    }

    uintmax_t partOffset() const {
        return partOffset_;
    }

    void setPartOffset(uintmax_t partOffset) {
        partOffset_ = partOffset;
    }

    uintmax_t partSize() const {
        return partSize_;
    }

    void setPartSize(uintmax_t partSize) {
        partSize_ = partSize;
    }

    const MemBuff &utf16content() const {
        return *utf16content_;
    }

    MemBuff &utf16content() {
        return *utf16content_;
    }

private:
    int64_t partId_ = 0;
    uintmax_t fileSize_ = 0;
    uintmax_t partOffset_ = 0;
    uintmax_t partSize_ = 0;
    std::shared_ptr<MemBuff> utf16content_;
};


}
