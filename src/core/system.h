#pragma once

#include <cstddef>
#include <vector>

#include "core/fs.h"


namespace SystemUtil
{

size_t pageSize();

int processorCount();

fs::path userHome();

std::vector<fs::path> fonts();

// 屏幕水平方向 DPI
// 注意：内部实现用到了 QWidget ，所以需要先构造 QApplication
int screenHorizontalDpi();

// 屏幕竖直方向 DPI
// 注意：内部实现用到了 QWidget ，所以需要先构造 QApplication
int screenVerticalDpi();

}
