#pragma once

#include <string>
#include <memory_resource>


class u8str : public std::basic_string<signed char> {
public:

    template <typename ...T>
    u8str(T &&...args)
        : std::basic_string<signed char>(std::forward<T>(args)...) {}

    virtual ~u8str() {}
};

using u16str = std::u16string;

using u32str = std::u32string;
