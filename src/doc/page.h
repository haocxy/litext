#pragma once

#include <vector>
#include <deque>

#include "view_char.h"
#include "line_loc.h"


namespace doc
{


class VLine {
public:

    bool empty() const { return m_chars.empty(); }
    i64 size() const { return static_cast<i64>(m_chars.size()); }
    const VChar &operator[](i64 line) const { return m_chars[line]; }
    VChar &operator[](i64 line) { return m_chars[line]; }
    auto begin() const { return m_chars.begin(); }
    auto end() const { return m_chars.end(); }
    auto begin() { return m_chars.begin(); }
    auto end() { return m_chars.end(); }

    VChar &grow()
    {
        m_chars.resize(m_chars.size() + 1);
        return m_chars.back();
    }

    void append(const VChar &ch) {
        m_chars.push_back(ch);
    }

    const VChar &last() const { return m_chars.back(); }
    VChar &last() { return m_chars.back(); }

private:
    std::vector<VChar> m_chars;
};


class VRow {
public:
    VRow() = default;
    VRow(VRow &&row) : m_lines(std::move(row.m_lines)) {}
    VRow &operator=(VRow &&row) { m_lines = std::move(row.m_lines); return *this; }
    int size() const { return static_cast<int>(m_lines.size()); }
    const VLine &operator[](i64 line) const { return m_lines[line]; }
    VLine &operator[](i64 line) { return m_lines[line]; }
    auto begin() const { return m_lines.begin(); }
    auto end() const { return m_lines.end(); }
    auto begin() { return m_lines.begin(); }
    auto end() { return m_lines.end(); }
    VLine &grow()
    {
        m_lines.resize(m_lines.size() + 1);
        return m_lines.back();
    }
    VLine &back() {
        return m_lines.back();
    }
private:
    std::vector<VLine> m_lines;
};


class Page {
public:
    void clear()
    {
        m_rows.clear();
        m_lineCnt = 0;
    }
    i64 size() const { return static_cast<int>(m_rows.size()); }
    i64 lineCnt() const { return m_lineCnt; }
    const VRow &operator[](i64 line) const { return m_rows[line]; }
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
    std::deque<VRow> m_rows;
    i64 m_lineCnt = 0;
};


}
