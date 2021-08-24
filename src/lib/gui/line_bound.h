#pragma once

#include "core/basetype.h"


namespace gui
{


class LineBound {
public:
    LineBound() = default;
    LineBound(i32 top, i32 bottom) :m_top(top), m_bottom(bottom) {}
    i32 top() const { return m_top; }
    void setTop(i32 top) { m_top = top; }
    i32 bottom() const { return m_bottom; }
    void setBottom(i32 bottom) { m_bottom = bottom; }
private:
    i32 m_top = 0;
    i32 m_bottom = 0;
};


}
