#pragma once

#include <istream>

#include "core/charset.h"
#include "core/membuff.h"


namespace doc
{

bool skipRow(Charset charset, std::istream &in, MemBuff &buff, i32 bytesLimit);

}
