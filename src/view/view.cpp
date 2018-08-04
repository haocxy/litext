#include <cassert>
#include "view.h"
#include "view_config.h"
#include "doc/doc.h"
#include "doc/doc_row.h"
#include "text/doc_line_char_instream.h"
#include "text/txt_word_instream.h"
#include "editor/editor.h"

View::View(Editor * editor, view::Config *config)
    : m_editor(*editor)
    , m_config(*config)
{
    assert(editor);
    assert(config);
   
    m_listenerIdForLastActLineUpdate = m_editor.addOnLastActRowUpdateListener([this] {
        m_onUpdateListeners.call();
    });
}

View::~View()
{
    m_editor.removeOnLastActRowUpdateListener(m_listenerIdForLastActLineUpdate);
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

    const int rowCnt = m_page.size();
    const int row = lineAddr.row();

    for (int i = 0; i < rowCnt && i < row; ++i)
    {
        sum += m_page[i].size();
    }

    return sum;
}

int View::getLineOffsetByRowIndex(int row) const
{
    int sum = 0;

    const int rowCnt = m_page.size();

    for (int i = 0; i < rowCnt && i < row; ++i)
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

view::RowAddr View::convertToRowAddr(RowN row) const
{
    const int vrowIndex = row - m_viewStart;
    const int vrowCnt = m_page.size();
    if (vrowIndex < 0 || vrowIndex >= vrowCnt)
    {
        if (row >= m_editor.doc().rowCnt())
        {
            return view::RowAddr::newRowAddrAfterLastRow();
        }
        return view::RowAddr();
    }

    if (row >= m_editor.doc().rowCnt())
    {
        return view::RowAddr::newRowAddrAfterLastRow();
    }

    return view::RowAddr(vrowIndex);
}

view::CharAddr View::convertToCharAddr(const DocAddr & docAddr) const
{
    if (docAddr.isNull())
    {
        return view::CharAddr();
    }

    if (docAddr.isAfterLastRow())
    {
        return view::CharAddr::newCharAddrAfterLastRow();
    }

    const int r = docAddr.row() - m_viewStart;
    const int rowCnt = m_page.size();
    if (r < 0 || r >= rowCnt)
    {
        return view::CharAddr();
    }

    const view::VRow & vrow = m_page[r];

    if (docAddr.isAfterLastChar())
    {
        view::LineAddr lineAddr(r, vrow.size() - 1);
        return view::CharAddr::newCharAddrAfterLastChar(lineAddr);
    }
    
    int lineIndex = 0;
    int charIndex = 0;

    int prevLineCharCnt = 0;

    const CharN col = docAddr.col();

    for (const view::Line &vline : vrow)
    {
        for (const view::Char &vc : vline)
        {
            if (charIndex == col)
            {
                return view::CharAddr(r, lineIndex, col - prevLineCharCnt);
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

    if (charAddr.isAfterLastRow())
    {
        return DocAddr::newDocAddrAfterLastRow();
    }

    if (charAddr.isAfterLastChar())
    {
        if (charAddr.line() < m_page[charAddr.row()].size() - 1)
        {
            // 如果不是最后一个显示行，则把光标放在下一个显示行最开始处
            return convertToDocAddr(view::CharAddr(charAddr.row(), charAddr.line() + 1, 0));
        }
        else
        {
            return DocAddr::newDocAddrAfterLastChar(m_viewStart + charAddr.row());
        }
    }

    
    const view::VRow & vrow = m_page[charAddr.row()];
    const int lineIndex = charAddr.line();
    CharN col = charAddr.col();
    for (int i = 0; i < lineIndex; ++i)
    {
        col += vrow[i].size();
    }

    return DocAddr(m_viewStart + charAddr.row(), col);
}

const view::Char & View::getChar(const view::CharAddr & charAddr) const
{
    return m_page[charAddr.row()][charAddr.line()][charAddr.col()];
}

int View::getXByAddr(const view::CharAddr & charAddr) const
{
    if (charAddr.isNull())
    {
        return 0;
    }

    if (charAddr.isAfterLastRow())
    {
        return m_config.hGap();
    }

    if (charAddr.isAfterLastChar())
    {
        const view::Line & line = m_page[charAddr.row()][charAddr.line()];
        if (line.empty())
        {
            return m_config.hGap();
        }
        const view::Char &vc = m_page[charAddr.row()][charAddr.line()].last();
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
    if (lineAddr.isAfterLastRow())
    {
        return getLineBoundByLineOffset(m_page.lineCnt());
    }
    const int lineOffset = getLineOffsetByLineAddr(lineAddr);
    return getLineBoundByLineOffset(lineOffset);
}

view::RowBound View::getRowBound(const view::RowAddr & rowAddr) const
{
    if (rowAddr.isNull())
    {
        return view::RowBound();
    }

    if (rowAddr.isAfterLastRow())
    {
        const int lineOffset = m_page.lineCnt();
        const int lineHeight = m_config.lineHeight();
        const int top = lineHeight * lineOffset;
        return view::RowBound(top, lineHeight);
    }

    const int lineOffset = getLineOffsetByRowIndex(rowAddr.row());
    const int lineHeight = m_config.lineHeight();
    const int top = lineHeight * lineOffset;
    const int height = lineHeight * m_page[rowAddr.row()].size();
    return view::RowBound(top, height);
}


DocAddr View::getNextUpAddr(const DocAddr & addr) const
{
    const view::CharAddr charAddr = convertToCharAddr(addr);
    if (charAddr.isNull())
    {
        return DocAddr();
    }

    const view::LineAddr upLineAddr = m_page.getNextUpLineAddr(charAddr);
    const view::CharAddr upCharAddr = m_page.getCharAddrByLineAddrAndX(upLineAddr, m_stable_x);

    return convertToDocAddr(upCharAddr);
}

DocAddr View::getNextDownAddr(const DocAddr & addr) const
{
    const view::CharAddr charAddr = convertToCharAddr(addr);
    if (charAddr.isNull())
    {
        return DocAddr();
    }

    const view::LineAddr downLineAddr = m_page.getNextDownLineAddr(charAddr);
    const view::CharAddr downCharAddr = m_page.getCharAddrByLineAddrAndX(downLineAddr, m_stable_x);

    return convertToDocAddr(downCharAddr);
}

void View::onDirUpKeyPress()
{
    m_stable_x = 200; // TODO test data

    const DocAddr & newAddr = getNextUpAddr(m_editor.normalCursor().to());
    if (!newAddr.isNull())
    {
        m_editor.setNormalCursor(newAddr);
    }
}

void View::onDirDownKeyPress()
{
    m_stable_x = 200; // TODO test data

    const DocAddr & newAddr = getNextDownAddr(m_editor.normalCursor().to());
    if (!newAddr.isNull())
    {
        m_editor.setNormalCursor(newAddr);
    }
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
    const RowN row = m_editor.lastActRow();
    const view::RowAddr addr = convertToRowAddr(row);
    if (addr.isNull())
    {
        return view::Rect();
    }

    const view::RowBound bound = getRowBound(addr);

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

void View::drawEachLineNum(std::function<void(RowN lineNum, int baseline, const view::RowBound & bound, bool isLastAct)> && action) const
{
    const int rowCnt = m_page.size();

    int offset = 0;

    for (int r = 0; r < rowCnt; ++r)
    {
        const view::RowAddr addr(r);
        const view::RowBound bound = getRowBound(addr);
        const RowN lineNum = m_viewStart + r;
        const RowN lastAct = m_editor.lastActRow();
        const bool isLastAct = lineNum == lastAct;
        const int baseline = getBaseLineByLineOffset(offset);

        action(lineNum, baseline, bound, isLastAct);

        offset += m_page[r].size();
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

    const RowN maxShowLine = m_viewStart + m_size.height() / m_config.lineHeight() + 1;

    const RowN rowCnt = std::min(maxShowLine, m_editor.doc().rowCnt());

    for (RowN i = m_viewStart; i < rowCnt; ++i)
    {
        view::VRow & vrow = m_page.grow();

        makeVRow(m_editor.doc().rowAt(i), vrow);
    }

}

void View::makeVRow(const Row & row, view::VRow & vrow)
{
    if (m_config.wrapLine())
    {
        makeVRowWithWrapLine(row, vrow);
    }
    else
    {
        makeVRowNoWrapLine(row, vrow);
    }
}

void View::makeVRowWithWrapLine(const Row & row, view::VRow & vrow)
{
    assert(vrow.size() == 0);

    const int hGap = m_config.hGap();
    const int hMargin = m_config.hMargin();

    view::Line *vline = &vrow.grow();

    DocLineCharInStream charStream(row);
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
                    vline = &vrow.grow();
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
                vline = &vrow.grow();
            }
            for (const UChar c : word)
            {
                const int charWidth = m_config.charWidth(c);

                if (leftX + charWidth > m_size.width())
                {
                    leftX = hGap;
                    vline = &vrow.grow();
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

void View::makeVRowNoWrapLine(const Row & row, view::VRow & vrow)
{
    assert(vrow.size() == 0);

    const int hGap = m_config.hGap();
    const int hMargin = m_config.hMargin();

    view::Line &vline = vrow.grow();

    int leftX = hGap;

    const CharN cnt = row.charCnt();
    for (CharN i = 0; i < cnt; ++i)
    {
        const UChar c = row.charAt(i);
        const int charWidth = m_config.charWidth(c);

        view::Char &vchar = vline.grow();
        vchar.setUnicode(c);
        vchar.setX(leftX);
        vchar.setWidth(charWidth);

        leftX += charWidth;
        leftX += hMargin;
    }
}

