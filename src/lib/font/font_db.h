#pragma once

#include "core/fs.h"
#include "core/sqlite.h"

#include "font_info.h"


namespace font
{

class FontDb {
public:
    FontDb(const fs::path &dbFile);

    ~FontDb();

    opt<i64> lastWriteTimeOf(const fs::path &fontFile);

    void insertFile(const fs::path &fontFile, i64 writeTime);

    void insertFace(const FaceInfo &info);

    void removeFile(const fs::path &fontFile);

    void removeFaces(const fs::path &fontFile);

private:
    sqlite::Database db_;
    sqlite::Statement stmtSelectLastWriteTimeByPath_;
    sqlite::Statement stmtInsertFontFile_;
    sqlite::Statement stmtInsertFontFace_;
    sqlite::Statement stmtDeleteFontFile_;
    sqlite::Statement stmtDeleteFontFaces_;
};

}

