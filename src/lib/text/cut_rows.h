#pragma once

#include <vector>
#include <string>

#include "utf16row.h"


namespace text
{



std::vector<std::u32string> cutRows(const std::u32string &s);


std::vector<UTF16Row> cutRows(const std::u16string &s);

}
