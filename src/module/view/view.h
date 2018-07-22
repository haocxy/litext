#pragma once

#include <assert.h>
#include <vector>
#include <deque>

#include "module/model/model_define.h"
#include "util/stl_container_util.h"

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
        typedef int LineN;
        typedef Lines::const_iterator const_iterator;
        typedef Lines::iterator iterator;

        Phase() = default;
        Phase(Phase&& phase) : m_lines(std::move(phase.m_lines)) {}
        Phase &operator=(Phase &&phase) { m_lines = std::move(phase.m_lines); return *this; }
        LineN size() const { return static_cast<LineN>(m_lines.size()); }
        const Line &operator[](LineN line) const { return m_lines[line]; }
        Line &operator[](LineN line) { return m_lines[line]; }
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
        typedef int PhaseN;
        typedef Phases::const_iterator const_iterator;
        typedef Phases::iterator iterator;

        void clear() { m_phases.clear(); }
        PhaseN size() const { return static_cast<PhaseN>(m_phases.size()); }
        const Phase &operator[](PhaseN line) const { return m_phases[line]; }
        Phase &operator[](PhaseN line) { return m_phases[line]; }
        const_iterator begin() const { return m_phases.begin(); }
        const_iterator end() const { return m_phases.end(); }
        iterator begin() { return m_phases.begin(); }
        iterator end() { return m_phases.end(); }
        Phase &grow()
        {
            return StlContainerUtil::grow(m_phases);
        }
    private:
        Phases m_phases;
    };

    class Size
    {
    public:
        Size() {}
        Size(int width, int height) : m_width(width), m_height(height) {}
        int width() const { return m_width; }
        int height() const { return m_height; }
        void setWidth(int width) { m_width = width; }
        void setHeight(int height) { m_height = height; }
    private:
        int m_width = 0;
        int m_height = 0;
    };

    class Config;

} // namespace view


class Model;
class Line;

/*
视图
支持看文档某一段
如，滚动条相关逻辑由这个接口支持
*/
class View
{
public:
    View(Model *model, view::Config *config) :m_model(model),m_config(config) {
        assert(model != nullptr);
        assert(config != nullptr);
    }

    ~View() {}

    void Init(LineN viewStart, const view::Size &size);

    const view::Page &page() const;

    const view::Config &config() const
    {
        return *m_config;
    }

    int getBaseLineByLineOffset(int off) const;

private:
    void remakePage();

    void DocLineToViewPhase(const Line &line, view::Phase &phase);
    void DocLineToViewPhaseWithWrapLine(const Line &line, view::Phase &phase);
    void DocLineToViewPhaseNoWrapLine(const Line &line, view::Phase &phase);

private:
    bool m_dirty = true;
    Model * const m_model;
    view::Config * const m_config;
    view::Page m_page;
    view::Size m_size;
    LineN m_viewStart = 0;
};
