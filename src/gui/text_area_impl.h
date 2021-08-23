#pragma once

#include <assert.h>
#include <vector>
#include <deque>
#include <bitset>
#include <optional>

#include "core/signal.h"
#include "core/sigconns.h"
#include "doc/doc_define.h"
#include "doc/doc_loc.h"

#include "page.h"
#include "view_locs.h"
#include "rect.h"
#include "vertical_line.h"
#include "view_define.h"
#include "size.h"
#include "line_bound.h"
#include "row_bound.h"
#include "text_area_config.h"
#include "coordinate_converter.h"



class Row;
class DocLoc;
class Editor;


namespace gui::impl
{


// 实现GUI相关的逻辑中和具体GUI工具包无关的部分
class TextArea {
public:
    TextArea(Editor &editor, const TextAreaConfig &config, const Size &size);

    ~TextArea();

    void start();

public:

    void resize(const Size &size);

    void onPrimaryButtomPress(i32 x, i32 y);

    void onDirUpKeyPress();

    void onDirDownKeyPress();

    void onDirLeftKeyPress();

    void onDirRightKeyPress();

    // 向后移动一个line，移动成功则返回true，移动失败则返回false
    // 仅当视图中只显示文档最后一个line或文档没有内容时，返回false
    bool moveDownByOneLine();

public:
    int width() const { return size_.width(); }

    int height() const { return size_.height(); }

    Editor &editor() { return editor_; }

    const TextAreaConfig &config() const { return config_; }

    std::optional<Rect> getLastActLineDrawRect() const;

    std::optional<VerticalLine> getNormalCursorDrawData() const;

    int getLineNumBarWidth() const;

    void drawEachLineNum(std::function<void(RowN lineNum, i32 baseline, const RowBound &bound, bool isLastAct)> &&action) const;

    void drawEachChar(std::function<void(i32 x, i32 y, char32_t c)> &&action) const;

    Signal<void()> &sigShouldRepaint() {
        return sigShouldRepaint_;
    }

    Signal<void()> &sigViewLocChanged() {
        return sigViewLocChanged_;
    }

private:
    void setSize(const Size &size);
    int calcMaxShownLineCnt() const;

    LineBound getLineBoundByLineOffset(i32 lineOffset) const;
    LineBound getLineBound(const VLineLoc &lineLoc) const;
    RowBound getRowBound(const VRowLoc &rowLoc) const;
    bool hasPrevCharAtSameLine(const VCharLoc &charLoc) const;
    bool noPrevCharAtSameLine(const VCharLoc &charLoc) const;
    bool hasNextCharAtSameLine(const VCharLoc &charLoc) const;
    bool noNextCharAtSameLine(const VCharLoc &charLoc) const;
    bool needEnsureHasNextLine(const VCharLoc &charLoc) const;
    bool isLastLineOfRow(const VLineLoc &lineLoc) const;
    bool isEndOfVirtualLine(const VCharLoc &charLoc) const;
    VCharLoc betterLocForVerticalMove(const VCharLoc &charLoc) const;
    DocLoc getNextUpLoc(const DocLoc &docLoc) const;
    DocLoc getNextDownLoc(const DocLoc &docLoc) const;
    DocLoc getNextLeftLoc(const DocLoc &docLoc) const;
    void makeVRow(const Row &row, VRow &vrow);

    // 视图中最后一个可视line的LineLoc
    VLineLoc getShownLastLineLoc() const;

private:
    // 调用这个函数后，需要在合适的时刻调用removeSpareRow
    void ensureHasPrevLine(const VLineLoc &curLineLoc);

    // 确保参数行有下一行
    // 如果需要把页面头部向后移动则返回true
    // 返回true则需要在合适的时刻调用movePageHeadOneLine
    bool ensureHasNextLine(const VLineLoc &curLineLoc);

    void setViewLoc(const ViewLoc &viewLoc);

    // 根据当前游标位置更新stableX
    // 当视口大小调整时调用，视口大小调整时之前计算的stableX已经没有意义，需要重新计算
    void updateStableXByCurrentCursor();

private:
    void remakePage();

    // 把页面开头向后移动一个line
    void movePageHeadOneLine();
    // 删除多余的row
    void removeSpareRow();

private:
    Editor &editor_;
    TextAreaConfig config_;
    const CoordinateConverter cvt_;

    ViewLoc vloc_{ 0, 0 };

    Size size_;

    // 最多可显式的行的数量，包括最后的不能完整显式的行
    // 这个值仅取决于视口大小，和文档位置无关
    i32 lineCountLimit_ = 0;

    Page page_;

    // 对于非等宽字体，当光标多次上下移动时，希望横坐标相对稳定，
    // 记录一个稳定位置，每次上下移动时尽可能选取与之接近的位置
    // 在某些操作后更新，如左右移动光标等操作
    i32 stableX_ = 0;

private:
    Signal<void()> sigShouldRepaint_;
    Signal<void()> sigViewLocChanged_;

private:
    SigConns editorSigConns_;
};


}
