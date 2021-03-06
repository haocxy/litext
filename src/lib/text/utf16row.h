#pragma once

#include <string>
#include <memory_resource>

#include "core/uchar.h"
#include "core/row_end.h"


namespace text
{

class UTF16Row {
public:
    UTF16Row(RowEnd rowEnd, bool hasSuggorate, std::pmr::u16string &&content)
        : rowEnd_(rowEnd), hasSuggorate_(hasSuggorate), content_(std::move(content)) {}

    UTF16Row(const UTF16Row &) = delete;

    UTF16Row(UTF16Row &&b) noexcept
        : rowEnd_(b.rowEnd_)
        , hasSuggorate_(b.hasSuggorate_)
        , content_(std::move(b.content_))
    {

    }

    UTF16Row &operator=(const UTF16Row &) = delete;

    UTF16Row &operator=(UTF16Row &&b) noexcept {
        rowEnd_ = b.rowEnd_;
        hasSuggorate_ = b.hasSuggorate_;
        content_ = std::pmr::u16string(std::move(b.content_));
        return *this;
    }

    RowEnd rowEnd() const {
        return rowEnd_;
    }

    void setRowEnd(RowEnd rowEnd) {
        rowEnd_ = rowEnd;
    }

    bool hasSuggorate() const {
        return hasSuggorate_;
    }

    void markHasSuggorate() {
        hasSuggorate_ = true;
    }

    void setContent(std::u16string &&content) {
        content_ = std::move(content);
    }

    const std::pmr::u16string &content() const {
        return content_;
    }

    std::u32string toUTF32() const {
        std::u32string result;
        const auto u16Len = content_.size();
        if (!hasSuggorate_) {
            result.resize(u16Len);
            for (std::remove_const_t<decltype(u16Len)> i = 0; i < u16Len; ++i) {
                result[i] = content_[i];
            }
        } else {
            result.reserve(u16Len);
            for (std::remove_const_t<decltype(u16Len)> i = 0; i < u16Len; ++i) {
                const char16_t c = content_[i];
                if (!utf16::isSuggorate(c)) {
                    result.push_back(c);
                } else {
                    const char16_t next = content_[++i];
                    result.push_back(utf16::toUnicode(c, next));
                }
            }
        }
        return result;
    }

private:
    RowEnd rowEnd_ = RowEnd::NoEnd;
    bool hasSuggorate_ = false;
    std::pmr::u16string content_;
};


}
