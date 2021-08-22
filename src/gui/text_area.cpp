#include "text_area.h"

#include <cassert>
#include <stdexcept>

#include "doc/doc.h"
#include "doc/doc_row.h"
#include "editor/editor.h"
#include "text/doc_line_char_instream.h"
#include "text/txt_word_instream.h"

#include "text_area_config.h"
#include "row_walker.h"

namespace {

enum class LoadRowReason : i64 {
    EnsureHasNextLine,
};

}

namespace gui
{

TextArea::TextArea(Editor &editor, const TextAreaConfig &config, const Size &size)
    : editor_(editor)
    , config_(config)
    , size_(size)
    , cvt_(editor_, size_, page_, vloc_, config_)
{
    editorSigConns_ += editor_.sigLastActRowUpdated().connect([this] {
        sigShouldRepaint_();
    });
}

TextArea::~TextArea()
{

}

void TextArea::start()
{
    editor_.start();
}

void TextArea::resize(const Size & size)
{
    if (size_ == size)
    {
        return;
    }

    size_ = size;

    remakePage();

    updateStableXByCurrentCursor();
    sigShouldRepaint_();
}

void TextArea::lookAt(RowN row, const Size &size)
{
    std::unique_lock<std::mutex> lock(mtx_);

    // 等待直到上一次的lookAt处理完成
    while (!lookAtWaiting_.empty()) {
        cvLookAtDone_.wait(lock);
    }


}

i32 TextArea::getMaxShownLineCnt() const
{
    const i32 lineHeight = config_.lineHeight();
    return (size_.height() + lineHeight - 1) / lineHeight;
}

RowRange TextArea::lookAtRange(RowN row) const
{
    return RowRange::byOffAndLen(row, getMaxShownLineCnt());
}

LineBound TextArea::getLineBoundByLineOffset(i32 lineOffset) const
{
    const i32 lineHeight = config_.lineHeight();
    const i32 top = lineHeight * lineOffset;
    const i32 bottom = top + lineHeight;

    return LineBound(top, bottom);
}

LineBound TextArea::getLineBound(const VLineLoc & lineLoc) const
{
    if (lineLoc.isAfterLastRow())
    {
        return getLineBoundByLineOffset(page_.lineCnt());
    }
    const i32 lineOffset = cvt_.toLineOffset(lineLoc);
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
        const i32 lineOffset = page_.lineCnt();
        const i32 lineHeight = config_.lineHeight();
        const i32 top = lineHeight * lineOffset;
        return RowBound(top, lineHeight);
    }

    const i32 lineOffset = cvt_.toLineOffset(rowLoc);
    const i32 lineHeight = config_.lineHeight();
    const i32 top = lineHeight * lineOffset;
    const i32 height = lineHeight * page_[rowLoc.row()].size();
    return RowBound(top, height);
}

bool TextArea::hasPrevCharAtSameLine(const VCharLoc & charLoc) const
{
    return !noPrevCharAtSameLine(charLoc);
}

bool TextArea::noPrevCharAtSameLine(const VCharLoc & charLoc) const
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

bool TextArea::hasNextCharAtSameLine(const VCharLoc & charLoc) const
{
    return !noNextCharAtSameLine(charLoc);
}

bool TextArea::noNextCharAtSameLine(const VCharLoc & charLoc) const
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

bool TextArea::needEnsureHasNextLine(const VCharLoc & charLoc) const
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

bool TextArea::isEndOfVirtualLine(const VCharLoc & charLoc) const
{
    return charLoc.isAfterLastChar() && !isLastLineOfRow(charLoc);
}

// 避免出现因为定位到虚拟行尾部而使虚拟行被跳过的情况
VCharLoc TextArea::betterLocForVerticalMove(const VCharLoc & charLoc) const
{
    if (!isEndOfVirtualLine(charLoc))
    {
        return charLoc;
    }

    const VLine & line = page_.getLine(charLoc);
    const CharN charCnt = line.size();

    if (charCnt != 0)
    {
        return VCharLoc(charLoc.row(), charLoc.line(), charCnt - 1);
    }

    return charLoc;
}

DocLoc TextArea::getNextUpLoc(const DocLoc & docLoc) const
{
    const VCharLoc charLoc = cvt_.toCharLoc(docLoc);
    if (charLoc.isNull())
    {
        return DocLoc();
    }

    const VLineLoc upLineLoc = page_.getNextUpLineLoc(charLoc);
    const VCharLoc upCharLoc = cvt_.toCharLoc(upLineLoc, stableX_);

    return cvt_.toDocLoc(betterLocForVerticalMove(upCharLoc));
}

DocLoc TextArea::getNextDownLoc(const DocLoc & docLoc) const
{
    const VCharLoc charLoc = cvt_.toCharLoc(docLoc);
    if (charLoc.isNull())
    {
        return DocLoc();
    }

    const VLineLoc downLineLoc = page_.getNextDownLineLoc(charLoc);
    const VCharLoc downCharLoc = cvt_.toCharLoc(downLineLoc, stableX_);

    return cvt_.toDocLoc(betterLocForVerticalMove(downCharLoc));
}

DocLoc TextArea::getNextLeftLoc(const DocLoc &docLoc) const
{
    if (docLoc.isNull()) {
        return DocLoc();
    }

    if (docLoc.isAfterLastRow()) {
        const RowN rowCount = editor_.doc().rowCnt();
        if (rowCount > 0) {
            return DocLoc::newDocLocAfterLastChar(rowCount - 1);
        } else {
            return DocLoc();
        }
    }

    if (docLoc.isAfterLastChar()) {
        const VCharLoc charLoc = cvt_.toCharLoc(docLoc);
        const CharN charCount = page_[charLoc.row()].charCount();
        if (charCount > 0) {
            return DocLoc(docLoc.row(), charCount - 1);
        } else {
            if (docLoc.row() > 0) {
                return DocLoc::newDocLocAfterLastChar(docLoc.row() - 1);
            } else {
                return DocLoc();
            }
        }
    }

    if (docLoc.col() > 0) {
        return docLoc.nextLeft();
    } else {
        if (docLoc.row() > 0) {
            return DocLoc::newDocLocAfterLastChar(docLoc.row() - 1);
        } else {
            return DocLoc();
        }
    }
}

void TextArea::ensureHasPrevLine(const VLineLoc & curLineLoc)
{
    if (curLineLoc.row() == 0 && curLineLoc.line() <= vloc_.line())
    {
        if (vloc_.line() > 0)
        {
            setViewLoc(ViewLoc(vloc_.rowRange(), vloc_.line() - 1));
        }
        else
        {
            if (vloc_.rowRange().off() > 0)
            {
                std::optional<Row> row = editor_.doc().rowAt(vloc_.rowRange().off() - 1);
                VRow vrow;
                makeVRow(*row, vrow);
                const int newRowSize = vrow.size();
                page_.pushBack(std::move(vrow));
                setViewLoc(ViewLoc(vloc_.rowRange().shift(-1), newRowSize - 1));
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
	if (vloc_.rowRange().off() + curLineLoc.row() >= docRowCnt - 1)
	{
		return false;
	}

	const VRow &lastRow = page_[rowCnt - 1];

	const int prevLineCnt = page_.lineCnt() - vloc_.line() - lastRow.size();

	const int lineIndex = maxShownLineCnt - prevLineCnt - 1;

	if (isLastLineOfRow(VLineLoc(rowCnt - 1, lineIndex)))
	{
		const RowN newDocRowIndex = vloc_.rowRange().off() + rowCnt;
		if (newDocRowIndex > docRowCnt - 1)
		{
			return false;
		}

        std::optional<Row> row = editor_.doc().rowAt(newDocRowIndex);
        VRow vrow;
        makeVRow(*row, vrow);
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
    const VCharLoc oldCharLoc = cvt_.toCharLoc(oldDocLoc);
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
	const VCharLoc oldCharLoc = cvt_.toCharLoc(oldDocLoc);
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
    const VCharLoc oldCharLoc = cvt_.toCharLoc(oldDocLoc);
    if (noPrevCharAtSameLine(oldCharLoc))
    {
        ensureHasPrevLine(oldCharLoc);
    }

    const DocLoc newLoc = editor_.getNextLeftLocByChar(oldDocLoc);
    if (!newLoc.isNull())
    {
        editor_.setNormalCursor(newLoc);

		const VCharLoc charLoc = cvt_.toCharLoc(newLoc);
        stableX_ = cvt_.toX(charLoc);
    }
    
	removeSpareRow();
}

void TextArea::onDirRightKeyPress()
{
	const DocLoc oldDocLoc = editor_.normalCursor().to();
	const VCharLoc oldCharLoc = cvt_.toCharLoc(oldDocLoc);
	bool needMoveHead = false;
	if (needEnsureHasNextLine(oldCharLoc))
	{
		needMoveHead = ensureHasNextLine(oldCharLoc);
	}

	const DocLoc newLoc = editor_.getNextRightLocByChar(oldDocLoc);
	if (!newLoc.isNull())
	{
        editor_.setNormalCursor(newLoc);

		const VCharLoc charLoc = cvt_.toCharLoc(newLoc);
        stableX_ = cvt_.toX(charLoc);
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

    sigViewLocChanged_();
}

void TextArea::movePageHeadOneLine()
{
	if (isLastLineOfRow(VLineLoc(0, vloc_.line())))
	{
		setViewLoc(ViewLoc(vloc_.rowRange().shift(1), 0));
        page_.popFront();
	}
	else
	{
		setViewLoc(ViewLoc(vloc_.rowRange(), vloc_.line() + 1));
	}
}

std::optional<Rect> TextArea::getLastActLineDrawRect() const
{
    const RowN row = editor_.lastActRow();
    const VRowLoc loc = cvt_.toRowLoc(VRowOffset(row));
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

    const VCharLoc charLoc = cvt_.toCharLoc(docLoc);

    if (charLoc.isNull())
    {
        return std::nullopt;
    }

    const LineBound bound = getLineBound(charLoc);

    const i32 x = cvt_.toX(charLoc) + kHorizontalDelta;

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

void TextArea::drawEachLineNum(std::function<void(RowN lineNum, i32 baseline, const RowBound &bound, bool isLastAct)> && action) const
{
    const int rowCnt = page_.size();

    i32 offset = -vloc_.line();

    for (int r = 0; r < rowCnt; ++r)
    {
        const VRowLoc loc(r);
        const RowBound bound = getRowBound(loc);
        const RowN lineNum = vloc_.rowRange().off() + r;
        const RowN lastAct = editor_.lastActRow();
        const bool isLastAct = lineNum == lastAct;
        const i32 baseline = cvt_.toBaselineY(offset);

        action(lineNum, baseline, bound, isLastAct);

        offset += page_[r].size();
    }
}

void TextArea::drawEachChar(std::function<void(i32 x, i32 y, char32_t c)>&& action) const
{
    const int rowCnt = page_.size();
    if (rowCnt == 0)
    {
        return;
    }

    const VRow &curRow = page_[0];
    const int curRowSize = curRow.size();

    i32 lineOffset = 0;

    for (int i = vloc_.line(); i < curRowSize; ++i)
    {
        const VLine & line = curRow[i];

        const i32 baseline = cvt_.toBaselineY(lineOffset);

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
            const i32 baseline = cvt_.toBaselineY(lineOffset);

            for (const VChar & c : line)
            {
                action(c.x(), baseline, c.uchar());
            }

            ++lineOffset;
        }
    }
}

void TextArea::onPrimaryButtomPress(i32 x, i32 y)
{
    const VCharLoc charLoc = cvt_.toCharLoc(x, y);
    const DocLoc docLoc = cvt_.toDocLoc(charLoc);
    editor_.onPrimaryKeyPress(docLoc);

    stableX_ = cvt_.toX(charLoc);
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
	if (vloc_.rowRange().off() == docRowCnt - 1 && isLastLineOfRow(VLineLoc(page_.size() - 1, vloc_.line())))
	{
		return false;
	}

	// 如果最后一个可视line是row中最后一个line，则需要新解析一个row的内容
	const VLineLoc shownLastLineLoc = getShownLastLineLoc();
	if (isLastLineOfRow(shownLastLineLoc))
	{
		const RowN newDocRowIndex = vloc_.rowRange().off() + page_.size();
		if (newDocRowIndex <= docRowCnt - 1)
		{
            std::optional<Row> row = editor_.doc().rowAt(newDocRowIndex);
            VRow vrow;
            makeVRow(*row, vrow);
            page_.pushBack(std::move(vrow));
		}
	}

	movePageHeadOneLine();

	return true;
}

void TextArea::updateStableXByCurrentCursor()
{
    const DocLoc docLoc = editor_.normalCursor().to();
    const VCharLoc vCharLoc = cvt_.toCharLoc(docLoc);

    stableX_ = cvt_.toX(vCharLoc);
}

void TextArea::remakePage()
{
    page_.clear();

    const RowN rowCnt = editor_.doc().rowCnt();

    const int shownLineCnt = getMaxShownLineCnt();

    const int lineDelta = -vloc_.line();

    const RowRange range = RowRange::byOffAndLen(vloc_.rowRange().off(), shownLineCnt);

    std::map<RowN, Row> rows = editor_.doc().rowsAt(range);

    int h = lineDelta;
    for (RowN row = range.left(); row <= range.right(); ++row) {
        if (h >= shownLineCnt) {
            break;
        }

        VRow vrow;
        makeVRow(rows[row], vrow);

        const int rowSize = vrow.size();
        page_.pushBack(std::move(vrow));
        h += rowSize;
    }
}

void TextArea::makeVRow(const Row &row, VRow &vrow)
{
    assert(vrow.size() == 0);

    DocLineCharInStream charStream(row);

    class Provider : public GlyphWidthProvider {
    public:
        Provider(const FontOld &font) : fontOld_(font) {}
        virtual ~Provider() {}
        virtual int glyphWidth(char32_t unicode) override {
            return fontOld_.charWidth(unicode);
        }
    private:
        const FontOld &fontOld_;
    };

    Provider provider(config_.font());
    RowWalker walker(provider, charStream, config_.hLayout(), size_.width());

    walker.forEachChar([&vrow](bool isEmptyRow, size_t lineIndex, const VChar &vchar) {
        if (lineIndex == vrow.size()) {
            if (isEmptyRow) {
                vrow.grow();
            } else {
                vrow.grow().append(vchar);
            }
        } else if (lineIndex == vrow.size() - 1) {
            if (!isEmptyRow) {
                vrow.back().append(vchar);
            }
        } else {
            throw std::logic_error("forEachChar logic error");
        }
    });
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

