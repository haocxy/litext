#pragma once

#include "core/fs.h"
#include "core/basetype.h"


namespace font
{

class FaceInfo {
public:
    FaceInfo() {}

    const fs::path &filePath() const {
        return filePath_;
    }

    void setFilePath(const fs::path &filePath) {
        filePath_ = filePath;
    }

    i64 faceId() const {
        return faceId_;
    }

    void setFaceId(i64 faceId) {
        faceId_ = faceId;
    }

    const u8str &family() const {
        return family_;
    }

    void setFamily(const u8str &family) {
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
    fs::path filePath_;
    i64 faceId_ = 0;
    u8str family_;
    bool isScalable_ = false;
    bool isBold_ = false;
    bool isItalic_ = false;
};

}
