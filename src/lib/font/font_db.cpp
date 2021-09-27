#include "font_db.h"

#include "asset.font_db_init.sql.h"


namespace font
{



FontDb::FontDb(const fs::path &dbFile)
    : db_(dbFile)
{
    db_.exec(reinterpret_cast<const char *>(Asset::Data::font_db_init__sql));

}

FontDb::~FontDb()
{
}


}
