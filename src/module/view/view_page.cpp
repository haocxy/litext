#include "view_page.h"

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

    throw std::logic_error("view::Page::getLineAddrByLineOffset");
}

view::CharAddr view::Page::getCharAddrByLineAddrAndX(const LineAddr & lineAddr, int x) const
{
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

    throw std::logic_error("view::Page::getCharAddrByLineAddrAndX");

}
