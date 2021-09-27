#pragma once

#include "core/basetype.h"
#include "core/membuff.h"


namespace font
{

class FontDbItem {
public:
    FontDbItem() {}

    const MemBuff &filePath() const {
        return filePath_;
    }

    void setFilePath(const void *data, i64 nbytes) {
        filePath_.assign(data, nbytes);
    }

    i64 lastWriteTime() const {
        return lastWriteTime_;
    }

    void setLastWriteTime(i64 lastWriteTime) {
        lastWriteTime_ = lastWriteTime;
    }

    const std::string &family() const {
        return family_;
    }

    void setFamily(const std::string &family) {
        family_ = family;
    }

    bool isScalable() const {
        return isScalable_;
    }

    void setIsScalable(bool isScalable) {
        isScalable_ = isScalable;
    }

    bool isBold() const {
        return isBold_;
    }

    void setIsBold(bool isBold) {
        isBold_ = isBold;
    }

    bool isItalic() const {
        return isItalic_;
    }

    void setIsItalic(bool isItalic) {
        isItalic_ = isItalic;
    }

private:
    MemBuff filePath_;
    i64 lastWriteTime_ = 0;
    std::string family_;
    bool isScalable_ = false;
    bool isBold_ = false;
    bool isItalic_ = false;
};

}
