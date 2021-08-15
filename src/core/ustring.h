#pragma once

#include <string>
#include <string_view>

#include "uchar.h"


typedef std::basic_string<UChar> UString;

typedef std::basic_string_view<UChar> UStringView;


namespace UStringUtil
{

UString utf16ToUString(const void *data, size_t nbytes);

}
