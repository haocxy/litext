#include "docmodel.h"

#include <QFile>
#include <QTextStream>

DocModel::DocModel()
{
}


DocModel::~DocModel()
{
}

void DocModel::LoadFromFile(const QString & path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        return;
    }

    QTextStream in(&file);
    ParseStr(in.readAll());
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
        // 如果最后一行没有换行，则插入一个0字符作为哨兵字符表示文档结束
        line.push_back(QChar(0));
        m_lines.push_back(std::move(line));
    }
}

bool DocModel::IsEmpty() const
{
    return m_lines.empty();
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
    m_normalCursor.SetRow(row);
    m_normalCursor.SetCol(col);
}

void DocModel::SetCursor(const DocSel & cursor)
{
    m_normalCursor = cursor;
}

bool DocModel::CursorMovePrevChar()
{
    const RowIndex row = m_normalCursor.GetRow();
    const ColIndex col = m_normalCursor.GetCol();

    if (col > 0)
    {
        m_normalCursor.SetCol(col - 1);
        return true;
    }

    if (row > 0)
    {
        const RowIndex newRow = row - 1;
        const QString &line = m_lines[newRow];
        if (!line.isEmpty())
        {
            m_normalCursor.SetRowCol(newRow, line.size() - 1);
        }
        else
        {
            m_normalCursor.SetRowCol(newRow, 0);
        }
        return true;
    }

    return false;
}

bool DocModel::CursorMoveNextChar()
{
    const RowIndex row = m_normalCursor.GetRow();
    const ColIndex col = m_normalCursor.GetCol();

    const RowCnt rowCnt = static_cast<RowCnt>(m_lines.size());
    if (row >= rowCnt)
    {
        return false;
    }

    const QString &curLine = m_lines[row];

    if (col < curLine.size() - 1)
    {
        m_normalCursor.SetCol(col + 1);
        return true;
    }

    if (row < rowCnt - 1)
    {
        m_normalCursor.SetRowCol(row + 1, 0);
        return true;
    }

    return false;
}





