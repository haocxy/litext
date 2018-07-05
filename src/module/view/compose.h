#pragma once

#include <stdint.h>

#include "module/model/model_define.h"

class CompChar
{
public:
    wchar_t GetChar() const { return m_ch; }
    void SetChar(wchar_t ch) { m_ch = ch; }
    int GetX() const { return m_x; }
    void SetX(int x) { m_x = x; }
    int GetY() const { return m_y; }
    void SetY(int y) { m_y = y; }
private:
    wchar_t m_ch = 0;
    int m_x = 0;
    int m_y = 0;
};

class CompCharStream
{
public:
};
