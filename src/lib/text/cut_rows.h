#pragma once

#include <vector>
#include <string>
#include <string_view>

#include "utf16row.h"


namespace text
{



std::vector<std::u32string> cutRows(const std::u32string &s);


std::pmr::vector<UTF16Row> cutRows(const std::u16string_view &s, std::pmr::memory_resource *memres);

}
