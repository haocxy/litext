#pragma once

#include "core/strong_type_integer.h"


namespace gui
{

// 行偏移
// 某个位置距离视口顶端的行数
// 
// 举例：
// 
// 假设，在当前视口中，第1段被自动换行分为了5行，第2段被自动换行分为了3行，而视口顶端在第1段的第2行，
// 那么，第2段的第2行的偏移为3，它们分别是第1段的第2行、第1段的第3行、第2段的第1行。
// 
// 第1段第1行没有被算进来是因为它不在视口中。
DEFINE_STRONG_INT_TYPE(LineOffset, int);


}
