#pragma once

class CharDrawInfo
{
public:
    static CharDrawInfo makeNull()
    {
        return CharDrawInfo(0, 0, 0);
    }
public:
    CharDrawInfo(int x, int y, wchar_t unicode) :m_x(x), m_y(y), m_unicode(unicode) {}
    int x() const { return m_x; }
    int y() const { return m_y; }
    wchar_t unicode() const { return m_unicode; }
    bool isNull() const { return m_unicode == 0; }
    bool isValid() const { return m_unicode != 0; }
private:
    int m_x = 0;
    int m_y = 0;
    wchar_t m_unicode = 0;
};


