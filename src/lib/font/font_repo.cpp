#include "font_repo.h"


namespace font
{

FontRepo::FontRepo(const fs::path &dbFile)
    : dbFile_(dbFile)
{
    loadFromDb(); // TODO for test
}

FontRepo::~FontRepo()
{

}

void FontRepo::loadFromDb()
{
    fontDb_ = std::make_unique<FontDb>(dbFile_);
}

}

