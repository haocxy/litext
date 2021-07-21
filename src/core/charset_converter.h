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

    MemBuff convert(const void *data, size_t len);

    MemBuff convert(const MemBuff &from) {
        return convert(from.data(), from.size());
    }

private:
    detail::CharsetConverterImpl *impl_ = nullptr;
};
