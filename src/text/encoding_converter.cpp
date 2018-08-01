#include "encoding_converter.h"


#include <sstream>
#include "third_party/libiconv-win-build-master/include/iconv.h"

UString encoding_converter::gbkToUnicode(const std::string & s)
{
    iconv_t p = iconv_open("ucs-2-internal", "utf-8");
    if (p == (iconv_t)(-1))
    {
        throw std::runtime_error("unsupported charset");
    }

    std::istringstream is(s);

    is.seekg(0, is.end);
    size_t length = is.tellg();
    is.seekg(0, is.beg);
    char *readbuf = new char[length];

    is.read(readbuf, length);

    char writebuf[4096] = {};

    const char *pi = readbuf;
    char *po = writebuf;
    size_t inbytesleft = length;
    size_t outbytesleft = 4096;

    size_t converted = iconv(p, &pi, &inbytesleft, &po, &outbytesleft);

    intmax_t result = converted;
    int delta = 4096 - outbytesleft;
    int resultCharCnt = delta / 2;

    UString resultStr;

    for (int i = 0; i < resultCharCnt; ++i)
    {
        uint16_t c = 0;
        memcpy(&c, &writebuf[i * 2], 2);

        UChar uc = c;

        resultStr.push_back(uc);
    }

    return resultStr;
}
