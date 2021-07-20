#pragma once

#include "char_instream.h"


class UTF16CharInStream {
public:
    UTF16CharInStream(const void *data, size_t len)
        : data_(reinterpret_cast<const char16_t *>(data)), count_(len / 2) {}

    char16_t next() {
        if (index_ < count_) {
            return data_[index_++];
        } else {
            return 0;
        }
    }

private:
    const char16_t * const data_;
    const size_t count_;
    size_t index_ = 0;
};


class CharInStreamOverUTF16CharInStram : public CharInStream {
public:
    CharInStreamOverUTF16CharInStram(UTF16CharInStream &utf16stream)
        : utf16stream_(utf16stream) {}

    virtual ~CharInStreamOverUTF16CharInStram() {}

    virtual UChar Next() override {
        const char16_t ch = utf16stream_.next();
        if (ch != 0) {
            if (!UCharUtil::isSurrogate(ch)) {
                return static_cast<UChar>(ch);
            } else {
                const char16_t next = utf16stream_.next();
                if (next != 0) {
                    return UCharUtil::u16SurrogatePairToUnicode(ch, next);
                } else {
                    return 0;
                }
            }
        } else {
            return 0;
        }
    }

private:
    UTF16CharInStream &utf16stream_;
};
