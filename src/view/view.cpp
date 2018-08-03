#include <cassert>
#include "view.h"
#include "view_config.h"
#include "doc/doc.h"
#include "doc/doc_line.h"
#include "text/doc_line_char_instream.h"
#include "text/txt_word_instream.h"
#include "editor/editor.h"

View::View(Editor * editor, view::Config *config)
    : m_editor(*editor)
    , m_config(*config)
{
    assert(editor);
    assert(config);
   
    m_listenerIdForLastActLineUpdate = m_editor.addOnLastActLineUpdateListener([this] {
        m_onUpdateListeners.call();
    });
}

View::~View()
{
    m_editor.removeOnLastActLineUpdateListener(m_listenerIdForLastActLineUpdate);
}

void View::initSize(const view::Size & size)
{
    m_size = size;

    remakePage();

    m_onUpdateListeners.call();
}

void View::onResize(const view::Size & size)
{
    if (m_size == size)
    {
        return;
    }

    m_size = size;

    remakePage();

    m_onUpdateListeners.call();
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
    const view::CharAddr charAddr = getCharAddrByPoint(x, y);
    const DocAddr docAddr = convertToDocAddr(charAddr);
    return docAddr;
}

view::PhaseAddr View::convertToPhaseAddr(LineN line) const
{
    const int phase = line - m_viewStart;
    const int phaseCnt = m_page.size();
    if (phase < 0 || phase >= phaseCnt)
    {
        if (line >= m_editor.doc().lineCnt())
        {
            return view::PhaseAddr::newPhaseAddrAfterLastPhase();
        }
        return view::PhaseAddr();
    }

    if (line >= m_editor.doc().lineCnt())
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

    int prevLineCharCnt = 0;

    const CharN col = docAddr.col();

    for (const view::Line &vline : vphase)
    {
        for (const view::Char &vc : vline)
        {
            if (charIndex == col)
            {
                return view::CharAddr(phase, lineIndex, col - prevLineCharCnt);
            }

            ++charIndex;
        }

        ++lineIndex;

        prevLineCharCnt += vline.size();
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
        if (charAddr.line() < m_page[charAddr.phase()].size() - 1)
        {
            // 如果不是最后一个显示行，则把光标放在下一个显示行最开始处
            return convertToDocAddr(view::CharAddr(charAddr.phase(), charAddr.line() + 1, 0));
        }
        else
        {
            return DocAddr::newCharAddrAfterLastChar(m_viewStart + charAddr.phase());
        }
    }

    
    const view::Phase & phase = m_page[charAddr.phase()];
    const int lineIndex = charAddr.line();
    CharN col = charAddr.col();
    for (int i = 0; i < lineIndex; ++i)
    {
        col += phase[i].size();
    }

    return DocAddr(m_viewStart + charAddr.phase(), col);
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
        const view::Line & line = m_page[charAddr.phase()][charAddr.line()];
        if (line.empty())
        {
            return m_config.hGap();
        }
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


void View::onDirUpKeyPress()
{
}

void View::onDirDownKeyPress()
{
}

void View::onDirLeftKeyPress()
{
    const DocAddr & newAddr = m_editor.getNextLeftAddrByChar(m_editor.normalCursor().to());
    if (!newAddr.isNull())
    {
        m_editor.setNormalCursor(newAddr);
    }
}

void View::onDirRightKeyPress()
{
    const DocAddr & newAddr = m_editor.getNextRightAddrByChar(m_editor.normalCursor().to());
    if (!newAddr.isNull())
    {
        m_editor.setNormalCursor(newAddr);
    }
}

view::Rect View::getLastActLineDrawRect() const
{
    const LineN phase = m_editor.lastActLine();
    const view::PhaseAddr addr = convertToPhaseAddr(phase);
    if (addr.isNull())
    {
        return view::Rect();
    }

    const view::PhaseBound bound = getPhaseBound(addr);

    view::Rect rect;
    rect.setNull(false);
    rect.setLeft(0);
    rect.setTop(bound.top());
    rect.setWidth(m_size.width());
    rect.setHeight(bound.height());
    return rect;
}

draw::VerticalLine View::getNormalCursorDrawData() const
{
    enum { kHorizontalDelta = -1 };
    enum { kVerticalShrink = 2 };

    const DocCursor & cursor = m_editor.normalCursor();

    if (cursor.isNull())
    {
        return draw::VerticalLine();
    }

    if (!cursor.isInsert())
    {
        return draw::VerticalLine();
    }

    const DocAddr & docAddr = cursor.addr();

    const view::CharAddr & charAddr = convertToCharAddr(docAddr);

    if (charAddr.isNull())
    {
        return draw::VerticalLine();
    }

    const view::LineBound bound = getLineBound(charAddr);

    const int x = getXByAddr(charAddr) + kHorizontalDelta;

    draw::VerticalLine vl;
    vl.setNull(false);
    vl.setX(x);
    vl.setTop(bound.top() + kVerticalShrink);
    vl.setBottom(bound.bottom() - kVerticalShrink);
    return vl;
}

int View::getLineNumBarWidth() const
{
    return 100;
}

void View::drawEachLineNum(std::function<void(LineN lineNum, int baseline, const view::PhaseBound & bound, bool isLastAct)> && action) const
{
    const int phaseCnt = m_page.size();

    int offset = 0;

    for (int phase = 0; phase < phaseCnt; ++phase)
    {
        const view::PhaseAddr addr(phase);
        const view::PhaseBound bound = getPhaseBound(addr);
        const LineN lineNum = m_viewStart + phase;
        const LineN lastAct = m_editor.lastActLine();
        const bool isLastAct = lineNum == lastAct;
        const int baseline = getBaseLineByLineOffset(offset);

        action(lineNum, baseline, bound, isLastAct);

        offset += m_page[phase].size();
    }
}

ListenerID View::addOnUpdateListener(std::function<void()>&& action)
{
    return m_onUpdateListeners.add(std::move(action));
}

void View::removeOnUpdateListener(ListenerID id)
{
    m_onUpdateListeners.remove(id);
}



void View::onPrimaryButtomPress(int x, int y)
{
    const DocAddr da = getDocAddrByPoint(x, y);
    m_editor.onPrimaryKeyPress(da);
}

void View::onDirKeyPress(Dir dir)
{
    switch (dir)
    {
    case Dir::Up: onDirUpKeyPress(); break;
    case Dir::Down: onDirDownKeyPress(); break;
    case Dir::Left: onDirLeftKeyPress(); break;
    case Dir::Right: onDirRightKeyPress(); break;
    default: break;
    }
}

int View::getLineOffsetByY(int y) const
{
    return y / m_config.lineHeight();
}

void View::remakePage()
{
    m_page.clear();

    const LineN maxShowLine = m_viewStart + m_size.height() / m_config.lineHeight() + 1;

    const LineN rowCnt = std::min(maxShowLine, m_editor.doc().lineCnt());

    for (LineN i = m_viewStart; i < rowCnt; ++i)
    {
        view::Phase &vphase = m_page.grow();

        DocLineToViewPhase(m_editor.doc().lineAt(i), vphase);
    }

}

void View::DocLineToViewPhase(const Line& line, view::Phase & phase)
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

void View::DocLineToViewPhaseWithWrapLine(const Line& line, view::Phase & phase)
{
    assert(phase.size() == 0);

    const int hGap = m_config.hGap();
    const int hMargin = m_config.hMargin();

    view::Line *vline = &phase.grow();

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

void View::DocLineToViewPhaseNoWrapLine(const Line& line, view::Phase & phase)
{
    assert(phase.size() == 0);

    const int hGap = m_config.hGap();
    const int hMargin = m_config.hMargin();

    view::Line &vline = phase.grow();

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

