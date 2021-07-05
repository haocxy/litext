#include "page.h"

#include <assert.h>


namespace gui
{


VLineLoc Page::getNextUpLineLoc(const VLineLoc &lineLoc) const
{
    if (lineLoc.isNull() || lineLoc.isAfterLastRow()) {
        return VLineLoc();
    }

    if (lineLoc.line() > 0) {
        return VLineLoc(lineLoc.row(), lineLoc.line() - 1);
    }

    if (lineLoc.row() > 0) {
        const RowN upRowIndex = lineLoc.row() - 1;
        const VRow &upRow = m_rows[upRowIndex];
        assert(upRow.size() > 0);
        return VLineLoc(upRowIndex, upRow.size() - 1);
    }

    return VLineLoc();
}

VLineLoc Page::getNextDownLineLoc(const VLineLoc &lineLoc) const
{
    if (lineLoc.isNull() || lineLoc.isAfterLastRow()) {
        return VLineLoc();
    }

    const RowN r = lineLoc.row();
    const VRow &row = m_rows[r];
    const int curRowSize = row.size();

    if (lineLoc.line() < curRowSize - 1) {
        return VLineLoc(r, lineLoc.line() + 1);
    }

    const RowN rowCnt = size();
    if (r < rowCnt - 1) {
        return VLineLoc(r + 1, 0);
    }

    return VLineLoc();
}


}
