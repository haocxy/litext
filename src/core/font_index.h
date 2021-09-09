#pragma once

#include "fs.h"
#include "basetype.h"


namespace font
{

class FontIndex {
public:
    FontIndex() {}

    FontIndex(const u32str &file, long faceIndex)
        : file_(static_cast<const std::u32string &>(file)), faceIndex_(faceIndex) {}

    FontIndex(const fs::path &file, long faceIndex)
        : file_(file), faceIndex_(faceIndex) {}

    FontIndex(const FontIndex &b) {
        copy(*this, b);
    }

    FontIndex(FontIndex &&b) noexcept {
        move(*this, b);
    }

    FontIndex &operator=(const FontIndex &b) {
        copy(*this, b);
        return *this;
    }

    FontIndex &operator=(FontIndex &&b) noexcept {
        move(*this, b);
        return *this;
    }

    bool isValid() const {
        return !file_.empty();
    }

    operator bool() const {
        return isValid();
    }

    const fs::path &file() const {
        return file_;
    }

    void setFile(const fs::path &file) {
        file_ = file;
    }

    long faceIndex() const {
        return faceIndex_;
    }

    void setFaceIndex(long faceIndex) {
        faceIndex_ = faceIndex;
    }

private:
    static void copy(FontIndex &to, const FontIndex &from) {
        if (&to != &from) {
            to.file_ = from.file_;
            to.faceIndex_ = from.faceIndex_;
        }
    }

    static void move(FontIndex &to, FontIndex &from) {
        if (&to != &from) {
            to.file_ = std::move(from.file_);
            to.faceIndex_ = from.faceIndex_;
            from.faceIndex_ = 0;
        }
    }

private:
    fs::path file_;
    long faceIndex_ = 0;
};


}
