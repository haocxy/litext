#include "propdb.h"

#include "core/system.h"
#include "core/sqlite.h"

#include "charset/to_utf8.h"
#include "charset/to_utf32.h"


namespace
{
using Stmt = sqlite::Statement;
}

namespace gui
{

PropDb::PropDb()
    : dbFile_(SystemUtil::userHome() / LITEXT_WORKDIR_NAME / "props.propdb")
{
}

static std::string makeCreateTableSql(const std::string &name)
{
    std::string s;
    s.append("CREATE TABLE IF NOT EXISTS ");
    s.append(name);
    s.append("(k PRIMARY KEY,v NOT NULL);");
    return s;
}

static std::string makeSelectAllPairSql(const std::string &name)
{
    std::string s;
    s.append("SELECT k,v FROM ");
    s.append(name);
    s.append(";");
    return s;
}

static std::string makeSelectSql(const std::string &name)
{
    std::string s;
    s.append("SELECT v FROM ");
    s.append(name);
    s.append(" where k = ?;");
    return s;
}

static std::string makeInsertSql(const std::string &name)
{
    std::string s;
    s.append("INSERT INTO ");
    s.append(name);
    s.append(" VALUES(?,?);");
    return s;
}

static std::string makeUpdateSql(const std::string &name)
{
    std::string s;
    s.append("UPDATE ");
    s.append(name);
    s.append(" SET v = ? WHERE k = ?");
    return s;
}

PropRepo::PropRepo(PropDb &db, const std::string &name)
    : db_(db)
    , repoName_(name)
{
    loadFromDb();
}

PropRepo::~PropRepo()
{
    saveToDb();
}

static bool hasKey(Stmt &stmtSelect, const u8view &key)
{
    stmtSelect.reset();
    stmtSelect.arg(key);
    return stmtSelect.nextRow();
}

static void saveValue(Stmt &stmtSelect, Stmt &stmtUpdate, Stmt &stmtInsert, const u8view &key, const u8str &val)
{
    if (hasKey(stmtSelect, key)) {
        stmtUpdate.reset();
        stmtUpdate.arg(val).arg(key);
        stmtUpdate.step();
    } else {
        stmtInsert.reset();
        stmtInsert.arg(key).arg(val);
        stmtInsert.step();
    }
}

void PropRepo::set(const u8str &key, const u8str &val)
{
    setValue(key, val);
}

void PropRepo::set(const u8str &key, bool val)
{
    setValue(key, val ? 1 : 0);
}

void PropRepo::set(const u8str &key, int val)
{
    setValue(key, val);
}

void PropRepo::set(const u8str &key, long val)
{
    setValue(key, val);
}

void PropRepo::set(const u8str &key, long long val)
{
    setValue(key, val);
}

template <typename T>
static bool getValue(const std::map<u8str, u8str> &map, const u8str &key, T &to)
{
    auto it = map.find(key);
    if (it != map.end()) {
        to = it->second;
        return true;
    } else {
        return false;
    }
}

bool PropRepo::get(const u8str &key, bool &to) const
{
    int n = 0;
    if (getValue(props_, key, n)) {
        to = (n != 0);
        return true;
    } else {
        return false;
    }
}

bool PropRepo::get(const u8str &key, int &to) const
{
    return getValue(props_, key, to);
}

bool PropRepo::get(const u8str &key, long &to) const
{
    return getValue(props_, key, to);
}

bool PropRepo::get(const u8str &key, long long &to) const
{
    return getValue(props_, key, to);
}

bool PropRepo::get(const u8str &key, u8str &to) const
{
    return getValue(props_, key, to);
}

bool PropRepo::get(const u8str &key, u32str &to) const
{
    return getValue(props_, key, to);
}

void PropRepo::loadFromDb()
{
    RepoLock lockRepo(mtx_);

    props_.clear();
    dirtyKeys_.clear();

    DbLock lockDb(db_.mtx_);

    sqlite::Database db(db_.dbFile_);

    db.exec(makeCreateTableSql(repoName_));

    sqlite::Statement stmtSelectAllPair(db, makeSelectAllPairSql(repoName_));

    while (stmtSelectAllPair.nextRow()) {
        u8str key;
        u8str val;
        stmtSelectAllPair.getValue(0, key);
        stmtSelectAllPair.getValue(1, val);
        props_[key] = std::move(val);
    }
}

void PropRepo::saveToDb()
{
    RepoLock lockRepo(mtx_);

    if (dirtyKeys_.empty()) {
        return;
    }

    DbLock lockDb(db_.mtx_);

    sqlite::Database db(db_.dbFile_);

    db.exec(makeCreateTableSql(repoName_));

    sqlite::Statement stmtSelect(db, makeSelectSql(repoName_));
    sqlite::Statement stmtInsert(db, makeInsertSql(repoName_));
    sqlite::Statement stmtUpdate(db, makeUpdateSql(repoName_));

    for (auto dirtyKeyIt = dirtyKeys_.begin(); dirtyKeyIt != dirtyKeys_.end(); dirtyKeyIt = dirtyKeys_.erase(dirtyKeyIt)) {
        const auto propIt = props_.find(*dirtyKeyIt);
        if (propIt != props_.end()) {
            saveValue(stmtSelect, stmtUpdate, stmtInsert, *dirtyKeyIt, propIt->second);
        }
    }
}

}
