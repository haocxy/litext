#include "count_rows.h"


namespace text
{

i64 countRows(const std::u32string &s) {
    enum class State {
        Normal,
        MeetCR, // '\r'
    };

    i64 sum = 0;
    State st = State::Normal;

    for (char32_t u : s) {
        switch (st) {
        case State::Normal:
            if (u == '\r') {
                st = State::MeetCR;
            } else if (u == '\n') {
                ++sum;
            } else {
                // TODO 这里考虑是否应该处理下Unicode中的其它的换行符，也许应该由配置决定
            }
            break;
        case State::MeetCR:
            if (u == '\r') {
                ++sum;
            } else if (u == '\n') {
                ++sum;
                st = State::Normal;
            }
            break;
        default:
            break;
        }
    }

    return sum;
}

}
