#pragma once

#include "core/basetype.h"


namespace gui
{


class VerticalLine {
public:
    i32 x() const { return x_; }
	void setX(i32 x) { x_ = x; }
    i32 top() const { return top_; }
	void setTop(i32 top) { top_ = top; }
    i32 bottom() const { return bottom_; }
	void setBottom(i32 bottom) { bottom_ = bottom; }
private:
    i32 x_ = 0;
    i32 top_ = 0;
    i32 bottom_ = 0;
};


}
