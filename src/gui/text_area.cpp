#include "text_area.h"

#include <cassert>


#include "doc/doc.h"
#include "doc/doc_row.h"
#include "text/doc_line_char_instream.h"
#include "text/txt_word_instream.h"
#include "editor/editor.h"
#include "text_area_config.h"


namespace gui
{

TextArea::TextArea(Editor *editor, TextAreaConfig *config)
    : editor_(*editor)
    , config_(*config)
{
    assert(editor);
    assert(config);
   
    m_listenerIdForLastActLineUpdate = editor_.addOnLastActRowUpdateListener([this] {
        cbsShouldRepaint_.call();
    });
}

TextArea::~TextArea()
{

}

void TextArea::initSize(const Size & size)
{
    size_ = size;

    remakePage();

    cbsShouldRepaint_.call();
}

void TextArea::resize(const Size & size)
{
    if (size_ == size)
    {
        return;
    }

    size_ = size;

    remakePage();

    cbsShouldRepaint_.call();
}

int TextArea::getBaseLineByLineOffset(int off) const
{
    return (1 + off) * config_.lineHeight() - config_.font().descent();
}

int TextArea::getLineOffsetByLineLoc(const VLineLoc & loc) const
{
    int sum = loc.line();

    const int rowCnt = page_.size();
    const int row = loc.row();

    for (int i = 0; i < rowCnt && i < row; ++i)
    {
        sum += page_[i].size();
    }

    sum -= vloc_.line();
    
    return sum;
}

int TextArea::getMaxShownLineCnt() const
{
    const int lineHeight = config_.lineHeight();
    return (size_.height() + lineHeight - 1) / lineHeight;
}

int TextArea::getLineOffsetByRowIndex(int row) const
{
    int sum = 0;

    const int rowCnt = page_.size();

    for (int i = 0; i < rowCnt && i < row; ++i)
    {
        sum += page_[i].size();
    }

    sum -= vloc_.line();

    return sum;
}

CharLoc TextArea::getCharLocByPoint(int x, int y) const
{
    const int lineOffset = getLineOffsetByY(y);
    const VLineLoc lineLoc = getLineLocByLineOffset(lineOffset);
    return getCharLocByLineLocAndX(lineLoc, x);
}

DocLoc TextArea::getDocLocByPoint(int x, int y) const
{
    const CharLoc charLoc = getCharLocByPoint(x, y);
    const DocLoc docLoc = convertToDocLoc(charLoc);
    return docLoc;
}

VRowLoc TextArea::convertToRowLoc(RowN row) const
{
    const int vrowIndex = row - vloc_.row();
    const int vrowCnt = page_.size();
    if (vrowIndex < 0 || vrowIndex >= vrowCnt)
    {
        if (row >= editor_.doc().rowCnt())
        {
            return VRowLoc::newRowLocAfterLastRow();
        }
        return VRowLoc();
    }

    if (row >= editor_.doc().rowCnt())
    {
        return VRowLoc::newRowLocAfterLastRow();
    }

    return VRowLoc(vrowIndex);
}

CharLoc TextArea::convertToCharLoc(const DocLoc & docLoc) const
{
    if (docLoc.isNull())
    {
        return CharLoc();
    }

    if (docLoc.isAfterLastRow())
    {
        return CharLoc::newCharLocAfterLastRow();
    }

    const int r = docLoc.row() - vloc_.row();
    const int rowCnt = page_.size();
    if (r < 0 || r >= rowCnt)
    {
        return CharLoc();
    }

    const VRow & vrow = page_[r];

    if (docLoc.isAfterLastChar())
    {
        VLineLoc lineLoc(r, vrow.size() - 1);
        return CharLoc::newCharLocAfterLastChar(lineLoc);
    }
    
    int lineIndex = 0;
    int charIndex = 0;

    int prevLineCharCnt = 0;

    const CharN col = docLoc.col();

    for (const VLine &vline : vrow)
    {
        for (const VChar &vc : vline)
        {
            if (charIndex == col)
            {
                return CharLoc(r, lineIndex, col - prevLineCharCnt);
            }

            ++charIndex;
        }

        ++lineIndex;

        prevLineCharCnt += vline.size();
    }

    return CharLoc();
}

DocLoc TextArea::convertToDocLoc(const CharLoc & charLoc) const
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
            return DocLoc::newDocLocAfterLastChar(vloc_.row() + charLoc.row());
        }
        else
        {
            // 如果不是最后一个显示行，则把光标放在下一个显示行最开始处
            return convertToDocLoc(CharLoc(charLoc.row(), charLoc.line() + 1, 0));
        }
    }

    
    const VRow & vrow = page_[charLoc.row()];
    int lastLine = charLoc.line();
    if (vloc_.row() == 0)
    {
        // 如果在第一个VRow，则需要把偏移量加上，因为ViewLoc中的line属性为行偏移
        lastLine += vloc_.line();
    }
    CharN col = charLoc.col();
    for (int i = 0; i < lastLine; ++i)
    {
        col += vrow[i].size();
    }

    return DocLoc(vloc_.row() + charLoc.row(), col);
}

const VChar &TextArea::getChar(const CharLoc &charLoc) const
{
    return page_[charLoc.row()][charLoc.line()][charLoc.col()];
}

int TextArea::getXByCharLoc(const CharLoc &charLoc) const
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
        const VLine & line = page_[charLoc.row()][charLoc.line()];
        if (line.empty())
        {
            return config_.hGap();
        }
        const VChar &vc = page_[charLoc.row()][charLoc.line()].last();
        return vc.x() + vc.width();
    }

    return getChar(charLoc).x();
}

LineBound TextArea::getLineBoundByLineOffset(int lineOffset) const
{
    const int lineHeight = config_.lineHeight();
    const int top = lineHeight * lineOffset;
    const int bottom = top + lineHeight;

    return LineBound(top, bottom);
}

LineBound TextArea::getLineBound(const VLineLoc & lineLoc) const
{
    if (lineLoc.isAfterLastRow())
    {
        return getLineBoundByLineOffset(page_.lineCnt());
    }
    const int lineOffset = getLineOffsetByLineLoc(lineLoc);
    return getLineBoundByLineOffset(lineOffset);
}

RowBound TextArea::getRowBound(const VRowLoc & rowLoc) const
{
    if (rowLoc.isNull())
    {
        return RowBound();
    }

    if (rowLoc.isAfterLastRow())
    {
        const int lineOffset = page_.lineCnt();
        const int lineHeight = config_.lineHeight();
        const int top = lineHeight * lineOffset;
        return RowBound(top, lineHeight);
    }

    const int lineOffset = getLineOffsetByRowIndex(rowLoc.row());
    const int lineHeight = config_.lineHeight();
    const int top = lineHeight * lineOffset;
    const int height = lineHeight * page_[rowLoc.row()].size();
    return RowBound(top, height);
}

bool TextArea::hasPrevCharAtSameLine(const CharLoc & charLoc) const
{
    return !noPrevCharAtSameLine(charLoc);
}

bool TextArea::noPrevCharAtSameLine(const CharLoc & charLoc) const
{
    if (charLoc.isAfterLastChar())
    {
        return page_.getLine(charLoc).size() == 0;
    }
    else
    {
        return charLoc.col() == 0;
    }
}

bool TextArea::hasNextCharAtSameLine(const CharLoc & charLoc) const
{
    return !noNextCharAtSameLine(charLoc);
}

bool TextArea::noNextCharAtSameLine(const CharLoc & charLoc) const
{
    if (charLoc.isAfterLastChar())
    {
        return true;
    }
    else
    {
		const VLine & line = page_.getLine(charLoc);
        return charLoc.col() == line.size();
    }
}

bool TextArea::needEnsureHasNextLine(const CharLoc & charLoc) const
{
	if (hasNextCharAtSameLine(charLoc))
	{
		const VRow & vrow = page_[charLoc.row()];
		if (charLoc.line() < vrow.size() - 1)
		{
			const VLine & line = vrow[charLoc.line()];
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

bool TextArea::isLastLineOfRow(const VLineLoc & lineLoc) const
{
    return lineLoc.line() == page_[lineLoc.row()].size() - 1;
}

bool TextArea::isEndOfVirtualLine(const CharLoc & charLoc) const
{
    return charLoc.isAfterLastChar() && !isLastLineOfRow(charLoc);
}

// 避免出现因为定位到虚拟行尾部而使虚拟行被跳过的情况
CharLoc TextArea::betterLocForVerticalMove(const CharLoc & charLoc) const
{
    if (!isEndOfVirtualLine(charLoc))
    {
        return charLoc;
    }

    const VLine & line = page_.getLine(charLoc);
    const CharN charCnt = line.size();

    if (charCnt != 0)
    {
        return CharLoc(charLoc.row(), charLoc.line(), charCnt - 1);
    }

    return charLoc;
}

DocLoc TextArea::getNextUpLoc(const DocLoc & docLoc) const
{
    const CharLoc charLoc = convertToCharLoc(docLoc);
    if (charLoc.isNull())
    {
        return DocLoc();
    }

    const VLineLoc upLineLoc = page_.getNextUpLineLoc(charLoc);
    const CharLoc upCharLoc = getCharLocByLineLocAndX(upLineLoc, stableX_);

    return convertToDocLoc(betterLocForVerticalMove(upCharLoc));
}

DocLoc TextArea::getNextDownLoc(const DocLoc & docLoc) const
{
    const CharLoc charLoc = convertToCharLoc(docLoc);
    if (charLoc.isNull())
    {
        return DocLoc();
    }

    const VLineLoc downLineLoc = page_.getNextDownLineLoc(charLoc);
    const CharLoc downCharLoc = getCharLocByLineLocAndX(downLineLoc, stableX_);

    return convertToDocLoc(betterLocForVerticalMove(downCharLoc));
}

void TextArea::ensureHasPrevLine(const VLineLoc & curLineLoc)
{
    if (curLineLoc.row() == 0 && curLineLoc.line() <= vloc_.line())
    {
        if (vloc_.line() > 0)
        {
            setViewLoc(ViewLoc(vloc_.row(), vloc_.line() - 1));
        }
        else
        {
            if (vloc_.row() > 0)
            {
                VRow vrow;
                makeVRow(editor_.doc().rowAt(vloc_.row() - 1), vrow);
                const int newRowSize = vrow.size();
                page_.pushFront(std::move(vrow));

                setViewLoc(ViewLoc(vloc_.row() - 1, newRowSize - 1));
            }
        }
    }
}

bool TextArea::ensureHasNextLine(const VLineLoc &curLineLoc)
{
	const int maxShownLineCnt = getMaxShownLineCnt();

	const int rowCnt = page_.size();

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
	if (vloc_.row() + curLineLoc.row() >= docRowCnt - 1)
	{
		return false;
	}

	const VRow &lastRow = page_[rowCnt - 1];

	const int prevLineCnt = page_.lineCnt() - vloc_.line() - lastRow.size();

	const int lineIndex = maxShownLineCnt - prevLineCnt - 1;

	if (isLastLineOfRow(VLineLoc(rowCnt - 1, lineIndex)))
	{
		const RowN newDocRowIndex = vloc_.row() + rowCnt;
		if (newDocRowIndex > docRowCnt - 1)
		{
			return false;
		}

		const Row & docRow = editor_.doc().rowAt(newDocRowIndex);
		VRow vrow;
		makeVRow(docRow, vrow);
        page_.pushBack(std::move(vrow));
	}

	return true;
}

void TextArea::removeSpareRow()
{
    const int maxShownLineCnt = getMaxShownLineCnt();

    int remainLineCnt = page_.lineCnt() - vloc_.line();

    for (int r = page_.size() - 1; r >= 0; --r)
    {
        remainLineCnt -= page_[r].size();
        if (remainLineCnt >= maxShownLineCnt)
        {
            page_.popBack();
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
    const CharLoc oldCharLoc = convertToCharLoc(oldDocLoc);
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
	const CharLoc oldCharLoc = convertToCharLoc(oldDocLoc);
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
    const CharLoc oldCharLoc = convertToCharLoc(oldDocLoc);
    if (noPrevCharAtSameLine(oldCharLoc))
    {
        ensureHasPrevLine(oldCharLoc);
    }

    const DocLoc newLoc = editor_.getNextLeftLocByChar(oldDocLoc);
    if (!newLoc.isNull())
    {
        editor_.setNormalCursor(newLoc);

		const CharLoc charLoc = convertToCharLoc(newLoc);
        stableX_ = getXByCharLoc(charLoc);
    }
    
	removeSpareRow();
}

void TextArea::onDirRightKeyPress()
{
	const DocLoc oldDocLoc = editor_.normalCursor().to();
	const CharLoc oldCharLoc = convertToCharLoc(oldDocLoc);
	bool needMoveHead = false;
	if (needEnsureHasNextLine(oldCharLoc))
	{
		needMoveHead = ensureHasNextLine(oldCharLoc);
	}

	const DocLoc newLoc = editor_.getNextRightLocByChar(oldDocLoc);
	if (!newLoc.isNull())
	{
        editor_.setNormalCursor(newLoc);

		const CharLoc charLoc = convertToCharLoc(newLoc);
        stableX_ = getXByCharLoc(charLoc);
	}

	if (needMoveHead)
	{
		movePageHeadOneLine();
	}
}

void TextArea::setViewLoc(const ViewLoc & viewLoc)
{
    if (vloc_ == viewLoc)
    {
        return;
    }

    vloc_ = viewLoc;

    m_onViewLocChangeListeners.call();
}

void TextArea::movePageHeadOneLine()
{
	if (isLastLineOfRow(VLineLoc(0, vloc_.line())))
	{
		setViewLoc(ViewLoc(vloc_.row() + 1, 0));
        page_.popFront();
	}
	else
	{
		setViewLoc(ViewLoc(vloc_.row(), vloc_.line() + 1));
	}
}

std::optional<Rect> TextArea::getLastActLineDrawRect() const
{
    const RowN row = editor_.lastActRow();
    const VRowLoc loc = convertToRowLoc(row);
    if (loc.isNull())
    {
        return std::nullopt;
    }

    const RowBound bound = getRowBound(loc);

    Rect rect;
    rect.setLeft(0);
    rect.setTop(bound.top());
    rect.setWidth(size_.width());
    rect.setHeight(bound.height());
    return rect;
}

std::optional<VerticalLine> TextArea::getNormalCursorDrawData() const
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

    const CharLoc charLoc = convertToCharLoc(docLoc);

    if (charLoc.isNull())
    {
        return std::nullopt;
    }

    const LineBound bound = getLineBound(charLoc);

    const int x = getXByCharLoc(charLoc) + kHorizontalDelta;

    VerticalLine vl;
    vl.setX(x);
    vl.setTop(bound.top() + kVerticalShrink);
    vl.setBottom(bound.bottom() - kVerticalShrink);
    return vl;
}

int TextArea::getLineNumBarWidth() const
{
    return 100;
}

void TextArea::drawEachLineNum(std::function<void(RowN lineNum, int baseline, const RowBound &bound, bool isLastAct)> && action) const
{
    const int rowCnt = page_.size();

    int offset = -vloc_.line();

    for (int r = 0; r < rowCnt; ++r)
    {
        const VRowLoc loc(r);
        const RowBound bound = getRowBound(loc);
        const RowN lineNum = vloc_.row() + r;
        const RowN lastAct = editor_.lastActRow();
        const bool isLastAct = lineNum == lastAct;
        const int baseline = getBaseLineByLineOffset(offset);

        action(lineNum, baseline, bound, isLastAct);

        offset += page_[r].size();
    }
}

void TextArea::drawEachChar(std::function<void(int x, int y, UChar c)>&& action) const
{
    const int rowCnt = page_.size();
    if (rowCnt == 0)
    {
        return;
    }

    const VRow &curRow = page_[0];
    const int curRowSize = curRow.size();

    int lineOffset = 0;

    for (int i = vloc_.line(); i < curRowSize; ++i)
    {
        const VLine & line = curRow[i];

        const int baseline = getBaseLineByLineOffset(lineOffset);

        for (const VChar &c : line)
        {
            action(c.x(), baseline, c.uchar());
        }

        ++lineOffset;
    }

    for (int r = 1; r < rowCnt; ++r)
    {
        const VRow & row = page_[r];

        for (const VLine & line : row)
        {
            const int baseline = getBaseLineByLineOffset(lineOffset);

            for (const VChar & c : line)
            {
                action(c.x(), baseline, c.uchar());
            }

            ++lineOffset;
        }
    }
}

CallbackHandle TextArea::addShouldRepaintCallback(std::function<void()>&& action)
{
    return cbsShouldRepaint_.add(std::move(action));
}

CallbackHandle TextArea::addOnViewLocChangeListener(std::function<void()>&& action)
{
    return m_onViewLocChangeListeners.add(std::move(action));
}

void TextArea::onPrimaryButtomPress(int x, int y)
{
    const CharLoc charLoc = getCharLocByPoint(x, y);
    const DocLoc docLoc = convertToDocLoc(charLoc);
    editor_.onPrimaryKeyPress(docLoc);

    stableX_ = getXByCharLoc(charLoc);
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
	if (vloc_.row() == docRowCnt - 1 && isLastLineOfRow(VLineLoc(page_.size() - 1, vloc_.line())))
	{
		return false;
	}

	// 如果最后一个可视line是row中最后一个line，则需要新解析一个row的内容
	const VLineLoc shownLastLineLoc = getShownLastLineLoc();
	if (isLastLineOfRow(shownLastLineLoc))
	{
		const RowN newDocRowIndex = vloc_.row() + page_.size();
		if (newDocRowIndex <= docRowCnt - 1)
		{
			const Row & docRow = editor_.doc().rowAt(newDocRowIndex);
			VRow vrow;
			makeVRow(docRow, vrow);
            page_.pushBack(std::move(vrow));
		}
	}

	movePageHeadOneLine();

	return true;
}

int TextArea::getLineOffsetByY(int y) const
{
    return y / config_.lineHeight();
}

VLineLoc TextArea::getLineLocByLineOffset(int offset) const
{
    const int rowCnt = page_.size();

    if (vloc_.line() == 0)
    {
        int sum = 0;

        for (int i = 0; i < rowCnt; ++i)
        {
            const VRow &row = page_[i];
            const int lineCnt = row.size();

            if (sum + lineCnt > offset)
            {
                return VLineLoc(i, offset - sum);
            }

            sum += lineCnt;
        }

        return VLineLoc::newLineLocAfterLastRow();
    }

    if (rowCnt == 0)
    {
        return VLineLoc();
    }

    const VRow & curRow = page_[0];
    const int curRowSize = curRow.size();
    if (vloc_.line() + offset < curRowSize)
    {
        return VLineLoc(0, vloc_.line() + offset);
    }

    int sum = curRowSize - vloc_.line();

    for (int i = 1; i < rowCnt; ++i)
    {
        const VRow &row = page_[i];
        const int lineCnt = row.size();

        if (sum + lineCnt > offset)
        {
            return VLineLoc(i, offset - sum);
        }

        sum += lineCnt;
    }

    return VLineLoc::newLineLocAfterLastRow();
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

CharLoc TextArea::getCharLocByLineLocAndX(const VLineLoc & lineLoc, int x) const
{
    if (lineLoc.isNull() || lineLoc.isAfterLastRow())
    {
        return CharLoc::newCharLocAfterLastChar(lineLoc);
    }

    const VLine & line = page_.getLine(lineLoc);

    const int charCnt = line.size();

    if (charCnt == 0)
    {
        return CharLoc::newCharLocAfterLastChar(lineLoc);
    }

    const int margin = config_.hMargin();

    // 为了简化处理，把第一个字符单独处理，因为第一个字符没有前一个字符
    const VChar & firstChar = line[0];
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
        const VChar & c = line[mid];
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
            return CharLoc(lineLoc, mid);
        }
    }

    return CharLoc::newCharLocAfterLastChar(lineLoc);
}

void TextArea::remakePage()
{
    page_.clear();

    const RowN rowCnt = editor_.doc().rowCnt();

    const int maxShownLineCnt = getMaxShownLineCnt();

    int h = -vloc_.line();

    for (RowN i = vloc_.row(); i < rowCnt; ++i)
    {
        if (h >= maxShownLineCnt)
        {
            break;
        }

        VRow vrow;
        makeVRow(editor_.doc().rowAt(i), vrow);
        const int rowSize = vrow.size();
        page_.pushBack(std::move(vrow));

        h += rowSize;
    }
}


void TextArea::makeVRow(const Row &row, VRow &vrow) const
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

void TextArea::makeVRowWithWrapLine(const Row &row, VRow &vrow) const
{
    assert(vrow.size() == 0);

    const int hGap = config_.hGap();
    const int hMargin = config_.hMargin();

    VLine *vline = &vrow.grow();

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

                if (leftX + charWidth > size_.width())
                {
                    leftX = hGap;
                    vline = &vrow.grow();
                }

                VChar &vc = vline->grow();
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
            if (leftX + wordWidth > size_.width())
            {
                leftX = hGap;
                vline = &vrow.grow();
            }
            for (const UChar c : word)
            {
                const int charWidth = config_.charWidth(c);

                if (leftX + charWidth > size_.width())
                {
                    leftX = hGap;
                    vline = &vrow.grow();
                }

                VChar &vc = vline->grow();
                vc.setUChar(c);
                vc.setX(leftX);
                vc.setWidth(charWidth);

                leftX += charWidth;
                leftX += hMargin;
            }
        }
    }
}

void TextArea::makeVRowNoWrapLine(const Row &row, VRow &vrow) const
{
    assert(vrow.size() == 0);

    const int hGap = config_.hGap();
    const int hMargin = config_.hMargin();

    VLine &vline = vrow.grow();

    int leftX = hGap;

    const CharN cnt = row.charCnt();
    for (CharN i = 0; i < cnt; ++i)
    {
        const UChar c = row.charAt(i);
        const int charWidth = config_.charWidth(c);

        VChar &vchar = vline.grow();
        vchar.setUChar(c);
        vchar.setX(leftX);
        vchar.setWidth(charWidth);

        leftX += charWidth;
        leftX += hMargin;
    }
}

VLineLoc TextArea::getShownLastLineLoc() const
{
	const int rowCnt = page_.size();

	const VRow & lastRow = page_[rowCnt - 1];

	const int prevLineCnt = page_.lineCnt() - vloc_.line() - lastRow.size();

	const int maxShownLineCnt = getMaxShownLineCnt();

	if (page_.lineCnt() - vloc_.line() < maxShownLineCnt)
	{
		const int lastLineIndex = page_.lineCnt() - vloc_.line() - prevLineCnt - 1;
		return VLineLoc(page_.size() - 1, lastLineIndex);
	}
	else
	{
		const int lastLineIndex = getMaxShownLineCnt() - prevLineCnt - 1;
		return VLineLoc(page_.size() - 1, lastLineIndex);
	}
}


}

