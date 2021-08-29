#pragma once

#include <string>
#include <string_view>

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

    void set(const std::string_view &key, const std::u32string &val);

    void set(const std::string_view &key, int val);

    void set(const std::string_view &key, long val);

    void set(const std::string_view &key, long long val);

    bool get(const std::string_view &key, int &to);

    bool get(const std::string_view &key, long &to);

    bool get(const std::string_view &key, long long &to);

    bool get(const std::string_view &key, std::u32string &to);

private:
    sqlite::Statement stmtSelect_;
    sqlite::Statement stmtInsert_;
    sqlite::Statement stmtUpdate_;
};

}
