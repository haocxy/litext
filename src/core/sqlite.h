#pragma once

#include <cstddef>
#include <cstdint>

#include "primitive_types.h"
#include "membuff.h"
#include "fs.h"


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

    const fs::path &path() const {
        return path_;
    }

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

    Statement(const Statement &) = delete;

    Statement(Statement &&) = delete;

    Statement &operator=(const Statement &) = delete;

    Statement &operator=(Statement &&) = delete;

    ~Statement();

    void open(Database &db, const std::string &sql);

    void reset();

    void bindNull(int pos);

    Statement &arg() {
        bindNull(argBindIndex_++);
        return *this;
    }

    void bind(int pos, const void *data, size_t len);

    Statement &arg(const void *data, size_t len) {
        bind(argBindIndex_++, data, len);
        return *this;
    }

    void bind(int pos, const MemBuff &data) {
        bind(pos, data.data(), data.size());
    }

    Statement &arg(const MemBuff &data) {
        bind(argBindIndex_++, data);
        return *this;
    }

    void bind(int pos, int64_t value);

    Statement &arg(int64_t value) {
        bind(argBindIndex_++, value);
        return *this;
    }

    void step();

    int64_t lastInsertRowId() const;

    void close();

private:
    bool isOpened() const;

    void assertOpened() const;

    static void throwError(const char *func, int err);

private:
    sqlite3_stmt *stmt_ = nullptr;
    int argBindIndex_ = 1;
};

}
