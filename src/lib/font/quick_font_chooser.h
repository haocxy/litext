#pragma once

#include "core/font_index.h"
#include "core/font.h"


namespace font
{

// 快速字体选择
// 通过查找大概率能找到的主流安全字体来尽快地选择字体
// 主要用于程序部署后的首次启动时选择默认字体
class QuickFountChooser {
public:
    QuickFountChooser();

    ~QuickFountChooser();

    opt<FontIndex> choose();

private:
    opt<FontIndex> chooseForWindows();

    opt<FontIndex> chooseByScanAllFoundFont();

private:
    FontContext context_;
};

}
