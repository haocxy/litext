#pragma once


class VerticalLine {
public:
	int x() const { return x_; }
	void setX(int x) { x_ = x; }
	int top() const { return top_; }
	void setTop(int top) { top_ = top; }
	int bottom() const { return bottom_; }
	void setBottom(int bottom) { bottom_ = bottom; }
private:
	int x_ = 0;
	int top_ = 0;
	int bottom_ = 0;
};
