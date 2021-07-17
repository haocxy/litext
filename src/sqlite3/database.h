#pragma once

#include "core/fs.h"

struct sqlite3;


namespace sqlite3_wrapper
{

class Database {
public:
	Database(const fs::path &path);

	~Database();

	Database(const Database &) = delete;

	Database(Database &&) = delete;

	Database &operator=(const Database &) = delete;

	Database &operator=(Database &&) = delete;

private:
	const fs::path path_;
	sqlite3 *db_ = nullptr;
};

}
