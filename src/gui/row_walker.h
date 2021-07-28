#pragma once

#include <functional>

#include "text/char_instream.h"
#include "doc/layout_config.h"
#include "char_pix_width_provider.h"
#include "view_define.h"
#include "view_char.h"


namespace gui
{

class NewRowWalker {
public:
    NewRowWalker(CharPixWidthProvider &widthProvider, CharInStream &stream, const HLayoutConfig &h, int widthLimit)
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

    int charPixelWith(UChar c) const {
        // 换行符不占用空间
        if (c == '\n' || c == '\r') {
            return 0;
        }

        // tab符特殊处理
        if (c == '\t') {
            if (isFixWidth_) {
                // *[]*[]*[]*[]*
                return hPad_ * (tabSize_ - 1) + widthForFix_ * tabSize_;
            } else {
                return hPad_ * (tabSize_ - 1) + widthProvider_.charWidth(' ') * tabSize_;
            }
        }

        return widthProvider_.charWidth(c);
    }

private:
    CharPixWidthProvider &widthProvider_;
    CharInStream &charStream_;

    // 外部传入的设置
    bool wrapLine_ = false;
    int widthLimit_ = 0;
    int hGap_ = 0;
    int hPad_ = 0;
    int tabSize_ = 0;

private:
    // 内部计算得到的数据
    bool isFixWidth_ = false;
    int widthForFix_ = 0;
};

}
