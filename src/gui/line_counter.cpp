#include "line_counter.h"


namespace gui::detail
{

LineCounter::LineCounter(StrandPool &pool)
    : worker_(pool.allocate("LineCounter")) {

}

}
