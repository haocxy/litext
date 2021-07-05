#pragma once

#include <bitset>

#include "doc/doc_define.h"
#include "view_define.h"


namespace gui
{


class ViewLoc {
public:
    ViewLoc() { m_flag.set(flgNull); }
    explicit ViewLoc(RowN row) : m_row(row), m_line(0) {}
    ViewLoc(RowN row, LineN line) : m_row(row), m_line(line) {}

    bool isNull() const { return m_flag.test(flgNull); }
    bool atEnd() const { return m_flag.test(flgAtEnd); }

    RowN row() const { return m_row; }
    void setRow(RowN row) { m_row = row; }

    LineN line() const { return m_line; }
    void setLine(LineN line) { m_line = line; }

    bool operator==(const ViewLoc &b) const
    {
        return m_row == b.m_row && m_line == b.m_line && m_flag == b.m_flag;
    }
    bool operator!=(const ViewLoc &b) const
    {
        return !(*this == b);
    }

private:
    enum Flag {
        flgNull,
        flgAtEnd,

        flgCnt,
    };

private:
    RowN m_row = 0;
    LineN m_line = 0;
    std::bitset<flgCnt> m_flag;
};


}
