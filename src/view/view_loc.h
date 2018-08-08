#pragma once

#include "doc/doc_define.h"
#include "view/view_define.h"

namespace view
{
    class RowLoc
    {
    public:
        static RowLoc newRowLocAfterLastRow()
        {
            RowLoc loc(0);
            loc.setFlag(kAfterLastRow);
            loc.setFlag(kAfterLastLine);
            loc.setFlag(kAfterLastChar);
            return loc;
        }
    public:
        RowLoc() :m_flag(kIsNull), m_row(0) {}
        explicit RowLoc(int row) : m_row(row) {}
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

    class LineLoc : public RowLoc
    {
    public:
        static LineLoc newLineLocAfterLastRow()
        {
            LineLoc loc(0, 0);
            loc.setFlag(kAfterLastRow);
            loc.setFlag(kAfterLastLine);
            loc.setFlag(kAfterLastChar);
            return loc;
        }
    public:
        LineLoc() = default;
        LineLoc(int row, int line) :RowLoc(row), m_line(line) {}
        LineN line() const { return m_line; }
        void setLine(LineN line) { m_line = line; }
    private:
        LineN m_line = 0;
    };

    class CharLoc : public LineLoc
    {
    public:
        static CharLoc newCharLocAfterLastRow()
        {
            CharLoc loc(0, 0, 0);
            loc.setFlag(kAfterLastRow);
            loc.setFlag(kAfterLastLine);
            loc.setFlag(kAfterLastChar);
            return loc;
        }
        static CharLoc newCharLocAfterLastChar(const LineLoc & lineLoc)
        {
            CharLoc loc(lineLoc, 0);
            loc.setFlag(kAfterLastChar);
            return loc;
        }
    public:
        CharLoc() = default;
        CharLoc(const LineLoc & lineLoc, CharN col) : LineLoc(lineLoc), m_col(col) {}
        CharLoc(int row, int line, CharN col) :LineLoc(row, line), m_col(col) {}
        CharN col() const { return m_col; }
        void setCol(CharN col) { m_col = col; }
        bool isAfterLastChar() const { return hasFlag(kAfterLastChar); }
    private:
        CharN m_col = 0;
    };
}
