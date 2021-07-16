#include "database.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include <third/sqlite3/sqlite3.h>


namespace sqlite3_wrapper
{


Database::Database(const fs::path &path)
	: path_(path)
{
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
		std::ofstream ofs(path, std::ios::binary);
	}
	sqlite3 *db = nullptr;
	int n = sqlite3_open(path.generic_string().c_str(), &db);
	if (n != SQLITE_OK) {
		std::ostringstream ss;
		ss << "cannot open SQLite3 database by file path: [" << path.generic_string() << "] ";
		ss << "because [" << sqlite3_errstr(n) << "]";
		throw std::logic_error(ss.str());
	}

	db_ = db;
}

Database::~Database()
{
	int n = sqlite3_close(db_);
	if (n != SQLITE_OK) {
		std::ostringstream ss;
		ss << "cannot close SQLite3 database on file [" << path_.generic_string() << "] ";
		ss << "because [" << sqlite3_errstr(n) << "]";
		std::cerr << ss.str() << std::endl; // TODO
	}

	db_ = nullptr;
}


}
