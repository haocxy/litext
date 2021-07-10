#pragma once

#include <cstdint>

#include "core/fs.h"
#include "doc_define.h"


namespace doc::RowCountUtil
{


RowN quickRowCount(const fs::path &file, const std::string &charset, RowN rowOffset, size_t byteCountHint);


}
