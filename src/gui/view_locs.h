#pragma once

#include "core/flagset.h"
#include "core/basetype.h"
#include "doc/doc_define.h"


namespace gui
{

class ViewLoc {
public:
    ViewLoc() {}

    explicit ViewLoc(RowN row)
        : rowRange_(RowRange::byOffAndLen(row, 0))
        , line_(0) {}

    ViewLoc(const RowRange &rowRange)
        : rowRange_(rowRange)
        , line_(0) {}

    ViewLoc(const RowRange &rowRange, LineN lineOff)
        : rowRange_(rowRange)
        , line_(lineOff) {}

    const RowRange &rowRange() const {
        return rowRange_;
    }

    RowRange &rowRange() {
        return rowRange_;
    }

    LineN line() const {
        return line_;
    }

    void setLine(LineN line) {
        line_ = line;
    }

    bool operator==(const ViewLoc &b) const {
        return rowRange_ == b.rowRange_ && line_ == b.line_;
    }

    bool operator!=(const ViewLoc &b) const {
        return !(*this == b);
    }

private:
    RowRange rowRange_;
    LineN line_ = 0;
};

class VRowLoc {
public:
    static VRowLoc newRowLocAfterLastRow()
    {
        VRowLoc loc(0);
        loc.setFlag(kAfterLastRow);
        loc.setFlag(kAfterLastLine);
        loc.setFlag(kAfterLastChar);
        return loc;
    }
public:
    VRowLoc() :m_flag(kIsNull), row_(0) {}
    explicit VRowLoc(i64 row) : row_(row) {}
    i64 row() const { return row_; }
    void setRow(i64 row) { row_ = row; }
    bool isNull() const { return hasFlag(kIsNull); }
    bool isAfterLastRow() const { return hasFlag(kAfterLastRow); }
protected:
    enum {
        kIsNull,
        kAfterLastRow,
        kAfterLastLine,
        kAfterLastChar,
        kFlagCnt,
    };
    bool hasFlag(int f) const { return m_flag.has(f); }
    void clearFlag(int f) { m_flag.unset(f); }
    void setFlag(int f) { m_flag.set(f); }
private:
    FlagSet<kFlagCnt> m_flag;
    i64 row_ = 0;
};

class VLineLoc : public VRowLoc {
public:
    static VLineLoc newLineLocAfterLastRow()
    {
        VLineLoc loc(0, 0);
        loc.setFlag(kAfterLastRow);
        loc.setFlag(kAfterLastLine);
        loc.setFlag(kAfterLastChar);
        return loc;
    }
public:
    VLineLoc() = default;
    VLineLoc(i64 row, i64 line) : VRowLoc(row), m_line(line) {}
    i64 line() const { return m_line; }
    void setLine(i64 line) { m_line = line; }
private:
    i64 m_line = 0;
};

class VCharLoc : public VLineLoc {
public:
    static VCharLoc newCharLocAfterLastRow()
    {
        VCharLoc loc(0, 0, 0);
        loc.setFlag(kAfterLastRow);
        loc.setFlag(kAfterLastLine);
        loc.setFlag(kAfterLastChar);
        return loc;
    }
    static VCharLoc newCharLocAfterLastChar(const VLineLoc &lineLoc)
    {
        VCharLoc loc(lineLoc, 0);
        loc.setFlag(kAfterLastChar);
        return loc;
    }
public:
    VCharLoc() = default;
    VCharLoc(const VLineLoc &lineLoc, CharN col) : VLineLoc(lineLoc), m_col(col) {}
    VCharLoc(int row, int line, CharN col) : VLineLoc(row, line), m_col(col) {}
    CharN col() const { return m_col; }
    void setCol(CharN col) { m_col = col; }
    bool isAfterLastChar() const { return hasFlag(kAfterLastChar); }
private:
    CharN m_col = 0;
};



}
