#pragma once

#include <deque>
#include "common/common_define.h"
#include "util/stl_container_util.h"
#include "view/view_addr.h"

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

    typedef std::deque<Char> Chars;

    /*行，行的划分取决于显示区域的大小以及是否开启折行显示*/
    class Line
    {
    public:
        typedef int CharN;
        typedef Chars::const_iterator const_iterator;
        typedef Chars::iterator iterator;

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



    private:
        Chars m_chars;
    };

    typedef std::deque<Line> Lines;

    /*
    段落，段落以换行结束
    一个段落包含多个行
    */
    class Phase
    {
    public:
        typedef Lines::const_iterator const_iterator;
        typedef Lines::iterator iterator;

        Phase() = default;
        Phase(Phase&& phase) : m_lines(std::move(phase.m_lines)) {}
        Phase &operator=(Phase &&phase) { m_lines = std::move(phase.m_lines); return *this; }
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

    typedef std::deque<Phase> Phases;


    class Page
    {
    public:
        typedef Phases::const_iterator const_iterator;
        typedef Phases::iterator iterator;

        void clear() { m_phases.clear(); }
        int size() const { return static_cast<int>(m_phases.size()); }
        const Phase &operator[](int line) const { return m_phases[line]; }
        Phase &operator[](int line) { return m_phases[line]; }
        const_iterator begin() const { return m_phases.begin(); }
        const_iterator end() const { return m_phases.end(); }
        iterator begin() { return m_phases.begin(); }
        iterator end() { return m_phases.end(); }
        Phase &grow()
        {
            return StlContainerUtil::grow(m_phases);
        }
        LineAddr getLineAddrByLineOffset(int offset) const;
        CharAddr getCharAddrByLineAddrAndX(const LineAddr &lineAddr, int x) const;
        const Line & getLine(const view::LineAddr & addr) const
        {
            return m_phases[addr.phase()][addr.line()];
        }
        view::Line & getLine(const view::LineAddr & addr)
        {
            return m_phases[addr.phase()][addr.line()];
        }
    private:
        Phases m_phases;
    };
}
