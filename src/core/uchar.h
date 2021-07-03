#pragma once

typedef char32_t UChar;

namespace UCharUtil
{

template <typename T>
inline constexpr bool needSurrogate(T ch) {
	return ch > T(0xFFFF);
}

template <typename T>
inline constexpr bool isSurrogate(T ch) {
	return (ch & (~T(0b1'1111'1111))) == (T(0b1101'1) << T(11));
}

template <typename T>
inline constexpr bool isHighSurrogate(T ch) {
	return (ch & (~T(0b11'1111'1111))) == (T(0b1101'10) << T(10));
}

template <typename T>
inline constexpr bool isLowSurrogate(T ch) {
	return (ch & (~T(0b11'1111'1111))) == (T(0b1101'11) << T(10));
}

template <typename T>
inline constexpr UChar u16SurrogatePairToUnicode(T high, T low) {
	return '%'; // TODO
}

}
