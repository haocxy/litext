#pragma once


namespace gui
{


class LineBound {
public:
    LineBound() = default;
    LineBound(Pixel top, Pixel bottom) :m_top(top), m_bottom(bottom) {}
    Pixel top() const { return m_top; }
    void setTop(Pixel top) { m_top = top; }
    Pixel bottom() const { return m_bottom; }
    void setBottom(Pixel bottom) { m_bottom = bottom; }
private:
    Pixel m_top{ 0 };
    Pixel m_bottom{ 0 };
};


}
