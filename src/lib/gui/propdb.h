#pragma once

#include <set>
#include <map>
#include <mutex>
#include <sstream>
#include <string>

#include "core/basetype.h"
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

    void set(const u8str &key, const u8str &val);

    void set(const u8str &key, int val);

    void set(const u8str &key, long val);

    void set(const u8str &key, long long val);

    bool get(const u8str &key, int &to) const;

    bool get(const u8str &key, long &to) const;

    bool get(const u8str &key, long long &to) const;

    bool get(const u8str &key, u8str &to) const;

    bool get(const u8str &key, u32str &to) const;

    void loadFromDb();
    
    void saveToDb();

private:
    void setValue(const u8str &key, const u8str &val) {
        props_[key] = val;
        dirtyKeys_.insert(key);
    }

private:
    using DbLock = PropDb::Lock;

    using Mtx = std::mutex;
    using RepoLock = std::lock_guard<Mtx>;

    PropDb &db_;

    std::mutex mtx_;

    const std::string repoName_;

    std::map<u8str, u8str> props_;

    std::set<u8str> dirtyKeys_;
};

}
