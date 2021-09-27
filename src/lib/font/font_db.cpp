#include "font_db.h"

#include "asset.font_db_init.sql.h"


namespace font
{



FontDb::FontDb(const fs::path &dbFile)
    : db_(dbFile)
{
    db_.exec(reinterpret_cast<const char *>(Asset::Data::font_db_init__sql));

    stmtRemoveInfoForOldFontFiles_.open(db_,
        "DELETE FROM fonts WHERE file_path = ? AND last_write_time < ?;");

    stmtInsertInfo_.open(db_,
        "INSERT INTO fonts VALUES(?,?,?,?,?,?,?);");
}

FontDb::~FontDb()
{
}

void FontDb::updateFontInfo(const FontInfo &info)
{
    removeInfoForOldFontFiles(info.filePath(), info.lastWriteTime());

    insertInfo(info);
}

void FontDb::removeInfoForOldFontFiles(const fs::path &fontFilePath, i64 lastWriteTime)
{
    stmtRemoveInfoForOldFontFiles_.reset();

    stmtRemoveInfoForOldFontFiles_.arg(fontFilePath).arg(lastWriteTime);

    stmtRemoveInfoForOldFontFiles_.step();
}

void FontDb::insertInfo(const FontInfo &info)
{
    stmtInsertInfo_.reset();

    stmtInsertInfo_.arg(info.filePath());
    stmtInsertInfo_.arg(info.faceId());
    stmtInsertInfo_.arg(info.lastWriteTime());
    stmtInsertInfo_.arg(info.family());
    stmtInsertInfo_.arg(info.isScalable());
    stmtInsertInfo_.arg(info.isBold());
    stmtInsertInfo_.arg(info.isItalic());

    stmtInsertInfo_.step();
}

}
