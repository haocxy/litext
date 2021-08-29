#include "row_cache.h"

#include <set>

#include "core/logger.h"
#include "charset/to_utf32.h"
#include "text/cut_rows.h"


namespace doc
{

static const i64 CacheLimit = 100 * 1024 * 1024;

RowCache::RowCache(LineManager &lineManager, const fs::path &file)
    : partCache_(lineManager, file) {

}

void RowCache::updateCharset(Charset charset)
{
    Lock lock(mtx_);

    if (partCache_.charset() == charset) {
        return;
    }

    partCache_.updateCharset(charset);

    clearAllCache();
}

std::map<RowN, sptr<Row>> RowCache::loadRows(const std::map<RowN, RowIndex> &rowIndexes)
{
    Lock lock(mtx_);

    using GlobalRowN = RowN;
    using LocalRowN = RowN;

    std::map<PartId, std::map<GlobalRowN, LocalRowN>> partToRowOffs;

    // 把按照段落映射的位置信息转换为按照片段映射的信息
    // 这样做是因为缓存是以片段为单位进行的
    for (const auto &pair : rowIndexes) {
        const GlobalRowN globalRowOff = pair.first;
        const PartId partId = pair.second.partId();
        const LocalRowN localRowOff = pair.second.rowOff();
        partToRowOffs[partId][globalRowOff] = localRowOff;
    }

    std::map<RowN, sptr<Row>> result;

    // 加载每个片段中的需要的段落
    for (const auto &pair : partToRowOffs) {
        const PartId partId = pair.first;
        for (const auto [globalRowOff, localRowOff] : pair.second) {
            result[globalRowOff] = ensureRowCached(globalRowOff, partId, localRowOff);
        }
    }

    return result;
}

void RowCache::clearAllCache()
{
    rows_.clear();
}

sptr<Row> RowCache::ensureRowCached(RowN row, PartId partId, RowN rowOff)
{
    auto it = rows_.find(row);
    if (it != rows_.end()) {
        return it->second;
    }

    sptr<PartCache::Part> partPtr = partCache_.partAt(partId);
    if (!partPtr) {
        return nullptr;
    }

    const PartCache::Part &part = *partPtr;
    if (rowOff >= static_cast<i64>(part.size())) {
        return nullptr;
    }

    const text::UTF16Row &u16row = part[rowOff];
    sptr<Row> &rowPtr = rows_[row];
    rowPtr = std::make_shared<Row>(u16row.toUTF32(), u16row.rowEnd());
    return rowPtr;
}

}
