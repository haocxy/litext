#pragma once

#include <map>
#include <memory>

#include "text_repo.h"
#include "doc_row.h"
#include "row_index.h"


namespace doc
{

class RowCache {
public:
    RowCache(TextRepo &repo);

    std::map<RowN, std::shared_ptr<Row>> loadRows(const std::map<RowN, RowIndex> &rowIndexes);

private:
    TextRepo::QueryPartDataByPartIdStmt stmtQueryPartDataByPartId_;
};

}
