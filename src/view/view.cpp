#include <cassert>
#include "view.h"
#include "view_config.h"
#include "doc/doc.h"
#include "doc/doc_line.h"
#include "text/doc_line_char_instream.h"
#include "text/txt_word_instream.h"

void View::Init(LineN viewStart, const view::Size & size)
{
    m_viewStart = viewStart;
    m_size = size;

    remakePage();
}

const view::Page & View::page() const
{
    return m_page;
}

int View::getBaseLineByLineOffset(int off) const
{
    return (1 + off) * m_config.lineHeight() - m_config.font().descent();
}

int View::getLineOffsetByLineAddr(const view::LineAddr & lineAddr) const
{
    int sum = lineAddr.line();

    const int phaseCnt = m_page.size();
    const int phase = lineAddr.phase();

    for (int i = 0; i < phaseCnt && i < phase; ++i)
    {
        sum += m_page[i].size();
    }

    return sum;
}

int View::getLineOffsetByPhaseIndex(int phase) const
{
    int sum = 0;

    const int phaseCnt = m_page.size();

    for (int i = 0; i < phaseCnt && i < phase; ++i)
    {
        sum += m_page[i].size();
    }

    return sum;
}

view::LineAddr View::getLineAddrByY(int y) const
{
    return m_page.getLineAddrByLineOffset(getLineOffsetByY(y));
}

view::CharAddr View::getCharAddrByPoint(int x, int y) const
{
    const int lineOffset = getLineOffsetByY(y);
    const view::LineAddr lineAddr = m_page.getLineAddrByLineOffset(lineOffset);
    return m_page.getCharAddrByLineAddrAndX(lineAddr, x);
}

DocAddr View::getDocAddrByPoint(int x, int y) const
{
    return convertToDocAddr(getCharAddrByPoint(x, y));
}

view::PhaseAddr View::convertToPhaseAddr(LineN line) const
{
    const int phase = line - m_viewStart;
    const int phaseCnt = m_page.size();
    if (phase < 0 || phase >= phaseCnt)
    {
        if (line >= m_model.lineCnt())
        {
            return view::PhaseAddr::newPhaseAddrAfterLastPhase();
        }
        return view::PhaseAddr();
    }

    if (line >= m_model.lineCnt())
    {
        return view::PhaseAddr::newPhaseAddrAfterLastPhase();
    }

    return view::PhaseAddr(phase);
}

view::CharAddr View::convertToCharAddr(const DocAddr & docAddr) const
{
    if (docAddr.isNull())
    {
        return view::CharAddr();
    }

    if (docAddr.isAfterLastPhase())
    {
        return view::CharAddr::newCharAddrAfterLastPhase();
    }

    const int phase = docAddr.line() - m_viewStart;
    const int phaseCnt = m_page.size();
    if (phase < 0 || phase >= phaseCnt)
    {
        return view::CharAddr();
    }

    const view::Phase &vphase = m_page[phase];

    if (docAddr.isAfterLastChar())
    {
        view::LineAddr lineAddr(phase, vphase.size() - 1);
        return view::CharAddr::newCharAddrAfterLastChar(lineAddr);
    }
    
    int lineIndex = 0;
    int charIndex = 0;

    const CharN col = docAddr.col();

    for (const view::DocLine &vline : vphase)
    {
        for (const view::Char &vc : vline)
        {
            if (charIndex == col)
            {
                return view::CharAddr(phase, lineIndex, charIndex);
            }

            ++charIndex;
        }

        ++lineIndex;
    }

    return view::CharAddr();
}

DocAddr View::convertToDocAddr(const view::CharAddr & charAddr) const
{
    if (charAddr.isNull())
    {
        return DocAddr();
    }

    if (charAddr.isAfterLastPhase())
    {
        return DocAddr::newCharAddrAfterLastPhase();
    }

    if (charAddr.isAfterLastChar())
    {
        return DocAddr::newCharAddrAfterLastChar(m_viewStart + charAddr.phase());
    }

    return DocAddr(m_viewStart + charAddr.phase(), charAddr.col());
}

const view::Char & View::getChar(const view::CharAddr & charAddr) const
{
    return m_page[charAddr.phase()][charAddr.line()][charAddr.col()];
}

int View::getXByAddr(const view::CharAddr & charAddr) const
{
    if (charAddr.isNull())
    {
        return 0;
    }

    if (charAddr.isAfterLastPhase())
    {
        return m_config.hGap();
    }

    if (charAddr.isAfterLastChar())
    {
        const view::Char &vc = m_page[charAddr.phase()][charAddr.line()].last();
        return vc.x() + vc.width();
    }

    return getChar(charAddr).x();
}

view::LineBound View::getLineBoundByLineOffset(int lineOffset) const
{
    const int lineHeight = m_config.lineHeight();
    const int top = lineHeight * lineOffset;
    const int bottom = top + lineHeight;

    return view::LineBound(top, bottom);
}

view::LineBound View::getLineBound(const view::LineAddr & lineAddr) const
{
    if (lineAddr.isAfterLastPhase())
    {
        return getLineBoundByLineOffset(m_page.lineCnt());
    }
    const int lineOffset = getLineOffsetByLineAddr(lineAddr);
    return getLineBoundByLineOffset(lineOffset);
}

view::PhaseBound View::getPhaseBound(const view::PhaseAddr & phaseAddr) const
{
    if (phaseAddr.isNull())
    {
        return view::PhaseBound();
    }

    if (phaseAddr.isAfterLastPhase())
    {
        const int lineOffset = m_page.lineCnt();
        const int lineHeight = m_config.lineHeight();
        const int top = lineHeight * lineOffset;
        return view::PhaseBound(top, lineHeight);
    }

    const int lineOffset = getLineOffsetByPhaseIndex(phaseAddr.phase());
    const int lineHeight = m_config.lineHeight();
    const int top = lineHeight * lineOffset;
    const int height = lineHeight * m_page[phaseAddr.phase()].size();
    return view::PhaseBound(top, height);
}

int View::getLineOffsetByY(int y) const
{
    return y / m_config.lineHeight();
}

void View::remakePage()
{
    m_page.clear();

    const LineN maxShowLine = m_viewStart + m_size.height() / m_config.lineHeight() + 1;

    const LineN rowCnt = std::min(maxShowLine, m_model.lineCnt());

    for (LineN i = m_viewStart; i < rowCnt; ++i)
    {
        view::Phase &vphase = m_page.grow();

        DocLineToViewPhase(m_model.lineAt(i), vphase);
    }

}

void View::DocLineToViewPhase(const DocLine& line, view::Phase & phase)
{
    if (m_config.wrapLine())
    {
        DocLineToViewPhaseWithWrapLine(line, phase);
    }
    else
    {
        DocLineToViewPhaseNoWrapLine(line, phase);
    }
}

void View::DocLineToViewPhaseWithWrapLine(const DocLine& line, view::Phase & phase)
{
    assert(phase.size() == 0);

    const int hGap = m_config.hGap();
    const int hMargin = m_config.hMargin();

    view::DocLine *vline = &phase.grow();

    DocLineCharInStream charStream(line);
    TxtWordStream wordStream(charStream);

    int leftX = hGap;

    while (true)
    {
        const UString word = wordStream.Next();
        if (check::isNull(word))
        {
            return;
        }

        if (vline->size() == 0)
        {
            for (const UChar c : word)
            {
                const int charWidth = m_config.charWidth(c);

                if (leftX + charWidth > m_size.width())
                {
                    leftX = hGap;
                    vline = &phase.grow();
                }

                view::Char &vc = vline->grow();
                vc.setUnicode(c);
                vc.setX(leftX);
                vc.setWidth(charWidth);

                leftX += charWidth;
                leftX += hMargin;
            }
        }
        else
        {
            int wordWidth = 0;
            for (const UChar c : word)
            {
                wordWidth += m_config.charWidth(c);
                wordWidth += hMargin;
            }
            if (leftX + wordWidth > m_size.width())
            {
                leftX = hGap;
                vline = &phase.grow();
            }
            for (const UChar c : word)
            {
                const int charWidth = m_config.charWidth(c);

                if (leftX + charWidth > m_size.width())
                {
                    leftX = hGap;
                    vline = &phase.grow();
                }

                view::Char &vc = vline->grow();
                vc.setUnicode(c);
                vc.setX(leftX);
                vc.setWidth(charWidth);

                leftX += charWidth;
                leftX += hMargin;
            }
        }
    }
}

void View::DocLineToViewPhaseNoWrapLine(const DocLine& line, view::Phase & phase)
{
    assert(phase.size() == 0);

    const int hGap = m_config.hGap();
    const int hMargin = m_config.hMargin();

    view::DocLine &vline = phase.grow();

    int leftX = hGap;

    const CharN cnt = line.charCnt();
    for (CharN i = 0; i < cnt; ++i)
    {
        const UChar c = line.charAt(i);
        const int charWidth = m_config.charWidth(c);

        view::Char &vchar = vline.grow();
        vchar.setUnicode(c);
        vchar.setX(leftX);
        vchar.setWidth(charWidth);

        leftX += charWidth;
        leftX += hMargin;
    }
}
