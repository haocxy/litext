#include "propdb.h"

#include "core/system.h"


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


}
