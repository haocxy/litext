#pragma once

#include "core/row_end.h"

#include "doc_define.h"


class Row {
public:
    Row() {}

    Row(std::u32string &&content, RowEnd rowEnd)
        : content_(content), rowEnd_(rowEnd) {}

    explicit Row(RowEnd rowEnd)
        : rowEnd_(rowEnd) {}

    Row(const Row &) = delete;

    Row(Row &&b) noexcept {
        move(*this, b);
    }

    Row &operator=(const Row &) = delete;

    Row &operator=(Row &&b) noexcept {
        move(*this, b);
        return *this;
    }

    CharN charCnt() const {
        return content_.size();
    }

    char32_t charAt(CharN i) const {
        return content_[i];
    }

    RowEnd rowEnd() const {
        return rowEnd_;
    }

private:
    static void move(Row &to, Row &from) {
        to.content_ = std::move(from.content_);
        to.rowEnd_ = from.rowEnd_;
        from.rowEnd_ = RowEnd::NoEnd;
    }

private:
    std::u32string content_;
    RowEnd rowEnd_ = RowEnd::NoEnd;
};
