#pragma once

#include <string>

#include "core/charset.h"
#include "core/basetype.h"


namespace charset
{

std::string toUTF8(Charset srcCharset, const void *data, i32 nbytes);

std::string toUTF8(const std::u32string &s);

}

