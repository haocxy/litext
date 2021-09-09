#pragma once

#include <cstring>
#include <string>
#include <string_view>
#include <ostream>
#include <memory_resource>


namespace ustrdetail
{

inline std::string convertToU8(const std::u32string &u32s)
{
    constexpr char32_t keeplow = 0b0011'1111;
    constexpr char32_t decolow = 0b1000'0000;

    constexpr char32_t keephi2 = 0b1111'1;
    constexpr char32_t keephi3 = 0b1111;
    constexpr char32_t keephi4 = 0b111;
    constexpr char32_t keephi5 = 0b11;
    constexpr char32_t keephi6 = 0b1;

    constexpr char32_t decohi2 = 0b1100'0000;
    constexpr char32_t decohi3 = 0b1110'0000;
    constexpr char32_t decohi4 = 0b1111'0000;
    constexpr char32_t decohi5 = 0b1111'1000;
    constexpr char32_t decohi6 = 0b1111'1100;

    std::basic_string<char> result;

    for (char32_t u : u32s) {
        if (u < 0x80) {
            result.push_back(static_cast<char>(u));
        } else if (u < 0x800) {
            result.push_back(static_cast<char>(decohi2 | (keephi2 & (u >> 6))));
            result.push_back(static_cast<char>(decolow | (keeplow & u)));
        } else if (u < 0x10000) {
            result.push_back(static_cast<char>(decohi3 | (keephi3 & (u >> 12))));
            result.push_back(static_cast<char>(decolow | (keeplow & (u >> 6))));
            result.push_back(static_cast<char>(decolow | (keeplow & u)));
        } else if (u < 0x200000) {
            result.push_back(static_cast<char>(decohi4 | (keephi4 & (u >> 18))));
            result.push_back(static_cast<char>(decolow | (keeplow & (u >> 12))));
            result.push_back(static_cast<char>(decolow | (keeplow & (u >> 6))));
            result.push_back(static_cast<char>(decolow | (keeplow & u)));
        } else if (u < 0x4000000) {
            result.push_back(static_cast<char>(decohi5 | (keephi5 & (u >> 24))));
            result.push_back(static_cast<char>(decolow | (keeplow & (u >> 18))));
            result.push_back(static_cast<char>(decolow | (keeplow & (u >> 12))));
            result.push_back(static_cast<char>(decolow | (keeplow & (u >> 6))));
            result.push_back(static_cast<char>(decolow | (keeplow & u)));
        } else {
            result.push_back(static_cast<char>(decohi6 | (keephi6 & (u >> 30))));
            result.push_back(static_cast<char>(decolow | (keeplow & (u >> 24))));
            result.push_back(static_cast<char>(decolow | (keeplow & (u >> 18))));
            result.push_back(static_cast<char>(decolow | (keeplow & (u >> 12))));
            result.push_back(static_cast<char>(decolow | (keeplow & (u >> 6))));
            result.push_back(static_cast<char>(decolow | (keeplow & u)));
        }
    }

    return result;
}

}


class u8str : public std::string {
public:
    u8str() {}

    u8str(const u8str &b)
        : std::string(b) {}

    u8str(u8str &&b) noexcept
        : std::string(std::move(b)) {}

    u8str(allocator_type allocator)
        : std::string(allocator) {}

    u8str(const char *cstr)
        : std::string(cstr) {}

    u8str(short n)
        : std::string(std::to_string(n)) {}

    u8str(int n)
        : std::string(std::to_string(n)) {}

    u8str(long n)
        : std::string(std::to_string(n)) {}

    u8str(long long n)
        : std::string(std::to_string(n)) {}

    u8str(const std::u32string &s)
        : std::string(ustrdetail::convertToU8(s)) {}

    virtual ~u8str() {}

    u8str &operator=(const u8str &b) {
        std::string::operator=(b);
        return *this;
    }

    u8str &operator=(u8str &&b) noexcept {
        std::string::operator=(std::move(b));
        return *this;
    }

    operator int() const {
        return std::stoi(*this);
    }

    operator long() const {
        return std::stol(*this);
    }

    operator long long() const {
        return std::stoll(*this);
    }

private:

    explicit u8str(std::string &&b)
        : std::string(std::move(b)) {}
};

class u8view : public std::string_view {
public:

    u8view() {}

    u8view(const u8str &s)
        : std::string_view(s) {}

    virtual ~u8view() {}
};

using u16str = std::u16string;

using u16view = std::u16string_view;

class u32str : public std::u32string {
public:

    u32str() {}

    u32str(const std::u32string &b)
        : std::u32string(b) {}

    u32str(std::u32string &&b)
        : std::u32string(std::move(b)) {}

    u32str(allocator_type allocator)
        : std::u32string(allocator) {}

    u32str(const u8str &u8s);

    virtual ~u32str() {}

    u32str &operator=(const std::u32string &b) {
        std::u32string::operator=(b);
        return *this;
    }

    u32str &operator=(std::u32string &&b) {
        std::u32string::operator=(std::move(b));
        return *this;
    }
};

using u32view = std::u32string_view;
