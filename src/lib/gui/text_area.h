#pragma once

#include <optional>
#include <mutex>
#include <shared_mutex>

#include <QImage>

#include "core/signal.h"
#include "doc/document.h"

#include "size.h"
#include "view_locs.h"
#include "scroll_ratio.h"
#include "row_bound.h"
#include "line_bound.h"
#include "view_define.h"
#include "text_area_config.h"


class Editor;


namespace gui
{

class TextAreaImpl;


// 实现GUI相关的逻辑中和具体GUI工具包无关的部分
class TextArea {
public:
    TextArea(Editor &editor, const TextAreaConfig &config);

    ~TextArea();

    const doc::Document &doc() const;

    doc::Document &doc();

    void open(const Size &size, RowN row = 0);

    void resize(const Size &size);

    void jumpTo(RowN row);

    void jumpTo(float ratio);

    RowN rowOff() const;

    RowN scrollPos() const;

    ScrollRatio scrollRatio() const;

    LineN lineCountLimit() const;

    enum class Dir { Up, Down, Left, Right };

    void moveCursor(Dir dir);

    void putCursor(i32 x, i32 y);

    void movePage(Dir dir);

    void scroll(Dir dir, LineN lineCount);

    const QImage &widgetImg() const;

    int width() const;

    int height() const;

    Editor &editor();

    const TextAreaConfig &config() const;

    void drawEachLineNum(std::function<void(RowN lineNum, i32 baseline, const RowBound &bound, bool isLastAct)> &&action) const;

    int fontSizeByPoint() const;

    void setFontSizeByPoint(int pt);

    Signal<void(int)> &sigFontSizeUpdated();

    Signal<void()> &sigShouldRepaint();

    Signal<void()> &sigViewportChanged();

private:
    using Mtx = std::shared_mutex;
    using ReadLock = std::shared_lock<Mtx>;
    using WriteLock = std::lock_guard<Mtx>;
    mutable Mtx mtx_;
    TextAreaImpl *impl_ = nullptr;
};


}
