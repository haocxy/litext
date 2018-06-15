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
    int baseLineY = 0; // ����y����
    int drawTopY = 0; // ��һ�еĻ�����������y����
    int drawBottomY = 0; // ��һ�еĻ�����������y����
    bool rowEnd = false;
    std::vector<CharDrawInfo> charInfos;
};

struct DrawInfo
{
    std::vector<LineDrawInfo> lineInfos;
};

