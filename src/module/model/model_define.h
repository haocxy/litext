#pragma once

#include <stdint.h>
#include <string>

// uchar = unicode character
typedef int32_t UChar;
typedef std::basic_string<UChar> UString;

typedef int64_t LineN;
typedef int32_t CharN;

const UChar kDocEnd = 0;
