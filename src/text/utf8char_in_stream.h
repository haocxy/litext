#pragma once

#include <string>

#include "char_instream.h"


class UTF8CharInStream {
public:
    UTF8CharInStream(const std::string &str)
        : str_(str) {
    }

    char next() {
        if (i_ < str_.size()) {
            return str_[i_++];
        } else {
            return 0;
        }
    }

private:
    const std::string &str_;
    size_t i_ = 0;
};


class CharInStreamOverUTF8CharInStream : public CharInStream {
public:
    CharInStreamOverUTF8CharInStream(UTF8CharInStream &u8in)
        : u8in_(u8in) {}

    virtual ~CharInStreamOverUTF8CharInStream() {}

    virtual UChar Next() override {
        const char c = u8in_.next();
        if (c != 0) {
            if ((c & 0b1000'0000) == 0) {
                return c;
            }
            if ((c & 0b1110'0000) == 0b1100'0000) {
                const UChar c1 = c & 0b0001'1111;
                const UChar c2 = u8in_.next() & 0b0011'1111;
                return (c1 << 7) | c2;
            }
            if ((c & 0b1111'0000) == 0b1110'0000) {
                const UChar c1 = c & 0b0000'1111;
                const UChar c2 = u8in_.next() & 0b0011'1111;
                const UChar c3 = u8in_.next() & 0b0011'1111;
                return (c1 << 12) | (c2 << 6) | c3;
            }
            if ((c & 0b1111'1000) == 0b1111'0000) {
                const UChar c1 = c & 0b0000'0111;
                const UChar c2 = u8in_.next() & 0b0011'1111;
                const UChar c3 = u8in_.next() & 0b0011'1111;
                const UChar c4 = u8in_.next() & 0b0011'1111;
                return (c1 << 18) | (c2 << 12) | (c3 << 6) | c4;
            }
            return 0;
        } else {
            return 0;
        }
    }

private:
    UTF8CharInStream &u8in_;
};
