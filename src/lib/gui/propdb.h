#pragma once

#include <set>
#include <map>
#include <mutex>
#include <sstream>
#include <string>

#include "core/fs.h"


namespace gui
{

class PropDb {
public:
    PropDb();

private:
    const fs::path dbFile_;

    using Mtx = std::mutex;
    using Lock = std::lock_guard<Mtx>;
    Mtx mtx_;

    friend class PropRepo;
};

class PropRepo {
public:
    PropRepo(PropDb &db, const std::string &name);

    ~PropRepo();

    void set(const std::string &key, const std::string &val);

    void set(const std::string &key, const std::u32string &val);

    void set(const std::string &key, int val);

    void set(const std::string &key, long val);

    void set(const std::string &key, long long val);

    bool get(const std::string &key, int &to) const;

    bool get(const std::string &key, long &to) const;

    bool get(const std::string &key, long long &to) const;

    bool get(const std::string &key, std::u32string &to) const;

    void loadFromDb();
    
    void saveToDb();

private:
    template <typename T>
    void setValue(const std::string &key, const T &val) {
        std::ostringstream ss;
        ss << val;
        props_[key] = ss.str();
        dirtyKeys_.insert(key);
    }

private:
    using DbLock = PropDb::Lock;

    using Mtx = std::mutex;
    using RepoLock = std::lock_guard<Mtx>;

    PropDb &db_;

    std::mutex mtx_;

    const std::string repoName_;

    std::map<std::string, std::string> props_;

    std::set<std::string> dirtyKeys_;
};

}
