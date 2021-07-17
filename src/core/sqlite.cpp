#include "sqlite.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include <third/sqlite3/sqlite3.h>

#include "core/logger.h"


namespace
{

class Sqlite3ErrorMsgDeleter {
public:
    Sqlite3ErrorMsgDeleter(char *&errmsg)
        : errmsg_(errmsg)
    {
        errmsg_ = nullptr;
    }

    ~Sqlite3ErrorMsgDeleter() {
        if (errmsg_) {
            sqlite3_free(errmsg_);
            errmsg_ = nullptr;
        }
    }

private:
    char *&errmsg_;
};

}


namespace sqlite
{

Database::Database()
{
}

Database::Database(const fs::path &path)
{
    open(path);
}

Database::~Database()
{
    try {
        if (isOpened()) {
            close();
        }
    }
    catch (const std::exception &e) {
        LOGE << "exception in sqlite3ns::Database::~Database(): " << e.what();
    }
    catch (...) {
        LOGE << "unknown exception in sqlite3ns::Database::~Database()";
    }
}

bool Database::isOpened() const
{
    return db_;
}

void Database::open(const fs::path &path)
{
    if (isOpened()) {
        close();
    }

    if (!fs::exists(path.parent_path())) {
        try {
            fs::create_directories(path.parent_path());
        }
        catch (const fs::filesystem_error &e) {
            std::ostringstream ss;
            ss << "cannot create directories for [" << path.parent_path().generic_string() << "] ";
            ss << "because [" << e.what() << "]";
            throw std::logic_error(ss.str());
        }
    }
    {
        // 确保数据库文件存在
        std::ofstream ofs(path, std::ios::binary | std::ios::app);
    }
    sqlite3 *db = nullptr;
    int n = sqlite3_open(path.generic_string().c_str(), &db);
    if (n != SQLITE_OK) {
        std::ostringstream ss;
        ss << "cannot open SQLite3 database by file path: [" << path.generic_string() << "] ";
        ss << "because [" << sqlite3_errstr(n) << "]";
        throw std::logic_error(ss.str());
    }

    path_ = path;
    db_ = db;
}

void Database::close()
{
    assertOpened();

    int n = sqlite3_close(db_);
    if (n != SQLITE_OK) {
        std::ostringstream ss;
        ss << "cannot close SQLite3 database on file [" << path_.generic_string() << "] ";
        ss << "because [" << sqlite3_errstr(n) << "]";
        throw std::logic_error(ss.str());
    }

    path_.clear();
    db_ = nullptr;
}

void Database::exec(const std::string &sql)
{
    assertOpened();

    char *errmsg = nullptr;
    Sqlite3ErrorMsgDeleter errmsgDeleter(errmsg);

    sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &errmsg);

    if (errmsg) {
        std::ostringstream ss;
        ss << "sqlite3ns::Database::exec() error: [" << errmsg << "]";
        throw std::logic_error(ss.str());
    }
}

void Database::assertOpened() const
{
    if (!isOpened()) {
        throw std::logic_error("sqlite3ns::Database::assertOpened(): db not opened");
    }
}


}
