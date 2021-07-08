#pragma once

#include "pixel.h"


namespace gui
{


class Size {
public:
    Size() {}
    Size(Pixel::Raw width, Pixel::Raw height) : m_width(width), m_height(height) {}
    Pixel::Raw width() const { return m_width; }
    Pixel::Raw height() const { return m_height; }
    void setWidth(Pixel::Raw width) { m_width = width; }
    void setHeight(Pixel::Raw height) { m_height = height; }
    bool operator==(const Size &b) const { return m_width == b.m_width && m_height == b.m_height; }
    bool operator!=(const Size &b) const { return !(*this == b); }
private:
    Pixel::Raw m_width = 0;
    Pixel::Raw m_height = 0;
};



}
