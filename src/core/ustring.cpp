#include "ustring.h"

#include "text/utf16char_in_stream.h"


namespace UStringUtil
{



UString utf16ToUString(const void *data, size_t nbytes)
{
    UTF16CharInStream utf16Stream(data, nbytes);

    CharInStreamOverUTF16CharInStram ucharStream(utf16Stream);

    UString result;

    UChar ch = 0;

    while (ch = ucharStream.Next(), ch != 0) {
        result.push_back(ch);
    }

    return result;
}

}
