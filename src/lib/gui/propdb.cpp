#include "propdb.h"

#include "core/system.h"


namespace
{
using Stmt = sqlite::Statement;
}

namespace gui
{

PropDb::PropDb()
    : db_(SystemUtil::userHome() / ".notesharp" / "props.propdb")
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

static std::string makeSelectSql(const std::string &name)
{
    std::string s;
    s.append("SELECT v from ");
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
{
    db.db_.exec(makeCreateTableSql(name));

    stmtSelect_.open(db.db_, makeSelectSql(name));
    stmtInsert_.open(db.db_, makeInsertSql(name));
    stmtUpdate_.open(db.db_, makeUpdateSql(name));
}

static bool hasKey(Stmt &stmtSelect, const std::string_view &key)
{
    stmtSelect.reset();
    stmtSelect.arg(key);
    return stmtSelect.nextRow();
}

template <typename T>
static void setValue(Stmt &stmtSelect, Stmt &stmtUpdate, Stmt &stmtInsert, const std::string_view &key, const T &val)
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

void PropRepo::set(const std::string_view &key, const std::string &val)
{
    setValue(stmtSelect_, stmtUpdate_, stmtInsert_, key, val);
}

void PropRepo::set(const std::string_view &key, const std::u32string &val)
{
    setValue(stmtSelect_, stmtUpdate_, stmtInsert_, key, val);
}

void PropRepo::set(const std::string_view &key, i32 val)
{
    setValue(stmtSelect_, stmtUpdate_, stmtInsert_, key, val);
}

void PropRepo::set(const std::string_view &key, i64 val)
{
    setValue(stmtSelect_, stmtUpdate_, stmtInsert_, key, val);
}

template <typename T>
static bool getValue(sqlite::Statement &stmt, const std::string_view &key, T &to)
{
    stmt.reset();
    stmt.arg(key);
    if (stmt.nextRow()) {
        stmt.getValue(0, to);
        return true;
    } else {
        return false;
    }
}

bool PropRepo::get(const std::string_view &key, i32 &to)
{
    return getValue(stmtSelect_, key, to);
}

bool PropRepo::get(const std::string_view &key, i64 &to)
{
    return getValue(stmtSelect_, key, to);
}

bool PropRepo::get(const std::string_view &key, std::u32string &to)
{
    return getValue(stmtSelect_, key, to);
}

}
