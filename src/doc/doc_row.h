#pragma once

#include "doc_define.h"


enum class RowEnd {
    Unknown = 0, // 结束符未知
    NO, // 没有结束符，只存在于最后一行
    CR, // 以 \r 结束
    LF, // 以 \n 结束
    CRLF, // 以 \r\n 结束
};

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
        from.rowEnd_ = RowEnd::Unknown;
    }

private:
    std::u32string content_;
    RowEnd rowEnd_ = RowEnd::Unknown;
};
