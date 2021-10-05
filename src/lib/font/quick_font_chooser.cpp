#include "quick_font_chooser.h"


// 不同操作系统的代码只有使用的字体文件等差异,没有和编译相关的差异
// 我希望所有平台的代码都被编译
static bool isWindows()
{
#ifdef WIN32
    return true;
#else
    return false;
#endif
}

namespace font
{

QuickFountChooser::QuickFountChooser()
{
}

QuickFountChooser::~QuickFountChooser()
{
}

opt<FontIndex> QuickFountChooser::choose()
{
    return std::nullopt;
}

}
