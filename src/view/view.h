#pragma once

#include <assert.h>
#include <vector>
#include <deque>

#include "doc/doc_define.h"
#include "util/stl_container_util.h"
#include "doc/doc_addr.h"
#include "view/view_page.h"
#include "view/view_addr.h"
#include "view/view_rect.h"
#include "view/view_line.h"

namespace view
{
    class Point
    {
    public:
        Point() = default;
        Point(int x, int y) :m_x(x), m_y(y) {}
        int x() const { return m_x; }
        void setX(int x) { m_x = x; }
        int y() const { return m_y; }
        void setY(int y) { m_y = y; }
    private:
        int m_x = 0;
        int m_y = 0;
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

    class LineBound
    {
    public:
        LineBound() = default;
        LineBound(int top, int bottom) :m_top(top), m_bottom(bottom) {}
        int top() const { return m_top; }
        void setTop(int top) { m_top = top; }
        int bottom() const { return m_bottom; }
        void setBottom(int bottom) { m_bottom = bottom; }
    private:
        int m_top = 0;
        int m_bottom = 0;
    };

    class PhaseBound
    {
    public:
        PhaseBound() = default;
        PhaseBound(int top, int height) :m_top(top), m_height(height) {}
        int top() const { return m_top; }
        void setTop(int top) { m_top = top; }
        int height() const { return m_height; }
        void setHeight(int height) { m_height = height; }
    private:
        int m_top = 0;
        int m_height = 0;
    };

} // namespace view


class DocLine;
class DocAddr;
class Editor;

/*
视图
支持看文档某一段
如，滚动条相关逻辑由这个接口支持
*/
class View
{
public:
    View(Editor * editor, view::Config *config) :m_editor(*editor),m_config(*config) {
        assert(editor);
        assert(config);
    }

    ~View() {}

public:

    void Init(LineN viewStart, const view::Size &size);

    void onPrimaryButtomPress(int x, int y);

public:

    const view::Page &page() const;

    const view::Config &config() const { return m_config; }

    int getBaseLineByLineOffset(int off) const;

    view::Rect getLastActLineDrawRect() const;

    draw::VerticalLine getNormalCursorDrawData() const;


private:
    int getLineOffsetByPhaseIndex(int phase) const;
    int getLineOffsetByLineAddr(const view::LineAddr &lineAddr) const;
    int getLineOffsetByY(int y) const;
    view::LineAddr getLineAddrByY(int y) const;
    view::CharAddr getCharAddrByPoint(int x, int y) const;
    DocAddr getDocAddrByPoint(int x, int y) const;
    view::PhaseAddr convertToPhaseAddr(LineN line) const;

    view::CharAddr convertToCharAddr(const DocAddr &docAddr) const;

    DocAddr convertToDocAddr(const view::CharAddr &charAddr) const;

    const view::Char & getChar(const view::CharAddr & charAddr) const;

    int getXByAddr(const view::CharAddr & charAddr) const;

    view::LineBound getLineBoundByLineOffset(int lineOffset) const;

    view::LineBound getLineBound(const view::LineAddr &lineAddr) const;

    view::PhaseBound getPhaseBound(const view::PhaseAddr &phaseAddr) const;

private:
    void remakePage();

    void DocLineToViewPhase(const DocLine &line, view::Phase &phase);
    void DocLineToViewPhaseWithWrapLine(const DocLine &line, view::Phase &phase);
    void DocLineToViewPhaseNoWrapLine(const DocLine &line, view::Phase &phase);

private:
    bool m_dirty = true;
    Editor & m_editor;
    const view::Config & m_config;
    view::Page m_page;
    view::Size m_size;
    LineN m_viewStart = 0;
};
