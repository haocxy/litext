#pragma once

#include "core/fs.h"


namespace doc::dbfiles
{

fs::path docPathToDbPath(const fs::path &doc);

void removeUselessDbFiles();

}
