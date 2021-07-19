#pragma once

#include <cstdint>


namespace doc
{

// 文档加载的比例，各属性单位都是字节
class LoadProgress {
public:
    LoadProgress() {}

    LoadProgress(uintmax_t fileSize, uintmax_t partOffset, uintmax_t partSize)
        : fileSize_(fileSize), partOffset_(partOffset), partSize_(partSize) {}

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

    // 百分比，0到100的整数
    // 例如加载了 80% 就会返回整数 80
    int percent() const {
        const uintmax_t loadedSize = partOffset_ + partSize_;
        if (fileSize_ != 0) {
            if (loadedSize < fileSize_) {
                return static_cast<int>(loadedSize * 100 / fileSize_);
            } else {
                return 100;
            }
        } else {
            return 0;
        }
    }

private:
    uintmax_t fileSize_ = 0;
    uintmax_t partOffset_ = 0;
    uintmax_t partSize_ = 0;
    
};


}
