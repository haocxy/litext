#pragma once


namespace gui
{


class LineBound {
public:
    LineBound() = default;
    LineBound(int top, int bottom) :m_top(top), m_bottom(bottom) {}
    int top() const { return m_top; }
    void setTop(int top) { m_top = top; }
    int bottom() const { return m_bottom; }
    void setBottom(int bottom) { m_bottom = bottom; }
private:
    int m_top = 0;
    int m_bottom = 0;
};


}
