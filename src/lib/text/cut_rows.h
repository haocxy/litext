#pragma once

#include <vector>
#include <string>
#include <string_view>

#include "core/scoped_container.h"
#include "utf16row.h"


namespace text
{

scc::vector<UTF16Row> cutRows(const scc::u16string &s);

}
