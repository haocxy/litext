#pragma once

#include <cstddef>
#include <vector>

#include "fs.h"
#include "basetype.h"


namespace SystemUtil
{

i32 pageSize();

i32 processorCount();

fs::path userHome();

std::vector<fs::path> fonts();

// 屏幕水平方向 DPI
// 注意：内部实现用到了 QWidget ，所以需要先构造 QApplication
i32 screenHorizontalDpi();

// 屏幕竖直方向 DPI
// 注意：内部实现用到了 QWidget ，所以需要先构造 QApplication
i32 screenVerticalDpi();

}
