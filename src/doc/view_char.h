#pragma once


namespace doc
{

class VChar {
public:
    VChar() {}

    VChar(char32_t unicode, int x, int width)
        : m_unicode(unicode), m_x(x), m_width(width) {}

    char32_t uchar() const { return m_unicode; }
    int x() const { return m_x; }

    void setUChar(char32_t unicode) { m_unicode = unicode; }
    void setX(int x) { m_x = x; }

    int width() const { return m_width; }
    void setWidth(int width) { m_width = width; }

private:
    char32_t m_unicode = 0;
    int m_x = 0;
    int m_width = 0;
};

}
