#include "cut_rows.h"


namespace text
{



std::vector<std::u32string> cutRows(const std::u32string &s)
{
    enum class State {
        Normal,
        MeetCR, // '\r'
    };

    std::vector<std::u32string> result;
    State st = State::Normal;
    std::u32string buff;

    for (char32_t u : s) {
        switch (st) {
        case State::Normal:
            if (u != '\r' && u != '\n') {
                buff.push_back(u);
            } else {
                if (u == '\r') {
                    st = State::MeetCR;
                } else {
                    result.push_back(std::move(buff));
                }
            }
            break;
        case State::MeetCR:
            if (u == '\r') {
                result.push_back(std::move(buff));
            } else if (u == '\n') {
                result.push_back(std::move(buff));
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
            result.push_back(std::move(buff));
        }
    }

    return result;
}

}
