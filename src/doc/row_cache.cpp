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
    std::map<RowN, std::shared_ptr<Row>> result;

    

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
