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

Pixel CoordinateConverter::toBaselineY(LineOffset off) const
{
    return Pixel((1 + off.value()) * config_.lineHeight() - config_.font().descent());
}

}
