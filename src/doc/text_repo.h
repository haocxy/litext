#pragma once

#include "core/sqlite.h"
#include "core/membuff.h"

#include "doc_part.h"


namespace doc
{

// thread safe
class TextRepo {
public:
    TextRepo(const fs::path &dbFile);

    ~TextRepo();

    // 保存一段文本
    class SavePartStmt {
    public:
        SavePartStmt(TextRepo &repo);
        
        void operator()(const DocPart &docPart);

    private:
        sqlite::Statement stmt_;
    };

private:
    bool shouldClearDb();

    void clearDb();

private:
    const fs::path &dbFile_;
    sqlite::Database db_;
};

}
