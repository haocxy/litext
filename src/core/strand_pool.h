#pragma once

#include <string>

#include "strand.h"


class StrandPool {
public:
    virtual ~StrandPool() {}

    virtual Strand &allocate(const std::string &name) = 0;
};
