#pragma once

#include <vector>
#include <deque>

#include "module/model/model_define.h"

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

    private:
        UChar m_unicode = 0;
        int m_x = 0;
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

        LineN size() const { return static_cast<LineN>(m_lines.size()); }
        const Line &operator[](LineN line) const { return m_lines[line]; }
        Line &operator[](LineN line) { return m_lines[line]; }
        const_iterator begin() const { return m_lines.begin(); }
        const_iterator end() const { return m_lines.end(); }
        iterator begin() { return m_lines.begin(); }
        iterator end() { return m_lines.end(); }

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

    private:
        Phases m_phases;
    };

    class Size
    {
    public:
        Size() {}
        Size(int x, int y) :m_x(x), m_y(y) {}
        int x() const { return m_x; }
        int y() const { return m_y; }
        void setX(int x) { m_x = x; }
        void setY(int y) { m_y = y; }
        void set(int x, int y) { m_x = x; m_y = y; }
    private:
        int m_x = 0;
        int m_y = 0;
    };

    class Config
    {
    public:
        const float kDefaultLineHeightFactor = 1.2f; // 默认行高系数
        static const int kDefaultHGap = 2; // 默认水平字符间距
        static const int kDefaultTabSize = 4; // 默认TAB尺寸
    public:
        float lineHeightFactor() const { return m_lineHeightFactor; }
        void setLineHeightFactor(float f) { m_lineHeightFactor = f; }

        int hGap() const { return m_hGap; }
        void setHGap(int hGap) { m_hGap = hGap; }

        int tabSize() const { return m_tabSize; }
        void setTabSize(int tabSize) { m_tabSize = tabSize; }

    private:
        float m_lineHeightFactor = kDefaultLineHeightFactor; // 行高系数，行高 = 行高系数 x 字体高度，结果四舍五入
        int m_hGap = kDefaultHGap; // 水平方向字符间距
        int m_tabSize = kDefaultTabSize; // 一个TAB的宽度为若干个空格
    };

} // namespace view

/*
视图
支持看文档某一段
如，滚动条相关逻辑由这个接口支持
*/
class View
{
public:
    View(view::Config *config) :m_config(config) {}

    ~View() {}

    void Init(DocPhaseN viewStart, const view::Size &size);

    const view::Page &page() const;
private:
    void remakePage();

private:
    bool m_dirty = true;
    view::Config *const m_config;
    view::Page m_page;
    view::Size m_size;
    DocPhaseN m_viewStart = 0;
};
