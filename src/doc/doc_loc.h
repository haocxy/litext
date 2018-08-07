#pragma once

#include "doc_define.h"
#include "common/dir_enum.h"

class DocLoc
{
public:
    static DocLoc newDocLocAfterLastRow()
    {
        DocLoc loc(0, 0);
        loc.setFlag(kAfterLastRow);
        loc.setFlag(kAfterLastChar);
        return loc;
    }
    static DocLoc newDocLocAfterLastChar(RowN row)
    {
        DocLoc loc(row, 0);
        loc.setFlag(kAfterLastChar);
        return loc;
    }
public:
    DocLoc() :m_flag(kIsNull), m_row(0), m_col(0) {}
    DocLoc(RowN row, CharN col) : m_row(row), m_col(col) {}
    RowN row() const { return m_row; }
    void setRow(RowN row) { m_row = row; }
    CharN col() const { return m_col; }
    void setCol(CharN col) { m_col = col; }
    bool isNull() const { return hasFlag(kIsNull); }
    bool isAfterLastRow() const { return hasFlag(kAfterLastRow); }
    bool isAfterLastChar() const { return hasFlag(kAfterLastChar); }
    bool operator<(const DocLoc &b) const
    {
        if (m_row < b.m_row)
        {
            return true;
        }
        if (b.m_row < m_row)
        {
            return false;
        }
        return m_col < b.m_col;
    }
    bool operator>(const DocLoc &b) const
    {
        return b < (*this);
    }
    bool operator==(const DocLoc &b) const
    {
        return m_row == b.m_row && m_col == b.m_col;
    }
    bool operator!=(const DocLoc &b) const
    {
        return !((*this) == b);
    }
    DocLoc nextUp() const { return DocLoc(m_row - 1, m_col); }
    DocLoc nextDown() const { return DocLoc(m_row + 1, m_col); }
    DocLoc nextLeft() const { return DocLoc(m_row, m_col - 1); }
    DocLoc nextRight() const { return DocLoc(m_row, m_col + 1); }
private:
    typedef uint_least8_t flag_t;
    enum : flag_t
    {
        kIsNull = 1,
        kAfterLastRow = 1 << 1,
        kAfterLastChar = 1 << 2,
    };
    bool hasFlag(flag_t f) const { return (m_flag & f) != 0; }
    void setFlag(flag_t f) { m_flag |= f; }
private:
    flag_t m_flag = 0;
    RowN m_row = 0;
    CharN m_col = 0;
};

