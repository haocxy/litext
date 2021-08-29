#include "part_cache.h"

#include <cassert>

#include "core/uchar.h"
#include "charset/to_utf16.h"
#include "text/cut_rows.h"


namespace doc
{

PartCache::PartCache(LineManager &lineManager, const fs::path &file)
    : lineManager_(lineManager)
    , ifs_(file, std::ios::binary)
{
}

sptr<PartCache::Part> PartCache::partAt(PartId partId)
{
    Lock lock(mtx_);

    sptr<Part> &partPtr = parts_[partId];
    if (partPtr) {
        return partPtr;
    }

    partPtr = std::make_shared<Part>();

    const Range<i64> byteRange = lineManager_.findByteRange(partId);

    MemBuff buff;
    buff.resize(byteRange.count());

    ifs_.seekg(byteRange.off());
    ifs_.read(reinterpret_cast<char *>(buff.data()), buff.size());

    const std::u16string partContent = charset::toUTF16(charset_, buff.data(), buff.size());

    *partPtr = text::cutRows(partContent);

    // TODO 清除过多的缓存

    return partPtr;
}

}
