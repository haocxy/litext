#pragma once

#include <string>
#include <sstream>


enum class Charset {
	Unknown = 0,
	Ascii,
	UTF_8,
    UTF_16,
	GB18030,
};


namespace CharsetUtil
{

Charset strToCharset(const std::string &s);

const char *charsetToStr(Charset charset);

}

// 如果把stream的类型用模板定义，虽然可以正确编译，但某些IDE的静态检查会报错。
// 考虑到实际上也只会有std::ostringstream使用charset，就直接明确定义，改善开发体验。
inline std::ostringstream &operator<<(std::ostringstream &stream, Charset charset)
{
    stream << CharsetUtil::charsetToStr(charset);
    return stream;
}

struct uchardet;

class CharsetDetector {
public:
    CharsetDetector();

    ~CharsetDetector();

    CharsetDetector(const CharsetDetector &) = delete;

    CharsetDetector(CharsetDetector &&) = delete;

    CharsetDetector &operator=(const CharsetDetector &) = delete;

    CharsetDetector &operator=(CharsetDetector &&) = delete;

    bool feed(const void *data, size_t len);

    void end();

    void reset();

    const char *charset() const;

private:
    uchardet *handle_ = nullptr;
};
