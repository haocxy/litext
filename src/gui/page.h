#pragma once

#include <vector>
#include <deque>

#include "core/uchar.h"
#include "line_loc.h"


namespace gui
{


class VChar {
public:
    UChar uchar() const { return m_unicode; }
    int x() const { return m_x; }

    void setUChar(UChar unicode) { m_unicode = unicode; }
    void setX(int x) { m_x = x; }

    int width() const { return m_width; }
    void setWidth(int width) { m_width = width; }

private:
    UChar m_unicode = 0;
    int m_x = 0;
    int m_width = 0;
};

typedef std::vector<VChar> VChars;


class VLine {
public:

    bool empty() const { return m_chars.empty(); }
    CharN size() const { return static_cast<CharN>(m_chars.size()); }
    const VChar &operator[](CharN line) const { return m_chars[line]; }
    VChar &operator[](CharN line) { return m_chars[line]; }
    auto begin() const { return m_chars.begin(); }
    auto end() const { return m_chars.end(); }
    auto begin() { return m_chars.begin(); }
    auto end() { return m_chars.end(); }

    VChar &grow()
    {
        m_chars.resize(m_chars.size() + 1);
        return m_chars.back();
    }

    const VChar &last() const { return m_chars.back(); }
    VChar &last() { return m_chars.back(); }

private:
    VChars m_chars;
};

typedef std::vector<VLine> Lines;


class VRow {
public:
    VRow() = default;
    VRow(VRow &&row) : m_lines(std::move(row.m_lines)) {}
    VRow &operator=(VRow &&row) { m_lines = std::move(row.m_lines); return *this; }
    int size() const { return static_cast<int>(m_lines.size()); }
    const VLine &operator[](int line) const { return m_lines[line]; }
    VLine &operator[](int line) { return m_lines[line]; }
    auto begin() const { return m_lines.begin(); }
    auto end() const { return m_lines.end(); }
    auto begin() { return m_lines.begin(); }
    auto end() { return m_lines.end(); }
    VLine &grow()
    {
        m_lines.resize(m_lines.size() + 1);
        return m_lines.back();
    }
private:
    Lines m_lines;
};

typedef std::deque<VRow> VRows;


class Page {
public:
    void clear()
    {
        m_rows.clear();
        m_lineCnt = 0;
    }
    int size() const { return static_cast<int>(m_rows.size()); }
    int lineCnt() const { return m_lineCnt; }
    const VRow &operator[](int line) const { return m_rows[line]; }
    auto begin() const { return m_rows.begin(); }
    auto end() const { return m_rows.end(); }
    VLineLoc getNextUpLineLoc(const VLineLoc &lineLoc) const;
    VLineLoc getNextDownLineLoc(const VLineLoc &lineLoc) const;
    const VLine &getLine(const VLineLoc &lineLoc) const { return m_rows[lineLoc.row()][lineLoc.line()]; }
    void pushFront(VRow &&row)
    {
        m_lineCnt += row.size();
        m_rows.push_front(std::move(row));
    }
    void popFront()
    {
        if (!m_rows.empty()) {
            m_lineCnt -= m_rows.front().size();
            m_rows.pop_front();
        }
    }
    void pushBack(VRow &&row)
    {
        m_lineCnt += row.size();
        m_rows.push_back(std::move(row));
    }
    void popBack()
    {
        if (!m_rows.empty()) {
            m_lineCnt -= m_rows.back().size();
            m_rows.pop_back();
        }
    }

private:
    VRows m_rows;
    int m_lineCnt = 0;
};


}
