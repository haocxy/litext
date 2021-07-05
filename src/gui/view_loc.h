#pragma once

#include "doc/doc_define.h"
#include "view_define.h"


namespace gui
{


class ViewLoc {
public:
    ViewLoc() {}

    explicit ViewLoc(RowN row) : row_(row), line_(0) {}

    ViewLoc(RowN row, LineN line) : row_(row), line_(line) {}

    RowN row() const { return row_; }

    void setRow(RowN row) { row_ = row; }

    LineN line() const { return line_; }

    void setLine(LineN line) { line_ = line; }

    bool operator==(const ViewLoc &b) const
    {
        return row_ == b.row_ && line_ == b.line_;
    }

    bool operator!=(const ViewLoc &b) const
    {
        return !(*this == b);
    }

private:
    RowN row_ = 0;
    LineN line_ = 0;
};


}
