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

    FontContext(const FontContext &) = delete;

    FontContext(FontContext &&b) noexcept {
        move(*this, b);
    }

    FontContext &operator=(const FontContext &) = delete;

    FontContext &operator=(FontContext &&b) noexcept {
        move(*this, b);
        return *this;
    }

    ~FontContext() {
        close();
    }

private:
    static void move(FontContext &to, FontContext &from) {
        if (&to != &from) {
            to.close();
            to.ftLib_ = from.ftLib_;
            from.ftLib_ = nullptr;
        }
    }

    void close() {
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
    FontFile() {}

    FontFile(const FontContext &context, const fs::path &path, bool loadToMemory = false);

    FontFile(const FontFile &) = delete;

    FontFile(FontFile &b) noexcept {
        move(*this, b);
    }

    FontFile &operator=(const FontFile &) = delete;

    FontFile &operator=(FontFile &&b) noexcept {
        move(*this, b);
        return *this;
    }

    ~FontFile() {}

    bool isValid() const {
        return isValid_;
    }

    operator bool() const {
        return isValid_;
    }

    const fs::path &path() const {
        return path_;
    }

    long faceCount() const {
        return faceCount_;
    }

private:
    static void move(FontFile &to, FontFile &from) {
        if (&to != &from) {
            to.context_ = from.context_;
            from.context_ = nullptr;

            to.path_ = std::move(from.path_);

            to.data_ = std::move(from.data_);

            to.isValid_ = from.isValid_;
            from.isValid_ = false;

            to.faceCount_ = from.faceCount_;
            from.faceCount_ = 0;
        }
    }

private:
    const FontContext *context_ = nullptr;

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
    FontFace() {}

    FontFace(const FontFile &file, long faceIndex);

    FontFace(const FontFace &) = delete;

    FontFace(FontFace &&b) noexcept {
        move(*this, b);
    }

    FontFace &operator=(const FontFace &) = delete;

    FontFace &operator=(FontFace &&b) noexcept {
        move(*this, b);
        return *this;
    }

    ~FontFace() {
        close();
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

    void setPointSize(int pt);

    int64_t mapUnicodeToGlyphIndex(char32_t unicode) const;

    void loadGlyph(int64_t glyphIndex);

    void renderGlyph();

    struct BitmapInfo {
        unsigned char *buffer = nullptr;
        unsigned int width = 0;
        unsigned int rows = 0;
        unsigned int pitch = 0;
    };

    BitmapInfo bitmapInfo() const {
        const FT_Bitmap &bm = ftFace_->glyph->bitmap;
        BitmapInfo i;
        i.buffer = bm.buffer;
        i.width = bm.width;
        i.rows = bm.rows;
        i.pitch = bm.pitch;
        return i;
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

    void close() {
        if (ftFace_) {
            FT_Done_Face(ftFace_);
            ftFace_ = nullptr;
        }
    }

    static void move(FontFace &to, FontFace &from) {
        if (&to != &from) {
            to.close();
            to.file_ = from.file_;
            from.file_ = nullptr;
            to.ftFace_ = from.ftFace_;
            from.ftFace_ = nullptr;
        }
    }



private:
    const FontFile *file_ = nullptr;
    FT_Face ftFace_ = nullptr;
};

class FontMetrics {

};

}
