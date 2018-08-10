#include "view_page.h"

#include <assert.h>

static inline int calcLeftBound(int x, int leftWidth)
{
    assert(leftWidth > 0);

    return x - (leftWidth >> 1);
}

static inline int calcRightBound(int x, int curWidth)
{
    assert(curWidth > 0);

    if ((curWidth & 1) == 0)
    {
        return x + (curWidth >> 1) - 1;
    }
    else
    {
        return x + (curWidth >> 1);
    }
}

view::CharLoc view::Page::getCharLocByLineLocAndX(const LineLoc & lineLoc, int x) const
{
    if (lineLoc.isNull() || lineLoc.isAfterLastRow())
    {
        return view::CharLoc::newCharLocAfterLastChar(lineLoc);
    }

    const Line & line = getLine(lineLoc);

    const int charCnt = line.size();

    if (charCnt == 0)
    {
        return CharLoc::newCharLocAfterLastChar(lineLoc);
    }

    // 为了简化处理，把第一个字符单独处理，因为第一个字符没有前一个字符
    const Char & firstChar = line[0];
    const int firstX = firstChar.x();
    const int firstWidth = firstChar.width();
    const int firstCharRightBound = calcRightBound(firstX, firstWidth);
    if (x <= firstCharRightBound)
    {
        return CharLoc(lineLoc, 0);
    }

    int left = 1;
    int right = charCnt - 1;
    while (left <= right)
    {
        const int mid = ((left + right) >> 1);
        const Char & c = line[mid];
        const int cx = c.x();
        const int a = calcLeftBound(cx, line[mid - 1].width());
        const int b = calcRightBound(cx, c.width());
        if (x < a)
        {
            right = mid - 1;
        }
        else if (x > b)
        {
            left = mid + 1;
        }
        else
        {
            return CharLoc(lineLoc, mid);
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
