#pragma once

#include <assert.h>
#include <vector>
#include <deque>

#include "module/model/model_define.h"
#include "util/stl_container_util.h"
#include "module/model/doc_addr.h"
#include "module/view/view_page.h"
#include "module/view/view_addr.h"

namespace view
{
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

} // namespace view


class Model;
class Line;
class DocAddr;

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

    int getLineOffsetByLineAddr(const view::LineAddr &lineAddr) const;

    view::LineAddr getLineAddrByY(int y) const;

    view::CharAddr getCharAddrByPoint(int x, int y) const;

    DocAddr getDocAddrByPoint(int x, int y) const;

    view::CharAddr convertToCharAddr(const DocAddr &docAddr) const;

    DocAddr convertToDocAddr(const view::CharAddr &charAddr) const;

    const view::Char & getChar(const view::CharAddr & charAddr) const;

    view::LineBound getLineBoundByLineOffset(int lineOffset) const;

    view::LineBound getLineBound(const view::LineAddr &lineAddr) const;

private:
    int getLineOffsetByY(int y) const;

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
