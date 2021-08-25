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

    void set(const std::string_view &key, const std::string &val);

    void set(const std::string_view &key, i32 val);

    void set(const std::string_view &key, i64 val);

    bool get(const std::string_view &key, i32 &to);

    bool get(const std::string_view &key, i64 &to);

private:
    sqlite::Statement stmtSelect_;
    sqlite::Statement stmtInsert_;
    sqlite::Statement stmtUpdate_;
};

}
