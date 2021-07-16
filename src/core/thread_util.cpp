#include "thread_util.h"

#include <atomic>

namespace ThreadUtil
{

uint32_t currentThreadShortId() {
    static std::atomic<uint32_t> s_nextShortId = 1;
    thread_local uint32_t tl_curThreadShortId = s_nextShortId++;
    return tl_curThreadShortId;
}

}
