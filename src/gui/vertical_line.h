#pragma once

#include "pixel.h"


namespace gui
{


class VerticalLine {
public:
	Pixel x() const { return x_; }
	void setX(Pixel x) { x_ = x; }
	Pixel top() const { return top_; }
	void setTop(Pixel top) { top_ = top; }
	Pixel bottom() const { return bottom_; }
	void setBottom(Pixel bottom) { bottom_ = bottom; }
private:
	Pixel x_{ 0 };
	Pixel top_{ 0 };
	Pixel bottom_{ 0 };
};


}
