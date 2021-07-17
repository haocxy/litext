#pragma once

#include "charset.h"
#include "membuff.h"


namespace detail
{

class CharsetConverterImpl;

}

class CharsetConverter
{
public:
    CharsetConverter();

    ~CharsetConverter();

    void open(Charset from, Charset to);

    void convert(const void *data, size_t len, MemBuff &buff);

    void convert(const MemBuff &from, MemBuff &to) {
        convert(from.data(), from.size(), to);
    }

private:
    detail::CharsetConverterImpl *impl_ = nullptr;
};
