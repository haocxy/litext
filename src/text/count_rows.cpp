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
            if (u != '\r' && u != '\n') {
                // do nothing
            } else {
                if (u == '\r') {
                    st = State::MeetCR;
                } else {
                    ++sum;
                }
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

    if (!s.empty()) {
        const char32_t lastCode = s.back();
        if (lastCode != '\r' && lastCode != '\n') {
            ++sum;
        }
    }

    return sum;
}

}
