#include "ustring.h"

#include <QString>


namespace UStringUtil
{

static UString toUString(const QString &qstr)
{
    const int u16count = qstr.length();

    UString result;
    result.reserve(u16count);

    char16_t high = 0;
    bool highFound = false;

    for (int i = 0; i < u16count; ++i) {
        const char16_t u16ch = qstr[i].unicode();
        if (!highFound) {
            if (!UCharUtil::isSurrogate(u16ch)) {
                result.push_back(u16ch);
            } else if (UCharUtil::isHighSurrogate(u16ch)) {
                high = u16ch;
                highFound = true;
            } else { // if (UCharUtil::isLowSurrogate(u16ch))
                // do nothing
            }
        } else { // if (highFound)
            if (UCharUtil::isLowSurrogate(u16ch)) {
                result.push_back(UCharUtil::u16SurrogatePairToUnicode(high, u16ch));
            } else if (!UCharUtil::isSurrogate(u16ch)) {
                result.push_back(u16ch);
            } else {
                // do nothing
            }
            high = 0;
            highFound = false;
        }
    }

    return result;
}


}
