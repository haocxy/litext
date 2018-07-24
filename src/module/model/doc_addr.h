#pragma once

#include "module/model/model_define.h"

class DocAddr
{
public:
    DocAddr() :m_isNull(true), m_line(0), m_col(0) {}
    DocAddr(LineN line, CharN col) : m_line(line), m_col(col) {}
    LineN line() const { return m_line; }
    void setLine(LineN line) { m_line = line; }
    CharN col() const { return m_col; }
    void setCol(CharN col) { m_col = col; }
    bool isNull() const { return m_isNull; }
    bool operator<(const DocAddr &b) const
    {
        if (m_line < b.m_line)
        {
            return true;
        }
        if (b.m_line < m_line)
        {
            return false;
        }
        return m_line < b.m_line;
    }
    bool operator>(const DocAddr &b) const
    {
        return b < (*this);
    }
    bool operator==(const DocAddr &b) const
    {
        return m_line == b.m_line && m_col == b.m_col;
    }
    bool operator!=(const DocAddr &b) const
    {
        return !((*this) == b);
    }
private:
    bool m_isNull = false;
    LineN m_line = 0;
    CharN m_col = 0;
};

