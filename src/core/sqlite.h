#pragma once

#include "core/fs.h"

struct sqlite3;


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
};

}
