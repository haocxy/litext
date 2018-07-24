#pragma once

#include <string>

// uchar = unicode character
typedef wchar_t UChar;
typedef std::basic_string<UChar> UString;

namespace check
{
    inline bool isNull(const UString &s) { return s.empty(); }
}
