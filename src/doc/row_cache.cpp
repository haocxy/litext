#include "row_cache.h"

#include <QString>
#include <QTextStream>


namespace doc
{

RowCache::RowCache(TextRepo &repo) {

}

std::map<RowN, Row> RowCache::loadRows(const std::map<RowN, RowIndex> &rowIndexes)
{
    std::map<RowN, Row> result;

    // 测试数据
    for (const auto &pair : rowIndexes) {
        const RowN row = pair.first;
        QString testData = QString().sprintf("test data for %d", static_cast<int>(row));
        result[row] = Row(testData.toStdU32String(), RowEnd::CRLF);
    }

    return result;
}

}
