#include "text_area.h"

#include <cassert>


#include "doc/doc.h"
#include "doc/doc_row.h"
#include "text/doc_line_char_instream.h"
#include "text/txt_word_instream.h"
#include "editor/editor.h"
#include "text_area_config.h"


TextArea::TextArea(Editor *editor, view::Config *config)
    : editor_(*editor)
    , config_(*config)
{
    assert(editor);
    assert(config);
   
    m_listenerIdForLastActLineUpdate = editor_.addOnLastActRowUpdateListener([this] {
        m_onUpdateListeners.call();
    });
}

TextArea::~TextArea()
{

}

void TextArea::initSize(const view::Size & size)
{
    m_size = size;

    remakePage();

    m_onUpdateListeners.call();
}

void TextArea::resize(const view::Size & size)
{
    if (m_size == size)
    {
        return;
    }

    m_size = size;

    remakePage();

    m_onUpdateListeners.call();
}

int TextArea::getBaseLineByLineOffset(int off) const
{
    return (1 + off) * config_.lineHeight() - config_.font().descent();
}

int TextArea::getLineOffsetByLineLoc(const view::LineLoc & loc) const
{
    int sum = loc.line();

    const int rowCnt = m_page.size();
    const int row = loc.row();

    for (int i = 0; i < rowCnt && i < row; ++i)
    {
        sum += m_page[i].size();
    }

    sum -= m_loc.line();
    
    return sum;
}

int TextArea::getMaxShownLineCnt() const
{
    const int lineHeight = config_.lineHeight();
    return (m_size.height() + lineHeight - 1) / lineHeight;
}

int TextArea::getLineOffsetByRowIndex(int row) const
{
    int sum = 0;

    const int rowCnt = m_page.size();

    for (int i = 0; i < rowCnt && i < row; ++i)
    {
        sum += m_page[i].size();
    }

    sum -= m_loc.line();

    return sum;
}

view::CharLoc TextArea::getCharLocByPoint(int x, int y) const
{
    const int lineOffset = getLineOffsetByY(y);
    const view::LineLoc lineLoc = getLineLocByLineOffset(lineOffset);
    return getCharLocByLineLocAndX(lineLoc, x);
}

DocLoc TextArea::getDocLocByPoint(int x, int y) const
{
    const view::CharLoc charLoc = getCharLocByPoint(x, y);
    const DocLoc docLoc = convertToDocLoc(charLoc);
    return docLoc;
}

view::RowLoc TextArea::convertToRowLoc(RowN row) const
{
    const int vrowIndex = row - m_loc.row();
    const int vrowCnt = m_page.size();
    if (vrowIndex < 0 || vrowIndex >= vrowCnt)
    {
        if (row >= editor_.doc().rowCnt())
        {
            return view::RowLoc::newRowLocAfterLastRow();
        }
        return view::RowLoc();
    }

    if (row >= editor_.doc().rowCnt())
    {
        return view::RowLoc::newRowLocAfterLastRow();
    }

    return view::RowLoc(vrowIndex);
}

view::CharLoc TextArea::convertToCharLoc(const DocLoc & docLoc) const
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

DocLoc TextArea::convertToDocLoc(const view::CharLoc & charLoc) const
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
        if (isLastLineOfRow(charLoc))
        {
            return DocLoc::newDocLocAfterLastChar(m_loc.row() + charLoc.row());
        }
        else
        {
            // 如果不是最后一个显示行，则把光标放在下一个显示行最开始处
            return convertToDocLoc(view::CharLoc(charLoc.row(), charLoc.line() + 1, 0));
        }
    }

    
    const view::VRow & vrow = m_page[charLoc.row()];
    int lastLine = charLoc.line();
    if (m_loc.row() == 0)
    {
        // 如果在第一个VRow，则需要把偏移量加上，因为ViewLoc中的line属性为行偏移
        lastLine += m_loc.line();
    }
    CharN col = charLoc.col();
    for (int i = 0; i < lastLine; ++i)
    {
        col += vrow[i].size();
    }

    return DocLoc(m_loc.row() + charLoc.row(), col);
}

const view::Char &TextArea::getChar(const view::CharLoc & charLoc) const
{
    return m_page[charLoc.row()][charLoc.line()][charLoc.col()];
}

int TextArea::getXByCharLoc(const view::CharLoc & charLoc) const
{
    if (charLoc.isNull())
    {
        return 0;
    }

    if (charLoc.isAfterLastRow())
    {
        return config_.hGap();
    }

    if (charLoc.isAfterLastChar())
    {
        const view::Line & line = m_page[charLoc.row()][charLoc.line()];
        if (line.empty())
        {
            return config_.hGap();
        }
        const view::Char &vc = m_page[charLoc.row()][charLoc.line()].last();
        return vc.x() + vc.width();
    }

    return getChar(charLoc).x();
}

view::LineBound TextArea::getLineBoundByLineOffset(int lineOffset) const
{
    const int lineHeight = config_.lineHeight();
    const int top = lineHeight * lineOffset;
    const int bottom = top + lineHeight;

    return view::LineBound(top, bottom);
}

view::LineBound TextArea::getLineBound(const view::LineLoc & lineLoc) const
{
    if (lineLoc.isAfterLastRow())
    {
        return getLineBoundByLineOffset(m_page.lineCnt());
    }
    const int lineOffset = getLineOffsetByLineLoc(lineLoc);
    return getLineBoundByLineOffset(lineOffset);
}

view::RowBound TextArea::getRowBound(const view::RowLoc & rowLoc) const
{
    if (rowLoc.isNull())
    {
        return view::RowBound();
    }

    if (rowLoc.isAfterLastRow())
    {
        const int lineOffset = m_page.lineCnt();
        const int lineHeight = config_.lineHeight();
        const int top = lineHeight * lineOffset;
        return view::RowBound(top, lineHeight);
    }

    const int lineOffset = getLineOffsetByRowIndex(rowLoc.row());
    const int lineHeight = config_.lineHeight();
    const int top = lineHeight * lineOffset;
    const int height = lineHeight * m_page[rowLoc.row()].size();
    return view::RowBound(top, height);
}

bool TextArea::hasPrevCharAtSameLine(const view::CharLoc & charLoc) const
{
    return !noPrevCharAtSameLine(charLoc);
}

bool TextArea::noPrevCharAtSameLine(const view::CharLoc & charLoc) const
{
    if (charLoc.isAfterLastChar())
    {
        return m_page.getLine(charLoc).size() == 0;
    }
    else
    {
        return charLoc.col() == 0;
    }
}

bool TextArea::hasNextCharAtSameLine(const view::CharLoc & charLoc) const
{
    return !noNextCharAtSameLine(charLoc);
}

bool TextArea::noNextCharAtSameLine(const view::CharLoc & charLoc) const
{
    if (charLoc.isAfterLastChar())
    {
        return true;
    }
    else
    {
		const view::Line & line = m_page.getLine(charLoc);
        return charLoc.col() == line.size();
    }
}

bool TextArea::needEnsureHasNextLine(const view::CharLoc & charLoc) const
{
	if (hasNextCharAtSameLine(charLoc))
	{
		const view::VRow & vrow = m_page[charLoc.row()];
		if (charLoc.line() < vrow.size() - 1)
		{
			const view::Line & line = vrow[charLoc.line()];
			return charLoc.col() >= line.size() - 1;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return true;
	}
}

bool TextArea::isLastLineOfRow(const view::LineLoc & lineLoc) const
{
    return lineLoc.line() == m_page[lineLoc.row()].size() - 1;
}

bool TextArea::isEndOfVirtualLine(const view::CharLoc & charLoc) const
{
    return charLoc.isAfterLastChar() && !isLastLineOfRow(charLoc);
}

// 避免出现因为定位到虚拟行尾部而使虚拟行被跳过的情况
view::CharLoc TextArea::betterLocForVerticalMove(const view::CharLoc & charLoc) const
{
    if (!isEndOfVirtualLine(charLoc))
    {
        return charLoc;
    }

    const view::Line & line = m_page.getLine(charLoc);
    const CharN charCnt = line.size();

    if (charCnt != 0)
    {
        return view::CharLoc(charLoc.row(), charLoc.line(), charCnt - 1);
    }

    return charLoc;
}

DocLoc TextArea::getNextUpLoc(const DocLoc & docLoc) const
{
    const view::CharLoc charLoc = convertToCharLoc(docLoc);
    if (charLoc.isNull())
    {
        return DocLoc();
    }

    const view::LineLoc upLineLoc = m_page.getNextUpLineLoc(charLoc);
    const view::CharLoc upCharLoc = getCharLocByLineLocAndX(upLineLoc, m_stable_x);

    return convertToDocLoc(betterLocForVerticalMove(upCharLoc));
}

DocLoc TextArea::getNextDownLoc(const DocLoc & docLoc) const
{
    const view::CharLoc charLoc = convertToCharLoc(docLoc);
    if (charLoc.isNull())
    {
        return DocLoc();
    }

    const view::LineLoc downLineLoc = m_page.getNextDownLineLoc(charLoc);
    const view::CharLoc downCharLoc = getCharLocByLineLocAndX(downLineLoc, m_stable_x);

    return convertToDocLoc(betterLocForVerticalMove(downCharLoc));
}

void TextArea::ensureHasPrevLine(const view::LineLoc & curLineLoc)
{
    if (curLineLoc.row() == 0 && curLineLoc.line() <= m_loc.line())
    {
        if (m_loc.line() > 0)
        {
            setViewLoc(ViewLoc(m_loc.row(), m_loc.line() - 1));
        }
        else
        {
            if (m_loc.row() > 0)
            {
                view::VRow vrow;
                makeVRow(editor_.doc().rowAt(m_loc.row() - 1), vrow);
                const int newRowSize = vrow.size();
                m_page.pushFront(std::move(vrow));

                setViewLoc(ViewLoc(m_loc.row() - 1, newRowSize - 1));
            }
        }
    }
}

bool TextArea::ensureHasNextLine(const view::LineLoc & curLineLoc)
{
	const int maxShownLineCnt = getMaxShownLineCnt();

	const int rowCnt = m_page.size();

	// 没有内容则返回
	if (rowCnt == 0)
	{
		return false;
	}

	// 当前视图坐标不在视图最后则返回
	if (curLineLoc.row() < rowCnt - 1)
	{
		return false;
	}

	// 当前坐标为文档最后一行则返回
	const RowN docRowCnt = editor_.doc().rowCnt();
	if (m_loc.row() + curLineLoc.row() >= docRowCnt - 1)
	{
		return false;
	}

	const view::VRow & lastRow = m_page[rowCnt - 1];

	const int prevLineCnt = m_page.lineCnt() - m_loc.line() - lastRow.size();

	const int lineIndex = maxShownLineCnt - prevLineCnt - 1;

	if (isLastLineOfRow(view::LineLoc(rowCnt - 1, lineIndex)))
	{
		const RowN newDocRowIndex = m_loc.row() + rowCnt;
		if (newDocRowIndex > docRowCnt - 1)
		{
			return false;
		}

		const Row & docRow = editor_.doc().rowAt(newDocRowIndex);
		view::VRow vrow;
		makeVRow(docRow, vrow);
		m_page.pushBack(std::move(vrow));
	}

	return true;
}

void TextArea::removeSpareRow()
{
    const int maxShownLineCnt = getMaxShownLineCnt();

    int remainLineCnt = m_page.lineCnt() - m_loc.line();

    for (int r = m_page.size() - 1; r >= 0; --r)
    {
        remainLineCnt -= m_page[r].size();
        if (remainLineCnt >= maxShownLineCnt)
        {
            m_page.popBack();
        }
        else
        {
            break;
        }
    }
}

void TextArea::onDirUpKeyPress()
{
	// TODO !!!
	// 如果光标位置不在视图内，则只做一件事：把文档视图滚动到恰好显示光标所在行

    // 第一步：确保上一行在视图内
    // 注意，在第一步完成后，第一步依赖的视图元素位置可能已经失效，第二步需要重新获取
    const DocLoc & oldDocLoc = editor_.normalCursor().to();
    const view::CharLoc oldCharLoc = convertToCharLoc(oldDocLoc);
    ensureHasPrevLine(oldCharLoc);

    // 第二步：使用第一步更新后的新的视图数据取目标位置
    const DocLoc newLoc = getNextUpLoc(editor_.normalCursor().to());
    if (!newLoc.isNull())
    {
        editor_.setNormalCursor(newLoc);
    }

	// 第三步：移除尾部多余的行
	removeSpareRow();
}

void TextArea::onDirDownKeyPress()
{
	// 第一步，确保当前位置在视图中有下一行
	const DocLoc & oldDocLoc = editor_.normalCursor().to();
	const view::CharLoc oldCharLoc = convertToCharLoc(oldDocLoc);
	const bool shouldMovePageHead = ensureHasNextLine(oldCharLoc);

	// 第二步，更新编辑器中的文档位置
	const DocLoc newLoc = getNextDownLoc(editor_.normalCursor().to());
	if (!newLoc.isNull() && !newLoc.isAfterLastRow())
	{
        editor_.setNormalCursor(newLoc);
	}

	// 第三步，向下移动页面头部
	if (shouldMovePageHead)
	{
		movePageHeadOneLine();
	}
}

void TextArea::onDirLeftKeyPress()
{
    const DocLoc oldDocLoc = editor_.normalCursor().to();
    const view::CharLoc oldCharLoc = convertToCharLoc(oldDocLoc);
    if (noPrevCharAtSameLine(oldCharLoc))
    {
        ensureHasPrevLine(oldCharLoc);
    }

    const DocLoc newLoc = editor_.getNextLeftLocByChar(oldDocLoc);
    if (!newLoc.isNull())
    {
        editor_.setNormalCursor(newLoc);

		const view::CharLoc charLoc = convertToCharLoc(newLoc);
		m_stable_x = getXByCharLoc(charLoc);
    }
    
	removeSpareRow();
}

void TextArea::onDirRightKeyPress()
{
	const DocLoc oldDocLoc = editor_.normalCursor().to();
	const view::CharLoc oldCharLoc = convertToCharLoc(oldDocLoc);
	bool needMoveHead = false;
	if (needEnsureHasNextLine(oldCharLoc))
	{
		needMoveHead = ensureHasNextLine(oldCharLoc);
	}

	const DocLoc newLoc = editor_.getNextRightLocByChar(oldDocLoc);
	if (!newLoc.isNull())
	{
        editor_.setNormalCursor(newLoc);

		const view::CharLoc charLoc = convertToCharLoc(newLoc);
		m_stable_x = getXByCharLoc(charLoc);
	}

	if (needMoveHead)
	{
		movePageHeadOneLine();
	}
}

void TextArea::setViewLoc(const ViewLoc & viewLoc)
{
    if (m_loc == viewLoc)
    {
        return;
    }

    m_loc = viewLoc;

    m_onViewLocChangeListeners.call();
}

void TextArea::movePageHeadOneLine()
{
	if (isLastLineOfRow(view::LineLoc(0, m_loc.line())))
	{
		setViewLoc(ViewLoc(m_loc.row() + 1, 0));
		m_page.popFront();
	}
	else
	{
		setViewLoc(ViewLoc(m_loc.row(), m_loc.line() + 1));
	}
}

std::optional<view::Rect> TextArea::getLastActLineDrawRect() const
{
    const RowN row = editor_.lastActRow();
    const view::RowLoc loc = convertToRowLoc(row);
    if (loc.isNull())
    {
        return std::nullopt;
    }

    const view::RowBound bound = getRowBound(loc);

    view::Rect rect;
    rect.setLeft(0);
    rect.setTop(bound.top());
    rect.setWidth(m_size.width());
    rect.setHeight(bound.height());
    return rect;
}

std::optional<draw::VerticalLine> TextArea::getNormalCursorDrawData() const
{
    enum { kHorizontalDelta = -1 };
    enum { kVerticalShrink = 2 };

    const DocCursor & cursor = editor_.normalCursor();

    if (cursor.isNull())
    {
        return std::nullopt;
    }

    if (!cursor.isInsert())
    {
        return std::nullopt;
    }

    const DocLoc & docLoc = cursor.loc();

    const view::CharLoc charLoc = convertToCharLoc(docLoc);

    if (charLoc.isNull())
    {
        return std::nullopt;
    }

    const view::LineBound bound = getLineBound(charLoc);

    const int x = getXByCharLoc(charLoc) + kHorizontalDelta;

    draw::VerticalLine vl;
    vl.setX(x);
    vl.setTop(bound.top() + kVerticalShrink);
    vl.setBottom(bound.bottom() - kVerticalShrink);
    return vl;
}

int TextArea::getLineNumBarWidth() const
{
    return 100;
}

void TextArea::drawEachLineNum(std::function<void(RowN lineNum, int baseline, const view::RowBound & bound, bool isLastAct)> && action) const
{
    const int rowCnt = m_page.size();

    int offset = -m_loc.line();

    for (int r = 0; r < rowCnt; ++r)
    {
        const view::RowLoc loc(r);
        const view::RowBound bound = getRowBound(loc);
        const RowN lineNum = m_loc.row() + r;
        const RowN lastAct = editor_.lastActRow();
        const bool isLastAct = lineNum == lastAct;
        const int baseline = getBaseLineByLineOffset(offset);

        action(lineNum, baseline, bound, isLastAct);

        offset += m_page[r].size();
    }
}

void TextArea::drawEachChar(std::function<void(int x, int y, UChar c)>&& action) const
{
    const int rowCnt = m_page.size();
    if (rowCnt == 0)
    {
        return;
    }

    const view::VRow & curRow = m_page[0];
    const int curRowSize = curRow.size();

    int lineOffset = 0;

    for (int i = m_loc.line(); i < curRowSize; ++i)
    {
        const view::Line & line = curRow[i];

        const int baseline = getBaseLineByLineOffset(lineOffset);

        for (const view::Char & c : line)
        {
            action(c.x(), baseline, c.uchar());
        }

        ++lineOffset;
    }

    for (int r = 1; r < rowCnt; ++r)
    {
        const view::VRow & row = m_page[r];

        for (const view::Line & line : row)
        {
            const int baseline = getBaseLineByLineOffset(lineOffset);

            for (const view::Char & c : line)
            {
                action(c.x(), baseline, c.uchar());
            }

            ++lineOffset;
        }
    }
}

CallbackHandle TextArea::addOnUpdateListener(std::function<void()>&& action)
{
    return m_onUpdateListeners.add(std::move(action));
}

CallbackHandle TextArea::addOnViewLocChangeListener(std::function<void()>&& action)
{
    return m_onViewLocChangeListeners.add(std::move(action));
}

void TextArea::onPrimaryButtomPress(int x, int y)
{
    const view::CharLoc charLoc = getCharLocByPoint(x, y);
    const DocLoc docLoc = convertToDocLoc(charLoc);
    editor_.onPrimaryKeyPress(docLoc);

    m_stable_x = getXByCharLoc(charLoc);
}

bool TextArea::moveDownByOneLine()
{
	const RowN docRowCnt = editor_.doc().rowCnt();

	// 文档没有内容则不能移动
	if (docRowCnt == 0)
	{
		return false;
	}

	// 视图位于文档最后一个line
	if (m_loc.row() == docRowCnt - 1 && isLastLineOfRow(view::LineLoc(m_page.size() - 1, m_loc.line())))
	{
		return false;
	}

	// 如果最后一个可视line是row中最后一个line，则需要新解析一个row的内容
	const view::LineLoc shownLastLineLoc = getShownLastLineLoc();
	if (isLastLineOfRow(shownLastLineLoc))
	{
		const RowN newDocRowIndex = m_loc.row() + m_page.size();
		if (newDocRowIndex <= docRowCnt - 1)
		{
			const Row & docRow = editor_.doc().rowAt(newDocRowIndex);
			view::VRow vrow;
			makeVRow(docRow, vrow);
			m_page.pushBack(std::move(vrow));
		}
	}

	movePageHeadOneLine();

	return true;
}

int TextArea::getLineOffsetByY(int y) const
{
    return y / config_.lineHeight();
}

view::LineLoc TextArea::getLineLocByLineOffset(int offset) const
{
    const int rowCnt = m_page.size();

    if (m_loc.line() == 0)
    {
        int sum = 0;

        for (int i = 0; i < rowCnt; ++i)
        {
            const view::VRow &row = m_page[i];
            const int lineCnt = row.size();

            if (sum + lineCnt > offset)
            {
                return view::LineLoc(i, offset - sum);
            }

            sum += lineCnt;
        }

        return view::LineLoc::newLineLocAfterLastRow();
    }

    if (rowCnt == 0)
    {
        return view::LineLoc();
    }

    const view::VRow & curRow = m_page[0];
    const int curRowSize = curRow.size();
    if (m_loc.line() + offset < curRowSize)
    {
        return view::LineLoc(0, m_loc.line() + offset);
    }

    int sum = curRowSize - m_loc.line();

    for (int i = 1; i < rowCnt; ++i)
    {
        const view::VRow &row = m_page[i];
        const int lineCnt = row.size();

        if (sum + lineCnt > offset)
        {
            return view::LineLoc(i, offset - sum);
        }

        sum += lineCnt;
    }

    return view::LineLoc::newLineLocAfterLastRow();
}

static inline int calcLeftBound(int x, int leftWidth, int margin)
{
    assert(leftWidth > 0);

    return x - (leftWidth >> 1) - margin;
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

view::CharLoc TextArea::getCharLocByLineLocAndX(const view::LineLoc & lineLoc, int x) const
{
    if (lineLoc.isNull() || lineLoc.isAfterLastRow())
    {
        return view::CharLoc::newCharLocAfterLastChar(lineLoc);
    }

    const view::Line & line = m_page.getLine(lineLoc);

    const int charCnt = line.size();

    if (charCnt == 0)
    {
        return view::CharLoc::newCharLocAfterLastChar(lineLoc);
    }

    const int margin = config_.hMargin();

    // 为了简化处理，把第一个字符单独处理，因为第一个字符没有前一个字符
    const view::Char & firstChar = line[0];
    const int firstX = firstChar.x();
    const int firstWidth = firstChar.width();
    const int firstCharRightBound = calcRightBound(firstX, firstWidth);
    if (x <= firstCharRightBound)
    {
        return view::CharLoc(lineLoc, 0);
    }

    int left = 1;
    int right = charCnt - 1;
    while (left <= right)
    {
        const int mid = ((left + right) >> 1);
        const view::Char & c = line[mid];
        const int cx = c.x();
        const int a = calcLeftBound(cx, line[mid - 1].width(), margin);
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
            return view::CharLoc(lineLoc, mid);
        }
    }

    return view::CharLoc::newCharLocAfterLastChar(lineLoc);
}

void TextArea::remakePage()
{
    m_page.clear();

    const RowN rowCnt = editor_.doc().rowCnt();

    const int maxShownLineCnt = getMaxShownLineCnt();

    int h = -m_loc.line();

    for (RowN i = m_loc.row(); i < rowCnt; ++i)
    {
        if (h >= maxShownLineCnt)
        {
            break;
        }

        view::VRow vrow;
        makeVRow(editor_.doc().rowAt(i), vrow);
        const int rowSize = vrow.size();
        m_page.pushBack(std::move(vrow));

        h += rowSize;
    }
}


void TextArea::makeVRow(const Row & row, view::VRow & vrow) const
{
    if (config_.wrapLine())
    {
        makeVRowWithWrapLine(row, vrow);
    }
    else
    {
        makeVRowNoWrapLine(row, vrow);
    }
}

void TextArea::makeVRowWithWrapLine(const Row & row, view::VRow & vrow) const
{
    assert(vrow.size() == 0);

    const int hGap = config_.hGap();
    const int hMargin = config_.hMargin();

    view::Line *vline = &vrow.grow();

    DocLineCharInStream charStream(row);
    TxtWordStream wordStream(charStream);

    int leftX = hGap;

    while (true)
    {
        const UString word = wordStream.Next();
        if (word.empty())
        {
            return;
        }

        if (vline->size() == 0)
        {
            for (const UChar c : word)
            {
                const int charWidth = config_.charWidth(c);

                if (leftX + charWidth > m_size.width())
                {
                    leftX = hGap;
                    vline = &vrow.grow();
                }

                view::Char &vc = vline->grow();
                vc.setUChar(c);
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
                wordWidth += config_.charWidth(c);
                wordWidth += hMargin;
            }
            if (leftX + wordWidth > m_size.width())
            {
                leftX = hGap;
                vline = &vrow.grow();
            }
            for (const UChar c : word)
            {
                const int charWidth = config_.charWidth(c);

                if (leftX + charWidth > m_size.width())
                {
                    leftX = hGap;
                    vline = &vrow.grow();
                }

                view::Char &vc = vline->grow();
                vc.setUChar(c);
                vc.setX(leftX);
                vc.setWidth(charWidth);

                leftX += charWidth;
                leftX += hMargin;
            }
        }
    }
}

void TextArea::makeVRowNoWrapLine(const Row & row, view::VRow & vrow) const
{
    assert(vrow.size() == 0);

    const int hGap = config_.hGap();
    const int hMargin = config_.hMargin();

    view::Line &vline = vrow.grow();

    int leftX = hGap;

    const CharN cnt = row.charCnt();
    for (CharN i = 0; i < cnt; ++i)
    {
        const UChar c = row.charAt(i);
        const int charWidth = config_.charWidth(c);

        view::Char &vchar = vline.grow();
        vchar.setUChar(c);
        vchar.setX(leftX);
        vchar.setWidth(charWidth);

        leftX += charWidth;
        leftX += hMargin;
    }
}

view::LineLoc TextArea::getShownLastLineLoc() const
{
	const int rowCnt = m_page.size();

	const view::VRow & lastRow = m_page[rowCnt - 1];

	const int prevLineCnt = m_page.lineCnt() - m_loc.line() - lastRow.size();

	const int maxShownLineCnt = getMaxShownLineCnt();

	if (m_page.lineCnt() - m_loc.line() < maxShownLineCnt)
	{
		const int lastLineIndex = m_page.lineCnt() - m_loc.line() - prevLineCnt - 1;
		return view::LineLoc(m_page.size() - 1, lastLineIndex);
	}
	else
	{
		const int lastLineIndex = getMaxShownLineCnt() - prevLineCnt - 1;
		return view::LineLoc(m_page.size() - 1, lastLineIndex);
	}
}

