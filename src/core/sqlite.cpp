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


Statement::Statement()
{
}

Statement::~Statement()
{
    try {
        if (isOpened()) {
            close();
        }
    }
    catch (const std::exception &e) {
        LOGE << "sqlite::Statement::~Statement() " << "error: [" << e.what() << "]";
    }
    catch (...) {
        LOGE << "sqlite::Statement::~Statement() " << "meet unknown exception";
    }
}

void Statement::open(Database &db, const std::string &sql)
{
    if (sql.empty()) {
        throw std::logic_error("sqlite::Statement::open() failed because SQL in argument is empty");
    }

    if (isOpened()) {
        close();
    }

    db.assertOpened();

    sqlite3_stmt *stmt = nullptr;

    const int n = sqlite3_prepare_v2(db.db_, sql.c_str(), static_cast<int>(sql.size()), &stmt, nullptr);

    if (n != SQLITE_OK) {
        std::ostringstream ss;
        ss << "sqlite::Statement::open() failed because: [" << sqlite3_errstr(n) << "]";
        throw std::logic_error(ss.str());
    }

    stmt_ = stmt;
}

void Statement::reset()
{
    assertOpened();

    const int n = sqlite3_reset(stmt_);
    if (n != SQLITE_OK) {
        std::ostringstream ss;
        ss << "sqlite::Statement::reset() failed because: [" << sqlite3_errstr(n) << "]";
        throw std::logic_error(ss.str());
    }
}

void Statement::bind(int pos, const void *data, size_t len)
{
    if (pos > 0 && data != nullptr && len > 0) {
        assertOpened();

        const int n = sqlite3_bind_blob(stmt_, pos, data, static_cast<int>(len), SQLITE_TRANSIENT);
        if (n != SQLITE_OK) {
            std::ostringstream ss;
            ss << "sqlite::Statement::bind() failed because: [" << sqlite3_errstr(n) << "]";
            throw std::logic_error(ss.str());
        }
    }
}

void Statement::close()
{
    assertOpened();

    const int n = sqlite3_finalize(stmt_);
    if (n != SQLITE_OK) {
        std::ostringstream ss;
        ss << "sqlite::Statement::close() failed because: [" << sqlite3_errstr(n) << "]";
        throw std::logic_error(ss.str());
    }

    stmt_ = nullptr;
}

bool Statement::isOpened() const
{
    return stmt_;
}

void Statement::assertOpened() const
{
    if (!stmt_) {
        throw std::logic_error("sqlite::Statement not opened");
    }
}

}
