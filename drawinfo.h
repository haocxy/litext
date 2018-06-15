#pragma once

#include <vector>

#include "doc_model_define.h"

struct CharDrawInfo
{
    wchar_t ch = 0;
    int drawLeftX = 0;
    int rawFontWidth = 0;
    int drawTotalWidth = 0;
};

namespace NSDrawInfo
{
    namespace LineFlag
    {
        const int RowEnd = 1 << 1;
    }
}

struct LineDrawInfo
{
    RowIndex rowModelIndex = 0;
    ColCnt colOffset = 0;
    int flag = 0;
    std::vector<CharDrawInfo> charInfos;
};

struct DrawInfo
{
    std::vector<LineDrawInfo> lineInfos;
};

