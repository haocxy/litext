#pragma once

namespace UCharUtil
{

template <typename T>
inline constexpr bool needSurrogate(T ch) {
	return ch > T(0xFFFF);
}

template <typename T>
inline constexpr bool isSurrogate(T ch) {
	return (ch & (~T(0b111'1111'1111))) == (T(0b1101'1) << T(11));
}

template <typename T>
inline constexpr bool isHighSurrogate(T ch) {
	return (ch & (~T(0b11'1111'1111))) == (T(0b1101'10) << T(10));
}

template <typename T>
inline constexpr bool isLowSurrogate(T ch) {
	return (ch & (~T(0b11'1111'1111))) == (T(0b1101'11) << T(10));
}

constexpr char32_t U16SurrogatePairToUnicodeDelta = (char32_t(0b1101'10) << char32_t(20)) + (char32_t(0b1101'11) << char32_t(10)) - char32_t(0x10000);

template <typename T>
inline constexpr char32_t u16SurrogatePairToUnicode(T high, T low) {
	return (char32_t(high) << char32_t(10)) + char32_t(low) - U16SurrogatePairToUnicodeDelta;
}

}

namespace utf16
{

inline bool needSuggorate(char32_t unicode) {
    return UCharUtil::needSurrogate(unicode);
}

inline bool isSuggorate(char16_t c16) {
    return UCharUtil::isSurrogate(c16);
}

inline bool isHiSuggorate(char16_t c16) {
    return UCharUtil::isHighSurrogate(c16);
}

inline bool isLoSuggorate(char16_t c16) {
    return UCharUtil::isLowSurrogate(c16);
}

inline char32_t toUnicode(char16_t hi, char16_t lo) {
    return UCharUtil::u16SurrogatePairToUnicode(hi, lo);
}

}
