#pragma once

#include <deque>
#include "common/common_define.h"
#include "util/stl_container_util.h"
#include "view/view_addr.h"

namespace view
{
    /*�ַ�*/
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

    /*�У��еĻ���ȡ������ʾ����Ĵ�С�Լ��Ƿ���������ʾ*/
    class DocLine
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

        const Char &last() const { return m_chars.back(); }
        Char &last() { return m_chars.back(); }

    private:
        Chars m_chars;
    };

    typedef std::deque<DocLine> Lines;

    /*
    ���䣬�����Ի��н���
    һ��������������
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
        const DocLine &operator[](int line) const { return m_lines[line]; }
        DocLine &operator[](int line) { return m_lines[line]; }
        const_iterator begin() const { return m_lines.begin(); }
        const_iterator end() const { return m_lines.end(); }
        iterator begin() { return m_lines.begin(); }
        iterator end() { return m_lines.end(); }
        DocLine &grow()
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
        const DocLine & getLine(const view::LineAddr & addr) const
        {
            return m_phases[addr.phase()][addr.line()];
        }
        view::DocLine & getLine(const view::LineAddr & addr)
        {
            return m_phases[addr.phase()][addr.line()];
        }
        int lineCnt() const
        {
            int sum = 0;
            for (const Phase &phase : m_phases)
            {
                sum += phase.size();
            }
            return sum;
        }
    private:
        Phases m_phases;
    };
}
