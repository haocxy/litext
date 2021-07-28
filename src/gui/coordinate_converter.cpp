#include "coordinate_converter.h"

#include "doc/doc.h"
#include "editor/editor.h"
#include "text_area_config.h"



namespace gui
{





LineOffset::Raw CoordinateConverter::toLineOffset(const VRowLoc &vRowLoc) const
{
    const RowN row = vRowLoc.row();

    LineOffset::Raw sum = 0;

    const RowN rowCnt = page_.size();

    for (RowN i = 0; i < rowCnt && i < row; ++i) {
        sum += page_[i].size();
    }

    sum -= vloc_.line();

    return sum;
}

LineOffset::Raw CoordinateConverter::toLineOffset(const VLineLoc &vLineLoc) const
{
    LineOffset::Raw sum = vLineLoc.line();

    const RowN rowCnt = page_.size();
    const RowN row = vLineLoc.row();

    for (RowN i = 0; i < rowCnt && i < row; ++i) {
        sum += page_[i].size();
    }

    sum -= vloc_.line();

    return sum;
}

LineOffset::Raw CoordinateConverter::toLineOffset(Pixel y) const
{
    return y.value() / config_.lineHeight();
}

Pixel::Raw CoordinateConverter::toX(const VCharLoc &charLoc) const
{
    if (charLoc.isNull()) {
        return 0;
    }

    if (charLoc.isAfterLastRow()) {
        return config_.hLayout().gap();
    }

    if (charLoc.isAfterLastChar()) {
        const VLine &line = page_[charLoc.row()][charLoc.line()];
        if (line.empty()) {
            return config_.hLayout().gap();
        }
        const VChar &vc = page_[charLoc.row()][charLoc.line()].last();
        return vc.x() + vc.width();
    }

    return page_[charLoc.row()][charLoc.line()][charLoc.col()].x();
}

Pixel::Raw CoordinateConverter::toBaselineY(LineOffset off) const
{
    return (1 + off.value()) * config_.lineHeight() - config_.font().descent();
}

VRowLoc CoordinateConverter::toRowLoc(VRowOffset vRowOffset) const
{
    const VRowOffset::Raw row = vRowOffset.value();
    const int vrowIndex = row - vloc_.row();
    const int vrowCnt = page_.size();
    if (vrowIndex < 0 || vrowIndex >= vrowCnt) {
        if (row >= editor_.doc().rowCnt()) {
            return VRowLoc::newRowLocAfterLastRow();
        }
        return VRowLoc();
    }

    if (row >= editor_.doc().rowCnt()) {
        return VRowLoc::newRowLocAfterLastRow();
    }

    return VRowLoc(vrowIndex);
}

VLineLoc CoordinateConverter::toVLineLoc(LineOffset lineOffset) const
{
    const int rowCnt = page_.size();
    const int offset = lineOffset.value();

    if (vloc_.line() == 0) {
        int sum = 0;

        for (int i = 0; i < rowCnt; ++i) {
            const VRow &row = page_[i];
            const int lineCnt = row.size();

            if (sum + lineCnt > offset) {
                return VLineLoc(i, offset - sum);
            }

            sum += lineCnt;
        }

        return VLineLoc::newLineLocAfterLastRow();
    }

    if (rowCnt == 0) {
        return VLineLoc();
    }

    const VRow &curRow = page_[0];
    const int curRowSize = curRow.size();
    if (vloc_.line() + offset < curRowSize) {
        return VLineLoc(0, vloc_.line() + offset);
    }

    int sum = curRowSize - vloc_.line();

    for (int i = 1; i < rowCnt; ++i) {
        const VRow &row = page_[i];
        const int lineCnt = row.size();

        if (sum + lineCnt > offset) {
            return VLineLoc(i, offset - sum);
        }

        sum += lineCnt;
    }

    return VLineLoc::newLineLocAfterLastRow();
}

static inline Pixel::Raw calcLeftBound(Pixel::Raw x, Pixel::Raw leftWidth, Pixel::Raw margin)
{
    assert(leftWidth > 0);

    return x - (leftWidth >> 1) - margin;
}

static inline Pixel::Raw calcRightBound(Pixel::Raw x, Pixel::Raw curWidth)
{
    assert(curWidth > 0);

    if ((curWidth & 1) == 0) {
        return x + (curWidth >> 1) - 1;
    } else {
        return x + (curWidth >> 1);
    }
}

VCharLoc CoordinateConverter::toCharLoc(const VLineLoc &lineLoc, Pixel xPixel) const
{
    if (lineLoc.isNull() || lineLoc.isAfterLastRow()) {
        return VCharLoc::newCharLocAfterLastChar(lineLoc);
    }

    const VLine &line = page_.getLine(lineLoc);
    const Pixel::Raw x = xPixel.value();
    const int charCnt = line.size();

    if (charCnt == 0) {
        return VCharLoc::newCharLocAfterLastChar(lineLoc);
    }

    const Pixel::Raw margin = config_.hLayout().hMargin();

    // 为了简化处理，把第一个字符单独处理，因为第一个字符没有前一个字符
    const VChar &firstChar = line[0];
    const Pixel::Raw firstX = firstChar.x();
    const Pixel::Raw firstWidth = firstChar.width();
    const Pixel::Raw firstCharRightBound = calcRightBound(firstX, firstWidth);
    if (x <= firstCharRightBound) {
        return VCharLoc(lineLoc, 0);
    }

    int left = 1;
    int right = charCnt - 1;
    while (left <= right) {
        const int mid = ((left + right) >> 1);
        const VChar &c = line[mid];
        const Pixel::Raw cx = c.x();
        const Pixel::Raw a = calcLeftBound(cx, line[mid - 1].width(), margin);
        const Pixel::Raw b = calcRightBound(cx, c.width());
        if (x < a) {
            right = mid - 1;
        } else if (x > b) {
            left = mid + 1;
        } else {
            return VCharLoc(lineLoc, mid);
        }
    }

    return VCharLoc::newCharLocAfterLastChar(lineLoc);
}

VCharLoc CoordinateConverter::toCharLoc(Pixel x, Pixel y) const
{
    const LineOffset::Raw lineOffset = toLineOffset(y);
    const VLineLoc lineLoc = toVLineLoc(LineOffset(lineOffset));
    return toCharLoc(lineLoc, x);
}

VCharLoc CoordinateConverter::toCharLoc(const DocLoc &docLoc) const
{
    if (docLoc.isNull()) {
        return VCharLoc();
    }

    if (docLoc.isAfterLastRow()) {
        return VCharLoc::newCharLocAfterLastRow();
    }

    const int r = docLoc.row() - vloc_.row();
    const int rowCnt = page_.size();
    if (r < 0 || r >= rowCnt) {
        return VCharLoc();
    }

    const VRow &vrow = page_[r];

    if (docLoc.isAfterLastChar()) {
        VLineLoc lineLoc(r, vrow.size() - 1);
        return VCharLoc::newCharLocAfterLastChar(lineLoc);
    }

    int lineIndex = 0;
    int charIndex = 0;

    int prevLineCharCnt = 0;

    const CharN col = docLoc.col();

    for (const VLine &vline : vrow) {
        for (const VChar &vc : vline) {
            if (charIndex == col) {
                return VCharLoc(r, lineIndex, col - prevLineCharCnt);
            }

            ++charIndex;
        }

        ++lineIndex;

        prevLineCharCnt += vline.size();
    }

    return VCharLoc();
}

DocLoc CoordinateConverter::toDocLoc(const VCharLoc &charLoc) const
{
    if (charLoc.isNull()) {
        return DocLoc();
    }

    if (charLoc.isAfterLastRow()) {
        return DocLoc::newDocLocAfterLastRow();
    }

    if (charLoc.isAfterLastChar()) {
        if (isLastLineOfRow(charLoc)) {
            return DocLoc::newDocLocAfterLastChar(vloc_.row() + charLoc.row());
        } else {
            // 如果不是最后一个显示行，则把光标放在下一个显示行最开始处
            return toDocLoc(VCharLoc(charLoc.row(), charLoc.line() + 1, 0));
        }
    }


    const VRow &vrow = page_[charLoc.row()];
    int lastLine = charLoc.line();
    if (vloc_.row() == 0) {
        // 如果在第一个VRow，则需要把偏移量加上，因为ViewLoc中的line属性为行偏移
        lastLine += vloc_.line();
    }
    CharN col = charLoc.col();
    for (int i = 0; i < lastLine; ++i) {
        col += vrow[i].size();
    }

    return DocLoc(vloc_.row() + charLoc.row(), col);
}

DocLoc CoordinateConverter::toDocLoc(Pixel x, Pixel y) const
{
    const VCharLoc charLoc = toCharLoc(x, y);
    const DocLoc docLoc = toDocLoc(charLoc);
    return docLoc;
}

bool CoordinateConverter::isLastLineOfRow(const VLineLoc &lineLoc) const
{
    return lineLoc.line() == page_[lineLoc.row()].size() - 1;
}

}
