#pragma once

#include "core/primitive_types.h"
#include "core/uchar.h"
#include "core/font_old.h"
#include "core/font_index.h"

#include "doc/layout_config.h"


namespace gui
{

class TextAreaConfig {
public:
    TextAreaConfig() {
    }

    i32 lineHeight() const {
        return lineHeightFactor_ * font_.height();
    }

    const HLayoutConfig &hLayout() const {
        return hLayout_;
    }

    void setHLayout(const HLayoutConfig &config) {
        hLayout_ = config;
    }

    const FontOld &font() const {
        return font_;
    }

    void setFont(const FontOld &font);

    const font::FontIndex &fontIndex() const {
        return fontIndex_;
    }

    void setFontIndex(const font::FontIndex &fontIndex) {
        fontIndex_ = fontIndex;
    }

    bool showLineNum() const {
        return showLineNum_;
    }

    void setShowLineNum(bool showLineNum) {
        showLineNum_ = showLineNum;
    }

    i32 lineNumOffset() const {
        return lineNumOffset_;
    }

    void setLineNumOffset(i32 lineNumOffset) {
        lineNumOffset_ = lineNumOffset;
    }

private:
    HLayoutConfig hLayout_;
    f32 lineHeightFactor_ = 1.0f; // 行高系数，行高 = 行高系数 * 字体高度
    bool showLineNum_ = false; // 是否显示行号
    i32 lineNumOffset_ = 0; // 行号偏移，显示行号时，把程序内部从0开始的行索引加上这个值

    FontOld font_;
    font::FontIndex fontIndex_;
};


}
