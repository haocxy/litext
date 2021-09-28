#include "font_repo.h"

#include "core/system.h"
#include "core/font.h"
#include "core/time.h"
#include "core/logger.h"

namespace font
{

FontRepo::FontRepo(const fs::path &dbFile)
    : dbFile_(dbFile)
{
}

FontRepo::~FontRepo()
{

}

void FontRepo::test()
{
    loadFromDb();
}

void FontRepo::loadFromDb()
{
    fontDb_ = std::make_unique<FontDb>(dbFile_);
    fontUpdater_ = std::make_unique<FontUpdater>(*fontDb_);
}

}

