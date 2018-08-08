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

int View::getLineOffsetByLineLoc(const view::LineLoc & loc) const
{
    int sum = loc.line();

    const int rowCnt = m_page.size();
    const int row = loc.row();

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

view::CharLoc View::getCharLocByPoint(int x, int y) const
{
    const int lineOffset = getLineOffsetByY(y);
    const view::LineLoc lineLoc = m_page.getLineLocByLineOffset(lineOffset);
    return m_page.getCharLocByLineLocAndX(lineLoc, x);
}

DocLoc View::getDocLocByPoint(int x, int y) const
{
    const view::CharLoc charLoc = getCharLocByPoint(x, y);
    const DocLoc docLoc = convertToDocLoc(charLoc);
    return docLoc;
}

view::RowLoc View::convertToRowLoc(RowN row) const
{
    const int vrowIndex = row - m_loc.row();
    const int vrowCnt = m_page.size();
    if (vrowIndex < 0 || vrowIndex >= vrowCnt)
    {
        if (row >= m_editor.doc().rowCnt())
        {
            return view::RowLoc::newRowLocAfterLastRow();
        }
        return view::RowLoc();
    }

    if (row >= m_editor.doc().rowCnt())
    {
        return view::RowLoc::newRowLocAfterLastRow();
    }

    return view::RowLoc(vrowIndex);
}

view::CharLoc View::convertToCharLoc(const DocLoc & docLoc) const
{
    if (docLoc.isNull())
    {
        return view::CharLoc();
    }

    if (docLoc.isAfterLastRow())
    {
        return view::CharLoc::newCharLocAfterLastRow();
    }

    const int r = docLoc.row() - m_loc.row();
    const int rowCnt = m_page.size();
    if (r < 0 || r >= rowCnt)
    {
        return view::CharLoc();
    }

    const view::VRow & vrow = m_page[r];

    if (docLoc.isAfterLastChar())
    {
        view::LineLoc lineLoc(r, vrow.size() - 1);
        return view::CharLoc::newCharLocAfterLastChar(lineLoc);
    }
    
    int lineIndex = 0;
    int charIndex = 0;

    int prevLineCharCnt = 0;

    const CharN col = docLoc.col();

    for (const view::Line &vline : vrow)
    {
        for (const view::Char &vc : vline)
        {
            if (charIndex == col)
            {
                return view::CharLoc(r, lineIndex, col - prevLineCharCnt);
            }

            ++charIndex;
        }

        ++lineIndex;

        prevLineCharCnt += vline.size();
    }

    return view::CharLoc();
}

DocLoc View::convertToDocLoc(const view::CharLoc & charLoc) const
{
    if (charLoc.isNull())
    {
        return DocLoc();
    }

    if (charLoc.isAfterLastRow())
    {
        return DocLoc::newDocLocAfterLastRow();
    }

    if (charLoc.isAfterLastChar())
    {
        if (charLoc.line() < m_page[charLoc.row()].size() - 1)
        {
            // 如果不是最后一个显示行，则把光标放在下一个显示行最开始处
            return convertToDocLoc(view::CharLoc(charLoc.row(), charLoc.line() + 1, 0));
        }
        else
        {
            return DocLoc::newDocLocAfterLastChar(m_loc.row() + charLoc.row());
        }
    }

    
    const view::VRow & vrow = m_page[charLoc.row()];
    const int lineIndex = charLoc.line();
    CharN col = charLoc.col();
    for (int i = 0; i < lineIndex; ++i)
    {
        col += vrow[i].size();
    }

    return DocLoc(m_loc.row() + charLoc.row(), col);
}

const view::Char & View::getChar(const view::CharLoc & charLoc) const
{
    return m_page[charLoc.row()][charLoc.line()][charLoc.col()];
}

int View::getXByCharLoc(const view::CharLoc & charLoc) const
{
    if (charLoc.isNull())
    {
        return 0;
    }

    if (charLoc.isAfterLastRow())
    {
        return m_config.hGap();
    }

    if (charLoc.isAfterLastChar())
    {
        const view::Line & line = m_page[charLoc.row()][charLoc.line()];
        if (line.empty())
        {
            return m_config.hGap();
        }
        const view::Char &vc = m_page[charLoc.row()][charLoc.line()].last();
        return vc.x() + vc.width();
    }

    return getChar(charLoc).x();
}

view::LineBound View::getLineBoundByLineOffset(int lineOffset) const
{
    const int lineHeight = m_config.lineHeight();
    const int top = lineHeight * lineOffset;
    const int bottom = top + lineHeight;

    return view::LineBound(top, bottom);
}

view::LineBound View::getLineBound(const view::LineLoc & lineLoc) const
{
    if (lineLoc.isAfterLastRow())
    {
        return getLineBoundByLineOffset(m_page.lineCnt());
    }
    const int lineOffset = getLineOffsetByLineLoc(lineLoc);
    return getLineBoundByLineOffset(lineOffset);
}

view::RowBound View::getRowBound(const view::RowLoc & rowLoc) const
{
    if (rowLoc.isNull())
    {
        return view::RowBound();
    }

    if (rowLoc.isAfterLastRow())
    {
        const int lineOffset = m_page.lineCnt();
        const int lineHeight = m_config.lineHeight();
        const int top = lineHeight * lineOffset;
        return view::RowBound(top, lineHeight);
    }

    const int lineOffset = getLineOffsetByRowIndex(rowLoc.row());
    const int lineHeight = m_config.lineHeight();
    const int top = lineHeight * lineOffset;
    const int height = lineHeight * m_page[rowLoc.row()].size();
    return view::RowBound(top, height);
}


DocLoc View::getNextUpLoc(const DocLoc & docLoc) const
{
    const view::CharLoc charLoc = convertToCharLoc(docLoc);
    if (charLoc.isNull())
    {
        return DocLoc();
    }

    const view::LineLoc upLineLoc = m_page.getNextUpLineLoc(charLoc);
    const view::CharLoc upCharLoc = m_page.getCharLocByLineLocAndX(upLineLoc, m_stable_x);

    return convertToDocLoc(upCharLoc);
}

DocLoc View::getNextDownLoc(const DocLoc & docLoc) const
{
    const view::CharLoc charLoc = convertToCharLoc(docLoc);
    if (charLoc.isNull())
    {
        return DocLoc();
    }

    const view::LineLoc downLineLoc = m_page.getNextDownLineLoc(charLoc);
    const view::CharLoc downCharLoc = m_page.getCharLocByLineLocAndX(downLineLoc, m_stable_x);

    return convertToDocLoc(downCharLoc);
}

void View::onDirUpKeyPress()
{
    const DocLoc newLoc = getNextUpLoc(m_editor.normalCursor().to());
    if (!newLoc.isNull())
    {
        m_editor.setNormalCursor(newLoc);
    }
}

void View::onDirDownKeyPress()
{
    const DocLoc newLoc = getNextDownLoc(m_editor.normalCursor().to());
    if (!newLoc.isNull())
    {
        m_editor.setNormalCursor(newLoc);
    }
}

void View::onDirLeftKeyPress()
{
    const DocLoc newLoc = m_editor.getNextLeftLocByChar(m_editor.normalCursor().to());
    if (newLoc.isNull())
    {
        return;
    }
    
    m_editor.setNormalCursor(newLoc);
    
    const view::CharLoc charLoc = convertToCharLoc(newLoc);
    m_stable_x = getXByCharLoc(charLoc);
}

void View::onDirRightKeyPress()
{
    const DocLoc newLoc = m_editor.getNextRightLocByChar(m_editor.normalCursor().to());
    if (newLoc.isNull())
    {
        return;
    }
    
    m_editor.setNormalCursor(newLoc);

    const view::CharLoc charLoc = convertToCharLoc(newLoc);
    m_stable_x = getXByCharLoc(charLoc);
}

view::Rect View::getLastActLineDrawRect() const
{
    const RowN row = m_editor.lastActRow();
    const view::RowLoc loc = convertToRowLoc(row);
    if (loc.isNull())
    {
        return view::Rect();
    }

    const view::RowBound bound = getRowBound(loc);

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

    const DocLoc & docLoc = cursor.loc();

    const view::CharLoc charLoc = convertToCharLoc(docLoc);

    if (charLoc.isNull())
    {
        return draw::VerticalLine();
    }

    const view::LineBound bound = getLineBound(charLoc);

    const int x = getXByCharLoc(charLoc) + kHorizontalDelta;

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
        const view::RowLoc loc(r);
        const view::RowBound bound = getRowBound(loc);
        const RowN lineNum = m_loc.row() + r;
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
    const view::CharLoc charLoc = getCharLocByPoint(x, y);
    const DocLoc docLoc = convertToDocLoc(charLoc);
    m_editor.onPrimaryKeyPress(docLoc);

    m_stable_x = getXByCharLoc(charLoc);
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

    const RowN maxShowLine = m_loc.row() + m_size.height() / m_config.lineHeight() + 1;

    const RowN rowCnt = std::min(maxShowLine, m_editor.doc().rowCnt());

    for (RowN i = m_loc.row(); i < rowCnt; ++i)
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

