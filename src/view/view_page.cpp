#include "view_page.h"

#include <assert.h>

view::LineAddr view::Page::getLineAddrByLineOffset(int offset) const
{
    const int rowCnt = static_cast<int>(m_rows.size());

    int sum = 0;

    for (int i = 0; i < rowCnt; ++i)
    {
        const view::VRow &row = m_rows[i];
        const int lineCnt = row.size();

        if (sum + lineCnt > offset)
        {
            return view::LineAddr(i, offset - sum);
        }

        sum += lineCnt;
    }

    return view::LineAddr::newLineAddrAfterLastRow();
}

view::CharAddr view::Page::getCharAddrByLineAddrAndX(const LineAddr & lineAddr, int x) const
{
    if (lineAddr.isNull() || lineAddr.isAfterLastRow())
    {
        return view::CharAddr::newCharAddrAfterLastChar(lineAddr);
    }

    const Line & line = getLine(lineAddr);

    const int charCnt = line.size();
    for (int i = 0; i < charCnt; ++i)
    {
        const Char & c = line[i];
        if (x < c.x() + c.width() / 2)
        {
            return CharAddr(lineAddr, i);
        }
    }

    return CharAddr::newCharAddrAfterLastChar(lineAddr);
}

view::LineAddr view::Page::getNextUpLineAddr(const LineAddr & addr) const
{
    if (addr.isNull() || addr.isAfterLastRow())
    {
        return LineAddr();
    }

    if (addr.line() > 0)
    {
        return LineAddr(addr.row(), addr.line() - 1);
    }

    if (addr.row() > 0)
    {
        const int upRowIndex = addr.row() - 1;
        const VRow & upRow = m_rows[upRowIndex];
        assert(upRow.size() > 0);
        return LineAddr(upRowIndex, upRow.size() - 1);
    }

    return LineAddr();
}

view::LineAddr view::Page::getNextDownLineAddr(const LineAddr & addr) const
{
    if (addr.isNull() || addr.isAfterLastRow())
    {
        return LineAddr();
    }

    const int r = addr.row();
    const VRow & row = m_rows[r];
    const int curRowSize = row.size();
    
    if (addr.line() < curRowSize - 1)
    {
        return LineAddr(r, addr.line() + 1);
    }

    const int rowCnt = size();
    if (r < rowCnt - 1)
    {
        return LineAddr(r + 1, 0);
    }

    return LineAddr();
}
