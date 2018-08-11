#pragma once

#include <deque>
#include "common/common_define.h"
#include "util/stl_container_util.h"
#include "view/view_loc.h"

namespace view
{
    /*字符*/
    class Char
    {
    public:
        UChar unicode() const { return m_unicode; }
        int x() const { return m_x; }

        void setUnicode(UChar unicode) { m_unicode = unicode; }
        void setX(int x) { m_x = x; }

        int width() const { return m_width; }
        void setWidth(int width) { m_width = width; }

    private:
        UChar m_unicode = 0;
        int m_x = 0;
        int m_width = 0;
    };

    typedef std::vector<Char> Chars;

    /*行，行的划分取决于显示区域的大小以及是否开启折行显示*/
    class Line
    {
    public:
        typedef int CharN;
        typedef Chars::const_iterator const_iterator;
        typedef Chars::iterator iterator;

        bool empty() const { return m_chars.empty(); }
        CharN size() const { return static_cast<CharN>(m_chars.size()); }
        const Char &operator[](CharN line) const { return m_chars[line]; }
        Char &operator[](CharN line) { return m_chars[line]; }
        const_iterator begin() const { return m_chars.begin(); }
        const_iterator end() const { return m_chars.end(); }
        iterator begin() { return m_chars.begin(); }
        iterator end() { return m_chars.end(); }

        Char &grow()
        {
            return StlContainerUtil::grow(m_chars);
        }

        const Char &last() const { return m_chars.back(); }
        Char &last() { return m_chars.back(); }

    private:
        Chars m_chars;
    };

    typedef std::vector<Line> Lines;

    /*
    段落，段落以换行结束
    一个段落包含多个行
    */
    class VRow
    {
    public:
        typedef Lines::const_iterator const_iterator;
        typedef Lines::iterator iterator;

        VRow() = default;
        VRow(VRow&& row) : m_lines(std::move(row.m_lines)) {}
        VRow &operator=(VRow && row) { m_lines = std::move(row.m_lines); return *this; }
        int size() const { return static_cast<int>(m_lines.size()); }
        const Line &operator[](int line) const { return m_lines[line]; }
        Line &operator[](int line) { return m_lines[line]; }
        const_iterator begin() const { return m_lines.begin(); }
        const_iterator end() const { return m_lines.end(); }
        iterator begin() { return m_lines.begin(); }
        iterator end() { return m_lines.end(); }
        Line &grow()
        {
            return StlContainerUtil::grow(m_lines);
        }
    private:
        Lines m_lines;
    };

    typedef std::deque<VRow> VRows;


    class Page
    {
    public:
        typedef VRows::const_iterator const_iterator;
        typedef VRows::iterator iterator;

        void clear() { m_rows.clear(); }
        int size() const { return static_cast<int>(m_rows.size()); }
        const VRow &operator[](int line) const { return m_rows[line]; }
        VRow &operator[](int line) { return m_rows[line]; }
        const_iterator begin() const { return m_rows.begin(); }
        const_iterator end() const { return m_rows.end(); }
        iterator begin() { return m_rows.begin(); }
        iterator end() { return m_rows.end(); }
        void pushFront(VRow && row)
        {
            m_rows.push_front(std::move(row));
        }
        void popFront()
        {
            m_rows.pop_front();
        }
        void pushBack(VRow && row)
        {
            m_rows.push_back(std::move(row));
        }
        void popBack()
        {
            m_rows.pop_back();
        }
        LineLoc getNextUpLineLoc(const LineLoc & lineLoc) const;
        LineLoc getNextDownLineLoc(const LineLoc & lineLoc) const;
        const Line & getLine(const view::LineLoc & lineLoc) const
        {
            return m_rows[lineLoc.row()][lineLoc.line()];
        }
        view::Line & getLine(const view::LineLoc & lineLoc)
        {
            return m_rows[lineLoc.row()][lineLoc.line()];
        }
        int lineCnt() const
        {
            int sum = 0;
            for (const VRow & row : m_rows)
            {
                sum += row.size();
            }
            return sum;
        }
    private:
        VRows m_rows;
    };
}
