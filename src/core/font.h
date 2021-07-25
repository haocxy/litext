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

};

class FontMetrics {

};

}
