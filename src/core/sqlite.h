#pragma once

#include <cstddef>

#include "core/fs.h"

struct sqlite3;
struct sqlite3_stmt;

namespace sqlite
{

class Database {
public:
	Database();

	Database(const fs::path &path);

	~Database();

	Database(const Database &) = delete;

	Database(Database &&) = delete;

	Database &operator=(const Database &) = delete;

	Database &operator=(Database &&) = delete;

    bool isOpened() const;

	void open(const fs::path &path);

	void close();

    void exec(const std::string &sql);

private:
    void assertOpened() const;

private:
	fs::path path_;
	sqlite3 *db_ = nullptr;

    friend class Statement;
};


class Statement {
public:
    Statement();

    ~Statement();

    void open(Database &db, const std::string &sql);

    void reset();

    void bind(int pos, const void *data, size_t len);

    void close();

private:
    bool isOpened() const;

    void assertOpened() const;

private:
    sqlite3_stmt *stmt_ = nullptr;
};

}
