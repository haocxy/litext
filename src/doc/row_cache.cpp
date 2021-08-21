#include "row_cache.h"

#include "simple_doc_row.h"

#include <QString>
#include <QTextStream>


namespace doc
{

RowCache::RowCache(TextRepo &repo) {

}

std::map<RowN, uptr<Row>> RowCache::loadRows(const std::map<RowN, RowIndex> &rowIndexes)
{
    std::map<RowN, uptr<Row>> result;

    // 测试数据
    for (const auto &pair : rowIndexes) {
        const RowN row = pair.first;
        QString testData = QString().sprintf("test data for %d", static_cast<int>(row));
        uptr<SimpleRow> p = std::make_unique<SimpleRow>();
        p->setContent(testData.toStdU32String());
        p->setRowEnd(RowEnd::CRLF);
        result[row] = std::move(p);
    }

    return result;
}

}
