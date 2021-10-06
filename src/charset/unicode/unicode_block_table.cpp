#include "unicode_block_table.h"

#include <algorithm>

#include "core/basetype.h"


namespace charset::unicode
{

BlockTable::BlockTable()
{
    makeTable();

    std::sort(itemsOrderedByBeg_.begin(), itemsOrderedByBeg_.end(), [](const Item &a, const Item &b) {
        return a.range.beg() < b.range.beg();
    });

    std::sort(itemsOrderedById_.begin(), itemsOrderedById_.end(), [](const Item &a, const Item &b) {
        return a.id < b.id;
    });
}

BlockId BlockTable::getBlockIdByCode(char32_t code) const
{
    const Item *item = findByCode(code);
    if (item) {
        return item->id;
    } else {
        return BlockId::Unknown;
    }
}

void BlockTable::makeTable()
{
    using Id = BlockId;

    add(0x0000, 0x007F, Id::Basic_Latin);
    add(0x3000, 0x303F, Id::CJK_Symbols_And_Punctuation);
    add(0x4E00, 0x9FFF, Id::CJK_Unified_Ideographs);
}

const BlockTable::Item *BlockTable::findByCode(char32_t code) const
{
    i64 left = 0;
    i64 right = static_cast<i64>(itemsOrderedByBeg_.size()) - 1;

    while (left <= right) {
        const i64 mid = (left + right) / 2;
        const Item &item = itemsOrderedByBeg_[mid];
        const char32_t leftUnicode = item.range.left();
        const char32_t rightUnicode = item.range.right();
        if (leftUnicode <= code && code <= rightUnicode) {
            return &item;
        } else if (code < leftUnicode) {
            right = left - 1;
        } else { // unicde > leftUnicode
            left = right + 1;
        }
    }

    return nullptr;
}

}
