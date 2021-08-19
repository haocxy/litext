#pragma once

#include <string>

#include "core/charset.h"
#include "core/basetype.h"


namespace csconv
{

std::string toUTF8(Charset srcCharset, const void *data, i32 nbytes);

}

