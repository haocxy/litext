#pragma once

#include "pixel.h"


namespace gui
{


class VerticalLine {
public:
	Pixel::Raw x() const { return x_; }
	void setX(Pixel::Raw x) { x_ = x; }
	Pixel::Raw top() const { return top_; }
	void setTop(Pixel::Raw top) { top_ = top; }
	Pixel::Raw bottom() const { return bottom_; }
	void setBottom(Pixel::Raw bottom) { bottom_ = bottom; }
private:
	Pixel::Raw x_ = 0;
	Pixel::Raw top_ = 0;
	Pixel::Raw bottom_ = 0;
};


}
