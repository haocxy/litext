#include "docmodel.h"

DocModel::DocModel()
{
}


DocModel::~DocModel()
{
}

void DocModel::ParseStr(const QString &s)
{
    QString line;
    for (QChar c : s) {
        line.push_back(c);
        if (c == '\n') {
            m_lines.push_back(std::move(line));
        }
    }
    if (!line.isEmpty())
    {
        // 如果最后一行没有换行，则插入一个0字符作为哨兵字符
        line.push_back(QChar(0));
        m_lines.push_back(std::move(line));
    }
}

RowCnt DocModel::GetRowCnt() const
{
    return static_cast<RowCnt>(m_lines.size());
}

ColCnt DocModel::GetColCntOfLine(RowIndex lineIndex) const
{
    return static_cast<int64_t>(m_lines[lineIndex].size());
}

const QString &DocModel::operator[](RowIndex rowIndex) const
{
    return m_lines[rowIndex];
}

void DocModel::SetCursor(RowIndex row, ColIndex col)
{
    m_cursor.SetRow(row);
    m_cursor.SetCol(col);
}

void DocModel::SetCursor(const DocSel & cursor)
{
    m_cursor = cursor;
}





