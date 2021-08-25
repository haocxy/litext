#include "row_cache.h"

#include <set>

#include "core/logger.h"
#include "charset/to_utf32.h"
#include "text/cut_rows.h"


namespace doc
{

static const i64 CacheLimit = 100 * 1024 * 1024;

RowCache::RowCache(LineManager &lineManager, const fs::path &file)
    : lineManager_(lineManager)
    , ifs_(file, std::ios::binary) {

}

void RowCache::updateCharset(Charset charset)
{
    Lock lock(mtx_);

    if (charset_ == charset) {
        return;
    }

    charset_ = charset;

    clearAllCache();
}

std::map<RowN, Row> RowCache::loadRows(const std::map<RowN, RowIndex> &rowIndexes)
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

    std::map<RowN, Row> result;

    // 加载每个片段中的需要的段落
    for (const auto &pair : partToRowOffs) {
        const PartId partId = pair.first;
        PartCache &partCache = ensurePartCached(partId);

        partCache.lastUse = std::chrono::steady_clock::now();

        const std::vector<std::u32string> &rows = partCache.rows;

        for (const auto [globalRowOff, localRowOff] : pair.second) {
            if (static_cast<i64>(rows.size()) > localRowOff) {
                result[globalRowOff] = Row(std::u32string(rows[localRowOff]), RowEnd::CRLF);
            } else {
                LOGE << "RowCache::loadRows() logic error, bad local row offset";
            }
        }
    }

    return result;
}

void RowCache::clearAllCache()
{
    partCaches_.clear();
    cachedByteCount_ = 0;
}

RowCache::PartCache &RowCache::ensurePartCached(PartId partId)
{
    auto it = partCaches_.find(partId);
    if (it != partCaches_.end()) {
        return it->second;
    }

    const Range<i64> byteRange = lineManager_.findByteRange(partId);

    MemBuff buff;
    buff.resize(byteRange.count());

    ifs_.seekg(byteRange.off());
    ifs_.read(reinterpret_cast<char *>(buff.data()), buff.size());

    const std::u32string partContent = charset::toUTF32(charset_, buff.data(), buff.size());

    std::vector<std::u32string> rows = text::cutRows(partContent);

    if (!rows.empty()) {
        PartCache &partCache = partCaches_[partId];
        partCache.lastUse = std::chrono::steady_clock::now();
        partCache.rows = std::move(rows);
        using Str = std::u32string;
        for (const Str &row : rows) {
            partCache.byteCount += row.size() * sizeof(Str::value_type);
        }
        cachedByteCount_ += partCache.byteCount;
        checkAndRemoveCache();
        return partCache;
    } else {
        throw std::logic_error("RowCache::ensurePartCached() part has no row");
    }
}

void RowCache::checkAndRemoveCache()
{
    while (!partCaches_.empty() && cachedByteCount_ > CacheLimit) {
        auto begIt = partCaches_.begin();
        auto endIt = partCaches_.end();
        auto oldestIt = begIt;
        std::chrono::time_point<std::chrono::steady_clock> oldestTime = oldestIt->second.lastUse;
        for (auto it = begIt; it != endIt; ++it) {
            const PartCache &partCache = it->second;
            if (partCache.lastUse < oldestTime) {
                oldestTime = partCache.lastUse;
                oldestIt = it;
            }
        }
        cachedByteCount_ -= oldestIt->second.byteCount;
        partCaches_.erase(oldestIt);
    }
}

}
