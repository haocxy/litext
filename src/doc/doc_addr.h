#pragma once

#include "doc_define.h"
#include "common/dir_enum.h"

class DocAddr
{
public:
    static DocAddr newCharAddrAfterLastPhase()
    {
        DocAddr docAddr(0, 0);
        docAddr.setFlag(kAfterLastPhase);
        docAddr.setFlag(kAfterLastChar);
        return docAddr;
    }
    static DocAddr newCharAddrAfterLastChar(LineN line)
    {
        DocAddr docAddr(line, 0);
        docAddr.setFlag(kAfterLastChar);
        return docAddr;
    }
public:
    DocAddr() :m_flag(kIsNull), m_line(0), m_col(0) {}
    DocAddr(LineN line, CharN col) : m_line(line), m_col(col) {}
    LineN line() const { return m_line; }
    void setLine(LineN line) { m_line = line; }
    CharN col() const { return m_col; }
    void setCol(CharN col) { m_col = col; }
    bool isNull() const { return hasFlag(kIsNull); }
    bool isAfterLastPhase() const { return hasFlag(kAfterLastPhase); }
    bool isAfterLastChar() const { return hasFlag(kAfterLastChar); }
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
    DocAddr nextUp() const { return DocAddr(m_line - 1, m_col); }
    DocAddr nextDown() const { return DocAddr(m_line + 1, m_col); }
    DocAddr nextLeft() const { return DocAddr(m_line, m_col - 1); }
    DocAddr nextRight() const { return DocAddr(m_line, m_col + 1); }
private:
    typedef uint_least8_t flag_t;
    enum : flag_t
    {
        kIsNull = 1,
        kAfterLastPhase = 1 << 1,
        kAfterLastChar = 1 << 2,
    };
    bool hasFlag(flag_t f) const { return (m_flag & f) != 0; }
    void clearFlag(flag_t f) { m_flag &= (~f); }
    void setFlag(flag_t f) { m_flag |= f; }
private:
    flag_t m_flag = 0;
    LineN m_line = 0;
    CharN m_col = 0;
};

