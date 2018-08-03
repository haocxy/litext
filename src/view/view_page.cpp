#include "view_page.h"

#include <assert.h>

view::LineAddr view::Page::getLineAddrByLineOffset(int offset) const
{
    const int phaseCnt = static_cast<int>(m_phases.size());

    int sum = 0;

    for (int i = 0; i < phaseCnt; ++i)
    {
        const view::Phase &vphase = m_phases[i];
        const int lineCnt = vphase.size();

        if (sum + lineCnt > offset)
        {
            return view::LineAddr(i, offset - sum);
        }

        sum += lineCnt;
    }

    return view::LineAddr::newLineAddrAfterLastPhase();
}

view::CharAddr view::Page::getCharAddrByLineAddrAndX(const LineAddr & lineAddr, int x) const
{
    if (lineAddr.isNull() || lineAddr.isAfterLastPhase())
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
    if (addr.isNull() || addr.isAfterLastPhase())
    {
        return LineAddr();
    }

    if (addr.line() > 0)
    {
        return LineAddr(addr.phase(), addr.line() - 1);
    }

    if (addr.phase() > 0)
    {
        const int upPhaseIndex = addr.phase() - 1;
        const Phase & upPhase = m_phases[upPhaseIndex];
        assert(upPhase.size() > 0);
        return LineAddr(upPhaseIndex, upPhase.size() - 1);
    }

    return LineAddr();
}

view::LineAddr view::Page::getNextDownLineAddr(const LineAddr & addr) const
{
    if (addr.isNull() || addr.isAfterLastPhase())
    {
        return LineAddr();
    }

    const Phase & curPhase = m_phases[addr.phase()];
    const int curPhaseLineCnt = curPhase.size();
    
    if (addr.line() < curPhaseLineCnt - 1)
    {
        return LineAddr(addr.phase(), addr.line() + 1);
    }

    const int phaseCnt = size();
    if (addr.phase() < phaseCnt - 1)
    {
        return LineAddr(addr.phase() + 1, 0);
    }

    return LineAddr();
}
