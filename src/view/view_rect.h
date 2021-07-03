#pragma once


namespace view
{
    class Rect
    {
    public:
        int top() const { return m_top; }
        void setTop(int top) { m_top = top; }
        int left() const { return m_left; }
        void setLeft(int left) { m_left = left; }
        int width() const { return m_width; }
        void setWidth(int width) { m_width = width; }
        int height() const { return m_height; }
        void setHeight(int height) { m_height = height; }
    private:
        int m_top = 0;
        int m_left = 0;
        int m_width = 0;
        int m_height = 0;
    };
}
