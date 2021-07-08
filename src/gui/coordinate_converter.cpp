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

}
