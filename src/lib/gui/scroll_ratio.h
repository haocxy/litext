#pragma once

#include "doc/doc_define.h"


namespace gui
{

class ScrollRatio {
public:
    ScrollRatio() {}

    ScrollRatio(RowN off, RowN total)
        : off_(off)
        , total_(total) {}

    operator bool() const {
        return off_ > 0 && total_ >= off_;
    }

    RowN off() const {
        return off_;
    }

    RowN total() const {
        return total_;
    }

    // 把滚动比例映射到区间 [0,max]
    template <typename T>
    T map(T max) const {
        //    off / total = result / max
        // => result = off * max / total
        return static_cast<T>(off_ * max / total_);
    }

private:
    RowN off_ = 0;
    RowN total_ = 0;
};

}
