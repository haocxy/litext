#pragma once

#include <cstdint>
#include <string>
#include "fs.h"

#include <sstream>
#include <stdexcept>
#include <freetype/freetype.h>


namespace font
{

// 字体上下文，在不同上下文中的各种字体类的对象无关
// 
// 把这个类明确地体现出来，而不是作为隐式的全局对象，是为了提高多线程的性能
// 通过使用不同的上下文，不同线程可以安全而快速地使用各种字体类的对象
class FontContext {
public:
    FontContext() {
        const FT_Error error = FT_Init_FreeType(&ftLib_);
        if (error != 0) {
            std::ostringstream ss;
            ss << "FT_Init_FreeType() failed with error [" << error << "]";
            throw std::runtime_error(ss.str());
        }
    }

    ~FontContext() {
        if (ftLib_) {
            FT_Done_FreeType(ftLib_);
            ftLib_ = nullptr;
        }
    }

private:
    FT_Library ftLib_ = nullptr;

    friend class FontFile;
    friend class FontFace;
};


class FontFile {
public:
    FontFile(FontContext &context, const fs::path &path, bool loadToMemory = false);

    ~FontFile() {}

    bool isValid() const {
        return isValid_;
    }

    operator bool() const {
        return isValid_;
    }

    long faceCount() const {
        return faceCount_;
    }

private:
    FontContext &context_;

    fs::path path_;

    // 如果选择把字体载入内存，则把字体文件的内容存于此处
    // 如果不把字体载入内存，这个容器的长度为 0
    std::basic_string<unsigned char> data_;

    bool isValid_ = false;

    long faceCount_ = 0;

    friend class FontFace;
};

class FontFace {
public:
    FontFace(const FontFile &file, long faceIndex);

    ~FontFace() {
        if (ftFace_) {
            FT_Done_Face(ftFace_);
            ftFace_ = nullptr;
        }
    }

    bool isValid() const {
        return ftFace_ != nullptr;
    }

    operator bool() const {
        return isValid();
    }

    const char *familyName() const {
        return strOrEmpty(ftFace_->family_name);
    }

    const char *styleName() const {
        return strOrEmpty(ftFace_->style_name);
    }

    bool isScalable() const {
        return hasFlag(ftFace_->face_flags, FT_FACE_FLAG_SCALABLE);
    }

    bool isItalic() const {
        return hasFlag(ftFace_->style_flags, FT_STYLE_FLAG_ITALIC);
    }

    bool isBold() const {
        return hasFlag(ftFace_->style_flags, FT_STYLE_FLAG_BOLD);
    }

private:
    static const char *strOrEmpty(const char *s) {
        if (s) {
            return s;
        } else {
            return "";
        }
    }

    template <typename T, typename U>
    static bool hasFlag(T bitset, U mask) {
        return (bitset & mask) == mask;
    }

private:
    const FontFile &file_;
    FT_Face ftFace_ = nullptr;
};

class FontMetrics {

};

}
