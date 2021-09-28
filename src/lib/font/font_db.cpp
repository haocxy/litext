#include "font_db.h"

#include "asset.font_db_init.sql.h"


namespace font
{



FontDb::FontDb(const fs::path &dbFile)
    : db_(dbFile)
{
    db_.exec(reinterpret_cast<const char *>(Asset::Data::font_db_init__sql));
}

FontDb::~FontDb()
{
}

FontDb::StmtForEachFontFile::StmtForEachFontFile(FontDb &db)
    : stmt_(db.db_, "SELECT file_path FROM font_files;")
{
}

void FontDb::StmtForEachFontFile::operator()(std::function<void(const fs::path &p)> &&action)
{
    stmt_.reset();

    while (stmt_.nextRow()) {
        fs::path p;
        stmt_.getValue(0, p);
        action(p);
    }
}

FontDb::StmtLastWriteTimeOf::StmtLastWriteTimeOf(FontDb &db)
    : stmt_(db.db_, "SELECT last_write_time FROM font_files WHERE file_path = ?;")
{}

opt<i64> FontDb::StmtLastWriteTimeOf::operator()(const fs::path &fontFile)
{
    stmt_.reset();

    stmt_.arg(fontFile);

    if (stmt_.nextRow()) {
        i64 result = 0;
        stmt_.getValue(0, result);
        return result;
    } else {
        return std::nullopt;
    }
}

FontDb::StmtInsertFile::StmtInsertFile(FontDb &db)
    : stmt_(db.db_, "INSERT INTO font_files VALUES(?,?);")
{}

void FontDb::StmtInsertFile::operator()(const fs::path &fontFile, i64 writeTime)
{
    stmt_.reset();
    stmt_.arg(fontFile).arg(writeTime);
    stmt_.step();
}

FontDb::StmtInsertFace::StmtInsertFace(FontDb &db)
    : stmt_(db.db_, "INSERT INTO font_faces VALUES(?,?,?,?,?,?);")
{
}

void FontDb::StmtInsertFace::operator()(const FaceInfo &info)
{
    stmt_.reset();
    stmt_.arg(info.filePath());
    stmt_.arg(info.faceId());
    stmt_.arg(info.family());
    stmt_.arg(info.isScalable());
    stmt_.arg(info.isBold());
    stmt_.arg(info.isItalic());
    stmt_.step();
}

FontDb::StmtDeleteFile::StmtDeleteFile(FontDb &db)
    : stmt_(db.db_, "DELETE FROM font_files WHERE file_path = ?;")
{

}

void FontDb::StmtDeleteFile::operator()(const fs::path &fontFile)
{
    stmt_.reset();
    stmt_.arg(fontFile);
    stmt_.step();
}

FontDb::StmtDeleteFaces::StmtDeleteFaces(FontDb &db)
    : stmt_(db.db_, "DELETE FROM font_faces WHERE file_path = ?;")
{
}

void FontDb::StmtDeleteFaces::operator()(const fs::path &fontFile)
{
    stmt_.reset();
    stmt_.arg(fontFile);
    stmt_.step();
}

}
