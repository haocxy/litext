#include "text_area_impl.h"

#include <cassert>
#include <stdexcept>

#include <QPainter>

#include "doc/doc.h"
#include "doc/doc_row.h"
#include "editor/editor.h"
#include "text/doc_line_char_instream.h"
#include "text/txt_word_instream.h"

#include "text_area_config.h"
#include "row_walker.h"
#include "qt/font_to_qfont.h"


namespace gui
{

TextAreaImpl::TextAreaImpl(Editor &editor, const TextAreaConfig &config)
    : editor_(editor)
    , config_(config)
    , cvt_(editor_, size_, page_, vloc_, config_, glyphCache_)
{
    editorSigConns_ += editor_.sigLastActRowUpdated().connect([this] {
        sigShouldRepaint_();
    });

    glyphCache_.setFont(config_.fontIndex(), config_.font().size());

    for (int i = 0; i < 256; ++i) {
        colorTable_ << qRgba(0, 0, 0, i);
    }
}

TextAreaImpl::~TextAreaImpl()
{

}

void TextAreaImpl::open(const Size &size, RowN row)
{
    editor_.start();

    Lock lock(mtx_);

    vloc_ = ViewLoc(row, 0);

    setSize(size);

    const RowRange docRange = RowRange::byOffAndLen(vloc_.row(), lineCountLimit_ - vloc_.line());
    if (editor_.doc().rowCnt() >= docRange.end()) {
        remakePage();
        updateStableXByCurrentCursor();
        sigViewportChanged_();
        sigShouldRepaint_();
        sigConnForWaitingRange_.disconnect();
        opened_ = true;
    } else {
        sigConnForWaitingRange_ = editor_.doc().sigLoadProgress().connect([this](const doc::LoadProgress &p) {
            // 在加载过程中视口尺寸可能被改变，所以每次都要获取当前的可以显式的行数
            RowRange curRange = RowRange::byOffAndLen(vloc_.row(), lineCountLimit_ - vloc_.line());
            if (p.done() && p.loadedRowCount() < curRange.end()) {
                curRange.setEnd(p.loadedRowCount());
            }
            if (p.loadedRowCount() >= curRange.end()) {
                Lock lock(mtx_);
                remakePage();
                updateStableXByCurrentCursor();
                sigViewportChanged_();
                sigShouldRepaint_();
                sigConnForWaitingRange_.disconnect();
                opened_ = true;
            }
            // TODO 没有处理文档段落数超过了需求段落数的情况
        });
    }
}

void TextAreaImpl::resize(const Size &size)
{
    Lock lock(mtx_);

    if (size_ == size) {
        return;
    }

    setSize(size);

    if (!opened_) {
        return;
    }

    remakePage();

    updateStableXByCurrentCursor();
    sigShouldRepaint_();
}

void TextAreaImpl::jumpTo(RowN row)
{
    Lock lock(mtx_);

    if (!opened_) {
        return;
    }

    if (row < 0) {
        row = editor_.doc().rowCnt() + row;
    }

    vloc_.setRow(row);
    vloc_.setLine(0);

    remakePage();

    sigViewportChanged_();
    sigShouldRepaint_();
}

void TextAreaImpl::jumpTo(float ratio)
{
    if (ratio < 0) {
        ratio = 0;
    } else if (ratio > 1) {
        ratio = 1;
    }

    Lock lock(mtx_);

    // 不应该使用浮点数计算，会有各种意想不到的问题，
    // 应该用整数表示由参数传入的比例
    const RowN row = double(ratio) * doc().rowCnt();

    jumpTo(row);
}

void TextAreaImpl::setSize(const Size &size)
{
    if (size_ == size) {
        return;
    }

    size_ = size;

    lineCountLimit_ = calcMaxShownLineCnt();

    widgetImg_ = QImage(size.width(), size.height(), QImage::Format_ARGB32_Premultiplied);
    textImg_ = QImage(size.width(), size.height(), QImage::Format_ARGB32_Premultiplied);
}

int TextAreaImpl::calcMaxShownLineCnt() const
{
    const i32 lineHeight = glyphCache_.face().height();
    return (size_.height() + lineHeight - 1) / lineHeight;
}

LineBound TextAreaImpl::getLineBoundByLineOffset(i32 lineOffset) const
{
    const i32 lineHeight = glyphCache_.face().height();
    const i32 top = lineHeight * lineOffset;
    const i32 bottom = top + lineHeight;

    return LineBound(top, bottom);
}

LineBound TextAreaImpl::getLineBound(const VLineLoc & lineLoc) const
{
    if (lineLoc.isAfterLastRow())
    {
        return getLineBoundByLineOffset(page_.lineCnt());
    }
    const i32 lineOffset = cvt_.toLineOffset(lineLoc);
    return getLineBoundByLineOffset(lineOffset);
}

RowBound TextAreaImpl::getRowBound(const VRowLoc & rowLoc) const
{
    if (rowLoc.isNull())
    {
        return RowBound();
    }

    if (rowLoc.isAfterLastRow())
    {
        const i32 lineOffset = page_.lineCnt();
        const i32 lineHeight = glyphCache_.face().height();
        const i32 top = lineHeight * lineOffset;
        return RowBound(top, lineHeight);
    }

    const i32 lineOffset = cvt_.toLineOffset(rowLoc);
    const i32 lineHeight = glyphCache_.face().height();
    const i32 top = lineHeight * lineOffset;
    const i32 height = lineHeight * page_[rowLoc.row()].size();
    return RowBound(top, height);
}

bool TextAreaImpl::hasPrevCharAtSameLine(const VCharLoc & charLoc) const
{
    return !noPrevCharAtSameLine(charLoc);
}

bool TextAreaImpl::noPrevCharAtSameLine(const VCharLoc & charLoc) const
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

bool TextAreaImpl::hasNextCharAtSameLine(const VCharLoc & charLoc) const
{
    return !noNextCharAtSameLine(charLoc);
}

bool TextAreaImpl::noNextCharAtSameLine(const VCharLoc & charLoc) const
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

bool TextAreaImpl::needEnsureHasNextLine(const VCharLoc & charLoc) const
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

bool TextAreaImpl::isLastLineOfRow(const VLineLoc & lineLoc) const
{
    return lineLoc.line() == page_[lineLoc.row()].size() - 1;
}

bool TextAreaImpl::isEndOfVirtualLine(const VCharLoc & charLoc) const
{
    return charLoc.isAfterLastChar() && !isLastLineOfRow(charLoc);
}

// 避免出现因为定位到虚拟行尾部而使虚拟行被跳过的情况
VCharLoc TextAreaImpl::betterLocForVerticalMove(const VCharLoc & charLoc) const
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

DocLoc TextAreaImpl::getNextUpLoc(const DocLoc & docLoc) const
{   
    const VCharLoc charLoc = cvt_.toCharLoc(docLoc);
    if (charLoc.isNull())
    {
        return DocLoc();
    }

    if (charLoc.row() == 0 && charLoc.line() == 0) {
        return DocLoc();
    }

    const VLineLoc upLineLoc = page_.getNextUpLineLoc(charLoc);
    const VCharLoc upCharLoc = cvt_.toCharLoc(upLineLoc, stableX_);

    return cvt_.toDocLoc(betterLocForVerticalMove(upCharLoc));
}

DocLoc TextAreaImpl::getNextDownLoc(const DocLoc & docLoc) const
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

DocLoc TextAreaImpl::getNextLeftLoc(const DocLoc &docLoc) const
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

bool TextAreaImpl::ensureHasPrevLine(const VLineLoc & curLineLoc)
{
    if (curLineLoc.row() == 0 && curLineLoc.line() <= vloc_.line())
    {
        if (vloc_.line() > 0)
        {
            setViewLoc(ViewLoc(vloc_.row(), vloc_.line() - 1));
            return true;
        }
        else
        {
            if (vloc_.row() > 0)
            {
                sptr<Row> row = editor_.doc().rowAt(vloc_.row() - 1);
                VRow vrow;
                makeVRow(*row, vrow);
                const int newRowSize = vrow.size();
                page_.pushFront(std::move(vrow));
                setViewLoc(ViewLoc(vloc_.row() - 1, newRowSize - 1));
                return true;
            }
        }
    }

    return false;
}

bool TextAreaImpl::ensureHasNextLine(const VLineLoc &curLineLoc)
{
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

	const int lineIndex = lineCountLimit_ - prevLineCnt - 1;

	if (isLastLineOfRow(VLineLoc(rowCnt - 1, lineIndex)))
	{
		const RowN newDocRowIndex = vloc_.row() + rowCnt;
		if (newDocRowIndex > docRowCnt - 1)
		{
			return false;
		}

        sptr<Row> row = editor_.doc().rowAt(newDocRowIndex);
        VRow vrow;
        makeVRow(*row, vrow);
        page_.pushBack(std::move(vrow));
	}

    const i32 lineOff = cvt_.toLineOffset(curLineLoc);
    const i32 requiredHeight = (lineOff + 1) * glyphCache_.face().height();
    return height() < requiredHeight;
}

void TextAreaImpl::removeSpareRow()
{
    int remainLineCnt = page_.lineCnt() - vloc_.line();

    for (int r = page_.size() - 1; r >= 0; --r)
    {
        remainLineCnt -= page_[r].size();
        if (remainLineCnt >= lineCountLimit_)
        {
            page_.popBack();
        }
        else
        {
            break;
        }
    }
}

void TextAreaImpl::drawChar(QPainter & p, i32 x, i32 y, char32_t unicode)
{
    const font::Glyph &g = glyphCache_.glyphOf(unicode);

    QImage glyphImg(reinterpret_cast<const uchar *>(g.bitmapData()),
        g.bitmapWidth(), g.bitmapHeight(), g.bitmapPitch(), QImage::Format_Indexed8);

    glyphImg.setColorTable(colorTable_);

    p.drawImage(x + g.leftBear(), y - g.topBear(), glyphImg);
}

void TextAreaImpl::repaint()
{
    QPainter p(&widgetImg_);

    // background
    p.fillRect(0, 0, size_.width(), size_.height(), Qt::white);

    // last act row
    {
        const RowN row = editor_.lastActRow();
        const VRowLoc loc = cvt_.toRowLoc(VRowOffset(row));
        if (!loc.isNull()) {
            const RowBound b = getRowBound(loc);
            p.fillRect(0, b.top(), size_.width(), b.height(), QColor(Qt::green).lighter(192));
        }
    }

    // text
    if (isTextImgDirty()) {
        QPainter textPainter(&textImg_);
        textImg_.fill(QColor(0, 0, 0, 0));

        drawEachChar(textPainter);

        markTextImgClean();
    }

    p.drawImage(0, 0, textImg_);

    // normal cursor
    {
        constexpr int HorizontalDelta = -1;
        constexpr int VerticalShrink = 2;

        const DocCursor &cursor = editor_.normalCursor();
        if (!cursor.isNull() && cursor.isInsert()) {
            const DocLoc &docLoc = cursor.loc();
            const VCharLoc charLoc = cvt_.toCharLoc(docLoc);
            if (!charLoc.isNull()) {
                const LineBound b = getLineBound(charLoc);
                const i32 x = cvt_.toX(charLoc) + HorizontalDelta;
                p.drawLine(x, b.top() + VerticalShrink, x, b.bottom() - VerticalShrink);
            }
        }
    }
}

void TextAreaImpl::moveCursorUp()
{

	// TODO !!!
	// 如果光标位置不在视图内，则只做一件事：把文档视图滚动到恰好显示光标所在行

    // 第一步：确保上一行在视图内
    // 注意，在第一步完成后，第一步依赖的视图元素位置可能已经失效，第二步需要重新获取
    const DocLoc & oldDocLoc = editor_.normalCursor().to();
    const VCharLoc oldCharLoc = cvt_.toCharLoc(oldDocLoc);
    const bool viewportMoved = ensureHasPrevLine(oldCharLoc);

    // 第二步：使用第一步更新后的新的视图数据取目标位置
    const DocLoc newLoc = getNextUpLoc(editor_.normalCursor().to());
    if (!newLoc.isNull())
    {
        editor_.setNormalCursor(newLoc);
    }

	// 第三步：移除尾部多余的行
    if (viewportMoved) {
        removeSpareRow();
        sigViewportChanged_();
    }
}

void TextAreaImpl::moveCursorDown()
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
        sigViewportChanged_();
	}
}

void TextAreaImpl::moveCursorLeft()
{
    const DocLoc oldDocLoc = editor_.normalCursor().to();
    const VCharLoc oldCharLoc = cvt_.toCharLoc(oldDocLoc);
    bool viewportChanged = false;
    if (noPrevCharAtSameLine(oldCharLoc))
    {
        viewportChanged = ensureHasPrevLine(oldCharLoc);
    }

    const DocLoc newLoc = editor_.getNextLeftLocByChar(oldDocLoc);
    if (!newLoc.isNull())
    {
        editor_.setNormalCursor(newLoc);

		const VCharLoc charLoc = cvt_.toCharLoc(newLoc);
        stableX_ = cvt_.toX(charLoc);
    }

    if (viewportChanged) {
        removeSpareRow();
        sigViewportChanged_();
    }
}

void TextAreaImpl::moveCursorRight()
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
        sigViewportChanged_();
	}
}

void TextAreaImpl::movePageUp()
{
}

void TextAreaImpl::movePageDown()
{
    gui::VLineLoc newViewLoc = getShownLastLineLoc();

}

void TextAreaImpl::setViewLoc(const ViewLoc & viewLoc)
{
    if (vloc_ == viewLoc)
    {
        return;
    }

    vloc_ = viewLoc;

    markTextImgDirty();
}

void TextAreaImpl::movePageHeadOneLine()
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

void TextAreaImpl::drawEachLineNum(std::function<void(RowN lineNum, i32 baseline, const RowBound &bound, bool isLastAct)> && action) const
{
    const int rowCnt = page_.size();

    i32 offset = -vloc_.line();

    for (int r = 0; r < rowCnt; ++r)
    {
        const VRowLoc loc(r);
        const RowBound bound = getRowBound(loc);
        const RowN lineNum = vloc_.row() + r;
        const RowN lastAct = editor_.lastActRow();
        const bool isLastAct = lineNum == lastAct;
        const i32 baseline = cvt_.toBaselineY(offset);

        action(lineNum, baseline, bound, isLastAct);

        offset += page_[r].size();
    }
}

void TextAreaImpl::drawEachChar(QPainter &p)
{
    const int rowCnt = page_.size();
    if (rowCnt == 0)
    {
        return;
    }

    const VRow &curRow = page_[0];
    const int curRowSize = curRow.size();

    i32 lineOffset = 0;

    for (int i = vloc_.line(); i < curRowSize && lineOffset < lineCountLimit_; ++i)
    {
        const VLine & line = curRow[i];

        const i32 baseline = cvt_.toBaselineY(lineOffset);

        for (const VChar &c : line)
        {
            drawChar(p, c.x(), baseline, c.uchar());
        }

        ++lineOffset;
    }

    for (int r = 1; r < rowCnt && lineOffset < lineCountLimit_; ++r)
    {
        const VRow & row = page_[r];

        for (const VLine & line : row)
        {
            const i32 baseline = cvt_.toBaselineY(lineOffset);

            for (const VChar & c : line)
            {
                drawChar(p, c.x(), baseline, c.uchar());
            }

            ++lineOffset;
        }
    }
}

void TextAreaImpl::moveCursor(TextArea::Dir dir)
{
    Lock lock(mtx_);

    switch (dir) {
    case TextArea::Dir::Up:
        moveCursorUp();
        break;
    case TextArea::Dir::Down:
        moveCursorDown();
        break;
    case TextArea::Dir::Left:
        moveCursorLeft();
        break;
    case TextArea::Dir::Right:
        moveCursorRight();
        break;
    default:
        break;
    }
}

void TextAreaImpl::putCursor(i32 x, i32 y)
{
    Lock lock(mtx_);
    const VCharLoc charLoc = cvt_.toCharLoc(x, y);
    const DocLoc docLoc = cvt_.toDocLoc(charLoc);
    editor_.setNormalCursor(docLoc);
    stableX_ = cvt_.toX(charLoc);

    sigShouldRepaint_();
}

void TextAreaImpl::movePage(TextArea::Dir dir)
{
    Lock lock(mtx_);

    switch (dir) {
    case TextArea::Dir::Up:
        movePageUp();
        break;
    case TextArea::Dir::Down:
        movePageDown();
        break;
    default:
        throw std::logic_error("unsupported dir for TextArea::movePage(...)");
    }
}

bool TextAreaImpl::moveDownByOneLine()
{
    Lock lock(mtx_);

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
            sptr<Row> row = editor_.doc().rowAt(newDocRowIndex);
            VRow vrow;
            makeVRow(*row, vrow);
            page_.pushBack(std::move(vrow));
		}
	}

	movePageHeadOneLine();

	return true;
}

void TextAreaImpl::updateStableXByCurrentCursor()
{
    const DocLoc docLoc = editor_.normalCursor().to();
    const VCharLoc vCharLoc = cvt_.toCharLoc(docLoc);

    stableX_ = cvt_.toX(vCharLoc);
}

void TextAreaImpl::remakePage()
{
    page_.clear();

    markTextImgDirty();

    const RowN rowCnt = editor_.doc().rowCnt();

    const int lineDelta = -vloc_.line();

    RowRange range = RowRange::byOffAndLen(vloc_.row(), lineCountLimit_);
    if (rowCnt < range.end()) {
        range.setEnd(rowCnt);
    }

    std::map<RowN, sptr<Row>> rows = editor_.doc().rowsAt(range);

    int h = lineDelta;
    for (RowN row = range.left(); row <= range.right(); ++row) {
        if (h >= lineCountLimit_) {
            break;
        }

        VRow vrow;
        makeVRow(*(rows[row]), vrow);

        const int rowSize = vrow.size();
        page_.pushBack(std::move(vrow));
        h += rowSize;
    }
}

void TextAreaImpl::makeVRow(const Row &row, VRow &vrow)
{
    assert(vrow.size() == 0);

    DocLineCharInStream charStream(row);

    class Provider : public GlyphWidthProvider {
    public:
        Provider(GlyphCache &glyphCache) : glyphCache_(glyphCache) {}
        virtual ~Provider() {}
        virtual int glyphWidth(char32_t unicode) override {
            return glyphCache_.advance(unicode);
        }
    private:
        GlyphCache &glyphCache_;
    };

    Provider provider(glyphCache_);
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

VLineLoc TextAreaImpl::getShownLastLineLoc() const
{
	const int rowCnt = page_.size();

	const VRow & lastRow = page_[rowCnt - 1];

	const int prevLineCnt = page_.lineCnt() - vloc_.line() - lastRow.size();

	if (page_.lineCnt() - vloc_.line() < lineCountLimit_)
	{
		const int lastLineIndex = page_.lineCnt() - vloc_.line() - prevLineCnt - 1;
		return VLineLoc(page_.size() - 1, lastLineIndex);
	}
	else
	{
		const int lastLineIndex = lineCountLimit_ - prevLineCnt - 1;
		return VLineLoc(page_.size() - 1, lastLineIndex);
	}
}


}

