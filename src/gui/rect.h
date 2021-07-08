#pragma once

#include "pixel.h"


namespace gui
{
    class Rect
    {
    public:
        Pixel::Raw top() const { return m_top; }
        void setTop(Pixel::Raw top) { m_top = top; }
        Pixel::Raw left() const { return m_left; }
        void setLeft(Pixel::Raw left) { m_left = left; }
        Pixel::Raw width() const { return m_width; }
        void setWidth(Pixel::Raw width) { m_width = width; }
        Pixel::Raw height() const { return m_height; }
        void setHeight(Pixel::Raw height) { m_height = height; }
    private:
        Pixel::Raw m_top{ 0 };
        Pixel::Raw m_left{ 0 };
        Pixel::Raw m_width{ 0 };
        Pixel::Raw m_height{ 0 };
    };
}
