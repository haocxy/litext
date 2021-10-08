#pragma once

#include <cassert>
#include <string>


// 把计算机中的数值转换为更为可读的字符串
namespace Readable
{

namespace detail
{

template <typename N, typename U>
inline std::string fileSizeImpl(N n, U unit, int prec, const char *suffix) {
    assert(suffix);
    std::ostringstream ss;
    if (prec > 0) {
        const double f = static_cast<double>(n) / static_cast<double>(unit);
        ss << f;
        std::string result = ss.str();
        const int len = static_cast<int>(result.length());
        for (int i = 0; i < len; ++i) {
            if (result[i] == '.') {
                const int resultDigitLen = i + 1 + prec;
                if (len > resultDigitLen) {
                    result.resize(resultDigitLen);
                    result.append(suffix);
                    return result;
                } else {
                    const int d = resultDigitLen - len;
                    for (int j = 0; j < d; ++j) {
                        result.push_back('0');
                    }
                    result.append(suffix);
                    return result;
                }
            }
        }
        result.push_back('.');
        for (int i = 0; i < prec; ++i) {
            result.push_back('0');
        }
        result.append(suffix);
        return result;
    } else {
        ss << (n / unit) << suffix;
        return ss.str();
    }
}

}

template <typename N>
inline std::string fileSize(N n, int prec = 1) {
    constexpr N kb = 1024;
    constexpr N mb = kb * 1024;
    constexpr N gb = mb * 1024;
    if (n > gb) {
        return detail::fileSizeImpl(n, gb, prec, "GB");
    }
    if (n > mb) {
        return detail::fileSizeImpl(n, mb, prec, "MB");
    }
    if (n > kb) {
        return detail::fileSizeImpl(n, kb, prec, "KB");
    }
    return detail::fileSizeImpl(n, 1, 0, "B");
}

}
