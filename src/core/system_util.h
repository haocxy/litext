#pragma once

#include <cstddef>
#include <vector>
#include <string>

#include "core/fs.h"


namespace SystemUtil
{

size_t pageSize();

int processorCount();

fs::path userHome();

struct FontInfo {
    std::string family;
    std::string style;
    fs::path file;
};

std::vector<FontInfo> fonts();

}
