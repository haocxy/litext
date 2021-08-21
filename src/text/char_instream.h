#pragma once


class CharInStream
{
public:
    virtual ~CharInStream() {}

    virtual char32_t Next() = 0;
};
