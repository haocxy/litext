#pragma once

#include "pixel.h"


namespace gui
{


class RowBound {
public:
    RowBound() = default;
    RowBound(Pixel top, Pixel height) :m_top(top), m_height(height) {}
    Pixel top() const { return m_top; }
    void setTop(Pixel top) { m_top = top; }
    Pixel height() const { return m_height; }
    void setHeight(Pixel height) { m_height = height; }
private:
    Pixel m_top{ 0 };
    Pixel m_height{ 0 };
};


}
