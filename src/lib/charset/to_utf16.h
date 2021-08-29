#pragma once

#include <string>

#include "core/basetype.h"
#include "core/charset.h"


namespace charset
{

std::u16string toUTF16(Charset srcCharset, const void *data, i64 nbytes);

}
