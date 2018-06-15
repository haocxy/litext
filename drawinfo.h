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

struct LineDrawInfo
{

    RowIndex rowModelIndex = 0;
    ColCnt colOffset = 0;
    int flag = 0;
    int baseLineY = 0; // 基线y坐标
    int drawTopY = 0; // 这一行的绘制区域上限y坐标
    int drawBottomY = 0; // 这一行的绘制区域下限y坐标
    bool rowEnd = false;
    std::vector<CharDrawInfo> charInfos;
};

struct DrawInfo
{
    std::vector<LineDrawInfo> lineInfos;
};

