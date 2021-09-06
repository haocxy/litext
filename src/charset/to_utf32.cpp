#include "to_utf32.h"

#include <stdexcept>
#include <sstream>
#include <memory>

#include <QString>
#include <QTextCodec>

#include "core/uchar.h"


namespace charset
{

static std::u32string toUTF32ForAscii(const void *data, i64 nbytes)
{
    std::u32string s(nbytes, 0);
    const char *str = reinterpret_cast<const char *>(data);
    for (i64 i = 0; i < nbytes; ++i) {
        s[i] = str[i];
    }
    return s;
}

std::u32string toUTF32(Charset srcCharset, const void *data, i64 nbytes)
{
    // Qt库没有提供ASCII的解码器，所以使用Qt库解码时需要单独处理ASCII
    if (srcCharset == Charset::Ascii) {
        return toUTF32ForAscii(data, nbytes);
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
    return qs.toStdU32String();
}

}
