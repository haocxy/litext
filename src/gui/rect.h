#pragma once

#include "core/basetype.h"


namespace gui
{
    class Rect
    {
    public:
        i32 top() const { return m_top; }
        void setTop(i32 top) { m_top = top; }
        i32 left() const { return m_left; }
        void setLeft(i32 left) { m_left = left; }
        i32 width() const { return m_width; }
        void setWidth(i32 width) { m_width = width; }
        i32 height() const { return m_height; }
        void setHeight(i32 height) { m_height = height; }
    private:
        i32 m_top{ 0 };
        i32 m_left{ 0 };
        i32 m_width{ 0 };
        i32 m_height{ 0 };
    };
}
