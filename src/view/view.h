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

#include "util/listeners.h"
#include "common/dir_enum.h"

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
        bool operator==(const Size & b) const { return m_width == b.m_width && m_height == b.m_height; }
        bool operator!=(const Size & b) const { return !(*this == b); }
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


class Line;
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
    View(Editor * editor, view::Config *config);

    ~View();

public:

    void initSize(const view::Size &size);

    void onResize(const view::Size & size);

    void onPrimaryButtomPress(int x, int y);

    void onDirKeyPress(Dir dir);

public:

    const view::Page &page() const;

    const view::Config &config() const { return m_config; }

    int getBaseLineByLineOffset(int off) const;

    view::Rect getLastActLineDrawRect() const;

    draw::VerticalLine getNormalCursorDrawData() const;

    int getLineNumBarWidth() const;

    void drawEachLineNum(std::function<void(LineN lineNum, int baseline, const view::PhaseBound & bound, bool isLastAct)> && action) const;

public:
    ListenerID addOnUpdateListener(std::function<void()> && action);
    void removeOnUpdateListener(ListenerID id);

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
    void onDirUpKeyPress();
    void onDirDownKeyPress();
    void onDirLeftKeyPress();
    void onDirRightKeyPress();

private:
    void remakePage();

    void DocLineToViewPhase(const Line &line, view::Phase &phase);
    void DocLineToViewPhaseWithWrapLine(const Line &line, view::Phase &phase);
    void DocLineToViewPhaseNoWrapLine(const Line &line, view::Phase &phase);

private:
    Editor & m_editor;
    const view::Config & m_config;
    view::Page m_page;
    view::Size m_size;
    LineN m_viewStart = 0;
    
    // 对于非等宽字体，当光标多次上下移动时，希望横坐标相对稳定，记录一个稳定位置，每次上下移动时尽可能选取与之接近的位置
    // 在某些操作后更新，如左右移动光标等操作
    int m_stable_x = 0;

private:
    Listeners<void()> m_onUpdateListeners;

private:
    ListenerID m_listenerIdForLastActLineUpdate = 0;
};
