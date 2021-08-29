#include "to_utf16.h"

#include <stdexcept>
#include <sstream>
#include <memory>

#include <QString>
#include <QTextCodec>


namespace charset
{

static std::pmr::u16string toUTF16ForAscii(const void *data, i64 nbytes, std::pmr::memory_resource *memres)
{
    std::pmr::u16string s(nbytes, 0, memres);
    const char *str = reinterpret_cast<const char *>(data);
    for (i64 i = 0; i < nbytes; ++i) {
        s[i] = str[i];
    }
    return s;
}

std::pmr::u16string toUTF16(Charset srcCharset, const void *data, i64 nbytes, std::pmr::memory_resource *memres)
{
    // Qt库没有提供ASCII的解码器，所以使用Qt库解码时需要单独处理ASCII
    if (srcCharset == Charset::Ascii) {
        return toUTF16ForAscii(data, nbytes, memres);
    }

    QTextCodec *codec = QTextCodec::codecForName(CharsetUtil::charsetToStr(srcCharset));
    if (!codec) {
        std::ostringstream ss;
        ss << "unsupported srcCharset [" << CharsetUtil::charsetToStr(srcCharset) << "]";
        throw std::logic_error(ss.str());
    }

    uptr<QTextDecoder> decoder{ codec->makeDecoder() };
    if (!decoder) {
        std::ostringstream ss;
        ss << "cannot make decoder for srcCharset [" << CharsetUtil::charsetToStr(srcCharset) << "]";
        throw std::logic_error(ss.str());
    }

    QString qs = decoder->toUnicode(reinterpret_cast<const char *>(data), static_cast<int>(nbytes));
    return std::pmr::u16string(qs.toStdU16String(), memres);
}

}
