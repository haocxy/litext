#include "font_repo.h"

#include "core/system.h"
#include "core/font.h"


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

    updateDb();
}

static FontInfo mkInfo(const FontFile &fontFile, const FontFace &fontFace)
{
    FontInfo info;
    info.setFilePath(fontFile.path());
    info.setFaceId(fontFace.faceIndex());
    const i64 lastWriteTime = fs::last_write_time(fontFile.path()).time_since_epoch().count();
    info.setLastWriteTime(lastWriteTime);
    info.setFamily(fontFace.familyName());
    info.setIsScalable(fontFace.isScalable());
    info.setIsBold(fontFace.isBold());
    info.setIsItalic(fontFace.isItalic());
    return info;
}

void FontRepo::updateDb()
{
    font::FontContext context;

    const std::vector<fs::path> fontFilePaths = SystemUtil::fonts();

    for (const fs::path &fontFilePath : fontFilePaths) {
        FontFile fontFile(context, fontFilePath);
        if (fontFile.isValid()) {
            const i64 faceCount = fontFile.faceCount();
            for (i64 faceIndex = 0; faceIndex < faceCount; ++faceIndex) {
                FontFace fontFace(fontFile, faceIndex);
                if (fontFace.isValid()) {
                    if (fontFile.path() == "C:\\Windows\\Fonts\\AGENCYR.TTF") {
                        int n = 0;
                    }
                    FontInfo info = mkInfo(fontFile, fontFace);
                    try {
                        fontDb_->updateFontInfo(info);
                    } catch (const std::exception &e) {
                        std::string msg = e.what();
                        throw;
                    }
                }
            }
        }
    }
}

}

