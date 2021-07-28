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
        stSavePart_.open(db_, "INSERT INTO doc VALUES(?,?,?,?);");
    }
    catch (const std::exception &e) {
        LOGE << "db exception: " << e.what();
        throw;
    }
}

TextRepo::~TextRepo()
{
}

i64 TextRepo::savePart(i64 off, const void *data, i64 nbytes)
{
    stSavePart_.reset();
    stSavePart_.arg(); // id
    stSavePart_.arg(off); // off
    stSavePart_.arg(data, nbytes); // data
    stSavePart_.arg(nbytes); // nbytes
    stSavePart_.step();
    return stSavePart_.lastInsertRowId();
}

bool TextRepo::shouldClearDb()
{
    return true;
}

void TextRepo::clearDb()
{
    std::ofstream ofs(dbFile_, std::ios::binary);
}

}
