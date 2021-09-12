#pragma once

#include "doc_define.h"
#include "core/flagset.h"


class DocRowLoc {
public:
    DocRowLoc() {
        m_flag.set(kIsNull);
    }

    DocRowLoc(RowN row)
        : m_row(row) {}

    static DocRowLoc newRowLocAfterLastRow() {
        DocRowLoc loc(0);
        loc.m_flag.set(kAfterLastRow);
        return loc;
    }

protected:
    enum {
        kIsNull = 0,
        kAfterLastRow = 1,
        kAfterLastChar = 2,
        kFlagCnt,
    };

    DocRowLoc(FlagSet<kFlagCnt> flag)
        : m_flag(flag) {}

protected:
    FlagSet<kFlagCnt> m_flag;
    RowN m_row = 0;
};


class DocLoc : public DocRowLoc
{
public:
    virtual ~DocLoc() {}

    static DocLoc newDocLocAfterLastRow()
    {
        DocLoc loc(0, 0);
        loc.m_flag.set(kAfterLastRow);
        loc.m_flag.set(kAfterLastChar);
        return loc;
    }
    static DocLoc newDocLocAfterLastChar(RowN row)
    {
        DocLoc loc(row, 0);
        loc.m_flag.set(kAfterLastChar);
        return loc;
    }

public:
    DocLoc() {}

    DocLoc(RowN row, CharN col) : DocRowLoc(row), m_col(col) {}
    RowN row() const { return m_row; }
    void setRow(RowN row) { m_row = row; }
    CharN col() const { return m_col; }
    void setCol(CharN col) { m_col = col; }
    bool isNull() const { return m_flag.has(kIsNull); }
    bool isAfterLastRow() const { return m_flag.has(kAfterLastRow); }
    bool isAfterLastChar() const { return m_flag.has(kAfterLastChar); }
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
    CharN m_col = 0;
};

