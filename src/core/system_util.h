#pragma once

#include <cstddef>

#include "core/fs.h"


namespace SystemUtil
{

size_t pageSize();

int processorCount();

fs::path userHome();

}
