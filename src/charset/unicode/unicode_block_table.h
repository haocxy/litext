#pragma once

#include <vector>

#include "unicode_block_id.h"
#include "unicode_block_range.h"


namespace charset::unicode
{

// 区块表
// 维护Unicode各个区块的范围,用于根据字符编码定位其所在的区块
// 这个类的对象在构造时会填入各个区块的数据,此后就不再修改,所以应该构造一次反复使用
class BlockTable {
public:

    BlockTable();

    // 定位某个字符所在的区块
    // 若定位失败则返回BlockId::Unknown
    BlockId getBlockIdByCode(char32_t code) const;

private:

    struct Item {
        Item(BlockId id, BlockRange range)
            : id(id), range(range) {}

        BlockId id = BlockId::Unknown;
        BlockRange range;
    };

    void makeTable();

    void add(BlockRange::value_type left, BlockRange::value_type right, BlockId id) {
        const Item item(id, BlockRange::byLeftAndRight(left, right));
        itemsOrderedByBeg_.push_back(item);
        itemsOrderedById_.push_back(item);
    }

    const Item *findByCode(char32_t code) const;

private:
    using Items = std::vector<Item>;

    Items itemsOrderedByBeg_;
    Items itemsOrderedById_;
};

}
