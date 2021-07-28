#pragma once

#include <assert.h>
#include <vector>
#include <deque>
#include <bitset>
#include <optional>

#include "core/signal.h"
#include "core/strong_type_integer.h"
#include "doc/doc_define.h"
#include "doc/doc_loc.h"

#include "page.h"
#include "line_offset.h"
#include "char_loc.h"
#include "rect.h"
#include "vertical_line.h"
#include "view_define.h"
#include "declare_text_area_config.h"
#include "size.h"
#include "line_bound.h"
#include "row_bound.h"
#include "view_loc.h"
#include "pixel.h"
#include "coordinate_converter.h"
#include "line_manger.h"


class Row;
class DocLoc;
class Editor;


namespace gui
{


// 实现GUI相关的逻辑中和具体GUI工具包无关的部分
class TextArea {
public:
    TextArea(Editor &editor, const TextAreaConfig &config, const Size &size);

    ~TextArea();

    void start();

public:
    LineManager &lineManager() {
        return lineManager_;
    }

    void resize(const Size &size);

    void onPrimaryButtomPress(Pixel x, Pixel y);

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

    void drawEachLineNum(std::function<void(RowN lineNum, Pixel::Raw baseline, const RowBound &bound, bool isLastAct)> &&action) const;

    void drawEachChar(std::function<void(Pixel::Raw x, Pixel::Raw y, UChar c)> &&action) const;

public:
    auto &sigShouldRepaint() {
        return sigShouldRepaint_;
    }

    auto &sigViewLocChanged() {
        return sigViewLocChanged_;
    }

private:
    int getMaxShownLineCnt() const;

    LineBound getLineBoundByLineOffset(LineOffset lineOffset) const;
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
    Size size_;
    LineManager lineManager_;
    Page page_;
    
    ViewLoc vloc_{ 0, 0 };
    const CoordinateConverter cvt_;

    // 对于非等宽字体，当光标多次上下移动时，希望横坐标相对稳定，记录一个稳定位置，每次上下移动时尽可能选取与之接近的位置
    // 在某些操作后更新，如左右移动光标等操作
    Pixel::Raw stableX_ = 0;

private:
    Signal<void()> sigShouldRepaint_;
    Signal<void()> sigViewLocChanged_;

private:
    SigConns editorSigConns_;
};


}
