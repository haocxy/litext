#pragma once

#include "core/basetype.h"
#include "core/fs.h"

#include "font_db.h"


namespace font
{

class FontRepo {
public:
    FontRepo(const fs::path &dbFile);

    ~FontRepo();

    void test();

private:
    void loadFromDb();

    void updateDb();

private:
    const fs::path dbFile_;
    uptr<FontDb> fontDb_;
};

}
