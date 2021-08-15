#include "row_cache.h"

#include "simple_doc_row.h"

#include <QString>
#include <QTextStream>


namespace doc
{

RowCache::RowCache(TextRepo &repo)
    : stmtQueryPartDataByPartId_(repo) {

}

std::map<RowN, std::shared_ptr<Row>> RowCache::loadRows(const std::map<RowN, RowIndex> &rowIndexes)
{
    struct Item {
        Item(RowN row, const Range<i64> &byteRange)
            : row(row), byteRange(byteRange) {}

        RowN row;
        Range<i64> byteRange;
    };

    std::map<PartId, std::vector<Item>> merged;
    for (auto &pair : rowIndexes) {
        merged[pair.second.partId()].push_back(Item(pair.first, pair.second.byteRange()));
    }

    std::map<RowN, std::shared_ptr<Row>> result;

    for (const auto &pair : merged) {
        const PartId partId = pair.first;
        const auto result = stmtQueryPartDataByPartId_(partId);
        const MemBuff &data = result.partData;
        QString qstr = QString::fromUtf16(reinterpret_cast<const char16_t*>(data.data()), data.size() / 2);


        std::shared_ptr<SimpleRow> p = std::make_shared<SimpleRow>();
        p->setContent(UStringUtil::utf16ToUString(data.data(), data.size()));
        p->setRowEnd(RowEnd::CRLF); // TODO

    }

    // 测试数据
    for (const auto &pair : rowIndexes) {
        const RowN row = pair.first;
        QString testData = QString().sprintf("test data for %d", static_cast<int>(row));
        UString ustr = UStringUtil::utf16ToUString(testData.data(), testData.size() * 2);
        std::shared_ptr<SimpleRow> p = std::make_shared<SimpleRow>();
        p->setContent(std::move(ustr));
        p->setRowEnd(RowEnd::CRLF);
        result[row] = p;
    }

    return result;
}

}
