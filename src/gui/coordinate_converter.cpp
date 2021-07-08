#include "coordinate_converter.h"

#include "text_area_config.h"



namespace gui
{





LineOffset CoordinateConverter::toLineOffset(const VRowLoc &vRowLoc) const
{
    const RowN row = vRowLoc.row();

    LineOffset sum(0);

    const RowN rowCnt = page_.size();

    for (RowN i = 0; i < rowCnt && i < row; ++i) {
        sum += page_[i].size();
    }

    sum -= vloc_.line();

    return sum;
}

LineOffset CoordinateConverter::toLineOffset(const VLineLoc &vLineLoc) const
{
    LineOffset sum(vLineLoc.line());

    const RowN rowCnt = page_.size();
    const RowN row = vLineLoc.row();

    for (RowN i = 0; i < rowCnt && i < row; ++i) {
        sum += page_[i].size();
    }

    sum -= vloc_.line();

    return sum;
}

LineOffset CoordinateConverter::toLineOffset(Pixel y) const
{
    return LineOffset(y.value() / config_.lineHeight());
}

Pixel CoordinateConverter::toBaselineY(LineOffset off) const
{
    return Pixel((1 + off.value()) * config_.lineHeight() - config_.font().descent());
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

CharLoc CoordinateConverter::toCharLoc(const VLineLoc &lineLoc, Pixel xPixel) const
{
    if (lineLoc.isNull() || lineLoc.isAfterLastRow()) {
        return CharLoc::newCharLocAfterLastChar(lineLoc);
    }

    const VLine &line = page_.getLine(lineLoc);
    const Pixel::Raw x = xPixel.value();
    const int charCnt = line.size();

    if (charCnt == 0) {
        return CharLoc::newCharLocAfterLastChar(lineLoc);
    }

    const Pixel::Raw margin = config_.hMargin();

    // 为了简化处理，把第一个字符单独处理，因为第一个字符没有前一个字符
    const VChar &firstChar = line[0];
    const Pixel::Raw firstX = firstChar.x();
    const Pixel::Raw firstWidth = firstChar.width();
    const Pixel::Raw firstCharRightBound = calcRightBound(firstX, firstWidth);
    if (x <= firstCharRightBound) {
        return CharLoc(lineLoc, 0);
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
            return CharLoc(lineLoc, mid);
        }
    }

    return CharLoc::newCharLocAfterLastChar(lineLoc);
}

}
