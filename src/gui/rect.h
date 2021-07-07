#pragma once

#include "pixel.h"


namespace gui
{
    class Rect
    {
    public:
        Pixel top() const { return m_top; }
        void setTop(Pixel top) { m_top = top; }
        Pixel left() const { return m_left; }
        void setLeft(Pixel left) { m_left = left; }
        Pixel width() const { return m_width; }
        void setWidth(Pixel width) { m_width = width; }
        Pixel height() const { return m_height; }
        void setHeight(Pixel height) { m_height = height; }
    private:
        Pixel m_top{ 0 };
        Pixel m_left{ 0 };
        Pixel m_width{ 0 };
        Pixel m_height{ 0 };
    };
}
