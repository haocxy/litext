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
    return LineOffset(y.value() / config_.lineHeight().value());
}

Pixel CoordinateConverter::toBaselineY(LineOffset off) const
{
    return Pixel((1 + off.value()) * config_.lineHeight().value() - config_.font().descent());
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

static inline Pixel calcLeftBound(Pixel x, Pixel leftWidth, Pixel margin)
{
    assert(leftWidth.value() > 0);

    return Pixel(x.value() - (leftWidth.value() >> 1) - margin.value());
}

static inline Pixel calcRightBound(Pixel x, Pixel curWidth)
{
    assert(curWidth.value() > 0);

    if ((curWidth.value() & 1) == 0) {
        return Pixel(x.value() + (curWidth.value() >> 1) - 1);
    } else {
        return Pixel(x.value() + (curWidth.value() >> 1));
    }
}

CharLoc CoordinateConverter::toCharLoc(const VLineLoc &lineLoc, Pixel x) const
{
    if (lineLoc.isNull() || lineLoc.isAfterLastRow()) {
        return CharLoc::newCharLocAfterLastChar(lineLoc);
    }

    const VLine &line = page_.getLine(lineLoc);

    const int charCnt = line.size();

    if (charCnt == 0) {
        return CharLoc::newCharLocAfterLastChar(lineLoc);
    }

    const Pixel margin = config_.hMargin();

    // 为了简化处理，把第一个字符单独处理，因为第一个字符没有前一个字符
    const VChar &firstChar = line[0];
    const Pixel firstX(firstChar.x());
    const Pixel firstWidth(firstChar.width());
    const Pixel firstCharRightBound = calcRightBound(firstX, firstWidth);
    if (x <= firstCharRightBound) {
        return CharLoc(lineLoc, 0);
    }

    int left = 1;
    int right = charCnt - 1;
    while (left <= right) {
        const int mid = ((left + right) >> 1);
        const VChar &c = line[mid];
        const Pixel cx(c.x());
        const Pixel a = calcLeftBound(cx, Pixel(line[mid - 1].width()), margin);
        const Pixel b = calcRightBound(cx, Pixel(c.width()));
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
