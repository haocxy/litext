#include "font_db.h"

#include "asset.font_db_init.sql.h"


namespace font
{



FontDb::FontDb(const fs::path &dbFile)
    : db_(dbFile)
{
    db_.exec(reinterpret_cast<const char *>(Asset::Data::font_db_init__sql));

    stmtSelectLastWriteTimeByPath_.open(db_,
        "SELECT last_write_time FROM font_files WHERE file_path = ?;");

    stmtInsertFontFile_.open(db_,
        "INSERT INTO font_files VALUES(?,?);");

    stmtInsertFontFace_.open(db_,
        "INSERT INTO font_faces VALUES(?,?,?,?,?,?);");

    stmtDeleteFontFile_.open(db_,
        "DELETE FROM font_files WHERE file_path = ?;");

    stmtDeleteFontFaces_.open(db_,
        "DELETE FROM font_faces WHERE file_path = ?;");
}

FontDb::~FontDb()
{
}

opt<i64> FontDb::lastWriteTimeOf(const fs::path &fontFile)
{
    stmtSelectLastWriteTimeByPath_.reset();

    stmtSelectLastWriteTimeByPath_.arg(fontFile);

    if (stmtSelectLastWriteTimeByPath_.nextRow()) {
        i64 result = 0;
        stmtSelectLastWriteTimeByPath_.getValue(0, result);
        return result;
    } else {
        return std::nullopt;
    }
}

void FontDb::insertFile(const fs::path &fontFile, i64 writeTime)
{
    stmtInsertFontFile_.reset();
    stmtInsertFontFile_.arg(fontFile).arg(writeTime);
    stmtInsertFontFile_.step();
}

void FontDb::insertFace(const FaceInfo &info)
{
    stmtInsertFontFace_.reset();
    stmtInsertFontFace_.arg(info.filePath());
    stmtInsertFontFace_.arg(info.faceId());
    stmtInsertFontFace_.arg(info.family());
    stmtInsertFontFace_.arg(info.isScalable());
    stmtInsertFontFace_.arg(info.isBold());
    stmtInsertFontFace_.arg(info.isItalic());
    stmtInsertFontFace_.step();
}

void FontDb::removeFile(const fs::path &fontFile)
{
    stmtDeleteFontFile_.reset();
    stmtDeleteFontFile_.arg(fontFile);
    stmtDeleteFontFile_.step();
}

void FontDb::removeFaces(const fs::path &fontFile)
{
    stmtDeleteFontFaces_.reset();
    stmtDeleteFontFaces_.arg(fontFile);
    stmtDeleteFontFaces_.step();
}


}
