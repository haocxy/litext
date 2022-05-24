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

// 属性数据库
// 存放为了让程序具有连贯性而需要持久保存的数据
// 底层对应一个数据库
// 
// 注意！
// 这个类维护的和配置无关，配置是用户显式指定的，
// 而这个类维护的是程序自身判断需要记录的数据（例如窗口位置）
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

// 属性仓库
// 为了提升性能和简化管理，把属性数据库中的某些相关性较强的属性放在一起
// 底层对应一个数据表
class PropRepo {
public:

    // db: 在哪个属性数据库中
    // name: 属性仓库的名称，在底层对应数据表的名称
    //
    // 构造时会调用 loadFromDb 加载数据
    PropRepo(PropDb &db, const std::string &name);

    // 析构时会调用 saveToDb 存储数据
    ~PropRepo();

    void set(const u8str &key, const u8str &val);

    void set(const u8str &key, bool val);

    void set(const u8str &key, int val);

    void set(const u8str &key, long val);

    void set(const u8str &key, long long val);

    bool get(const u8str &key, bool &to) const;

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
