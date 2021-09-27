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

    LOGI << "FontRepo::loadFromDb() use [" << elapse.ms() << " ms]";
}

static i64 lastWriteTimeOf(const fs::path &path)
{
    return fs::last_write_time(path).time_since_epoch().count();
}

static FaceInfo mkFaceInfo(const fs::path & fontFilePath, const FontFace &fontFace)
{
    FaceInfo info;
    info.setFilePath(fontFilePath);
    info.setFaceId(fontFace.faceIndex());
    info.setFamily(fontFace.familyName());
    info.setIsScalable(fontFace.isScalable());
    info.setIsBold(fontFace.isBold());
    info.setIsItalic(fontFace.isItalic());
    return info;
}

void FontRepo::updateDb()
{
    const std::vector<fs::path> fileVec = SystemUtil::fonts();

    for (const fs::path &file : fileVec) {
        updateByFontFile(file);
    }
}

void FontRepo::updateByFontFile(const fs::path &fontFilePath)
{
    const char *title = "FontRepo::updateByFontFile ";

    const opt<i64> writeTimeInDb = fontDb_->lastWriteTimeOf(fontFilePath);
    const i64 writeTimeInFs = lastWriteTimeOf(fontFilePath);
    if (writeTimeInDb) {
        if (*writeTimeInDb == writeTimeInFs) {
            LOGD << title << "font file [" << fontFilePath << "] is not changed, skip it";
            return;
        } else {
            LOGI << title << "font file [" << fontFilePath << "] is changed";

            // font_faces 表的 file_path 为引用了 font_file 中 file_path 的外键,
            // 所以必须先删除 font_faces 表中的对应数据
            fontDb_->removeFaces(fontFilePath);
            fontDb_->removeFile(fontFilePath);
        }
    } else {
        LOGI << title << "font file [" << fontFilePath << "] is new";
    }

    FontContext context;

    FontFile fontFile(context, fontFilePath);
    if (!fontFile.isValid()) {
        return;
    }

    const i64 faceCount = fontFile.faceCount();
    if (faceCount <= 0) {
        return;
    }

    fontDb_->insertFile(fontFilePath, writeTimeInFs);

    for (i64 faceIndex = 0; faceIndex < faceCount; ++faceIndex) {
        const FontFace fontFace(fontFile, faceIndex);
        if (fontFace.isValid()) {
            FaceInfo info = mkFaceInfo(fontFilePath, fontFace);
            fontDb_->insertFace(info);
        }
    }
}

}

