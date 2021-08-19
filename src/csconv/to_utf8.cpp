#include "to_utf8.h"

#include <stdexcept>
#include <sstream>
#include <memory>

#include <QString>
#include <QTextCodec>


namespace csconv
{

std::string toUTF8(Charset srcCharset, const void *data, i32 nbytes)
{
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
    QByteArray ba = qs.toUtf8();
    return std::string(ba.constData(), ba.size());
}

}
