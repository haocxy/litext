#pragma once

#include <functional>

#include "text/char_instream.h"
#include "layout_config.h"
#include "glyph_width_provider.h"
#include "view_char.h"


namespace gui
{

class RowWalker {
public:
    RowWalker(GlyphWidthProvider &widthProvider, CharInStream &stream, const HLayoutConfig &h, int widthLimit)
        : widthProvider_(widthProvider)
        , charStream_(stream)
        , wrapLine_(h.wrapLine())
        , widthLimit_(widthLimit)
        , hGap_(h.gap())
        , hPad_(h.pad())
        , tabSize_(h.tabSize()) {


    }

    void forEachChar(std::function<void(bool isEmptyRow, size_t lineIndex, const VChar &vchar)> &&action) {
        if (wrapLine_) {
            forEachCharWithWrapLine(std::move(action));
        } else {
            forEachCharNoWrapLine(std::move(action));
        }
    }

private:
    void forEachCharWithWrapLine(std::function<void(bool isEmptyRow, size_t lineIndex, const VChar &vchar)> &&action);

    void forEachCharNoWrapLine(std::function<void(bool isEmptyRow, size_t lineIndex, const VChar &vchar)> &&action);

    int charPixelWith(char32_t c) const {
        // 换行符不占用空间
        if (c == '\n' || c == '\r') {
            return 0;
        }

        // tab符特殊处理
        if (c == '\t') {
            // *[]*[]*[]*[]*
            return hPad_ * (tabSize_ - 1) + widthProvider_.glyphWidth(' ') * tabSize_;
        }

        return widthProvider_.glyphWidth(c);
    }

private:
    GlyphWidthProvider &widthProvider_;
    CharInStream &charStream_;

    // 外部传入的设置
    bool wrapLine_ = false;
    int widthLimit_ = 0;
    int hGap_ = 0;
    int hPad_ = 0;
    int tabSize_ = 0;
};

}
