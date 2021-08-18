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

i64 TextRepo::SavePartStmt::operator()(i64 off, i64 nrows, const void *data, i64 nbytes)
{
    stmt_.reset();
    stmt_.arg(); // id
    stmt_.arg(off); // off
    stmt_.arg(nrows); // nrows
    stmt_.arg(data, nbytes); // data
    stmt_.step();
    return stmt_.lastInsertRowId();
}

TextRepo::SavePartStmt::SavePartStmt(TextRepo &repo)
{
    stmt_.open(repo.db_, "INSERT INTO doc VALUES(?,?,?,?);");
}

TextRepo::QueryPartDataByPartIdStmt::QueryPartDataByPartIdStmt(TextRepo &repo)
{
    stmt_.open(repo.db_, "SELECT data FROM doc WHERE id = ?");
}

TextRepo::QueryPartDataByPartIdStmt::Result TextRepo::QueryPartDataByPartIdStmt::operator()(i64 partId) const
{
    stmt_.reset();
    stmt_.arg(partId);

    Result result;

    if (stmt_.nextRow()) {
        MemBuff mem;
        stmt_.getValue(0, result.partData);
        return result;
    } else {
        return result;
    }
}

}
