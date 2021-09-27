#pragma once

#include "core/fs.h"
#include "core/sqlite.h"


namespace font
{

class FontDb {
public:
    FontDb(const fs::path &dbFile);

    ~FontDb();

    

private:
    sqlite::Database db_;
};

}

