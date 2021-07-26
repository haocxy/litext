#pragma once

#include <functional>

#include "core/ustring.h"
#include "text/char_instream.h"

#include "declare_text_area_config.h"
#include "view_define.h"
#include "view_char.h"
#include "char_pix_width_provider.h"


namespace gui
{

class RowWalker {
public:
    using CharOperation = std::function<void(bool isEmptyRow, size_t lineIndex, const VChar &vchar)>;

    RowWalker(const TextAreaConfig &config, int widthLimit, CharInStream &charStream);

    void forEachChar(CharOperation &&operation);

private:
    const TextAreaConfig &config_;
    const int widthLimit_;
    CharInStream &charStream_;
};

class NewRowWalker {
public:
    struct Config {
        Config() {}
        Config(const TextAreaConfig &c, int widthLimit);
        bool wrapLine = false;
        int widthLimit = 0;
        int hGap = 0;
        int hMargin = 0;
        int tabSize = 0;
    };

    NewRowWalker(CharPixWidthProvider &widthProvider, CharInStream &stream, const Config &config)
        : widthProvider_(widthProvider)
        , charStream_(stream)
        , wrapLine_(config.wrapLine)
        , widthLimit_(config.widthLimit)
        , hGap_(config.hGap)
        , hMargin_(config.hMargin)
        , tabSize_(config.tabSize) {


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
                return hMargin_ * (tabSize_ - 1) + widthForFix_ * tabSize_;
            } else {
                return hMargin_ * (tabSize_ - 1) + widthProvider_.charWidth(' ') * tabSize_;
            }
        }

        // 不是等宽字体则直接返回宽度
        if (!isFixWidth_) {
            return widthProvider_.charWidth(c);
        }

        // 下面处理等宽字体

        const int rawWidth = widthProvider_.charWidth(c);
        if (rawWidth > widthForFix_) {
            // 如果当前字符宽度大于单字符宽度，则固定占用两个字符
            return hMargin_ + widthForFix_ * 2;
        } else {
            // 当前字符是普通的等宽单字符
            return widthForFix_;
        }
    }

private:
    CharPixWidthProvider &widthProvider_;
    CharInStream &charStream_;

    // 外部传入的设置
    bool wrapLine_ = false;
    int widthLimit_ = 0;
    int hGap_ = 0;
    int hMargin_ = 0;
    int tabSize_ = 0;

private:
    // 内部计算得到的数据
    bool isFixWidth_ = false;
    int widthForFix_ = 0;
};

}
