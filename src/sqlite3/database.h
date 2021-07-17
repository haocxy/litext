#pragma once

#include "core/fs.h"

struct sqlite3;


namespace sqlite3ns
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

	void open(const fs::path &path);

	void close();

private:
	fs::path path_;
	sqlite3 *db_ = nullptr;
};

}
