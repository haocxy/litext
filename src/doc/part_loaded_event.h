#pragma once

#include <cstdint>

#include "doc_define.h"


namespace doc
{

// 文档加载的比例，各属性单位都是字节
class PartLoadedEvent {
public:
    PartLoadedEvent() {}

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

private:
    int64_t partId_ = 0;
    uintmax_t fileSize_ = 0;
    uintmax_t partOffset_ = 0;
    uintmax_t partSize_ = 0;
    
};


}
