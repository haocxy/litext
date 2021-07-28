#pragma once

#include "core/primitive_types.h"


// 控制水平方向文本布局的配置
class HorizontalTextLayoutConfig {
public:
    HorizontalTextLayoutConfig() {}

    bool wrapLine() const {
        return wrapLine_;
    }

    void setWrapLine(bool wrapLine) {
        wrapLine_ = wrapLine;
    }

    i32 hGap() const {
        return hGap_;
    }

    void setHGap(int hGap) {
        hGap_ = hGap;
    }

    i32 hMargin() const {
        return hMargin_;
    }

    void setHMargin(int hMargin) {
        hMargin_ = hMargin;
    }

    i32 tabSize() const {
        return tabSize_;
    }

    void setTabSize(int tabSize) {
        tabSize_ = tabSize;
    }

private:
    bool wrapLine_ = false;
    i32 hGap_ = 2; // 水平方向最左侧字符左边的空白
    i32 hMargin_ = 2; // 水平字符间距
    i32 tabSize_ = 4; // 一个TAB的宽度为若干个空格
};
