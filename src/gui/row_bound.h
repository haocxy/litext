#pragma once

#include "pixel.h"


namespace gui
{


class RowBound {
public:
    RowBound() = default;
    RowBound(Pixel::Raw top, Pixel::Raw height) :m_top(top), m_height(height) {}
    Pixel::Raw top() const { return m_top; }
    void setTop(Pixel::Raw top) { m_top = top; }
    Pixel::Raw height() const { return m_height; }
    void setHeight(Pixel::Raw height) { m_height = height; }
private:
    Pixel::Raw m_top{ 0 };
    Pixel::Raw m_height{ 0 };
};


}
