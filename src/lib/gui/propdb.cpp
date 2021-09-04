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

static bool hasKey(Stmt &stmtSelect, const std::string_view &key)
{
    stmtSelect.reset();
    stmtSelect.arg(key);
    return stmtSelect.nextRow();
}

static void saveValue(Stmt &stmtSelect, Stmt &stmtUpdate, Stmt &stmtInsert, const std::string_view &key, const std::string &val)
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

void PropRepo::set(const std::string &key, const std::string &val)
{
    setValue(key, val);
}

void PropRepo::set(const std::string &key, const std::u32string &val)
{
    setValue(key, charset::toUTF8(val));
}

void PropRepo::set(const std::string &key, int val)
{
    setValue(key, val);
}

void PropRepo::set(const std::string &key, long val)
{
    setValue(key, val);
}

void PropRepo::set(const std::string &key, long long val)
{
    setValue(key, val);
}

template <typename T>
static bool getValue(const std::map<std::string, std::string> &map, const std::string &key, T &to)
{
    auto it = map.find(key);
    if (it != map.end()) {
        std::istringstream ss(it->second);
        ss >> to;
        return true;
    } else {
        return false;
    }
}

bool PropRepo::get(const std::string &key, int &to) const
{
    return getValue(props_, key, to);
}

bool PropRepo::get(const std::string &key, long &to) const
{
    return getValue(props_, key, to);
}

bool PropRepo::get(const std::string &key, long long &to) const
{
    return getValue(props_, key, to);
}

bool PropRepo::get(const std::string &key, std::u32string &to) const
{
    std::string utf8;
    const bool ok = getValue(props_, key, utf8);
    if (ok) {
        to = charset::toUTF32(Charset::UTF_8, utf8.data(), utf8.size());
    }
    return ok;
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
        std::string key;
        std::string val;
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
