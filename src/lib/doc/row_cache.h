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


namespace doc
{

class RowCache {
public:
    RowCache(LineManager &lineManager, const fs::path &file);

    void updateCharset(Charset charset);

    std::map<RowN, Row> loadRows(const std::map<RowN, RowIndex> &rowIndexes);

private:
    void clearAllCache();

    struct PartCache {
        std::chrono::time_point<std::chrono::steady_clock> lastUse;
        std::vector<std::u32string> rows;
        i64 byteCount = 0;
    };

    PartCache &ensurePartCached(PartId partId);

    void checkAndRemoveCache();

private:
    LineManager &lineManager_;

    using Mtx = std::mutex;
    using Lock = std::unique_lock<Mtx>;
    mutable Mtx mtx_;

    std::ifstream ifs_;
    std::atomic<Charset> charset_{ Charset::Ascii };
    std::map<PartId, PartCache> partCaches_;
    i64 cachedByteCount_ = 0;
};

}
