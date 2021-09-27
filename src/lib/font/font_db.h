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

    void updateFontInfo(const FontInfo &info);

private:
    void removeInfoForOldFontFiles(const fs::path &fontFilePath, i64 lastWriteTime);

    void insertInfo(const FontInfo &info);

private:
    sqlite::Database db_;
    sqlite::Statement stmtRemoveInfoForOldFontFiles_;
    sqlite::Statement stmtInsertInfo_;
};

}

