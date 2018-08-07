#include "view_page.h"

#include <assert.h>

view::LineLoc view::Page::getLineLocByLineOffset(int offset) const
{
    const int rowCnt = static_cast<int>(m_rows.size());

    int sum = 0;

    for (int i = 0; i < rowCnt; ++i)
    {
        const view::VRow &row = m_rows[i];
        const int lineCnt = row.size();

        if (sum + lineCnt > offset)
        {
            return view::LineLoc(i, offset - sum);
        }

        sum += lineCnt;
    }

    return view::LineLoc::newLineLocAfterLastRow();
}

view::CharLoc view::Page::getCharLocByLineLocAndX(const LineLoc & lineLoc, int x) const
{
    if (lineLoc.isNull() || lineLoc.isAfterLastRow())
    {
        return view::CharLoc::newCharLocAfterLastChar(lineLoc);
    }

    const Line & line = getLine(lineLoc);

    const int charCnt = line.size();
    for (int i = 0; i < charCnt; ++i)
    {
        const Char & c = line[i];
        if (x < c.x() + c.width() / 2)
        {
            return CharLoc(lineLoc, i);
        }
    }

    return CharLoc::newCharLocAfterLastChar(lineLoc);
}

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
