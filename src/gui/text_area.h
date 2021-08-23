#pragma once

#include <vector>
#include <optional>

#include "core/signal.h"
#include "doc/doc_define.h"
#include "doc/doc_loc.h"

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


namespace gui
{

namespace impl
{
class TextArea;
}


// 实现GUI相关的逻辑中和具体GUI工具包无关的部分
class TextArea {
public:
    TextArea(Editor &editor, const TextAreaConfig &config, const Size &size);

    ~TextArea();

    void start();

    void resize(const Size &size);

    void onPrimaryButtomPress(i32 x, i32 y);

    void onDirUpKeyPress();

    void onDirDownKeyPress();

    void onDirLeftKeyPress();

    void onDirRightKeyPress();

    // 向后移动一个line，移动成功则返回true，移动失败则返回false
    // 仅当视图中只显示文档最后一个line或文档没有内容时，返回false
    bool moveDownByOneLine();

    int width() const;

    int height() const;

    Editor &editor();

    const TextAreaConfig &config() const;

    std::optional<Rect> getLastActLineDrawRect() const;

    std::optional<VerticalLine> getNormalCursorDrawData() const;

    int getLineNumBarWidth() const;

    void drawEachLineNum(std::function<void(RowN lineNum, i32 baseline, const RowBound &bound, bool isLastAct)> &&action) const;

    void drawEachChar(std::function<void(i32 x, i32 y, char32_t c)> &&action) const;

    Signal<void()> &sigShouldRepaint();

    Signal<void()> &sigViewLocChanged();

private:
    impl::TextArea *impl_ = nullptr;
};


}
