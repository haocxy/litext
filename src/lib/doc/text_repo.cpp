#include "text_repo.h"

#include <fstream>

#include "core/logger.h"
#include "sql/asset.prepare_db.sql.h"


namespace doc
{

TextRepo::TextRepo(const fs::path &dbFile)
    : dbFile_(dbFile)
{
    if (shouldClearDb()) {
        clearDb();
    }

    try {
        db_.open(dbFile);
        db_.exec(reinterpret_cast<const char *>
            (Asset::Data::prepare_db__sql));
    }
    catch (const std::exception &e) {
        LOGE << "db exception: " << e.what();
        throw;
    }
}

TextRepo::~TextRepo()
{
}

bool TextRepo::shouldClearDb()
{
    return true;
}

void TextRepo::clearDb()
{
    std::ofstream ofs(dbFile_, std::ios::binary);
}

void TextRepo::SavePartStmt::operator()(const DocPart &docPart)
{
    stmt_.reset();
    stmt_.arg(docPart.id()); // id
    stmt_.arg(docPart.rowRange().off()); // rowoff
    stmt_.arg(docPart.rowRange().count()); // rowcnt
    stmt_.arg(docPart.byteRange().off()); // byteoff
    stmt_.arg(docPart.byteRange().count()); // bytecnt
    stmt_.step();
}

TextRepo::SavePartStmt::SavePartStmt(TextRepo &repo)
{
    stmt_.open(repo.db_, "INSERT INTO doc VALUES(?,?,?,?,?);");
}

}
