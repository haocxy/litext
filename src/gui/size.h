#pragma once

#include "core/basetype.h"


namespace gui
{


class Size {
public:
    Size() {}
    Size(i32 width, i32 height) : m_width(width), m_height(height) {}
    i32 width() const { return m_width; }
    i32 height() const { return m_height; }
    void setWidth(i32 width) { m_width = width; }
    void setHeight(i32 height) { m_height = height; }
    bool operator==(const Size &b) const { return m_width == b.m_width && m_height == b.m_height; }
    bool operator!=(const Size &b) const { return !(*this == b); }
private:
    i32 m_width = 0;
    i32 m_height = 0;
};



}
