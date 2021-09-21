#include "qtutil.h"

#include "doc/supported_charsets.h"



namespace gui::qt::util
{

QList<QString> toQList(const std::set<QString> &set)
{
    QStringList result;
    for (const QString &e : set) {
        result.push_back(e);
    }
    return result;
}

std::set<QString> supportedCharsetSet()
{
    std::set<QString> result;

    for (const Charset charset : doc::gSupportedCharsets) {
        if (charset != Charset::Ascii) {
            result.insert(CharsetUtil::charsetToStr(charset));
        }
    }

    return result;
}

}
