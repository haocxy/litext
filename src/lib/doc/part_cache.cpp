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
    , monoMemBuffer_(1024 * 1024)
    , mempool_(&monoMemBuffer_)
    , memres_(mempool_)
    , ifs_(file, std::ios::binary)
    , parts_(&memres_)
    , partToLastUse_(&memres_)
{
}

sptr<PartCache::Part> PartCache::partAt(PartId partId)
{
    Lock lock(mtx_);

    auto it = parts_.find(partId);
    if (it != parts_.end()) {
        partToLastUse_[partId] = std::chrono::steady_clock::now();
        return it->second;
    }

    sptr<Part> partPtr = std::make_shared<Part>(&memres_);

    parts_.insert({partId, partPtr});

    const Range<i64> byteRange = lineManager_.findByteRange(partId);

    {
        scc::string buff(&memres_);
        buff.resize(byteRange.count());

        ifs_.seekg(byteRange.off());
        ifs_.read(reinterpret_cast<char *>(buff.data()), buff.size());

        const scc::u16string partContent = charset::toUTF16(charset_, buff.data(), buff.size(), &memres_);

        *partPtr = text::cutRows(partContent);

        LOGD << "part cache size: [" << memres_.memUsage() / 1024 / 1024 << "MB] on part [" << partId << "] loaded";
    }

    partToLastUse_[partId] = std::chrono::steady_clock::now();

    while (!parts_.empty() && memres_.memUsage() > MemLimit) {
        PartId oldestPart = partToLastUse_.begin()->first;
        std::chrono::steady_clock::time_point oldestTime = partToLastUse_[oldestPart];
        for (const auto &pair : partToLastUse_) {
            if (pair.second < oldestTime) {
                oldestPart = pair.first;
                oldestTime = pair.second;
            }
        }
        parts_.erase(oldestPart);
        partToLastUse_.erase(oldestPart);
        LOGD << "part cache size: [" << memres_.memUsage() / 1024 / 1024 << "MB] on part [" << oldestPart << "] loaded";
    }

    return partPtr;
}

}
