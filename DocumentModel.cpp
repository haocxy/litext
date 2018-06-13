#include "DocumentModel.h"

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
            lines_.push_back(std::move(line));
        }
    }
}

RowCnt DocModel::GetRowCnt() const
{
    return static_cast<RowCnt>(lines_.size());
}

ColCnt DocModel::GetColCntOfLine(RowIndex lineIndex) const
{
    return static_cast<int64_t>(lines_[lineIndex].size());
}

const QString &DocModel::operator[](RowIndex rowIndex) const
{
    return lines_[rowIndex];
}

void DocModel::SetCursor(RowIndex row, ColIndex col)
{
    cursor_.SetRow(row);
    cursor_.SetCol(col);
}

void DocModel::SetCursor(const DocSel & cursor)
{
    cursor_ = cursor;
}





