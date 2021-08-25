#pragma once

#include <string>

#include "core/sqlite.h"


namespace gui
{

class PropDb {
public:
    PropDb();

private:
    sqlite::Database db_;

    friend class PropRepo;
};

class PropRepo {
public:
    PropRepo(PropDb &db, const std::string &name);



private:
    sqlite::Statement stmtSelect_;
    sqlite::Statement stmtInsert_;
    sqlite::Statement stmtUpdate_;
};

}
