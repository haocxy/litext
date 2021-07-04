#pragma once

#include "core/ustring.h"


class WordInStream
{
public:
    virtual ~WordInStream() {}

    virtual UString Next() = 0;
};
