#pragma once

#include <map>

#include "text_repo.h"
#include "doc_row.h"
#include "row_index.h"


namespace doc
{

class RowCache {
public:
    RowCache(TextRepo &repo);

    std::map<RowN, Row> loadRows(const std::map<RowN, RowIndex> &rowIndexes);

private:


private:
    std::map<PartId, std::vector<std::string>> partRows_;
};

}
