#pragma once

#include <stdint.h>
#include "common/nullable_class.h"
#include "doc/doc_define.h"

namespace doc
{
    class LineNum : public NullableClass
    {
    public:

    private:
        LineN m_value = 0;
    };
}
