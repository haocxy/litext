#pragma once

#include "common/nullable_class.h"

namespace draw
{
    class VerticalLine : public NullableClass
    {
    public:
        int x() const { return m_x; }
        void setX(int x) { m_x = x; }
        int top() const { return m_top; }
        void setTop(int top) { m_top = top; }
        int bottom() const { return m_bottom; }
        void setBottom(int bottom) { m_bottom = bottom; }
    private:
        int m_x = 0;
        int m_top = 0;
        int m_bottom = 0;
    };
}
