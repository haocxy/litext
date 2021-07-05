#pragma once


namespace gui
{


class RowBound {
public:
    RowBound() = default;
    RowBound(int top, int height) :m_top(top), m_height(height) {}
    int top() const { return m_top; }
    void setTop(int top) { m_top = top; }
    int height() const { return m_height; }
    void setHeight(int height) { m_height = height; }
private:
    int m_top = 0;
    int m_height = 0;
};


}
