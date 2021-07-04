#pragma once

#include <assert.h>
#include <vector>
#include <deque>
#include <bitset>
#include <optional>

#include "doc/doc_define.h"
#include "doc/doc_loc.h"
#include "page.h"
#include "char_loc.h"
#include "rect.h"
#include "vertical_line.h"
#include "view_define.h"

#include "core/callbacks.h"


class Row;
class DocLoc;
class Editor;



namespace gui
{

class TextAreaConfig;

class Size {
public:
    Size() {}
    Size(int width, int height) : m_width(width), m_height(height) {}
    int width() const { return m_width; }
    int height() const { return m_height; }
    void setWidth(int width) { m_width = width; }
    void setHeight(int height) { m_height = height; }
    bool operator==(const Size &b) const { return m_width == b.m_width && m_height == b.m_height; }
    bool operator!=(const Size &b) const { return !(*this == b); }
private:
    int m_width = 0;
    int m_height = 0;
};



class LineBound {
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

class RowBound {
public:
    RowBound() = default;
    RowBound(int top, int height) :m_top(top), m_height(height) {}
    int top() const { return m_top; }
    void setTop(int top) { m_top = top; }
    int height() const { return m_height; }
    void setHeight(int height) { m_height = height; }
private:
    int m_top = 0;
    int m_height = 0;
};

class ViewLoc {
public:
    ViewLoc() { m_flag.set(flgNull); }
    explicit ViewLoc(RowN row) : m_row(row), m_line(0) {}
    ViewLoc(RowN row, LineN line) : m_row(row), m_line(line) {}

    bool isNull() const { return m_flag.test(flgNull); }
    bool atEnd() const { return m_flag.test(flgAtEnd); }

    RowN row() const { return m_row; }
    void setRow(RowN row) { m_row = row; }

    LineN line() const { return m_line; }
    void setLine(LineN line) { m_line = line; }

    bool operator==(const ViewLoc &b) const
    {
        return m_row == b.m_row && m_line == b.m_line && m_flag == b.m_flag;
    }
    bool operator!=(const ViewLoc &b) const
    {
        return !(*this == b);
    }

private:
    enum Flag {
        flgNull,
        flgAtEnd,

        flgCnt,
    };

private:
    RowN m_row = 0;
    LineN m_line = 0;
    std::bitset<flgCnt> m_flag;
};


// 实现GUI相关的逻辑中和具体GUI工具包无关的部分
class TextArea {
public:
    TextArea(Editor *editor, TextAreaConfig *config);

    ~TextArea();

public:

    void initSize(const Size &size);

    void resize(const Size &size);

    void onPrimaryButtomPress(int x, int y);

    void onDirUpKeyPress();

    void onDirDownKeyPress();

    void onDirLeftKeyPress();

    void onDirRightKeyPress();

    // 向后移动一个line，移动成功则返回true，移动失败则返回false
    // 仅当视图中只显示文档最后一个line或文档没有内容时，返回false
    bool moveDownByOneLine();

public:

    const TextAreaConfig &config() const { return config_; }

    int getBaseLineByLineOffset(int off) const;

    std::optional<Rect> getLastActLineDrawRect() const;

    std::optional<VerticalLine> getNormalCursorDrawData() const;

    int getLineNumBarWidth() const;

    void drawEachLineNum(std::function<void(RowN lineNum, int baseline, const RowBound &bound, bool isLastAct)> &&action) const;

    void drawEachChar(std::function<void(int x, int y, UChar c)> &&action) const;



public:
    CallbackHandle addOnUpdateListener(std::function<void()> &&action);
    CallbackHandle addOnViewLocChangeListener(std::function<void()> &&action);

private:
    int getMaxShownLineCnt() const;
    int getLineOffsetByRowIndex(int row) const;
    int getLineOffsetByLineLoc(const LineLoc &loc) const;
    int getLineOffsetByY(int y) const;
    LineLoc getLineLocByLineOffset(int offset) const;
    CharLoc getCharLocByLineLocAndX(const LineLoc &lineLoc, int x) const;
    CharLoc getCharLocByPoint(int x, int y) const;
    DocLoc getDocLocByPoint(int x, int y) const;
    RowLoc convertToRowLoc(RowN row) const;
    CharLoc convertToCharLoc(const DocLoc &docLoc) const;
    DocLoc convertToDocLoc(const CharLoc &charLoc) const;
    const Char &getChar(const CharLoc &charLoc) const;
    int getXByCharLoc(const CharLoc &charLoc) const;
    LineBound getLineBoundByLineOffset(int lineOffset) const;
    LineBound getLineBound(const LineLoc &lineLoc) const;
    RowBound getRowBound(const RowLoc &rowLoc) const;
    bool hasPrevCharAtSameLine(const CharLoc &charLoc) const;
    bool noPrevCharAtSameLine(const CharLoc &charLoc) const;
    bool hasNextCharAtSameLine(const CharLoc &charLoc) const;
    bool noNextCharAtSameLine(const CharLoc &charLoc) const;
    bool needEnsureHasNextLine(const CharLoc &charLoc) const;
    bool isLastLineOfRow(const LineLoc &lineLoc) const;
    bool isEndOfVirtualLine(const CharLoc &charLoc) const;
    CharLoc betterLocForVerticalMove(const CharLoc &charLoc) const;
    DocLoc getNextUpLoc(const DocLoc &docLoc) const;
    DocLoc getNextDownLoc(const DocLoc &docLoc) const;
    void makeVRow(const Row &row, VRow &vrow) const;
    void makeVRowWithWrapLine(const Row &row, VRow &vrow) const;
    void makeVRowNoWrapLine(const Row &row, VRow &vrow) const;

    // 视图中最后一个可视line的LineLoc
    LineLoc getShownLastLineLoc() const;

private:
    // 调用这个函数后，需要在合适的时刻调用removeSpareRow
    void ensureHasPrevLine(const LineLoc &curLineLoc);

    // 确保参数行有下一行
    // 如果需要把页面头部向后移动则返回true
    // 返回true则需要在合适的时刻调用movePageHeadOneLine
    bool ensureHasNextLine(const LineLoc &curLineLoc);

    void setViewLoc(const ViewLoc &viewLoc);

private:
    void remakePage();

    // 把页面开头向后移动一个line
    void movePageHeadOneLine();
    // 删除多余的row
    void removeSpareRow();

private:
    Editor &editor_;
    const TextAreaConfig &config_;
    Page m_page;
    Size m_size;
    ViewLoc m_loc{ 0, 0 };

    // 对于非等宽字体，当光标多次上下移动时，希望横坐标相对稳定，记录一个稳定位置，每次上下移动时尽可能选取与之接近的位置
    // 在某些操作后更新，如左右移动光标等操作
    int m_stable_x = 0;

private:
    Callbacks<void()> m_onUpdateListeners;
    Callbacks<void()> m_onViewLocChangeListeners;

private:
    CallbackHandle m_listenerIdForLastActLineUpdate;
};

}
