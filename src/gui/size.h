#pragma once


namespace gui
{


class Size {
public:
    Size() {}
    Size(int width, int height) : m_width(width), m_height(height) {}
    int width() const { return m_width; }
    int height() const { return m_height; }
    void setWidth(int width) { m_width = width; }
    void setHeight(int height) { m_height = height; }
    bool operator==(const Size &b) const { return m_width == b.m_width && m_height == b.m_height; }
    bool operator!=(const Size &b) const { return !(*this == b); }
private:
    int m_width = 0;
    int m_height = 0;
};



}
