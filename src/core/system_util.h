#pragma once

#include <cstddef>
#include <vector>

#include "core/fs.h"


namespace SystemUtil
{

size_t pageSize();

int processorCount();

fs::path userHome();

std::vector<fs::path> fonts();

}
