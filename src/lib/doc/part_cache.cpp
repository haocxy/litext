#include "part_cache.h"

#include <cassert>

#include "core/uchar.h"
#include "charset/to_utf16.h"
#include "text/cut_rows.h"


namespace doc
{

static const std::size_t MemLimit = 50 * 1024 * 1024;


PartCache::PartCache(LineManager &lineManager, const fs::path &file)
    : lineManager_(lineManager)
    , ifs_(file, std::ios::binary)
    , parts_(&memres_)
{
}

sptr<PartCache::Part> PartCache::partAt(PartId partId)
{
    Lock lock(mtx_);

    auto it = parts_.find(partId);
    if (it != parts_.end()) {
        it->second->lastUse = std::chrono::steady_clock::now();
        return it->second;
    }

    sptr<Part> partPtr = std::make_shared<Part>(&memres_);

    const Range<i64> byteRange = lineManager_.findByteRange(partId);

    {
        // 把 buff 置于块作用域中，确保在后面的检查内存使用情况的逻辑前被释放
        scc::string buff(&memres_);
        buff.resize(byteRange.count());

        ifs_.seekg(byteRange.off());
        ifs_.read(reinterpret_cast<char *>(buff.data()), buff.size());

        const scc::u16string partContent = charset::toUTF16(charset_, buff.data(), buff.size(), &memres_);

        partPtr->lastUse = std::chrono::steady_clock::now();
        partPtr->rows = text::cutRows(partContent);
    }

    while (!parts_.empty() && memres_.memUsage() > MemLimit) {
        auto oldestIt = parts_.begin();
        auto oldestTime = oldestIt->second->lastUse;
        auto it = oldestIt;
        ++it;
        const auto end = parts_.end();
        for (; it != end; ++it) {
            const auto &lastUse = it->second->lastUse;
            if (lastUse < oldestTime) {
                oldestIt = it;
                oldestTime = lastUse;
            }
        }
        parts_.erase(oldestIt);
    }

    // 为了简化清理内存的逻辑，在清理内存之后再加入映射
    // 否则就需要在清理内存时检查是否是新加入的
    parts_.insert({ partId, partPtr });

    return partPtr;
}

}
