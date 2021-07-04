#include "view_page.h"

#include <assert.h>

view::LineLoc view::Page::getNextUpLineLoc(const LineLoc & lineLoc) const
{
    if (lineLoc.isNull() || lineLoc.isAfterLastRow())
    {
        return LineLoc();
    }

    if (lineLoc.line() > 0)
    {
        return LineLoc(lineLoc.row(), lineLoc.line() - 1);
    }

    if (lineLoc.row() > 0)
    {
        const int upRowIndex = lineLoc.row() - 1;
        const VRow & upRow = m_rows[upRowIndex];
        assert(upRow.size() > 0);
        return LineLoc(upRowIndex, upRow.size() - 1);
    }

    return LineLoc();
}

view::LineLoc view::Page::getNextDownLineLoc(const LineLoc & lineLoc) const
{
    if (lineLoc.isNull() || lineLoc.isAfterLastRow())
    {
        return LineLoc();
    }

    const int r = lineLoc.row();
    const VRow & row = m_rows[r];
    const int curRowSize = row.size();
    
    if (lineLoc.line() < curRowSize - 1)
    {
        return LineLoc(r, lineLoc.line() + 1);
    }

    const int rowCnt = size();
    if (r < rowCnt - 1)
    {
        return LineLoc(r + 1, 0);
    }

    return LineLoc();
}
