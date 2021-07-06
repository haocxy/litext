#include "coordinate_converter.h"


namespace gui
{





LineN CoordinateConverter::lineOffset(const VRowLoc &vRowLoc) const
{
    const RowN row = vRowLoc.row();

    LineN sum = 0;

    const RowN rowCnt = page_.size();

    for (RowN i = 0; i < rowCnt && i < row; ++i) {
        sum += page_[i].size();
    }

    sum -= vloc_.line();

    return sum;
}

}
