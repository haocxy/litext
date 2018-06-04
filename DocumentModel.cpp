#include "DocumentModel.h"

DocumentModel::DocumentModel()
{
}


DocumentModel::~DocumentModel()
{
}

void DocumentModel::ParseStr(const QString &s)
{
    QString line;
    for (QChar c : s) {
        if (c == '\n') {
            lines_.push_back(std::move(line));
        }
        else {
            line.push_back(c);
        }
    }
}

int64_t DocumentModel::GetLineCnt() const
{
    return static_cast<int64_t>(lines_.size());
}

int64_t DocumentModel::GetCharCntOfLine(int64_t lineIndex) const
{
    return static_cast<int64_t>(lines_[lineIndex].size());
}

int64_t DocumentModel::GetViewLineBegin() const
{
    return view_line_begin_;
}

int64_t DocumentModel::GetViewLineCnt() const
{
    const int64_t lineCnt = GetLineCnt();
    if (view_line_begin_ < lineCnt)
    {
        return static_cast<int64_t>(lineCnt - view_line_begin_);
    }
    else
    {
        return 0;
    }
}

int64_t DocumentModel::GetViewCharCntOfLine(int64_t viewLineIndex) const
{
    return static_cast<int64_t>(lines_[view_line_begin_ + viewLineIndex].size());
}

QChar DocumentModel::GetCharByViewPos(int64_t row, int64_t col) const
{
    return lines_[row].at(col);
}


