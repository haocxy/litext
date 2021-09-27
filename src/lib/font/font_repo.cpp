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
    ElapsedTime elapse;

    elapse.start();

    fontDb_ = std::make_unique<FontDb>(dbFile_);

    updateDb();

    LOGE << "FontRepo::loadFromDb() use [" << elapse.ms() << " ms]";
}

static i64 lastWriteTime(const fs::path &path)
{
    return fs::last_write_time(path).time_since_epoch().count();
}

static FontInfo mkInfo(const FontFile &fontFile, const FontFace &fontFace)
{
    FontInfo info;
    info.setFilePath(fontFile.path());
    info.setFaceId(fontFace.faceIndex());
    info.setLastWriteTime(lastWriteTime(fontFile.path()));
    info.setFamily(fontFace.familyName());
    info.setIsScalable(fontFace.isScalable());
    info.setIsBold(fontFace.isBold());
    info.setIsItalic(fontFace.isItalic());
    return info;
}

void FontRepo::updateDb()
{

}

}

