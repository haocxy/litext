#pragma once

#include <chrono>

#include <map>
#include <deque>
#include <vector>
#include <atomic>
#include <mutex>

#include "core/fs.h"
#include "core/charset.h"

#include "doc_row.h"
#include "row_index.h"
#include "line_manger.h"
#include "part_cache.h"


namespace doc
{

class RowCache {
public:
    RowCache(LineManager &lineManager, const fs::path &file);

    void clear();

    void updateCharset(Charset charset);

    std::map<RowN, sptr<Row>> loadRows(const std::map<RowN, RowIndex> &rowIndexes);

private:
    void clearAllCache();

    sptr<Row> ensureRowCached(RowN row, PartId partId, RowN rowOff);

private:
    PartCache partCache_;

    using Mtx = std::mutex;
    using Lock = std::unique_lock<Mtx>;
    mutable Mtx mtx_;

    std::map<RowN, sptr<Row>> rows_;
};

}
