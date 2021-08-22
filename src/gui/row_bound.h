#pragma once

#include "core/basetype.h"


namespace gui
{


class RowBound {
public:
    RowBound() = default;
    RowBound(i32 top, i32 height) :m_top(top), m_height(height) {}
    i32 top() const { return m_top; }
    void setTop(i32 top) { m_top = top; }
    i32 height() const { return m_height; }
    void setHeight(i32 height) { m_height = height; }
private:
    i32 m_top{ 0 };
    i32 m_height{ 0 };
};


}
