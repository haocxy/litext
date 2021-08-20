#include "utf.h"


namespace charset::utf
{

std::string u32ToU8(const std::u32string &s)
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

    std::string result;

    for (char32_t u : s) {
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
