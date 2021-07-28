#pragma once

#include "core/primitive_types.h"


// 控制水平方向文本布局的配置
class HLayoutConfig {
public:
    HLayoutConfig() {}

    bool wrapLine() const {
        return wrapLine_;
    }

    void setWrapLine(bool wrapLine) {
        wrapLine_ = wrapLine;
    }

    i32 gap() const {
        return gap_;
    }

    void setGap(int gap) {
        gap_ = gap;
    }

    i32 pad() const {
        return pad_;
    }

    void setPad(int pad) {
        pad_ = pad;
    }

    i32 tabSize() const {
        return tabSize_;
    }

    void setTabSize(int tabSize) {
        tabSize_ = tabSize;
    }

private:
    bool wrapLine_ = false;
    i32 gap_ = 2; // 水平方向最左侧字符左边的空白
    i32 pad_ = 2; // 水平字符间距
    i32 tabSize_ = 4; // 一个TAB的宽度为若干个空格
};
