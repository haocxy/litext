#pragma once

#include "pixel.h"


namespace gui
{


class Size {
public:
    Size() {}
    Size(Pixel width, Pixel height) : m_width(width), m_height(height) {}
    Pixel width() const { return m_width; }
    Pixel height() const { return m_height; }
    void setWidth(Pixel width) { m_width = width; }
    void setHeight(Pixel height) { m_height = height; }
    bool operator==(const Size &b) const { return m_width == b.m_width && m_height == b.m_height; }
    bool operator!=(const Size &b) const { return !(*this == b); }
private:
    Pixel m_width{ 0 };
    Pixel m_height{ 0 };
};



}
