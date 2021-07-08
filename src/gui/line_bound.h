#pragma once


namespace gui
{


class LineBound {
public:
    LineBound() = default;
    LineBound(Pixel::Raw top, Pixel::Raw bottom) :m_top(top), m_bottom(bottom) {}
    Pixel::Raw top() const { return m_top; }
    void setTop(Pixel::Raw top) { m_top = top; }
    Pixel::Raw bottom() const { return m_bottom; }
    void setBottom(Pixel::Raw bottom) { m_bottom = bottom; }
private:
    Pixel::Raw m_top = 0;
    Pixel::Raw m_bottom = 0;
};


}
