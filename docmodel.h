#pragma once

#include <vector>
#include <QString>

#include "doc_model_define.h"

class DocSel
{
public:
    DocSel() = default;
    DocSel(RowIndex row, ColIndex col, CharCnt cnt = 0)
        : m_row(row), m_col(col), m_cnt(cnt)
    {}

    DocSel(const DocSel &obj)
        : m_row(obj.m_row), m_col(obj.m_col), m_cnt(obj.m_cnt)
    {}

    void SetRow(RowIndex row) { m_row = row; }
    void SetCol(ColIndex col) { m_col = col; }
    void SetRowCol(RowIndex row, ColIndex col)
    {
        m_row = row;
        m_col = col;
    }

    RowIndex GetRow() const { return m_row; }
    ColIndex GetCol() const { return m_col; }

    bool operator==(const DocSel &obj) const
    {
        return m_row == obj.m_row
            && m_col == obj.m_col
            && m_cnt == obj.m_cnt
            ;
    }

    bool operator!=(const DocSel &obj) const
    {
        return !(*this == obj);
    }

private:
    RowIndex m_row = 0;
    ColIndex m_col = 0;
    CharCnt m_cnt = 0;
};

class DocModel
{
public:
    DocModel();
    ~DocModel();

    void LoadFromFile(const QString &path);

    void ParseStr(const QString &s);

    bool IsEmpty() const;

    RowCnt GetRowCnt() const;

    ColCnt GetColCntOfLine(RowIndex lineIndex) const;

    const QString &operator[](RowIndex rowIndex) const;

    void SetCursor(RowIndex row, ColIndex col);

    void SetCursor(const DocSel &cursor);

    const DocSel &GetCursor() const { return m_cursor; }

    bool CursorMovePrevChar();
    bool CursorMoveNextChar();

private:
    std::vector<QString> m_lines;
    DocSel m_cursor;
};


