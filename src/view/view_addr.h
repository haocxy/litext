#pragma once

#include "doc/doc_define.h"

namespace view
{
    class RowAddr
    {
    public:
        static RowAddr newRowAddrAfterLastRow()
        {
            RowAddr rowAddr(0);
            rowAddr.setFlag(kAfterLastRow);
            rowAddr.setFlag(kAfterLastLine);
            rowAddr.setFlag(kAfterLastChar);
            return rowAddr;
        }
    public:
        RowAddr() :m_flag(kIsNull), m_row(0) {}
        RowAddr(int row) : m_row(row) {}
        int row() const { return m_row; }
        void setRow(int row) { m_row = row; }
        bool isNull() const { return hasFlag(kIsNull); }
        bool isAfterLastRow() const { return hasFlag(kAfterLastRow); }
    protected:
        typedef uint_least8_t flag_t;
        enum : flag_t
        {
            kIsNull = 1,
            kAfterLastRow = 1 << 1,
            kAfterLastLine = 1 << 2,
            kAfterLastChar = 1 << 3,
        };
        bool hasFlag(flag_t f) const { return (m_flag & f) != 0; }
        void clearFlag(flag_t f) { m_flag &= (~f); }
        void setFlag(flag_t f) { m_flag |= f; }
    private:
        flag_t m_flag = 0;
        int m_row = 0;
    };

    class LineAddr : public RowAddr
    {
    public:
        static LineAddr newLineAddrAfterLastRow()
        {
            LineAddr lineAddr(0, 0);
            lineAddr.setFlag(kAfterLastRow);
            lineAddr.setFlag(kAfterLastLine);
            lineAddr.setFlag(kAfterLastChar);
            return lineAddr;
        }
    public:
        LineAddr() = default;
        LineAddr(int row, int line) :RowAddr(row), m_line(line) {}
        int line() const { return m_line; }
        void setLine(int line) { m_line = line; }
    private:
        int m_line = 0;
    };

    class CharAddr : public LineAddr
    {
    public:
        static CharAddr newCharAddrAfterLastRow()
        {
            CharAddr addr(0, 0, 0);
            addr.setFlag(kAfterLastRow);
            addr.setFlag(kAfterLastLine);
            addr.setFlag(kAfterLastChar);
            return addr;
        }
        static CharAddr newCharAddrAfterLastChar(const LineAddr & lineAddr)
        {
            CharAddr charAddr(lineAddr, 0);
            charAddr.setFlag(kAfterLastChar);
            return charAddr;
        }
    public:
        CharAddr() = default;
        CharAddr(const LineAddr & lineAddr, CharN col) : LineAddr(lineAddr), m_col(col) {}
        CharAddr(int row, int line, CharN col) :LineAddr(row, line), m_col(col) {}
        CharN col() const { return m_col; }
        void setCol(CharN col) { m_col = col; }
        bool isAfterLastChar() const { return hasFlag(kAfterLastChar); }
    private:
        CharN m_col = 0;
    };
}
