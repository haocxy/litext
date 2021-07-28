#pragma once

#include <istream>

#include "core/charset.h"
#include "core/membuff.h"


namespace doc
{

void skipRow(Charset charset, std::istream &in, MemBuff &buff);

}
