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

TextRepo::SavePartStmt TextRepo::stmtSavePart()
{
    return SavePartStmt(db_);
}

bool TextRepo::shouldClearDb()
{
    return true;
}

void TextRepo::clearDb()
{
    std::ofstream ofs(dbFile_, std::ios::binary);
}

i64 TextRepo::SavePartStmt::operator()(i64 off, const void *data, i64 nbytes)
{
    stmt_.reset();
    stmt_.arg(); // id
    stmt_.arg(off); // off
    stmt_.arg(data, nbytes); // data
    stmt_.arg(nbytes); // nbytes
    stmt_.step();
    return stmt_.lastInsertRowId();
}

TextRepo::SavePartStmt::SavePartStmt(sqlite::Database &db)
{
    stmt_.open(db, "INSERT INTO doc VALUES(?,?,?,?);");
}

}